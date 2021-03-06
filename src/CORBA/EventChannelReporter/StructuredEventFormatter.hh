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

#ifndef STRUCTURED_EVENT_FORMATTER_H
#define STRUCTURED_EVENT_FORMATTER_H

#include "EventFormatter.hh"
#include "orbsvcs/CosNotificationC.h"
#include <string>

namespace PLEXIL
{

  class StructuredEventFormatter : public EventFormatter
  {
  public:
    StructuredEventFormatter(const pugi::xml_node& xml);
    StructuredEventFormatter(const pugi::xml_node& xml, const std::string & domainName);
    virtual ~StructuredEventFormatter();

    virtual CORBA::Any_ptr formatTransition(NodeState prevState,
					    const NodeId& node) const;

    virtual CosNotification::StructuredEvent *
    formatTransitionStructured(NodeState prevState,
			       const NodeId& node) const;

    virtual CORBA::Any_ptr formatPlan(const PlexilNodeId& plan,
				      const LabelStr& parent) const;

    virtual CosNotification::StructuredEvent *
    formatPlanStructured(const PlexilNodeId& plan,
			 const LabelStr& parent) const;

  private:
    // deliberately unimplemented
    StructuredEventFormatter();
    StructuredEventFormatter(const StructuredEventFormatter &);
    StructuredEventFormatter& operator=(const StructuredEventFormatter &);

    // member variables
    std::string m_eventDomainName;

  };
}

#endif // STRUCTURED_EVENT_FORMATTER_H
