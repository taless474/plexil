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

package gov.nasa.luv;

import java.io.InterruptedIOException;
import org.xml.sax.Attributes;
import java.util.Stack;

import static gov.nasa.luv.Constants.*;

 /** SAX PlexilPlan XML handler */

public class PlexilPlanHandler extends AbstractDispatchableHandler
{
    private int row_number = 0;
    
    // marker for formatting condition and local variable information
    private final String SEPARATOR = "_Separator_";

    // variables and flags for collecting condition, local variable and action information
    private Stack<String> nodeInfoHolder;
    private boolean recordIN;
    private boolean recordINOUT;
    private boolean properName;

    // flags for library information
    private boolean libraryNodeCall;
    private boolean loadingLibrary;

    private Stack<Model> nodeHolder;

    private Model topLevelNode;              

    public PlexilPlanHandler()
    {
        super();

        nodeInfoHolder = new Stack<String>();    
        recordIN = recordINOUT = false;
        properName = false; 

        libraryNodeCall = false;
        loadingLibrary = false;

        nodeHolder = new Stack<Model>();
        topLevelNode = null;

        nodeHolder.push(Model.getRoot());
    }

    /** Handle start of an XML document */

    public void startDocument()
    {
    }

    /** Handle start of an XML element. */

    public void startElement(String uri, String tagName, String qName, Attributes attributes)
    {    
        // get the current node in the stack

        Model node = nodeHolder.peek();

        // error if loaded a script instead of plan

        if (tagName.equals(PLEXIL_SCRIPT))
        {
            Luv.getLuv().displayErrorMessage(null, "ERROR: loaded script instead of plan");           
        }

        // if this SHOULD be a child node, make that happen

        if (tagName.equals(NODE)) 
        {
            Model child = new Model(tagName, row_number++);

            // if there is a parent, add child to it
            // *** There should ALWAYS be a parent!
            node.addChild(child);

            // if the parent is the root node, save this as a (the?) top level node
            if (node.isRoot()) 
            {
                topLevelNode = child;
            }

            // this child is now the current node

            node = child;

            // add attributes for this child

            for (int i = 0; i < attributes.getLength(); ++i) 
            {
                child.setProperty(attributes.getQName(i), attributes.getValue(i));
            }
        }

        // starting to load a library def'n?

        else if (tagName.equals(PLEXIL_LIBRARY)) 
        {  
            loadingLibrary = true;
        }

        // if it's not a property we can ignore it

        else if (!isProperty(tagName)) 
        {
            node = null;
        }

        // catch condition, local variable and action information

        catchStartTag(tagName);

        // push new node onto the stack

        nodeHolder.push(node);
    }

    /** Handle end of an XML element. */

    public void endElement(String uri, String tagName, String qName)
    {
        Model topNode = nodeHolder.peek();

        // get tweener text and put it in its place
        String text = getTweenerText();

        try 
        {
            // catch condition, local variable and action information
            catchTweenerText(tagName, text);
        } 
        catch (InterruptedIOException ex) 
        {
            Luv.getLuv().displayErrorMessage(ex, "ERROR: exception occurred while locating library");
        }

        // catch condition, local variable and action information

        catchEndTag(tagName);

        // assign model name and path to the appropriate model

        if (topNode != null && text != null) 
        {
            topNode.setProperty(tagName, text);
            if (tagName.equals(NODE_ID)) 
            {
                topNode.setModelName(text);
            }
        }

        // if node, assign the main attributes for display in luv viewer
        // name, type, state and outcome
        if (topNode != null && tagName.equals(topNode.getType()))
            topNode.setMainAttributesOfNode();

        // pop the node off the stack
        nodeHolder.pop();        

        // Make sure Luv instance is notified if this was plan or library
        if (tagName.equals(PLEXIL_PLAN))
            Luv.getLuv().handleNewPlan(topLevelNode);
        else if (tagName.equals(PLEXIL_LIBRARY))
            Luv.getLuv().handleNewLibrary(topLevelNode);
    }

