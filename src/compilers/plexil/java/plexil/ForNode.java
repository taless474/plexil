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

import java.util.Set;
import java.util.TreeSet;

import org.antlr.runtime.*;
import org.antlr.runtime.tree.*;

import net.n3.nanoxml.*;

public class ForNode extends PlexilTreeNode
{

	public ForNode(Token t)
	{
		super(t);
	}

	public ForNode(ForNode n)
	{
		super(n);
	}

	// format is:
	// ^(FOR_KYWD (VARIABLE_DECLARATION typeName NCNAME $loopvarinit) $endtest $loopvarupdate action)
	public boolean checkSelf(NodeContext context, CompilerState myState)
	{
		boolean success = true;
		PlexilTreeNode loopVarDecl = this.getChild(0);
		PlexilTreeNode typeNode = loopVarDecl.getChild(0);
		PlexilDataType loopVarType = PlexilDataType.findByName(typeNode.getText());
		PlexilTreeNode loopVarName = loopVarDecl.getChild(1);
		if (!loopVarType.isNumeric()) {
			myState.addDiagnostic(typeNode,
								  "\"for\" loop variable type is not numeric",
								  Severity.ERROR);
			success = false;
		}
		// other checks will be handled by VariableDeclNode via checkChildren()

		ExpressionNode whileTest = (ExpressionNode) this.getChild(1);
		if (whileTest.getDataType() != PlexilDataType.BOOLEAN_TYPE) {
			myState.addDiagnostic(whileTest,
								  "\"for\" loop test expression is not Boolean",
								  Severity.ERROR);
			success = false;
		}

		ExpressionNode loopVarUpdate = (ExpressionNode) this.getChild(2);
		if (!loopVarUpdate.getDataType().isNumeric()) {
			myState.addDiagnostic(typeNode,
								  "\"for\" loop variable update expression is not a numeric expression",
								  Severity.ERROR);
			success = false;
		}
		
		return success;
	}

	protected void constructXML()
	{
		super.constructXML();

		IXMLElement loopVar = new XMLElement("LoopVariable");
		m_xml.addChild(loopVar);
		loopVar.addChild(this.getChild(0).getXML());

		IXMLElement condition = new XMLElement("Condition");
		m_xml.addChild(condition);
		condition.addChild(this.getChild(1).getXML());

		IXMLElement loopVarUpdate = new XMLElement("LoopVariableUpdate");
		m_xml.addChild(loopVarUpdate);
		loopVarUpdate.addChild(this.getChild(2).getXML());

		IXMLElement action = new XMLElement("Action");
		m_xml.addChild(action);
		action.addChild(this.getChild(3).getXML());
	}

	protected String getXMLElementName() { return "For"; }

}
