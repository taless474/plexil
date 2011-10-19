// Copyright (c) 2006-2011, Universities Space Research Association (USRA).
//  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//    * Neither the name of the Universities Space Research Association nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
// OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

package plexil;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;
import net.n3.nanoxml.*;

public class NodeVariableNode extends ExpressionNode
{
    private void setType ()
    {
        String type = this.getToken().getText();

        // ** KMD: obviate the magic strings!
        if (type.contentEquals ("command_handle")) {
            m_dataType = PlexilDataType.COMMAND_HANDLE_TYPE;
        }
        else if (type.contentEquals ("outcome")) {
            m_dataType = PlexilDataType.NODE_OUTCOME_TYPE;
        }
        else if (type.contentEquals ("state")) {
            m_dataType = PlexilDataType.NODE_STATE_TYPE;
        }
        else if (type.contentEquals ("failure")) {
            m_dataType = PlexilDataType.NODE_FAILURE_TYPE;
        }
        // * KMD: good enough?
        else m_dataType = PlexilDataType.ERROR_TYPE;
    }

    public NodeVariableNode(Token t)
    {
        super(t);
        setType();
    }

    public NodeVariableNode(NodeVariableNode n)
    {
        super(n);
        setType();
    }

    // TBD
    //    public void check(NodeContext context, CompilerState myState)
    //    {
    //        super.check (context, myState);
    //    }

    public void constructXML()
    {
        // NOTE: not invoking super, because it adds line/col attributes that I
        // want to omit for now, becuase it's messy to add them to the schema
        // for this construct.
        m_xml = new XMLElement(this.getXMLElementName());

        IXMLElement id = new XMLElement ("NodeId");
        id.setContent (this.getChild(0).getText());
        m_xml.addChild (id);
    }

    public String getXMLElementName()
    {
        if (m_dataType == PlexilDataType.NODE_STATE_TYPE) {
            return "NodeStateVariable";
        }
        else if (m_dataType == PlexilDataType.NODE_OUTCOME_TYPE) {
            return "NodeOutcomeVariable";
        }
        else if (m_dataType == PlexilDataType.COMMAND_HANDLE_TYPE) {
            return "NodeCommandHandleVariable";
        }
        else if (m_dataType == PlexilDataType.NODE_FAILURE_TYPE) {
            return "NodeFailureVariable";
        }
        // * KMD: do better here?
        else return "ERROR";
    }

}