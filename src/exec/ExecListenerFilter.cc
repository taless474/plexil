/* Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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

#include "ExecListenerFilter.hh"
#include "Debug.hh"
#include "Expression.hh"

namespace PLEXIL
{
  /**
   * @brief Constructor.
   */
  ExecListenerFilter::ExecListenerFilter()
    : m_id(this),
	  m_xml()
  {
  }

  /**
   * @brief Constructor from configuration XML.
   */
  ExecListenerFilter::ExecListenerFilter(const pugi::xml_node& xml)
    : m_id(this),
	  m_xml(xml)
  {
  }

  /**
   * @brief Destructor.
   */
  ExecListenerFilter::~ExecListenerFilter()
  {
    m_id.remove();
  }

  /**
   * @brief Determine whether this node transition event should be reported.
   * @param prevState The node's previous state.
   * @param node Smart pointer to the node that changed state.
   * @return true to notify on this event, false to ignore it.
   * @note The default method simply returns true.
   */
  bool 
  ExecListenerFilter::reportNodeTransition(NodeState /* prevState */, 
                                           const NodeId& /* node */)
  {
    return true;
  }

  /**
   * @brief Determine whether this AddPlan event should be reported.
   * @param plan Smart pointer to the plan's intermediate representation.
   * @param parent The LabelStr naming the new plan's parent node.
   * @return true to notify on this event, false to ignore it.
   * @note The default method simply returns true.
   */
  bool
  ExecListenerFilter::reportAddPlan(const PlexilNodeId& /* plan */,
                                    const LabelStr& /* parent */)
  {
    return true;
  }

  /**
   * @brief Determine whether this AddLibraryNode event should be reported.
   * @param plan Smart pointer to the library's intermediate representation.
   * @return true to notify on this event, false to ignore it.
   * @note The default method simply returns true.
   */
  bool 
  ExecListenerFilter::reportAddLibrary(const PlexilNodeId& /* plan */)
  {
    return true;
  }

  /**
   * @brief Determine whether this variable assignment should be reported.
   * @param dest The Expression being assigned to.
   * @param destName A LabelStr that names the destination.
   * @param value The value (in internal Exec representation) being assigned.
   */
  bool 
  ExecListenerFilter::reportAssignment(const ExpressionId & /* dest */,
                                       const std::string& /* destName */,
                                       const double& /* value */) const
  {
    return true;
  }

}
