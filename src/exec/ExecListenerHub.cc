/* Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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

#include "ExecListenerHub.hh"
#include "Debug.hh"

#include <algorithm> // for std::find

namespace PLEXIL
{
  ExecListenerHub::ExecListenerHub()
    : m_id(this)
  {
  }

  ExecListenerHub::~ExecListenerHub()
  {
    for (std::vector<ExecListenerBaseId>::iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it) {
      delete ((ExecListenerBase*) *it);
    }
    m_listeners.clear();
    m_id.remove();
  }

  /**
   * @brief Adds an Exec listener for publication of plan events.
   */
  void ExecListenerHub::addListener(const ExecListenerBaseId& listener)
  {
    check_error(listener.isValid());
    if (std::find(m_listeners.begin(), m_listeners.end(), listener) != m_listeners.end())
      return;
    m_listeners.push_back(listener);
  }

  /**
   * @brief Removes an Exec listener.
   */
  void ExecListenerHub::removeListener(const ExecListenerBaseId& listener)
  {
    std::vector<ExecListenerBaseId>::iterator it = 
      std::find(m_listeners.begin(), m_listeners.end(), listener);
    if (it != m_listeners.end())
      m_listeners.erase(it);
  }

  //
  // API to Exec
  //

  /**
   * @brief Notify that nodes have changed state.
   * @param Vector of node state transition info.
   * @note Current states are accessible via the node.
   */
  void ExecListenerHub::notifyOfTransitions(const std::vector<NodeTransition>& transitions) const
  {
    for (std::vector<ExecListenerBaseId>::const_iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it)
      (*it)->notifyOfTransitions(transitions);
  }

  /**
   * @brief Notify that a plan has been received by the Exec.
   * @param plan The intermediate representation of the plan.
   * @param parent The name of the parent node under which this plan will be inserted.
   */
  void ExecListenerHub::notifyOfAddPlan(const PlexilNodeId& plan, 
                                        const LabelStr& parent) const
  {
    for (std::vector<ExecListenerBaseId>::const_iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it)
      (*it)->notifyOfAddPlan(plan, parent);
  }

  /**
   * @brief Notify that a library node has been received by the Exec.
   * @param libNode The intermediate representation of the plan.
   */
  void ExecListenerHub::notifyOfAddLibrary(const PlexilNodeId& libNode) const
  {
    for (std::vector<ExecListenerBaseId>::const_iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it)
      (*it)->notifyOfAddLibrary(libNode);
  }

  /**
   * @brief Notify that a variable assignment has been performed.
   * @param dest The Expression being assigned to.
   * @param destName A string naming the destination.
   * @param value The value (in internal Exec representation) being assigned.
   */
  void ExecListenerHub::notifyOfAssignment(const ExpressionId & dest,
                                           const std::string& destName,
                                           const double& value) const
  {
    for (std::vector<ExecListenerBaseId>::const_iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it)
      (*it)->notifyOfAssignment(dest, destName, value);
  }

  //
  // API to InterfaceManager
  //

  /**
   * @brief Perform listener-specific initialization.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListenerHub::initialize()
  {
    bool success = true;
    for (std::vector<ExecListenerBaseId>::iterator it = m_listeners.begin();
         success && it != m_listeners.end();
         ++it) {
      ExecListenerBaseId l = *it;
      success = l->initialize();
      if (!success) {
        debugMsg("ExecListenerHub:initialize", " failed to initialize all Exec listeners, returning false");
        return false;
      }
    }
    return success;
  }

  /**
   * @brief Perform listener-specific startup.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListenerHub::start()
  {
    bool success = true;
    for (std::vector<ExecListenerBaseId>::iterator it = m_listeners.begin();
         success && it != m_listeners.end(); // stop at first failure
         ++it)
      success = (*it)->start();
    return success;
  }

  /**
   * @brief Perform listener-specific actions to stop.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListenerHub::stop()
  {
    bool success = true;
    for (std::vector<ExecListenerBaseId>::iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it)
      success = (*it)->stop() && success;
    return success;
  }

  /**
   * @brief Perform listener-specific actions to reset to initialized state.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListenerHub::reset()
  {
    bool success = true;
    for (std::vector<ExecListenerBaseId>::iterator it = m_listeners.begin();
         success && it != m_listeners.end();
         ++it)
      success = (*it)->reset();
    return success;
  }

  /**
   * @brief Perform listener-specific actions to shut down.
   * @return true if successful, false otherwise.
   * @note Default method provided as a convenience for backward compatibility.
   */
  bool ExecListenerHub::shutdown()
  {
    bool success = true;
    for (std::vector<ExecListenerBaseId>::iterator it = m_listeners.begin();
         it != m_listeners.end();
         ++it)
      success = (*it)->shutdown() && success;
    return success;
  }

}
