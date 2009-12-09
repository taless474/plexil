/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
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

//
// *** Ignore this file on systems that implement POSIX timers
//

#if !defined(_POSIX_TIMERS) || (_POSIX_TIMERS - 200112L) < 0L

#include "DarwinTimeAdapter.hh"
#include "AdapterExecInterface.hh"
#include "Debug.hh"
#include "Error.hh"
#include "StateCache.hh"
#include <cerrno>
#include <cmath> // for modf

namespace PLEXIL
{
  // Ugly singleton variable
  DarwinTimeAdapter* DarwinTimeAdapter::s_theInstance = NULL;

  /**
   * @brief Constructor.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   */
  DarwinTimeAdapter::DarwinTimeAdapter(AdapterExecInterface& execInterface)
    : InterfaceAdapter(execInterface)
  {
    commonInit();
  }

  /**
   * @brief Constructor from configuration XML.
   * @param execInterface Reference to the parent AdapterExecInterface object.
   * @param xml A const pointer to the TiXmlElement describing this adapter
   * @note The instance maintains a shared pointer to the TiXmlElement.
   */
  DarwinTimeAdapter::DarwinTimeAdapter(AdapterExecInterface& execInterface, 
				       const TiXmlElement * xml)
    : InterfaceAdapter(execInterface, xml)
  {
    commonInit();
  }

  /**
   * @brief Destructor.
   */
  DarwinTimeAdapter::~DarwinTimeAdapter()
  {
  }

  /**
   * @brief Initializes the adapter, possibly using its configuration data.
   * @return true if successful, false otherwise.
   */
  bool DarwinTimeAdapter::initialize()
  {
    // Automatically register self for time
    m_execInterface.registerLookupInterface(LabelStr("time"), getId());
    return true;
  }

  /**
   * @brief Starts the adapter, possibly using its configuration data.  
   * @return true if successful, false otherwise.
   */
  bool DarwinTimeAdapter::start()
  {
    return true;
  }

  /**
   * @brief Stops the adapter.  
   * @return true if successful, false otherwise.
   */
  bool DarwinTimeAdapter::stop()
  {
    // Disable the timer
    stopTimer();

    // Clear the map
    m_lookupToleranceMap.clear();

    return true;
  }

  /**
   * @brief Resets the adapter.  
   * @return true if successful, false otherwise.
   */
  bool DarwinTimeAdapter::reset()
  {
    return true;
  }

  /**
   * @brief Shuts down the adapter, releasing any of its resources.
   * @return true if successful, false otherwise.
   */
  bool DarwinTimeAdapter::shutdown()
  {
    // Restore previous SIGALRM handler
    int status = sigaction(SIGALRM, &m_oldsigaction, NULL);
    assertTrueMsg(status == 0,
                  "DarwinTimerAdapter::shutdown: sigaction failed, errno = " << errno);
    return true;
  }

