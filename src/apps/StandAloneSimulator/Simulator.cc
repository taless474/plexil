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

#include <iostream>

#include "Simulator.hh"
#include "timeval-utils.hh"
#include "ResponseBase.hh"
#include "ResponseMessage.hh"
#include "ResponseMessageManager.hh"
#include "CommRelayBase.hh"
#include "Debug.hh"

Simulator::Simulator(CommRelayBase* commRelay, ResponseManagerMap& map) : 
  m_CmdToRespMgr(map),
  m_CommRelay(commRelay),
  m_TimingService(this), 
  m_TimerScheduled(false),
  m_TimerMutex()
{
  m_CommRelay->registerSimulator(this);
}

Simulator::~Simulator()
{
  // Shut down the timing service
  m_TimingService.stopTimer();

  // delete all the response managers
  for (std::map<const std::string, ResponseMessageManager*>::iterator iter = m_CmdToRespMgr.begin();
       iter != m_CmdToRespMgr.end();
       ++iter)
    delete iter->second;

  debugMsg("Simulator:~Simulator",
	   " shutting down with " << m_TimeToResp.size() << " responses pending");
}

void Simulator::start()
{
  // Schedule all telemetry responses relative to this event
  ResponseManagerMap::const_iterator it = m_CmdToRespMgr.begin();
  while (it != m_CmdToRespMgr.end())
    {
      it->second->scheduleInitialEvents(this);
      it++;
    }
  
  // Set the timer for the first event, if any
  if (m_TimeToResp.begin() != m_TimeToResp.end())
    {
      timeval now;
      gettimeofday(&now, NULL);
      debugMsg("Simulator:start",
	       " at " << now.tv_sec << ", scheduling first event at "
	       << m_TimeToResp.begin()->first.tv_sec);
      scheduleNextResponse(m_TimeToResp.begin()->first);
    }
}

ResponseMessageManager* Simulator::getResponseMessageManager(const std::string& cmdName) const
{
  if (m_CmdToRespMgr.find(cmdName) != m_CmdToRespMgr.end())
    return m_CmdToRespMgr.find(cmdName)->second;

  return NULL;
}

void Simulator::scheduleResponseForCommand(const std::string& command,
                                           void* uniqueId)
{
  timeval time;
  debugMsg("Simulator:scheduleResponseForCommand",
	   " for : " << command);
  bool valid = constructNextResponse(command, uniqueId, time, MSG_COMMAND);
  if (valid)
    scheduleNextResponse(time);
  else
    debugMsg("Simulator:scheduleResponseForCommand",
	     "Not a command that needs a response.");
}

bool Simulator::constructNextResponse(const std::string& command,
                                      void* uniqueId, 
				      timeval& time,
                                      int type)
{
  ResponseManagerMap::const_iterator iter  = m_CmdToRespMgr.find(command);
  if (iter == m_CmdToRespMgr.end())
    {
      debugMsg("Simulator:constructNextResponse",
	       " No response manager for \"" << command << "\", ignoring.");
      return false;
    }
  ResponseMessageManager* msgMgr = iter->second;
  timeval tDelay;
  const ResponseBase* respBase = msgMgr->getResponses(tDelay);
  if (respBase == NULL) 
    {
      debugMsg("Simulator:constructNextResponse",
	       " No more responses for \"" << command << "\"");
      return false;
    }
  
  ResponseMessage* respMsg = new ResponseMessage(respBase, uniqueId, type);
  scheduleMessage(tDelay, respMsg);
  
  return true;
}

/**
 * @brief Get the current value of the named state.
 * @param stateName The state name to which we are responding.
 * @return Pointer to a const ResponseBase object, or NULL.
 */
ResponseMessage* Simulator::getLookupNowResponse(const std::string& stateName, void* uniqueId) const
{
  ResponseManagerMap::const_iterator it = m_CmdToRespMgr.find(stateName);
  if (it == m_CmdToRespMgr.end())
    return NULL; // Name not known
  const ResponseBase* response =  it->second->getLastResponse();
  if (response == NULL)
    return NULL; // Command name (not a state), or no "last" value established
  return new ResponseMessage(response, uniqueId, MSG_LOOKUP);
}

/**
 * @brief Schedules a message to be sent at a future time.
 * @param time The absolute time at which to send the message.
 * @param msg The message to be sent.
 */
void Simulator::scheduleMessage(const timeval& delay, ResponseMessage* msg)
{
  timeval currTime;
  gettimeofday(&currTime, NULL);
  timeval eventTime = currTime + delay;
  scheduleMessageAbsolute(eventTime, msg);
}

void Simulator::scheduleMessageAbsolute(const timeval& eventTime, ResponseMessage* msg)
{
  debugMsg("Simulator:scheduleMessage",
	   " scheduling message at " << eventTime.tv_sec);

  PLEXIL::ThreadMutexGuard mg(m_TimerMutex);
  m_TimeToResp.insert(std::pair<timeval, ResponseMessage*>(eventTime, msg));
}

/*
  By passing the time as argument we can avoid yet another mutex lock in this
  function.
 */
void Simulator::scheduleNextResponse(const timeval& time)
{
  bool immediateResp = false;

  if ((!m_TimerScheduled) || (time < m_TimerScheduledTime))
    {
      // Schedule timer
      debugMsg("Simulator:scheduleNextResponse", " Scheduling a timer");
      immediateResp = m_TimingService.setTimer(time);
      if (immediateResp)
	{
	  debugMsg("Simulator:scheduleNextResponse", " Immediate response required");
	  handleWakeUp();
	}
      else
	{
	  m_TimerScheduled = true;
	  m_TimerScheduledTime = time;
	  debugMsg("Simulator:scheduleNextResponse", " Setting timer for " << time.tv_sec);
	}
    }
  else
    {
      debugMsg("Simulator:scheduleNextResponse",
	       " A wakeup has already been scheduled for an earlier time.");
    }
}

void Simulator::handleWakeUp()
{
  debugMsg("Simulator:handleWakeUp", " entered");
  bool scheduleTimer=false;
  timeval time;
    
  {
    PLEXIL::ThreadMutexGuard mg(m_TimerMutex);
    int count = m_TimeToResp.count(m_TimeToResp.begin()->first);
    for (std::multimap<timeval, ResponseMessage*>::iterator iter = m_TimeToResp.begin();
	 count > 0; ++iter, --count)
      {
	ResponseMessage* resp = iter->second;
	m_CommRelay->sendResponse(resp);
	debugMsg("Simulator:handleWakeUp", " Sent response");
	// delete resp; // handled by comm relay
	m_TimeToResp.erase(iter);
      }
  }
    
  m_TimerScheduled = false;
  if (m_TimeToResp.size() > 0)
    {
      std::multimap<timeval, ResponseMessage*>::iterator iter = m_TimeToResp.begin();
      time = iter->first;
      scheduleTimer = true;
    }

  if (scheduleTimer) 
    {
      debugMsg("Simulator:handleWakeUp", " Scheduling next wakeup at " << time.tv_sec);
      scheduleNextResponse(time);
    }
  debugMsg("Simulator:handleWakeUp", " completed");
}
