/* Copyright (c) 2006-2009, Universities Space Research Association (USRA).
 *  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Universities Space Research Association nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "IpcAdapter.hh"
#include "ipc-data-formats.h"
#include "AdapterExecInterface.hh"
#include "Debug.hh"
#include "Error.hh"
#include "CoreExpressions.hh"
#include "StoredArray.hh"
#include "ThreadSpawn.hh"

#include "tinyxml.h"

// ooid classes
#include "uuid.h"
#include "devrand.h"

namespace PLEXIL
{

  /**
   * @brief Constructor.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   */
  IpcAdapter::IpcAdapter(AdapterExecInterface& execInterface)
    : InterfaceAdapter(execInterface),
      m_sem(),
      m_serial(0),
      m_pendingLookupSerial(0),
      m_pendingLookupDestination(NULL)
  {
    initializeUID();
  }

  /**
   * @brief Constructor from configuration XML.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   * @param xml A const pointer to the TiXmlElement describing this adapter
   */
  IpcAdapter::IpcAdapter(AdapterExecInterface& execInterface, 
			 const TiXmlElement * xml)
    : InterfaceAdapter(execInterface, xml),
      m_sem(),
      m_serial(0),
      m_pendingLookupSerial(0),
      m_pendingLookupDestination(NULL)
  {
    initializeUID();
  }

  /**
   * @brief Initialize unique ID string
   */
  void IpcAdapter::initializeUID()
  {
    kashmir::system::DevRand randomStream;
    kashmir::uuid_t uuid;
    randomStream >> uuid;
    std::ostringstream s;
    s << uuid;
    m_myUID = s.str();
    debugMsg("IpcAdapter:initializeUID", " generated UUID " << m_myUID);
  }


  /**
   * @brief Destructor.
   */
  IpcAdapter::~IpcAdapter()
  {
  }

  //
  // API to ExecApplication
  //

  /**
   * @brief Initializes the adapter, possibly using its configuration data.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::initialize()
  {
    // Get taskName, serverName from XML, if supplied
    const char* taskName = NULL;
    const char* serverName = NULL;

    const TiXmlElement* xml = this->getXml();
    if (xml != NULL) 
      {
	taskName = xml->Attribute("TaskName");
	serverName = xml->Attribute("Server");
      }

    // Use defaults if necessary
    if (taskName == NULL)
      {
	taskName = m_myUID.c_str();
      }
    if(serverName != NULL)
      {
	serverName = "localhost";
      }
    debugMsg("IpcAdapter:initialize", 
	     " Connecting module " << taskName <<
	     " to central server at " << serverName);

    // Initialize IPC
    // possibly redundant, but always safe
    IPC_initialize();

    // Connect to central
    assertTrueMsg(IPC_connectModule(taskName, serverName) == IPC_OK,
		  "IpcAdapter: Unable to connect to the central server at " << serverName);

    // Define messages
    if (!IPC_isMsgDefined(MSG_BASE))
      {
	assertTrueMsg(IPC_errno == IPC_No_Error,
		      "IpcAdapter: IPC_isMsgDefined failed, IPC_errno = " << IPC_errno);
	IPC_RETURN_TYPE status;
	status = IPC_defineMsg(MSG_BASE, IPC_VARIABLE_LENGTH, MSG_BASE_FORMAT);
	assertTrueMsg(status == IPC_OK,
		      "IpcAdapter: Error defining " << MSG_BASE << " message, IPC_errno = " << IPC_errno);
	status = IPC_defineMsg(RETURN_VALUE_MSG, IPC_VARIABLE_LENGTH, RETURN_VALUE_MSG_FORMAT);
	assertTrueMsg(status == IPC_OK,
		      "IpcAdapter: Error defining " << RETURN_VALUE_MSG << " message, IPC_errno = " << IPC_errno);
	status = IPC_defineMsg(NUMERIC_VALUE_MSG, IPC_VARIABLE_LENGTH, NUMERIC_VALUE_MSG_FORMAT);
	assertTrueMsg(status == IPC_OK,
		      "IpcAdapter: Error defining " << NUMERIC_VALUE_MSG << " message, IPC_errno = " << IPC_errno);
	status = IPC_defineMsg(STRING_VALUE_MSG, IPC_VARIABLE_LENGTH, STRING_VALUE_MSG_FORMAT);
	assertTrueMsg(status == IPC_OK,
		      "IpcAdapter: Error defining " << STRING_VALUE_MSG << " message, IPC_errno = " << IPC_errno);
	status = IPC_defineMsg(NUMERIC_PAIR_MSG, IPC_VARIABLE_LENGTH, NUMERIC_PAIR_MSG_FORMAT);
	assertTrueMsg(status == IPC_OK,
		      "IpcAdapter: Error defining " << NUMERIC_PAIR_MSG << " message, IPC_errno = " << IPC_errno);
	status = IPC_defineMsg(STRING_PAIR_MSG, IPC_VARIABLE_LENGTH, STRING_PAIR_MSG_FORMAT);
	assertTrueMsg(status == IPC_OK,
		      "IpcAdapter: Error defining " << STRING_PAIR_MSG << " message, IPC_errno = " << IPC_errno);
      }

    // Register with AdapterExecInterface
    m_execInterface.defaultRegisterAdapter(getId());
    // *** TODO: register lookup names for getting commands & msgs ***

    return true;
  }

  /**
   * @brief Starts the adapter, possibly using its configuration data.  
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::start()
  {
    // Spawn listener thread
    assertTrueMsg(threadSpawn((THREAD_FUNC_PTR)IPC_dispatch, NULL, m_thread),
		  "IpcAdapter: Unable to spawn IPC dispatch thread");
    debugMsg("IpcAdapter:start", " spawned IPC dispatch thread");

    // Subscribe to messages
    IPC_RETURN_TYPE status;
    status = IPC_subscribeData(MSG_BASE, messageHandler, this);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error subscribing to " << MSG_BASE << " messages, IPC_errno = " << IPC_errno);
    status = IPC_subscribeData(RETURN_VALUE_MSG, messageHandler, this);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error subscribing to " << RETURN_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_subscribeData(NUMERIC_VALUE_MSG, messageHandler, this);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error subscribing to " << NUMERIC_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_subscribeData(STRING_VALUE_MSG, messageHandler, this);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error subscribing to " << STRING_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_subscribeData(NUMERIC_PAIR_MSG, messageHandler, this);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error subscribing to " << NUMERIC_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_subscribeData(STRING_PAIR_MSG, messageHandler, this);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error subscribing to " << STRING_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);

    return true;
  }

  /**
   * @brief Stops the adapter.  
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::stop()
  {
    // Unsubscribe from messages
    IPC_RETURN_TYPE status;
    status = IPC_unsubscribe(MSG_BASE, NULL);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error unsubscribing from " << MSG_BASE << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(RETURN_VALUE_MSG, NULL);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error unsubscribing from " << RETURN_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(NUMERIC_VALUE_MSG, NULL);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error unsubscribing from " << NUMERIC_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(STRING_VALUE_MSG, NULL);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error unsubscribing from " << STRING_VALUE_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(NUMERIC_PAIR_MSG, NULL);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error unsubscribing from " << NUMERIC_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);
    status = IPC_unsubscribe(STRING_PAIR_MSG, NULL);
    assertTrueMsg(status == IPC_OK,
		  "IpcAdapter: Error unsubscribing from " << STRING_PAIR_MSG << " messages, IPC_errno = " << IPC_errno);

    // Cancel IPC dispatch thread
    int myErrno;
    assertTrueMsg((myErrno = pthread_cancel(m_thread)) == 0,
		  "IpcAdapter: error canceling IPC dispatch thread, errno = " << myErrno); 
    assertTrueMsg((myErrno = pthread_join(m_thread, NULL)) == 0,
		  "IpcAdapter: error joining IPC dispatch thread, errno = " << myErrno); 

    return true;
  }

  /**
   * @brief Resets the adapter.  
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::reset()
  {
    // No-op (?)
    return true;
  }

  /**
   * @brief Shuts down the adapter, releasing any of its resources.
   * @return true if successful, false otherwise.
   */
  bool IpcAdapter::shutdown()
  {
    // Disconnect from central
    IPC_disconnect();
    return true;
  }

  /**
   * @brief Register one LookupOnChange.
   * @param uniqueId The unique ID of this lookup.
   * @param stateKey The state key for this lookup.
   * @param tolerances A vector of tolerances for the LookupOnChange.
   */

  void IpcAdapter::registerChangeLookup(const LookupKey& uniqueId,
					const StateKey& stateKey,
					const std::vector<double>& tolerances)
  {
    State state;
    assertTrueMsg(getState(stateKey, state),
		  "IpcAdapter::registerChangeLookup: Internal error: state not found!");
    LabelStr nameLabel(state.first);
    const std::vector<double>& params = state.second;
    size_t nParams = params.size();
    debugMsg("IpcAdapter:registerChangeLookup",
	     " for state " << nameLabel.toString()
	     << " with " << nParams << " parameters");

    if (hasPrefix(nameLabel.toString(), MESSAGE_PREFIX()))
      {
	// Set up to receive this message
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::registerChangeLookup: Receiving messages is not yet implemented");
      }
    else if (hasPrefix(nameLabel.toString(), COMMAND_PREFIX()))
      {
	// Set up to receive this command
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::registerChangeLookup: Receiving commands is not yet implemented");
      }
    else if (hasPrefix(nameLabel.toString(), LOOKUP_PREFIX()))
      {
	// Set up to receive this lookup
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::registerChangeLookup: Receiving lookups is not yet implemented");
      }
    else if (hasPrefix(nameLabel.toString(), LOOKUP_ON_CHANGE_PREFIX()))
      {
	// Set up to receive this lookup
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::registerChangeLookup: Receiving lookups is not yet implemented");
      }
    else
      {
	// Send a request

	// Construct the messages
	// Leader
	uint32_t serial = getSerialNumber();
	struct PlexilStringValueMsg leader = {{PlexilMsgType_LookupOnChange,
					       nParams,
					       serial,
					       m_myUID.c_str()},
					      nameLabel.c_str()};
	// Construct parameter messages
	PlexilMsgBase* paramMsgs[nParams];
	for (size_t i = 0; i < nParams; i++)
	  {
	    double param = params[i];
	    PlexilMsgBase* paramMsg;
	    if (LabelStr::isString(param))
	      {
		// string
		struct PlexilStringValueMsg* strMsg = new PlexilStringValueMsg();
		strMsg->stringValue = LabelStr(param).c_str();
		paramMsg = (PlexilMsgBase*) strMsg;
		paramMsg->msgType = PlexilMsgType_StringValue;
	      }
	    else if (StoredArray::isKey(param))
	      {
		// array
		assertTrueMsg(ALWAYS_FAIL,
			      "IpcAdapter: Array values are not yet implemented");
	      }
	    else
	      {
		// number or Boolean
		struct PlexilNumericValueMsg* numMsg = new PlexilNumericValueMsg();
		numMsg->doubleValue = param;
		paramMsg = (PlexilMsgBase*) numMsg;
		paramMsg->msgType = PlexilMsgType_NumericValue;
	      }

	    // Fill in common fields
	    paramMsg->count = i;
	    paramMsg->serial = serial;
	    paramMsg->senderUID = m_myUID.c_str();
	    paramMsgs[i] = paramMsg;
	  }

	// Take care of bookkeeping
	m_changeLookups[IpcMessageId(m_myUID, serial)] = stateKey;

	// Send the messages
	// *** TODO: check for IPC errors ***
	IPC_publishData(STRING_VALUE_MSG, (void *) &leader);
	for (size_t i = 0; i < nParams; i++)
	  {
	    IPC_publishData(msgFormatForType((PlexilMsgType) paramMsgs[i]->msgType), 
			    paramMsgs[i]);
	  }
	// free the packets (NYI)
      }
  }

  /**
   * @brief Terminate one LookupOnChange.
   * @param uniqueId The unique ID of the lookup to be terminated.
   */

  void IpcAdapter::unregisterChangeLookup(const LookupKey& uniqueId)
  {
    StateKey key;
    
    IpcChangeLookupMap::iterator it = m_changeLookups.find(key);

    // FIXME: should this be an error?
    assertTrueMsg(it != m_changeLookups.end(),
		  "IpcAdapter::unregisterChangeLookup: internal error: lookup not found");
    
    State state;
    getState(key, state);
    const LabelStr& nameLabel = state.first;

    if (hasPrefix(nameLabel.toString(), MESSAGE_PREFIX()))
      {
	// Stop looking for this message
      }
    else if (hasPrefix(nameLabel.toString(), COMMAND_PREFIX()))
      {
	// Stop looking for this command
      }
    else if (hasPrefix(nameLabel.toString(), LOOKUP_PREFIX()))
      {
	// Stop looking for this lookup
      }
    else if (hasPrefix(nameLabel.toString(), LOOKUP_ON_CHANGE_PREFIX()))
      {
	// Stop looking for this lookup
      }
    else
      {
	// send a TerminateChangeLookup message with the serial #
	// of the original request

      }
  }

  /**
   * @brief Perform an immediate lookup of the requested state.
   * @param stateKey The state key for this lookup.
   * @param dest A (reference to a) vector of doubles where the result is to be stored.
   * @note Derived classes may implement this method.  The default method causes an assertion to fail.
   */

  void IpcAdapter::lookupNow(const StateKey& stateKey,
			     std::vector<double>& dest)
  {
    State state;
    assertTrueMsg(getState(stateKey, state),
		  "IpcAdapter::lookupNow: Internal error: state not found!");
    LabelStr nameLabel(state.first);
    const std::vector<double>& params = state.second;
    size_t nParams = params.size();
    debugMsg("IpcAdapter:lookupNow",
	     " for state " << nameLabel.toString()
	     << " with " << nParams << " parameters");

    // *** TODO: filter out command, message, lookup lookups here ***

    // Construct the messages
    // Leader
    uint32_t serial = getSerialNumber();
    struct PlexilStringValueMsg leader = {{PlexilMsgType_LookupNow,
					   nParams,
					   serial,
					   m_myUID.c_str()},
					  nameLabel.c_str()};
    // Construct parameter messages
    PlexilMsgBase* paramMsgs[nParams];
    for (size_t i = 0; i < nParams; i++)
      {
	double param = params[i];
	PlexilMsgBase* paramMsg;
	if (LabelStr::isString(param))
	  {
	    // string
	    struct PlexilStringValueMsg* strMsg = new PlexilStringValueMsg();
	    strMsg->stringValue = LabelStr(param).c_str();
	    paramMsg = (PlexilMsgBase*) strMsg;
	    paramMsg->msgType = PlexilMsgType_StringValue;
	  }
	else if (StoredArray::isKey(param))
	  {
	    // array
	    assertTrueMsg(ALWAYS_FAIL,
			  "IpcAdapter: Array values are not yet implemented");
	  }
	else
	  {
	    // number or Boolean
	    struct PlexilNumericValueMsg* numMsg = new PlexilNumericValueMsg();
	    numMsg->doubleValue = param;
	    paramMsg = (PlexilMsgBase*) numMsg;
	    paramMsg->msgType = PlexilMsgType_NumericValue;
	  }

	// Fill in common fields
	paramMsg->count = i;
	paramMsg->serial = serial;
	paramMsg->senderUID = m_myUID.c_str();
	paramMsgs[i] = paramMsg;
      }

    // Send the messages
    m_pendingLookupSerial = serial;
    m_pendingLookupDestination = &dest;
    // *** TODO: check for IPC errors ***
    IPC_publishData(STRING_VALUE_MSG, (void *) &leader);
    for (size_t i = 0; i < nParams; i++)
      {
	IPC_publishData(msgFormatForType((PlexilMsgType) paramMsgs[i]->msgType),
			paramMsgs[i]);
      }

    // Wait for results
    m_sem.wait();

    // Clean up
    m_pendingLookupSerial = 0;
    m_pendingLookupDestination = NULL;
  }


  /**
   * @brief Send the name of the supplied node, and the supplied value pairs, to the planner.
   * @param node The Node requesting the update.
   * @param valuePairs A map of <LabelStr key, value> pairs.
   * @param ack The expression in which to store an acknowledgement of completion.
   * @note Derived classes may implement this method.  The default method causes an assertion to fail.
   */

  void IpcAdapter::sendPlannerUpdate(const NodeId& node,
				     const std::map<double, double>& valuePairs,
				     ExpressionId ack)
  {
    assertTrueMsg(ALWAYS_FAIL,
		  "IpcAdapter: sendPlannerUpdate is not yet implemented");
  }

  /**
   * @brief Execute a command with the requested arguments.
   * @param name The LabelString representing the command name.
   * @param args The command arguments expressed as doubles.
   * @param dest The expression in which to store any value returned from the command.
   * @param ack The expression in which to store an acknowledgement of command transmission.
   */

  void IpcAdapter::executeCommand(const LabelStr& name,
				  const std::list<double>& args,
				  ExpressionId dest,
				  ExpressionId ack)
  {
    // Check for SendMessage command
    if (name == SEND_MESSAGE_COMMAND())
      {
	// Check for one argument, the message
	assertTrueMsg(args.size() == 1,
		      "IpcAdapter: The SendMessage command requires exactly one argument");
	assertTrueMsg(LabelStr::isString(args.front()),
		      "IpcAdapter: The argument to the SendMessage command, " << args.front()
		      << ", is not a string");
	LabelStr theMessage(args.front());
	struct PlexilStringValueMsg packet = {{PlexilMsgType_Message,
					       0,
					       getSerialNumber(),
					       m_myUID.c_str()},
					      theMessage.c_str()};
	IPC_publishData(STRING_VALUE_MSG, (void *) &packet);
	// store ack
	ack->setValue(CommandHandleVariable::COMMAND_SUCCESS().getKey());
      }
    else // general case
      {
	uint32_t serial = getSerialNumber();
	size_t nParams = args.size();
	struct PlexilStringValueMsg cmdPacket = {{PlexilMsgType_Command,
						  nParams,
						  serial,
						  m_myUID.c_str()},
						 name.c_str()};
	// Construct parameter messages
	PlexilMsgBase* paramMsgs[nParams];
	unsigned int i = 0;
	for (std::list<double>::const_iterator it = args.begin();
	     it != args.end();
	     it++, i++)
	  {
	    double param = *it;
	    PlexilMsgBase* paramMsg;
	    if (LabelStr::isString(param))
	      {
		// string
		struct PlexilStringValueMsg* strMsg = new PlexilStringValueMsg();
		strMsg->stringValue = LabelStr(param).c_str();
		paramMsg = (PlexilMsgBase*) strMsg;
		paramMsg->msgType = PlexilMsgType_StringValue;
	      }
	    else if (StoredArray::isKey(param))
	      {
		// array
		assertTrueMsg(ALWAYS_FAIL,
			      "IpcAdapter: Array values are not yet implemented");
	      }
	    else
	      {
		// number or Boolean
		struct PlexilNumericValueMsg* numMsg = new PlexilNumericValueMsg();
		numMsg->doubleValue = param;
		paramMsg = (PlexilMsgBase*) numMsg;
		paramMsg->msgType = PlexilMsgType_NumericValue;
	      }

	    // Fill in common fields
	    paramMsg->count = i;
	    paramMsg->serial = serial;
	    paramMsg->senderUID = m_myUID.c_str();
	    paramMsgs[i] = paramMsg;
	  }

	// Send the messages
	// *** TODO: check for IPC errors ***
	IPC_publishData(STRING_VALUE_MSG, (void *) &cmdPacket);
	for (size_t i = 0; i < nParams; i++)
	  {
	    IPC_publishData(msgFormatForType((PlexilMsgType) paramMsgs[i]->msgType),
			    paramMsgs[i]);
	  }

	// store ack
	ack->setValue(CommandHandleVariable::COMMAND_SENT_TO_SYSTEM().getKey());
      }
  }

  /**
   * @brief Abort the pending command with the supplied name and arguments.
   * @param name The LabelString representing the command name.
   * @param args The command arguments expressed as doubles.
   * @param ack The expression in which to store an acknowledgement of command abort.
   * @note Derived classes may implement this method.  The default method causes an assertion to fail.
   */

  void IpcAdapter::invokeAbort(const LabelStr& name, 
			       const std::list<double>& args, 
			       ExpressionId ack)
  {
    assertTrueMsg(ALWAYS_FAIL, "IpcAdapter::invokeAbort is not implemented");
  }

  //
  // Implementation methods
  //

  /**
   * @brief Handler function as seen by IPC.
   */

  void IpcAdapter::messageHandler(MSG_INSTANCE rawMsg,
				  void * unmarshalledMsg,
				  void * this_as_void_ptr)
  {
    IpcAdapter* theAdapter = reinterpret_cast<IpcAdapter*>(this_as_void_ptr);
    assertTrueMsg(theAdapter != NULL,
		  "IpcAdapter::messageHandler: pointer to adapter instance is null!");

    const PlexilMsgBase* msgData = reinterpret_cast<const PlexilMsgBase*>(unmarshalledMsg);
    assertTrueMsg(msgData != NULL,
		  "IpcAdapter::messageHandler: pointer to message data is null!");

    theAdapter->handleIpcMessage(msgData);
  }

  /**
   * @brief Handler function as seen by adapter.
   */

  void IpcAdapter::handleIpcMessage(const PlexilMsgBase * msgData)
  {
    if (strcmp(msgData->senderUID, m_myUID.c_str()) == 0)
      {
	debugMsg("IpcAdapter::handleIpcMessage", " ignoring my own outgoing message");
	return;
      }

    PlexilMsgType msgType = (PlexilMsgType) msgData->msgType;
    debugMsg("IpcAdapter::handleIpcMessage", " received message type = " << msgType);
    switch (msgType)
      {
	// NotifyExec is a PlexilMsgBase
      case PlexilMsgType_NotifyExec:

	// AddPlan/AddPlanFile is a PlexilStringValueMsg
      case PlexilMsgType_AddPlan:
      case PlexilMsgType_AddPlanFile:

 	// AddLibrary/AddLibraryFile is a PlexilStringValueMsg
      case PlexilMsgType_AddLibrary:
      case PlexilMsgType_AddLibraryFile:

	// In each case, simply send it to Exec and free the message
	enqueueMessage(msgData);
	break;

	// Command and Message are PlexilStringValueMsg
	// Optionally followed by parameters

	// *** TODO: filter out commands/msgs we aren't prepared to handle
      case PlexilMsgType_Command:
      case PlexilMsgType_Message:
	// Stash this and wait for the rest
	cacheMessageLeader(msgData);
	break;

	// LookupNow and LookupOnChange are PlexilStringValueMsg
	// Optionally followed by parameters

	// *** TODO: filter out commands/msgs we aren't prepared to handle
      case PlexilMsgType_LookupNow:
      case PlexilMsgType_LookupOnChange:
	// Stash this and wait for the rest
	cacheMessageLeader(msgData);
	break;

	// PlannerUpdate is a PlexilStringValueMsg
	// Followed by 0 (?) or more name/value pairs
      case PlexilMsgType_PlannerUpdate:
	// Stash this and wait for the rest
	cacheMessageLeader(msgData);
	break;

	// ReturnValues is a PlexilReturnValuesMsg
	// Followed by 0 (?) or more values
      case PlexilMsgType_ReturnValues:
	{
	  // Only pay attention to our return values
	  const PlexilReturnValuesMsg* returnLeader = (const PlexilReturnValuesMsg*) msgData;
	  if (strcmp(returnLeader->requesterUID, m_myUID.c_str()) == 0)
	    cacheMessageLeader(msgData);
	  break;
	}

	// Values - could be parameters or return values
      case PlexilMsgType_NumericValue:
      case PlexilMsgType_StringValue:

	// PlannerUpdate pairs
      case PlexilMsgType_PairNumeric:
      case PlexilMsgType_PairString:

	// Log with corresponding leader message
	cacheMessageTrailer(msgData);
	break;
			  
      default:
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::handleIpcMessage: received invalid message data type " << msgType);
	break;
      }

  }
 
  /**
   * @brief Cache start message of a multi-message sequence
   */

  // N.B. Presumes that messages are received in order.
  // Also presumes that any required filtering (e.g. on command name) has been done by the caller
    
  void IpcAdapter::cacheMessageLeader(const PlexilMsgBase* msgData)
  {
    IpcMessageId msgId(msgData->senderUID, msgData->serial);

    // Check that this isn't a duplicate header
    IncompleteMessageMap::iterator it = m_incompletes.find(msgId);
    assertTrueMsg(it == m_incompletes.end(),
		  "IpcAdapter::cacheMessageLeader: internal error: found existing sequence for sender "
		  << msgData->senderUID << ", serial " << msgData->serial);

    if (msgData->count == 0)
      {
	std::vector<const PlexilMsgBase*> msgVec(1, msgData);
	enqueueMessageSequence(msgVec);
      }
    else
      {
	m_incompletes[msgId] = std::vector<const PlexilMsgBase*>(msgData->count + 1);
	m_incompletes[msgId].push_back(msgData);
      }
  }


  /**
   * @brief Cache following message of a multi-message sequence
   */

  // N.B. Presumes that messages are received in order.
    
  void IpcAdapter::cacheMessageTrailer(const PlexilMsgBase* msgData)
  {
    IpcMessageId msgId(msgData->senderUID, msgData->serial);
    IncompleteMessageMap::iterator it = m_incompletes.find(msgId);
    assertTrueMsg(it == m_incompletes.end(),
		  "IpcAdapter::cacheMessageTrailer: no existing sequence for sender "
		  << msgData->senderUID << ", serial " << msgData->serial);
    std::vector<const PlexilMsgBase*>& msgs = (*it).second;
    msgs.push_back(msgData);
    // Have we got them all?
    if (msgs.size() > msgs[0]->count)
      {
	enqueueMessageSequence(msgs);
	m_incompletes.erase(it);
      }
  }


  /**
   * @brief Send a single message to the Exec's queue and free the message
   */
  void IpcAdapter::enqueueMessage(const PlexilMsgBase* msgData)
  {
    switch (msgData->msgType)
      {
      case PlexilMsgType_NotifyExec:
	getExecInterface().notifyOfExternalEvent();
	break;

	// AddPlan is a PlexilStringValueMsg
      case PlexilMsgType_AddPlan:
	// Always notify immediately when adding a plan
	getExecInterface().notifyOfExternalEvent();
	break;

	// AddPlanFile is a PlexilStringValueMsg
      case PlexilMsgType_AddPlanFile:
	// Always notify immediately when adding a plan
	getExecInterface().notifyOfExternalEvent();
	break;

 	// AddLibrary is a PlexilStringValueMsg
      case PlexilMsgType_AddLibrary:
	break;

 	// AddLibraryFile is a PlexilStringValueMsg
      case PlexilMsgType_AddLibraryFile:
	break;

      default:
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::enqueueMessage: invalid message type " << msgData->msgType);
      }

    // clean up
    IPC_freeData(IPC_msgFormatter(msgFormatForType((PlexilMsgType) msgData->msgType)),
		 (void*) msgData);
  }


  /**
   * @brief Send a message sequence to the Exec's queue and free the messages
   */
  void IpcAdapter::enqueueMessageSequence(std::vector<const PlexilMsgBase*>& msgs)
  {
    const PlexilMsgBase* leader = msgs[0];
    switch (leader->msgType)
      {

      case PlexilMsgType_Command:
	break;

      case PlexilMsgType_Message:
	break;

      case PlexilMsgType_LookupNow:
	break;

      case PlexilMsgType_LookupOnChange:
	break;

      case PlexilMsgType_ReturnValues:
	break;

      case PlexilMsgType_PlannerUpdate:
	break;

      default:
	assertTrueMsg(ALWAYS_FAIL,
		      "IpcAdapter::enqueueMessageSequence: invalid leader message type " << leader->msgType);
      }

    // clean up
    for (size_t i = 0; i < msgs.size(); i++)
      {
	const PlexilMsgBase* msg = msgs[i];
	msgs[i] = NULL;
	IPC_freeData(IPC_msgFormatter(msgFormatForType((PlexilMsgType) msg->msgType)),
		     (void *) msg);
      }
  }

  /**
   * @brief Process a ReturnValues message sequence
   */

  void IpcAdapter::handleReturnValuesSequence(std::vector<const PlexilMsgBase*>& msgs)
  {
    const PlexilReturnValuesMsg* leader = (const PlexilReturnValuesMsg*) msgs[0];
  }


  /**
   * @brief Get next serial number
   */
  uint32_t IpcAdapter::getSerialNumber()
  {
    m_serial++;
    // handle wraparound
    if (m_serial == 0)
      m_serial = 1;
    return m_serial;
  }


  /**
   * @brief Returns true if the string starts with the prefix, false otherwise.
   */
  bool IpcAdapter::hasPrefix(const std::string& s, const std::string& prefix)
  {
    if (s.size() < prefix.size())
      return false;
    return (0 == prefix.compare(0, prefix.size(), s));
  }


}
