// Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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

public class ArithmeticOperatorNode extends ExpressionNode
{
    public ArithmeticOperatorNode(ArithmeticOperatorNode a)
    {
        super(a);
    }

    public ArithmeticOperatorNode(Token t)
    {
        super(t);
    }

    public ArithmeticOperatorNode(int ttype)
    {
        super(new CommonToken(ttype, getTokenString(ttype)));
    }

	public Tree dupNode()
	{
		return new ArithmeticOperatorNode(this);
	}

    private static String getTokenString(int ttype)
    {
        switch (ttype) {
        case PlexilLexer.ABS_KYWD:
            return "ABS";
        default:
            return "UNKNOWN_ARITH_OP_" + String.valueOf(ttype);
        }
    }

    private void invalidateAux (CompilerState state,
                                ExpressionNode operand,
                                String s1, String s2)
    {
        state.addDiagnostic (operand, s1 + this.getToken().getText() +
                             " operator " + s2, Severity.ERROR);
        m_dataType = PlexilDataType.VOID_TYPE;
    }

    private void invalidateFirstOperand (CompilerState state,
                                         ExpressionNode operand,
                                         String desc)
    {
        invalidateAux (state, operand, "The first operand to the ", desc);
    }

    private void invalidateOnlyOperand (CompilerState state,
                                        ExpressionNode operand,
                                        String desc)
    {
        // TODO: improve message for things like absolute value
        invalidateAux (state, operand, "The operand to the ", desc);
    }

    private void invalidateOperands (CompilerState state,
                                     ExpressionNode operand,
                                     String desc)
    {
        invalidateAux (state, operand, "The operands to the ", desc);
    }


    // First pass of type checking: make implicit type assignments
    //
    public void earlyCheck (NodeContext context, CompilerState state)
    {
        earlyCheckChildren (context, state);

        switch (this.getType()) {
        case PlexilLexer.PLUS: m_dataType = earlyCheckPlus (state); break;
        case PlexilLexer.MINUS: m_dataType = earlyCheckMinus (state); break;
        case PlexilLexer.ASTERISK: m_dataType = earlyCheckMult (state); break;
        case PlexilLexer.SLASH: m_dataType = earlyCheckDiv (state); break;
        case PlexilLexer.ABS_KYWD: m_dataType = earlyCheckAbs (state); break;
        case PlexilLexer.MOD_KYWD:
        case PlexilLexer.PERCENT: m_dataType = earlyCheckMod (state); break;
        case PlexilLexer.SQRT_KYWD: m_dataType = PlexilDataType.REAL_TYPE; break; 
        default: {
            state.addDiagnostic(this,
                                "Shouldn't happen! Unknown expression type.",
                                Severity.ERROR);
            m_dataType = PlexilDataType.ERROR_TYPE;
        }
        }
    }


    private PlexilDataType earlyCheckPlus (CompilerState state)
    {
        // PLUS has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && rtype.isNumeric()) return rtype;

        // They can be certain combinations of duration and date...

        else if (ltype == PlexilDataType.DATE_TYPE &&
                 rtype == PlexilDataType.DURATION_TYPE) return ltype;
        else if (ltype == PlexilDataType.DURATION_TYPE &&
                 rtype == PlexilDataType.DATE_TYPE) return rtype;
        else if (ltype == PlexilDataType.DURATION_TYPE &&
                 rtype == PlexilDataType.DURATION_TYPE) return rtype;

        // They can be both be strings...

        else if (ltype == PlexilDataType.STRING_TYPE &&
                 rtype == PlexilDataType.STRING_TYPE)  return ltype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "PLUS given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }


    private PlexilDataType earlyCheckMinus (CompilerState state)
    {
        // MINUS has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && rtype.isNumeric()) return rtype;

        // They can be certain combinations of duration and date...

        else if (ltype == PlexilDataType.DATE_TYPE &&
                 rtype == PlexilDataType.DURATION_TYPE) return ltype;
        else if (ltype == PlexilDataType.DATE_TYPE &&
                 rtype == ltype) return PlexilDataType.DURATION_TYPE;
        else if (ltype == PlexilDataType.DURATION_TYPE &&
                 rtype == ltype) return rtype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "MINUS given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }


    private PlexilDataType earlyCheckMult (CompilerState state)
    {
        // MULT has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && rtype.isNumeric()) return rtype;

        // They can be certain combinations of duration and number...

        else if (ltype == PlexilDataType.DURATION_TYPE && rtype.isNumeric()) return ltype;
        else if (ltype.isNumeric() && rtype == PlexilDataType.DURATION_TYPE) return rtype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "Operator '*' given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }


    private PlexilDataType earlyCheckDiv (CompilerState state)
    {
        // DIV has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can both be numbers...

        if (ltype.isNumeric() && rtype.isNumeric()) return PlexilDataType.REAL_TYPE;

        // They can be certain combinations of duration and number...

        else if (ltype == PlexilDataType.DURATION_TYPE && rtype.isNumeric()) return ltype;
        else if (ltype == PlexilDataType.DURATION_TYPE && rtype == ltype) return ltype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "Operator '/' given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }


    private PlexilDataType earlyCheckMod (CompilerState state)
    {
        // MOD has exactly two children

        PlexilDataType ltype = ((ExpressionNode) this.getChild(0)).getDataType();
        PlexilDataType rtype = ((ExpressionNode) this.getChild(1)).getDataType();

        // They can both be numbers...

        if (ltype == PlexilDataType.INTEGER_TYPE && rtype == ltype) return ltype;
        else if (ltype.isNumeric() && ltype.isNumeric()) return PlexilDataType.REAL_TYPE;

        // They can be certain combinations of duration and number...

        else if (ltype == PlexilDataType.DURATION_TYPE && rtype.isNumeric()) return ltype;
        else if (ltype == PlexilDataType.DURATION_TYPE && rtype == ltype) return ltype;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "Modulo operator given invalid argument pair: " +
                                 ltype.toString() + ", " + rtype.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }

    private PlexilDataType earlyCheckAbs (CompilerState state)
    {
        // ABS has exactly one child

        PlexilDataType type = ((ExpressionNode) this.getChild(0)).getDataType();

        // It can be a number or duration

        if (type == PlexilDataType.DURATION_TYPE || type.isNumeric()) return type;

        // Otherwise, no good...

        else {
            state.addDiagnostic (this,
                                 "ABS operator given invalid argument: " +
                                 type.toString(),
                                 Severity.ERROR);
            return PlexilDataType.ERROR_TYPE;
        }
    }


    public void constructXML()
    {
        super.constructXML();
        for (int i = 0; i < this.getChildCount(); i++) {
            m_xml.addChild(this.getChild(i).getXML());
        }
    }

    public String getXMLElementName()
    {
        switch (this.getType()) {
        case PlexilLexer.ABS_KYWD:
            return "ABS";

        case PlexilLexer.ASTERISK:
            return "MUL";

        case PlexilLexer.MINUS:
            return "SUB";

        case PlexilLexer.MOD_KYWD:
        case PlexilLexer.PERCENT:
            return "MOD";

        case PlexilLexer.PLUS:
            if (m_dataType == PlexilDataType.STRING_TYPE)
                return "Concat";
            else return "ADD";

        case PlexilLexer.SLASH:
            return "DIV";

        case PlexilLexer.SQRT_KYWD:
            return "SQRT";

        default:
            return this.getToken().getText();
        }
    }

}