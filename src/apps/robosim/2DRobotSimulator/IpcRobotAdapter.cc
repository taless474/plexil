/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

#include "IpcRobotAdapter.hh"
#include "RobotBase.hh"

#include "LabelStr.hh"
#include "StoredArray.hh"
#include "Debug.hh"
#include "Error.hh"
#include "ThreadSpawn.hh"


// ooid classes
#include "uuid.h"
#include "devrand.h"

#include <sys/time.h>

/**
 * @brief Constructor. Opens the connection and spawns a listener thread.
 */
IpcRobotAdapter::IpcRobotAdapter(const std::string& centralhost)
  : m_robots(),
    m_stateUIDMap(),
    m_ipcFacade(),
    m_listener(*this)
{
  assertTrueMsg(m_ipcFacade.initialize(m_ipcFacade.getUID().c_str(), centralhost.c_str()) == IPC_OK,
      "IpcRobotAdapter: Unable to initialize ipc to central server at " << centralhost);

  // Spawn listener thread
  assertTrueMsg(m_ipcFacade.start() == IPC_OK,
		"IpcRobotAdapter constructor: Unable to start IPC dispatch thread");

  // Subscribe only to messages we care about
  m_ipcFacade.subscribe(&m_listener, PlexilMsgType_Command);
  m_ipcFacade.subscribe(&m_listener, PlexilMsgType_LookupNow);
  debugMsg("IpcRobotAdapter:IpcRobotAdapter", " succeeded");
}

/**
 * @brief Destructor. Shuts down the listener thread and closes the connection.
 */
IpcRobotAdapter::~IpcRobotAdapter()
{
}

/**
 * @brief Make the adapter aware of the robot.
 */
void IpcRobotAdapter::registerRobot(const std::string& name, 
				    RobotBase* robot)
{
  NameToRobotMap::const_iterator it = m_robots.find(name);
  assertTrueMsg(it == m_robots.end(),
		"Robot name conflict for \"" << name << "\"");
  m_robots[name] = robot;
}
/**
 * @brief Send a command to RoboSim
 */

// N.B. RoboSim commands take one argument, the robot name.
// Additional arguments are ignored.
void IpcRobotAdapter::processCommand(const std::vector<const PlexilMsgBase*>& msgs)
{
  const std::string cmdName(((const PlexilStringValueMsg*)msgs[0])->stringValue);
  assertTrueMsg(msgs[0]->count >= 1,
		"IpcRobotAdapter::processCommand: robot name argument missing for command \"" << cmdName << "\"");
  assertTrueMsg(msgs[1]->msgType == PlexilMsgType_StringValue,
		"IpcRobotAdapter::processCommand: robot name argument for command \"" << cmdName << "\" is not a string");
  checkError(msgs.size() >= 2,
	     "IpcRobotAdapter::processCommand: internal error: not enough arguments to \""
	     << cmdName << "\" command");
  const std::string robotName(((const PlexilStringValueMsg*)msgs[1])->stringValue);
  NameToRobotMap::const_iterator it = m_robots.find(robotName);
  assertTrueMsg(it != m_robots.end(),
		"IpcRobotAdapter::processCommand: no robot named \"" << robotName << "\"");
  RobotBase* robot = it->second;
  assertTrueMsg(robot != NULL,
		"IpcRobotAdapter::processCommand: robot named \"" << robotName << "\" is null!");
  IpcMessageId transId = IpcMessageId(msgs[0]->senderUID, msgs[0]->serial);
  double parameter = 0.0;
  // Check for missing parameter
  if (cmdName == "Move")
    {
      assertTrueMsg(msgs[0]->count >= 2,
		    "IpcRobotAdapter::processCommand: missing required direction argument to \""
		    << cmdName << "\" command");
      checkError(msgs.size() >= 3,
		 "IpcRobotAdapter::processCommand: internal error: not enough arguments to \""
		 << cmdName << "\" command");
      assertTrueMsg(msgs[2]->msgType == PlexilMsgType_NumericValue,
		    "IpcRobotAdapter::processCommand: direction argument for command \"" << cmdName << "\" is not a number");
      parameter = ((const PlexilNumericValueMsg*)msgs[2])->doubleValue;
      condDebugMsg(msgs[0]->count > 2,
		   "IpcRobotAdapter:processCommand",
		   "Ignoring " << msgs[0]->count - 2 << " argument(s)");
    }
  else
    {
      condDebugMsg(msgs[0]->count > 1,
		   "IpcRobotAdapter:processCommand",
		   "Ignoring " << msgs[0]->count - 1 << " argument(s)");
    }
  const std::vector<double>& ret_values = robot->processCommand(cmdName, parameter);
  if (ret_values.size() > 1)
    m_ipcFacade.publishReturnValues(transId.second, PLEXIL::LabelStr(transId.first), PLEXIL::StoredArray(ret_values).getKey());
  else
    m_ipcFacade.publishReturnValues(transId.second, PLEXIL::LabelStr(transId.first), ret_values.at(0));
}

/**
 * @brief Deal with a LookupNow request
 */

// N.B. RoboSim does not implement LookupNow
void IpcRobotAdapter::processLookupNow(const std::vector<const PlexilMsgBase*>& msgs)
{
  std::string stateName(((const PlexilStringValueMsg*)msgs[0])->stringValue);
  debugMsg("IpcRobotAdapter:lookupNow", " ignoring lookup request for " << stateName);
}


IpcRobotAdapter::MessageListener::MessageListener(IpcRobotAdapter& adapter) :
  m_adapter(adapter) {
}
IpcRobotAdapter::MessageListener::~MessageListener() {
}
void IpcRobotAdapter::MessageListener::ReceiveMessage(const std::vector<const PlexilMsgBase*>& msgs) {
  const PlexilMsgBase* leader = msgs[0];
  switch (leader->msgType)
    {
    case PlexilMsgType_Command:
      m_adapter.processCommand(msgs);
      break;

    case PlexilMsgType_LookupNow:
      m_adapter.processLookupNow(msgs);
      break;

    default:
      assertTrueMsg(ALWAYS_FAIL,
            "IpcRobotAdapter::ReceiveMessage: invalid leader message type " << leader->msgType);
    }
}