    /** Handle end of document event. */

    public void endDocument()
    {       
    }

    private void catchStartTag(String tagName)
    {
        if (tagName.equals(LIBRARYNODECALL))
        {
            libraryNodeCall = true;
            nodeInfoHolder.clear();
        }
        else if (isCondition(tagName) || isAction(tagName) || isVariableDeclaration(tagName))
        {           
            nodeInfoHolder.clear();
        }

        recordStartTag(tagName);
    }

    private void catchTweenerText(String tagName, String text) throws InterruptedIOException
    {
        if (text != null) 
        {            
            if (libraryNodeCall) 
            {
                currentNode().setLibraryName(text);
                currentNode().setUnresolvedLibraryCall(true);
                boolean askAboutMissingLibs = !loadingLibrary && !Luv.getLuv().getFileHandler().getStopSearchForMissingLibs();
                Model library = Luv.getLuv().findLibraryNode(text, askAboutMissingLibs);

                if (library == null) 
                {
                    topLevelNode.addMissingLibrary(text);
                }
                else 
                {
                    currentNode().linkLibrary(library);
                }

                libraryNodeCall = false;                
            }
            
            // if data is a string object and not a proper name, surround with quotes
            if (tagName.equals(STRING_VAL) && !properName && !text.equals(UNKNOWN))
            {
                text = "\"" + text + "\"";
            }

            nodeInfoHolder.push(text);
        }
    }

    private void catchEndTag(String tagName)
    {  
        recordEndTag(tagName);
    }

    private void recordStartTag(String tagName)
    {
        if (tagName.equals(ARGS) || tagName.equals(AND) || tagName.equals(OR) || tagName.equals(CONCAT))
        {
            nodeInfoHolder.push(tagName);
        }       
        else if (isLookup(tagName))
        {
            nodeInfoHolder.push(tagName);
            nodeInfoHolder.push(ARGS);
        }
        else if (tagName.equals(RESOURCE))
        {
            saveActionInfoToNode(tagName);
        }
        else if (isVariableDeclaration(tagName))
        {
            if (recordIN)           nodeInfoHolder.push("In");
            else if (recordINOUT)   nodeInfoHolder.push("InOut");
            else                    nodeInfoHolder.push("--");
        }
        else if (tagName.equals(NAME))
        {
            properName = true;
        }
        else if (tagName.equals(IN))
        {
            recordIN = true; 
        }
        else if (tagName.equals(INOUT))
        {
            recordINOUT = true;
        }       
    }

