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

#ifndef PLEXIL_UPDATE_HH
#define PLEXIL_UPDATE_HH

#include "ExecDefs.hh"


namespace PLEXIL
{
  // Forward declarations in PLEXIL namespace
  class PlexilUpdate;
  typedef Id<PlexilUpdate> PlexilUpdateId;

  class Update 
  {
  public:
    Update(const NodeId& node, const PlexilUpdateId& updateProto = PlexilUpdateId::noId());
    ~Update();

    const UpdateId& getId() const {return m_id;}
    const VariableId& getAck() const {return m_ack;}
    const std::map<double, double>& getPairs() const {return m_valuePairs;}
    const NodeId& getSource() const {return m_source;}
    void activate();
    void deactivate();
    void reset();

  protected:
    friend class UpdateNode;
    void fixValues();

  private:
    // Deliberately unimplemented
    Update();
    Update(const Update&);
    Update& operator=(const Update&);

    UpdateId m_id;
    NodeId m_source;
    ExpressionMap m_pairs;
    VariableId m_ack;
    std::vector<ExpressionId> m_garbage;
    std::map<double, double> m_valuePairs;
  };

}

#endif // PLEXIL_UPDATE_HH
