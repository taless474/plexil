/* Copyright (c) 2006-2011, Universities Space Research Association (USRA).
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

import java.util.Iterator;
import java.util.Map;
import java.util.HashMap;
import java.util.Vector;

//
// A singleton class to represent the global plan environment in the translator.
//

public class GlobalContext
    extends NodeContext
{
    protected Map<String, GlobalDeclaration> commands;
    protected Map<String, GlobalDeclaration> lookups;
    protected Map<String, GlobalDeclaration> libraryNodes;

    static GlobalContext s_instance = null;

    static GlobalContext getGlobalContext()
    {
        if (s_instance == null)
            s_instance = new GlobalContext();
        return s_instance;
    }

    protected GlobalContext()
    {
        super(null, "_GLOBAL_CONTEXT_");
        commands = new HashMap<String, GlobalDeclaration>();
        lookups = new HashMap<String, GlobalDeclaration>();
        libraryNodes = new HashMap<String, GlobalDeclaration>();
    }

    public boolean isGlobalContext()
    {
        return true;
    }

    public GlobalDeclaration getCommandDeclaration(String name)
    {
        return commands.get(name);
    }

    public boolean isCommandName(String name)
    {
        GlobalDeclaration ln = getCommandDeclaration(name);
        return (ln != null);
    }

    // TODO: handle resource list
    public void addCommandName(PlexilTreeNode declaration,
                               String name, 
                               Vector<VariableName> parm_spec,
                               Vector<VariableName> return_spec)
    {
        commands.put(name, 
                     new GlobalDeclaration(declaration,
                                           name,
                                           NameType.COMMAND_NAME,
                                           parm_spec,
                                           return_spec));
    }

    public GlobalDeclaration getLookupDeclaration(String name)
    {
        return lookups.get(name);
    }

    public boolean isLookupName(String name)
    {
        GlobalDeclaration ln = getLookupDeclaration(name);
        return (ln != null);
    }

    public void addLookupName(PlexilTreeNode declaration, 
                              String name, 
                              Vector<VariableName> parm_spec,
                              Vector<VariableName> return_spec)
    {
        lookups.put(name, 
                    new GlobalDeclaration(declaration,
                                          name,
                                          NameType.STATE_NAME,
                                          parm_spec,
                                          return_spec));
    }

    public GlobalDeclaration getLibraryNodeDeclaration(String name)
    {
        return libraryNodes.get(name);
    }

    public boolean isLibraryNodeName(String name)
    {
        GlobalDeclaration ln = getLibraryNodeDeclaration(name);
        return (ln != null);
    }

    public void addLibraryNode(PlexilTreeNode declaration,
                               String name,
                               Vector<VariableName> parm_spec)
    {
        libraryNodes.put(name, 
                         new GlobalDeclaration(declaration,
                                               name,
                                               NameType.LIBRARY_NODE_NAME,
                                               parm_spec,
                                               null));
    }

};