    private void recordEndTag(String tagName)
    {
        // complete condition info is ready to be saved to node
        if (isCondition(tagName))
        {
            saveConditionInfoToNode(tagName);
        }
        
        // complete action info is ready to be saved to node
        else if (isAction(tagName) || isResource(tagName) || tagName.equals(LIBRARYNODECALL)) 
        {     
            saveActionInfoToNode(tagName);
        }
        
        // complete local variable info is ready to be saved to node
        else if (isVariableDeclaration(tagName))
        {
            saveLocalVariableInfoToNode();
        }  
        
        // more data still needs to be collected before it can be saved to node
        else if (tagName.equals(ALIAS) || isReturnValue(tagName) || tagName.equals(PAIR) || isComparisonOperator(tagName) || isMathOperator(tagName))
        {
            if (nodeInfoHolder.size() > 1)
            {
                String second = nodeInfoHolder.pop();
                String first = nodeInfoHolder.pop();
                first += SEPARATOR + convertTagNameToLiteral(tagName) + SEPARATOR + second;
                nodeInfoHolder.push(first);
            }
        }
        else if (tagName.equals(CONCAT) || tagName.equals(AND) || tagName.equals(OR))
        {
            String elements = "";

            while (!nodeInfoHolder.isEmpty()) 
            {
                if (!nodeInfoHolder.peek().equals(tagName))
                {
                    if (elements.equals(""))
                        elements = nodeInfoHolder.pop() + SEPARATOR;
                    else
                        elements = nodeInfoHolder.pop() + SEPARATOR + convertTagNameToLiteral(tagName) + SEPARATOR + elements;
                }
                else
                {
                    nodeInfoHolder.pop();
                    break;
                }
            }

            if (!elements.equals(""))
            {
                nodeInfoHolder.push(elements);
            }
        }
        else if (tagName.equals(NOT) || tagName.equals(IS_KNOWN))
        {
            String complete = convertTagNameToLiteral(tagName) + SEPARATOR + "(" + SEPARATOR + nodeInfoHolder.pop() + SEPARATOR + ")";
            nodeInfoHolder.push(complete);
        }
        else if (isNodeState(tagName))
        {
            if (nodeInfoHolder.size() > 1)
            {
                String value = nodeInfoHolder.pop();
                String state = nodeInfoHolder.pop() + convertTagNameToLiteral(tagName);
                nodeInfoHolder.push(state);
                nodeInfoHolder.push(value);
            }
        }  
        else if (tagName.equals(ARRAYELEMENT))
        {
            if (nodeInfoHolder.size() > 1)
            {
                String index = "[" + nodeInfoHolder.pop() + "]";
                String arrayName = nodeInfoHolder.pop() + index;
                nodeInfoHolder.push(arrayName);
            }          
        } 
        else if (tagName.equals(TYPE))
        {
            nodeInfoHolder.push(INITIALVAL);
        }
        else if (tagName.equals(MAXSIZE))
        {
            if (nodeInfoHolder.size() > 1)
            {
                String maxsize = "[" + nodeInfoHolder.pop() + "]";
                String initVal = nodeInfoHolder.pop();
                String type = nodeInfoHolder.pop();
                String arrayName = nodeInfoHolder.pop() + maxsize;
                nodeInfoHolder.push(arrayName);
                nodeInfoHolder.push(type);
                nodeInfoHolder.push(initVal);
            }
        }
        else if (tagName.equals(TIME_VAL))
        {
            if (nodeInfoHolder.size() > 1)
            {
                String second = nodeInfoHolder.pop();
                String first = nodeInfoHolder.pop();
                first = "[" + SEPARATOR + first + "," + SEPARATOR + second + SEPARATOR + "]";
                nodeInfoHolder.push(first);
            }           
        }
        else if (tagName.equals(ARGS) || isLookup(tagName))
        {
            String arguments = "";

            while (!nodeInfoHolder.isEmpty()) 
            {
                if (!nodeInfoHolder.peek().equals(ARGS))
                {
                    if (arguments.equals(""))
                        arguments = nodeInfoHolder.pop() + SEPARATOR;
                    else
                        arguments = nodeInfoHolder.pop() + "," + SEPARATOR + arguments;
                }
                else
                {
                    nodeInfoHolder.pop();
                    break;
                }
            }

            if (!arguments.equals(""))
            {
                arguments = nodeInfoHolder.pop() + SEPARATOR + "(" + SEPARATOR + arguments + ")";
                nodeInfoHolder.push(arguments);
            }
        }
        else if (tagName.equals(TIMEPOINT))
        {
            if (nodeInfoHolder.size() > 2)
            {
                String timepoint = nodeInfoHolder.pop();
                String state = nodeInfoHolder.pop();
                String nodeid = nodeInfoHolder.pop();
                nodeid += "." + state + "." + timepoint;
                nodeInfoHolder.push(nodeid);
            }
        }
        else if (tagName.equals(NAME))
        {
            properName = false;
        }
        else if (tagName.equals(IN))
        {
            recordIN = false;
        }
        else if (tagName.equals(INOUT))
        {
            recordINOUT = false;
        }
    } 
    
