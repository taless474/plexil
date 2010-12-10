// Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

import java.util.Vector;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class ReturnSpecNode extends PlexilTreeNode
{
	private Vector<VariableName> m_returnSpecs = new Vector<VariableName>();

	public ReturnSpecNode(Token t)
	{
		super(t);
	}

	public Vector<VariableName> getReturnVector() { return m_returnSpecs; }

	public void earlyCheck(NodeContext context, CompilerState state)
	{
		for (int retnIdx = 0; retnIdx < this.getChildCount(); retnIdx++) {
			PlexilTreeNode retn = this.getChild(retnIdx);
			String typeName = retn.getToken().getText();
			String nam = "_return_" + retnIdx;
			m_returnSpecs.add(new VariableName(retn,
											   nam,
											   PlexilDataType.findByName(typeName)));
		}
	}

	public void constructReturnXML(IXMLElement parent)
	{
		for (VariableName vn : m_returnSpecs)
			parent.addChild(vn.makeGlobalDeclarationElement("Return"));
	}

}