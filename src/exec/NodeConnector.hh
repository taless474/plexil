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

#ifndef NODE_CONNECTOR_HH
#define NODE_CONNECTOR_HH

#include "ExecDefs.hh"
#include "LabelStr.hh"
#include "PlexilPlan.hh"

namespace PLEXIL
{

  class NodeConnector 
  {
  public:
    NodeConnector() : m_id(this) {}
    virtual ~NodeConnector() {m_id.remove();}
	
    const NodeConnectorId& getId() const {return m_id;}

    virtual const VariableId& findVariable(const PlexilVarRef* ref) = 0;
    virtual const VariableId& findVariable(const LabelStr& name, bool recursive = false) = 0;
    virtual const NodeId& getNode() const = 0;
    virtual const ExecConnectorId& getExec() const = 0;
    virtual const ExecListenerHubId& getExecListenerHub() const = 0;

  private:
    NodeConnectorId m_id;
  };

} // namespace PLEXIL

#endif // NODE_CONNECTOR_HH