  /**
   * @brief Register one LookupOnChange.
   * @param uniqueId The unique ID of this lookup.
   * @param stateKey The state key for this lookup.
   * @param tolerances A vector of tolerances for the LookupOnChange.
   */
  void DarwinTimeAdapter::registerChangeLookup(const LookupKey& uniqueId,
					       const StateKey& stateKey,
					       const std::vector<double>& tolerances)
  {
    assertTrueMsg(stateKey == m_execInterface.getStateCache()->getTimeStateKey(),
                  "DarwinTimeAdaptor only implements lookups for \"time\"");
    assertTrueMsg(tolerances.size() == 1,
                  "Wrong number of tolerances for LookupOnChange(\"time\")");
    assertTrueMsg(tolerances[0] > 0,
                  "LookupOnChange(\"time\") requires a positive tolerance");
    checkError(m_lookupToleranceMap.find(uniqueId) == m_lookupToleranceMap.end(),
               "Internal error: lookup key already in use!");

    // (Maybe) set up a timer to repeat at the specified tolerance
    timeval tolval;
    doubleToTimeval(tolerances[0], tolval);

    if (m_lookupToleranceMap.empty())
      {
	// If this is the first LookupOnChange, 
	// set up signal handler and start the timer
	int status = sigaction(SIGALRM, &m_sigaction, &m_oldsigaction);
	assertTrueMsg(status == 0,
		      "LookupOnChange: sigaction failed, errno = " << errno);
	m_lastItimerval.it_value = tolval;
	m_lastItimerval.it_interval = tolval;
	status = setitimer(ITIMER_REAL, &m_lastItimerval, NULL);
	assertTrueMsg(status == 0,
		      "LookupOnChange: setitimer failed, errno = " << errno);
      }
    else if (timevalLess(tolval, m_lastItimerval.it_interval))
      {
	// New tolerance is smaller than old - 
	// Get the current timer setting and update
	int status = getitimer(ITIMER_REAL, &m_lastItimerval);
	assertTrueMsg(status == 0,
		      "LookupOnChange: getitimer failed, errno = " << errno);
	m_lastItimerval.it_interval = tolval;
	// Only set it_value if remaining delay is greater than new
	if (timevalGreater(m_lastItimerval.it_value, tolval))
	  m_lastItimerval.it_value = tolval;
	status = setitimer(ITIMER_REAL, &m_lastItimerval, NULL);
	assertTrueMsg(status == 0,
		      "LookupOnChange: setitimer failed, errno = " << errno);
      }
    // else do nothing, smallest existing tolerance is smaller than new

    // Add it to the map
    m_lookupToleranceMap[uniqueId] = tolval;
  }

  /**
   * @brief Terminate one LookupOnChange.
   * @param uniqueId The unique ID of the lookup to be terminated.
   */
  void DarwinTimeAdapter::unregisterChangeLookup(const LookupKey& uniqueId)
  {
    LookupToleranceMap::iterator where = m_lookupToleranceMap.find(uniqueId);
    if (where == m_lookupToleranceMap.end())
      {
        return; // no such lookup, or already unregistered
      }

    // Delete the map entry
    m_lookupToleranceMap.erase(where);

    // If map is empty, stop itimer
    if (m_lookupToleranceMap.empty())
      {
	stopTimer();
      }
    // If not, find the remaining lookup with the smallest tolerance
    // and reset the timer if necessary
    else
      {
	where = m_lookupToleranceMap.begin();
	const timeval* tv = NULL;
	while (where != m_lookupToleranceMap.end())
	  {
	    if (tv == NULL || timevalLess(where->second, *tv))
	      tv = &(where->second);
	    where++;
	  }
	checkError(tv == NULL,
		   "LookupOnChange: Internal error: couldn't find smallest tolerance");
	// tv now points to timeval of smallest tolerance
	if (timevalGreater(*tv, m_lastItimerval.it_interval))
	  {
	    // Old interval was smaller - 
	    // set new longer interval, but preserve existing timer value
	    int status = getitimer(ITIMER_REAL, &m_lastItimerval);
	    assertTrueMsg(status == 0,
			  "LookupOnChange: getitimer failed, errno = " << errno);
	    m_lastItimerval.it_interval = *tv;
	    status = setitimer(ITIMER_REAL, &m_lastItimerval, NULL);
	    assertTrueMsg(status == 0,
			  "LookupOnChange: setitimer failed, errno = " << errno);
	  }
      }
  }

  /**
   * @brief Perform an immediate lookup of the requested state.
   * @param stateKey The state key for this lookup.
   * @param dest A (reference to a) vector of doubles where the result is to be stored.
   */
  void DarwinTimeAdapter::lookupNow(const StateKey& stateKey,
				    std::vector<double>& dest)
  {
    assertTrueMsg(stateKey == m_execInterface.getStateCache()->getTimeStateKey(),
                  "DarwinTimeAdaptor does not implement lookups for state \""
		  << m_execInterface.getStateCache()->stateNameForKey(stateKey) << "\"");
    dest.resize(1);
    dest[0] = getCurrentTime();
  }

