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

#ifndef EXEC_LISTENER_BASE_HH
#define EXEC_LISTENER_BASE_HH

#include "ExecDefs.hh"
#include "LabelStr.hh"
#include "pugixml.hpp"

namespace PLEXIL
{

  // Forward references
  class PlexilNode;
  typedef Id<PlexilNode> PlexilNodeId;
  class ExecListenerBase;
  typedef Id<ExecListenerBase> ExecListenerBaseId;

  /**
   * @brief An abstract base class for notifying the outside world of plan events.
   */
  class ExecListenerBase
  {
  public: 

    /**
     * @brief Default constructor.
     */
	ExecListenerBase()
	  : m_xml(),
		m_baseId(this)
	{
	}

    /**
     * @brief Constructor from configuration XML
     * @param xml Reference to the (shared) configuration XML describing this listener.
     */
	ExecListenerBase(const pugi::xml_node& xml)
	  : m_xml(xml),
		m_baseId(this)
	{
	}

	virtual ~ExecListenerBase()
	{
	  m_baseId.remove();
	}

	const ExecListenerBaseId& getId() const 
	{
	  return m_baseId;
	}

	const pugi::xml_node& getXml() const
	{
	  return m_xml;
	}

	//
	// API to Exec
	//

	/**
	 * @brief Notify that nodes have changed state.
	 * @param Vector of node state transition info.
	 * @note Current states are accessible via the node.
	 */
	virtual void notifyOfTransitions(const std::vector<NodeTransition>& transitions) const = 0;

    /**
     * @brief Notify that a plan has been received by the Exec.
     * @param plan The intermediate representation of the plan.
     * @param parent The name of the parent node under which this plan will be inserted.
     */
    virtual void notifyOfAddPlan(const PlexilNodeId& plan, 
                                 const LabelStr& parent) const = 0;

    /**
     * @brief Notify that a library node has been received by the Exec.
     * @param libNode The intermediate representation of the plan.
     */
    virtual void notifyOfAddLibrary(const PlexilNodeId& libNode) const = 0;

    //not sure if anybody wants this
    //virtual void notifyOfConditionChange(const NodeId& node,
    //                                     const LabelStr& condition,
    //                                     const bool value) const;

    /**
     * @brief Notify that a variable assignment has been performed.
     * @param dest The Expression being assigned to.
     * @param destName A string naming the destination.
     * @param value The value (in internal Exec representation) being assigned.
     */
    virtual void notifyOfAssignment(const ExpressionId & dest,
                                    const std::string& destName,
                                    const double& value) const = 0;

	//
	// Interface management API
	//

    /**
     * @brief Perform listener-specific initialization.
     * @return true if successful, false otherwise.
     */
    virtual bool initialize() = 0;

    /**
     * @brief Perform listener-specific startup.
     * @return true if successful, false otherwise.
     */
    virtual bool start() = 0;

    /**
     * @brief Perform listener-specific actions to stop.
     * @return true if successful, false otherwise.
     */
    virtual bool stop() = 0;

    /**
     * @brief Perform listener-specific actions to reset to initialized state.
     * @return true if successful, false otherwise.
     */
    virtual bool reset() = 0;

    /**
     * @brief Perform listener-specific actions to shut down.
     * @return true if successful, false otherwise.
     */
    virtual bool shutdown() = 0;

  protected:
	const pugi::xml_node m_xml;

  private:
	ExecListenerBaseId m_baseId;
  };

}

#endif // EXEC_LISTENER_BASE_HH
