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

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class ArrayVariableDeclNode extends VariableDeclNode
{

	public ArrayVariableDeclNode(int ttype)
	{
		super(new CommonToken(ttype, "ARRAY_VARIABLE_DECLARATION"));
	}

	public boolean checkSelf(NodeContext context, CompilerState myState)
	{
		PlexilTreeNode typeNode = this.getChild(0);
		PlexilDataType type = PlexilDataType.findByName(typeNode.getText());
		// FIXME: any chance that type could be null??
		// TODO: any chance type may not be array compatible?

		PlexilTreeNode varNameNode = this.getChild(1);
		String varName = varNameNode.getText();
		// Check for name conflict (issues diagnostics on failure)
		boolean success = context.checkVariableName(varNameNode);

		// TODO: If supplied, check initial value for type conflict
		// TODO: add to context
		return success;
	}

	public void constructXML()
	{
		// TODO
	}

}