    private void saveConditionInfoToNode(String tagName)
    {
        if (!nodeInfoHolder.isEmpty() && nodeInfoHolder.size() == 1)
        {
            int condition = getCondition(tagName);
            currentNode().addConditionInfo(condition, nodeInfoHolder.pop());
        }

        nodeInfoHolder.clear();
    }
    
    private void saveActionInfoToNode(String tagName)
    {
        if (tagName.equals(RESOURCE))
        {
            currentNode().addActionInfo(RESOURCE + SEPARATOR + ":");  
        }
        else if (isResource(tagName))
        {
            String resource = convertTagNameToLiteral(tagName) + SEPARATOR + nodeInfoHolder.pop();
            currentNode().addActionInfo(resource);       
        }
        else if (tagName.equals(UPDATE))
        {
            String update = "";

            while (!nodeInfoHolder.isEmpty()) 
            {
                if (update.equals(""))
                    update = nodeInfoHolder.pop();
                else
                    update = nodeInfoHolder.pop() + ", " + update;
            }

            currentNode().addActionInfo(update);
        }
        else if (tagName.equals(LIBRARYNODECALL))
        {
            String arguments = "";

            while (nodeInfoHolder.size() > 1) 
            {
                if (arguments.equals(""))
                    arguments = nodeInfoHolder.pop() + SEPARATOR;
                else
                    arguments = nodeInfoHolder.pop() + "," + SEPARATOR + arguments;               
            }

            arguments = nodeInfoHolder.pop() + SEPARATOR + "(" + SEPARATOR + arguments + ")"; 
            currentNode().addActionInfo(arguments); 
        }
        else if (!nodeInfoHolder.isEmpty())
        {
            if ((tagName.equals(COMMAND) || tagName.equals(FUNCCALL)) && !nodeInfoHolder.peek().endsWith(")"))
            {
                String closeParens = nodeInfoHolder.pop() + SEPARATOR + "(" + SEPARATOR + ")";
                nodeInfoHolder.push(closeParens);
            }

            if (nodeInfoHolder.size() == 2)
            {
                String second = nodeInfoHolder.pop();
                String first = nodeInfoHolder.pop();
                first += SEPARATOR + "=" + SEPARATOR + second;
                nodeInfoHolder.push(first);
            }

            if (nodeInfoHolder.size() > 1)
                currentNode().addActionInfo(tagName + SEPARATOR + ":" + SEPARATOR + nodeInfoHolder.pop());
            else
                currentNode().addActionInfo(tagName + SEPARATOR + ":" + SEPARATOR + nodeInfoHolder.pop());                
        }      

        nodeInfoHolder.clear();
    }
    
    private void saveLocalVariableInfoToNode()
    {
        if (!nodeInfoHolder.isEmpty())
        {
            if (nodeInfoHolder.size() == 4)
            {
                nodeInfoHolder.pop();
                nodeInfoHolder.push(UNKNOWN);
            }
            else
            {
                String elements = "";

                while (!nodeInfoHolder.isEmpty()) 
                {
                    if (!nodeInfoHolder.peek().equals(INITIALVAL))
                    {
                        if (elements.equals(""))
                            elements = nodeInfoHolder.pop();
                        else
                            elements = nodeInfoHolder.pop() + ", " + elements;
                    }
                    else
                    {
                        nodeInfoHolder.pop();
                        break;
                    }
                }

                if (!elements.equals(""))
                {
                    nodeInfoHolder.push(elements);
                }
            }

            if (nodeInfoHolder.size() == 4)
            {
                currentNode().addVariableInfo(nodeInfoHolder);
            }
        }

        nodeInfoHolder.clear();
    }

    private Model currentNode()
    {
        int i = nodeHolder.size() - 1;
        while (nodeHolder.elementAt(i) == null)
            i--; 
        return nodeHolder.elementAt(i);
    } 

    public Model getPlan()
    {
        return topLevelNode;
    }

