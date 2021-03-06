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
#ifndef RESPONSE_BASE_HH
#define RESPONSE_BASE_HH

#include "ResponseMessageManager.hh"

#include <string>
#include <sys/time.h>

class ResponseMessage;

/**
 * @brief ResponseBase is an abstract base class which represents one event in a simulator script.
 */
class ResponseBase
{
public:
  ResponseBase() 
    : m_Manager(NULL) 
  {
  }

  virtual ~ResponseBase()
  {
  }

  void setManager(ResponseMessageManager* mgr)
  {
    m_Manager = mgr;
  }

  ResponseMessageManager* getManager() const
  {
    return m_Manager;
  }

  void notifyMessageSent()
  {
    if (m_Manager != NULL)
      m_Manager->notifyMessageSent(this);
  }

  void setNumberOfResponses(int numOfResp)
  {
    m_NumberOfResponses = numOfResp;
  }

  int getNumberOfResponses() const 
  {
    return m_NumberOfResponses;
  }

  const timeval& getDelay() const 
  {
    return m_Delay;
  }

  void setDelay(const timeval& delay) 
  {
    m_Delay = delay;
  }

  const std::string& getName() const 
  {
    return m_Manager->getIdentifier();
  }

private:
  // Deliberately not implemented
  ResponseBase(const ResponseBase&);
  ResponseBase& operator=(const ResponseBase&);

  ResponseMessageManager* m_Manager;
  timeval m_Delay;
  int m_NumberOfResponses;
};

#endif // RESPONSE_BASE_HH