  //
  // Static member functions
  //

  /**
   * @brief Get the current time from the operating system.
   * @return A double representing the current time.
   */
  double DarwinTimeAdapter::getCurrentTime()
  {
    timeval tv;
    int status = gettimeofday(&tv, NULL);
    assertTrueMsg(status == 0,
                  "lookupNow: gettimeofday() failed, errno = " << errno);
    double tym = timevalToDouble(tv);
    debugMsg("DarwinTimeAdapter:getCurrentTime", " returning" << tym);
    return tym;
  }

  /**
   * @brief Convert a timeval value into a double.
   * @param ts Reference to a constant timeval instance.
   * @return The timeval value converted to a double float.
   */
  double DarwinTimeAdapter::timevalToDouble(const timeval& ts)
  {
    return ((double) ts.tv_sec) +
      ((double) ts.tv_usec) / 1.0e6;
  }

  /**
   * @brief Convert a double value into a timeval.
   * @param tym The double to be converted.
   * @param result Reference to a writable timeval instance.
   */

  void DarwinTimeAdapter::doubleToTimeval(double tym, timeval& result)
  {
    double seconds = 0;
    double fraction = modf(tym, &seconds);

    result.tv_sec = (time_t) seconds;
    result.tv_usec = (long) (fraction * 1.0e6);
  }

  //
  // Internal member functions
  //

  /**
   * @brief Helper for constructor methods.
   */

  void DarwinTimeAdapter::commonInit()
  {
    // Set the singleton variable here
    s_theInstance = this;

    // Zero the last timer setting
    m_lastItimerval.it_value.tv_sec = 0;
    m_lastItimerval.it_value.tv_usec = 0;
    m_lastItimerval.it_interval.tv_sec = 0;
    m_lastItimerval.it_interval.tv_usec = 0;

    // Zero the timer-disable settings
    m_disableItimerval.it_value.tv_sec = 0;
    m_disableItimerval.it_value.tv_usec = 0;
    m_disableItimerval.it_interval.tv_sec = 0;
    m_disableItimerval.it_interval.tv_usec = 0;

    // Pre-fill sigaction fields
    m_sigaction.sa_flags = SA_RESTART ;
    m_sigaction.sa_handler = DarwinTimeAdapter::timerNotifyFunction;
    int status = sigprocmask(0, NULL, &m_sigaction.sa_mask);
    assertTrueMsg(status == 0,
		  "DarwinTimeAdapter: call to sigprocmask() failed, errno = " << errno);
  }


  /**
   * @brief Stop the timer.
   */
  void DarwinTimeAdapter::stopTimer()
  {
    int status = setitimer(ITIMER_REAL, &m_disableItimerval, NULL);
    assertTrueMsg(status == 0,
		  "DarwinTimeAdapter::stopTimer: call to setitimer() failed, errno = " << errno);

    // Restore previous SIGALRM handler
    status = sigaction(SIGALRM, &m_oldsigaction, NULL);
    assertTrueMsg(status == 0,
                  "DarwinTimerAdapter::shutdown: sigaction failed, errno = " << errno);
  }

  /**
   * @brief Static member function invoked upon receiving a timer signal
   * @param signo The signal number
   */
  void DarwinTimeAdapter::timerNotifyFunction(int signo)
  {
    // Simply invoke the timeout method
    theInstance()->timerTimeout();
  }

  /**
   * @brief Report the current time to the Exec as an asynchronous lookup value.
   */
  void DarwinTimeAdapter::timerTimeout()
  {
    // report the current time and kick-start the Exec
    std::vector<double> timeVector = std::vector<double>(1);
    timeVector[0] = getCurrentTime();
    m_execInterface.handleValueChange(m_execInterface.getStateCache()->getTimeStateKey(),
                                      timeVector);
    m_execInterface.notifyOfExternalEvent();
  }

}

#endif // !defined(_POSIX_TIMERS) || (_POSIX_TIMERS - 200112L) < 0L