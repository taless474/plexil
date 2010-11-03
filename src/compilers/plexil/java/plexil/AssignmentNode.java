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

public class AssignmentNode extends PlexilTreeNode
{
	public AssignmentNode(int ttype)
	{
		super(new CommonToken(ttype, "ASSIGNMENT"));
	}

	public boolean checkSelf(NodeContext context, CompilerState myState)
	{
		boolean success = true;
		VariableNode lhs = (VariableNode) this.getChild(0);
		if (lhs.passedCheck()) {
			if (!lhs.isAssignable()) {
				PlexilTreeNode varDecl = lhs.getDeclaration();
				if (varDecl != null) {
					myState.addDiagnostic(lhs,
										  "Variable \"" + lhs.getText() + "\" is declared In",
										  Severity.ERROR);
					myState.addDiagnostic(varDecl,
										  "Variable \"" + lhs.getText() + "\" declared In here",
										  Severity.NOTE);
				}
				success = false;
			}
		}


		ExpressionNode rhs = (ExpressionNode) this.getChild(1);
		if (rhs.passedCheck()) {
			PlexilDataType rhsType = rhs.getDataType(); // is VOID_TYPE if RHS failed check or is command w/ no return value
			if (rhsType == PlexilDataType.VOID_TYPE) {
				myState.addDiagnostic(rhs,
									  "Expression or command has no return value",
									  Severity.ERROR);
				success = false;
			}
			else if (lhs.passedCheck()) {
				PlexilDataType lhsType = lhs.getDataType();
				if (lhsType.isNumeric() && rhsType.isNumeric()) {
					// it's all good
				}
				else if (lhsType != rhsType) {
					myState.addDiagnostic(rhs,
										  "Cannot assign expression of type " + rhsType.typeName()
										  + " to variable \"" + lhs.getText() 
										  + "\" of type " + lhsType.typeName(),
										  Severity.ERROR);
					return false;
				}
			}
		}

		return success;
	}

	protected void constructXML()
	{
		PlexilTreeNode lhs = this.getChild(0);
		ExpressionNode rhs = (ExpressionNode) this.getChild(1);
		if (rhs.getType() == PlexilLexer.COMMAND) {
			// This is really a Command node, 
			// so insert LHS into RHS's XML in the appropriate place
			m_xml = rhs.getXML();
			IXMLElement body = m_xml.getChildAtIndex(0);
			XMLElement command = (XMLElement) body.getChildAtIndex(0);
			// Find Name element and insert LHS in front of it
			for (int i = 0; i < command.getChildrenCount(); i++) {
				IXMLElement child = command.getChildAtIndex(i);
				if (child.getName().equals("Name")) {
					command.insertChild(lhs.getXML(), i);
					break;
				}
			}
		}
		else {
			super.constructXML();
			m_xml.setAttribute("NodeType", "Assignment");
			IXMLElement body = new XMLElement("NodeBody");
			m_xml.addChild(body);
			IXMLElement assign = new XMLElement("Assignment");
			body.addChild(assign);
			assign.addChild(lhs.getXML());
			IXMLElement rhsXML = new XMLElement(rhs.assignmentRHSElementName());
			assign.addChild(rhsXML);
			rhsXML.addChild(rhs.getXML());
		}
		// TODO: In either case, set source locators from LHS
	}

	protected String getXMLElementName() { return "Node"; }

}