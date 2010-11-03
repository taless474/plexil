// -*- Mode: Java; -*-

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

package plexil;

import plexil.PlexilName;
import plexil.PlexilDataType;

// XML
import net.n3.nanoxml.*;

public class VariableName extends PlexilName
{
    protected PlexilDataType m_variableType;
    protected PlexilTreeNode m_initialValue;
    protected String m_maxSize; // for arrays only

    // this variant used only in global declarations

    public VariableName(PlexilTreeNode decl, String myName, PlexilDataType varType)
    {
        super(myName, NameType.VARIABLE_NAME, decl);
        m_variableType = varType;
        m_initialValue = null;
        m_maxSize = null;
    }

    // used in local variable declarations in tree parser

    public VariableName(PlexilTreeNode decl, String myName, PlexilDataType varType, PlexilTreeNode initVal)
    {
        super(myName, NameType.VARIABLE_NAME, decl);
        m_variableType = varType;
        m_initialValue = initVal;
        m_maxSize = null;
    }

    // used in local array variable declarations in tree parser

    public VariableName(PlexilTreeNode decl,
						String myName, 
                        PlexilDataType varType, 
                        String maxSize,
                        PlexilTreeNode initVal)
    {
        super(myName, NameType.VARIABLE_NAME, decl);
        m_variableType = varType;
        m_maxSize = maxSize;
        m_initialValue = initVal;
    }

    //
    // Basic queries
    //

    public PlexilDataType getVariableType()
    {
        return m_variableType;
    }

    public PlexilTreeNode getInitialValue()
    {
        return m_initialValue;
    }

    public void setInitialValue(PlexilTreeNode initVal)
    {
        m_initialValue = initVal;
    }

    public String getMaxSize()
    {
        return m_maxSize;
    }

    public String getVariableTypeName()
    {
        return getVariableType().typeName();
    }

    public boolean isPrimitiveType()
    {
        return m_variableType.isPrimitive();
    }

    public boolean isArray()
    {
        return m_variableType.isArray();
    }

    public PlexilDataType getArrayElementType()
    {
        if (isArray())
            return m_variableType.arrayElementType();
        else
            return null;
    }

    public String getArrayElementTypeName()
    {
        if (isArray())
            return m_variableType.arrayElementType().typeName();
        else
            return null;
    }

    // Subclasses may override this method
    public boolean isLocal()
    {
        return true;
    }

    // Subclasses may override this method
    public boolean isAssignable()
    {
        return true;
    }

    // Subclasses may override this method
    public boolean isExplicit()
    {
        return true; // all local variables are explicitly declared
    }

    // For code generation purposes
    // Subclasses may override this method

    // *** to do: array vars ***

    public IXMLElement makeGlobalDeclarationElement(String elementType)
    {
        IXMLElement result = new XMLElement(elementType);
        result.setContent(getName());
        result.setAttribute("Type", getVariableType().typeName());
        return result;
    }


    // For code generation purposes
    // Subclasses may override this method

	// *** TODO: source locators
    public IXMLElement makeDeclarationXML()
    {
        IXMLElement result = 
            new XMLElement(isArray() ? "DeclareArray" : "DeclareVariable");

		// add source locators
		// TODO: add FileName attribute
		if (m_declaration != null) {
			result.setAttribute("LineNo", String.valueOf(m_declaration.getLine()));
			result.setAttribute("ColNo", String.valueOf(m_declaration.getCharPositionInLine()));
		}

        IXMLElement xname = new XMLElement("Name");
        xname.setContent(getName());
        result.addChild(xname);

        String typeName =
            isArray() ? getArrayElementTypeName()
            : getVariableTypeName(); 
        IXMLElement xtype = new XMLElement("Type");
        xtype.setContent(typeName);
        result.addChild(xtype);

        if (isArray()) {
            IXMLElement xsize = new XMLElement("MaxSize");
            xsize.setContent(m_maxSize);
            result.addChild(xsize);
        }

        if (m_initialValue != null) {
			IXMLElement init = new XMLElement("InitialValue");
			result.addChild(init);
			init.addChild(m_initialValue.getXML());
        }

        return result;
    }

}