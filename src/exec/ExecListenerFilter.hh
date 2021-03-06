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

#ifndef _H_ExecListenerFilter
#define _H_ExecListenerFilter

#include "ExecDefs.hh"
#include "LabelStr.hh"
#include "pugixml.hpp"

namespace PLEXIL
{
  // Forward references
  class Node;
  typedef Id<Node> NodeId;
  class PlexilNode;
  typedef Id<PlexilNode> PlexilNodeId;
  class ExecListener;
  typedef Id<ExecListener> ExecListenerId;
  class ExecListenerFilter;
  typedef Id<ExecListenerFilter> ExecListenerFilterId;
  class Expression;
  typedef Id<Expression> ExpressionId;

  //* Abstract base class for defining transition event filters
  class ExecListenerFilter
  {
  public:
    /**
     * @brief Constructor.
     */
    ExecListenerFilter();

    /**
     * @brief Constructor from configuration XML.
     */
    ExecListenerFilter(const pugi::xml_node& xml);

    /**
     * @brief Destructor.
     */
    virtual ~ExecListenerFilter();

    /**
     * @brief Get the configuration XML of this instance.
     * @return A const reference to the XML element.
     */
    const pugi::xml_node& getXml() const
    {
      return m_xml;
    }

    /**
     * @brief Determine whether this node transition event should be reported.
     * @param prevState The LabelStr naming the node's previous state.
     * @param node Smart pointer to the node that changed state.
     * @return true to notify on this event, false to ignore it.
     * @note The default method simply returns true.
     */
    virtual bool reportNodeTransition(NodeState prevState, 
                                      const NodeId& node);

    /**
     * @brief Determine whether this AddPlan event should be reported.
     * @param plan Smart pointer to the plan's intermediate representation.
     * @param parent The LabelStr naming the new plan's parent node.
     * @return true to notify on this event, false to ignore it.
     * @note The default method simply returns true.
     */
    virtual bool reportAddPlan(const PlexilNodeId& plan,
			       const LabelStr& parent);

    /**
     * @brief Determine whether this AddLibraryNode event should be reported.
     * @param plan Smart pointer to the library's intermediate representation.
     * @return true to notify on this event, false to ignore it.
     * @note The default method simply returns true.
     */
    virtual bool reportAddLibrary(const PlexilNodeId& plan);

    /**
     * @brief Determine whether this variable assignment should be reported.
     * @param dest The Expression being assigned to.
     * @param destName A string naming the destination.
     * @param value The value (in internal Exec representation) being assigned.
     */
    virtual bool reportAssignment(const ExpressionId & dest,
                                  const std::string& destName,
                                  const double& value) const;

    inline const ExecListenerFilterId getId() const
    {
      return m_id;
    }

  private:
    //
    // Deliberately unimplemented
    //
    ExecListenerFilter(const ExecListenerFilter &);
    ExecListenerFilter& operator=(const ExecListenerFilter &);

    //
    // Member variables
    //
    ExecListenerFilterId m_id;

    /**
     * @brief The configuration XML used at construction time.
     */
    const pugi::xml_node m_xml;
  };

}

#endif // _H_ExecListenerFilter
