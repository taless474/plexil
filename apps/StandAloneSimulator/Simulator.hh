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
#ifndef SIMULATOR_HH
#define SIMULATOR_HH

#include <map>
#include "TimingService.hh"
#include "SimulatorScriptReader.hh"

class ResponseMessageManager;
class ResponseMessage;
class ResponseFactory;
class CommRelayBase;

#define CONVERT_TIMESPEC_TO_DOUBLE(t) static_cast<double>(t.tv_sec) + \
                                      static_cast<double>(t.tv_usec) / 1000000.0


class Simulator
{
public:
  Simulator(ResponseFactory* respFactory, 
            CommRelayBase* commRelay);
  ~Simulator();

  ResponseMessageManager* getResponseMessageManager(const std::string& cmdName) const;
  void registerResponseMessageManager(ResponseMessageManager* msgMgr);
  void handleWakeUp();
  bool readScript(const std::string& fName);
  ResponseFactory* getResponseFactory() const {return m_ResponseFactory;}
  
  void scheduleResponseForCommand(const std::string& command, int uniqueId);
private:
  Simulator(){};
  void sendResponse(const ResponseMessage* respMsg);
  
  
  std::map<const std::string, ResponseMessageManager*> m_CmdToRespMgr;
  std::multimap<timeval, ResponseMessage*> m_TimeToResp;

  ResponseFactory* m_ResponseFactory;
  CommRelayBase* m_CommRelay;
  TimingService m_TimingService;
  SimulatorScriptReader m_SimulatorScriptReader;
  pthread_mutex_t m_TimerMutex;
};

#endif // SIMULATOR_HH