    private int getCondition(String tag)
    {
        int condition = -1;

        for (int i = 0; i < ALL_CONDITIONS.length; i++)
        {
            if (tag.equals(ALL_CONDITIONS[i]))
            {
                condition = i;
                break;
            }
        } 

        return condition;
    }

    private boolean isProperty(String tag)
    {
        for (String property: PROPERTY_TAGS)
            if (property.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isCondition(String tag)
    {
        for (String condition: ALL_CONDITIONS)
            if (condition.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isVariableDeclaration(String tag)
    {
        for (String varDecl: VARIABLE_DECL_TAGS)
            if (varDecl.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isAction(String tag)
    {
        for (String action: ACTION_TAGS)
            if (action.equalsIgnoreCase(tag))
                return true;

        return false;
    }   

    private boolean isNodeState(String tag)
    {
        for (String nodeState: NODE_STATE_TAGS)
            if (nodeState.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isReturnValue(String tag)
    {
        for (String returnValue: RETURN_TAGS)
            if (returnValue.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isResource(String tag)
    {
        for (String resource: RESOURCE_TAGS)
            if (resource.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isMathOperator(String tag)
    {
        for (String math: MATH_TAGS)
            if (math.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isLookup(String tag)
    {
        for (String lookup: LOOKUP_TAGS)
            if (lookup.equalsIgnoreCase(tag))
                return true;

        return false;
    }

    private boolean isComparisonOperator(String tag)
    {
        for (String comparison: COMPARISON_TAGS)
            if (comparison.equalsIgnoreCase(tag))
                return true;

        return false;
    } 

    private String convertTagNameToLiteral(String tag)
    {
        if (tag.equals(EQ) ||
            tag.equals(EQ_NUMERIC) || 
            tag.equals(EQ_STRING) || 
            tag.equals(EQ_BOOLEAN) ||
            tag.equals(EQ_INTERNAL))       
                                                        return "==";
        else if (tag.equals(NE) ||
                 tag.equals(NE_NUMERIC) ||
                 tag.equals(NE_STRING) ||
                 tag.equals(NE_BOOLEAN) || 
                 tag.equals(NE_INTERNAL))  
                                                        return "!=";
        else if (tag.equals(LT))                        return "<";
        else if (tag.equals(GT))                        return ">";
        else if (tag.equals(LE))                        return "<=";
        else if (tag.equals(GE))                        return ">=";
        else if (tag.equals(ADD) || tag.equals(CONCAT)) return "+"; 
        else if (tag.equals(SUB))                       return "-"; 
        else if (tag.equals(MUL))                       return "*"; 
        else if (tag.equals(DIV))                       return "/"; 
        else if (tag.equals(AND))                       return "&&"; 

        else if (tag.equals(ALIAS) || 
                 tag.equals(PAIR)  ||
                 isReturnValue(tag))                    return "=";       

        else if (tag.equals(OR))                        return "||";
        else if (tag.equals(NOT))                       return "!";
        else if (tag.equals(NODE_OUTCOME_VAL))          return ".outcome"; 
        else if (tag.equals(NODE_FAILURE_VAL))          return ".failure";               
        else if (tag.equals(NODE_STATE_VAL))            return ".state";    
        else if (tag.equals(NODE_TIMEPOINT_VAL))        return ".timepoint"; 
        else if (tag.equals(NODE_CMD_HANDLE_VAL))       return ".command_handle"; 

        else if (tag.equals(RESOURCE_NAME))             return "   Name" + SEPARATOR + "=";
        else if (tag.equals(RESOURCE_PRIORITY))         return "   Priority" + SEPARATOR + "=";
        else if (tag.equals(RESOURCE_LOWER_BOUND))      return "   LowerBound" + SEPARATOR + "=";
        else if (tag.equals(RESOURCE_UPPER_BOUND))      return "   UpperBound" + SEPARATOR + "=";
        else if (tag.equals(RESOURCE_RELEASE_AT_TERM))  return "   ReleaseAtTermination" + SEPARATOR + "=";
        else                                            return tag;
    }
}
