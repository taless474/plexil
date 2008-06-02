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

import java.util.*;
import java.io.*;
import net.n3.nanoxml.*;

/**
 * Creation date: (1/26/2004 12:07:04 PM)
 * @author: Andrew Bachmann
 */
public class Parse
{
    public static final String version = new String("PlexilParser version 0.3");

    public static boolean quiet = false;
    public static FileWriter debugWriter = null;

    public Parse() 
    {
        super();
    }
    
    private static void exit(int value) 
    {
	if (value != 0) 
	    {
		System.out.println("PlexilParser exiting.");
	    }
        System.exit(value);
    }

    private static File defaultOutputFile(String fullname)
    {
	File infile = new File(fullname);
	// replace extension with .plx
	String inname = infile.getName();
	String outname = inname.substring(0, inname.lastIndexOf('.')) + ".plx";
	return new File(infile.getParent(), outname);
    }
    
    private static void printUsage() 
    {
        System.out.println(version);
        System.out.println("Usage: java plexil.Parse [OPTION]... [FILE]...");
        System.out.println("");
        System.out.println("  -a, --arguments <filename> Read arguments from filename");
        System.out.println("  -d, --debug <filename>     Print debug information to filename");
        System.out.println("  -h, --help                 Print this help");
        System.out.println("  -o, --output <filename>    Print output to filename");
        System.out.println("  -q, --quiet                Parse files quietly");
        System.out.println("  -v, --version              Print the version");
        System.out.println("");
        System.out.println("Examples:");
        System.out.println("java plexil.Parse -o filename.plx filename.ple");
    }
  
    private static java.util.Vector getArgs(java.lang.String[] args) 
    {
        java.util.Vector arguments = new java.util.Vector();
        for (int i = 0 ; i < args.length ; i++) 
	    {
		if (!args[i].equals("-a") && !args[i].equals("--arguments")) 
		    {
			arguments.add(args[i]);
		    } 
		else
		    {
			i += 1;
			if (i == args.length) 
			    {
				System.out.println("-a or --arguments requires a filename");
				printUsage();
				exit(-1);
			    }
			try 
			    {
				File file = new File(args[i]);
				FileReader fis = new FileReader(file);
				BufferedReader buf = new BufferedReader(fis);
				while (true) 
				    {
					java.lang.String line = buf.readLine();
					if (line == null) 
					    {
						break;
					    }
					arguments.add(line.trim());
				    }
			    } 
			catch (Exception e) 
			    {
				System.out.println(e);
				printUsage();
				exit(-1);
			    }
		    }
	    }
        return arguments;
    }

    public static final File generateIncludeFileName(final String parent, final String filename) 
    {
	File returnFile = new File("");
	// handle the case of the parent dir being empty. 
	if (parent != "")
	    {
		returnFile = new File(parent, filename);
	    }
	else
	    {
		returnFile = new File(filename);
	    }
	if (returnFile.canRead()) 
	    {
		// file exists, no need to serach the search path. 
		return returnFile;
	    } 
	else 
	    {
		// search for file on search path. 
		// retrive search path.
		//Object[] customSearchPath = ModelAccessor.getCustomPath();
		Object[] customSearchPath = new Object[0];
		// if search path exists.
		if (customSearchPath.length > 0 ) 
		    {
			//System.out.print("File " + returnFile.toString() + " was not found. ");
			//System.out.println("Searching for it in the nddl_path... ");
			// create a version of the filename with all path info removed.
			String filenameWithoutPath = "";
			int lastDirSlash = filename.lastIndexOf('/');
			if (lastDirSlash != -1) 
			    {
				filenameWithoutPath = 
				    filename.substring(lastDirSlash + 1, filename.length());
			    }
			File candidateFile = new File("");
			boolean found = false;
			for (int i = 0; i < customSearchPath.length; i++) 
			    {
				String pathEntry = (String) customSearchPath[i];
				// check if file exists with path info from include statement. 
				candidateFile = new File(pathEntry, filename);
				//System.out.println("checking for " + candidateFile);
				if (candidateFile.canRead()) 
				    {
					found = true;
					break;
				    }
				// check if file exists without path info from include statement.
				candidateFile = new File(pathEntry, filenameWithoutPath);
				// System.out.println("checking for " + candidateFile);
				if (candidateFile.canRead()) {
				    found = true;
				    break;
				}
			    } // end examine each path. 
			if (found) 
			    {
				//System.out.println("File " + candidateFile.toString() + " found in search path.");
				return candidateFile;
			    }
			else 
			    {
				//System.out.println("The file was not found in the nddl_path.");
			    }
		    }
	    }
	return returnFile; // default behaviour - let caller handle case when the file is not found 
    }

    public static void load(IXMLElement xml, String filename) throws Exception 
    {
	if (!quiet) 
	    {
		System.out.println("  " + filename);
	    }
	File file = generateIncludeFileName("", filename); 
        if (!file.canRead()) 
	    {
		throw new FileNotFoundException("unable to read file: " + filename);
	    }
	PlexilParser parser = PlexilParser.parse(file, debugWriter);
	if (parser.getState().printErrorCount()) 
	    {
		// be sure to properly close debug writer if bailing out early
		if (debugWriter != null)
		    debugWriter.close();
		System.exit(1);
	    }
	if (debugWriter != null)
	    {
		antlr.collections.AST parseAST = parser.getAST();
		if (parseAST != null)
		    {
			debugWriter.write(parseAST.toStringTree());
		    }
		else
		    {
			debugWriter.write("Parser AST is null!");
		    }
		debugWriter.write('\n');
	    }
       	PlexilTreeParser treeParser = new PlexilTreeParser(parser.getState());
       	treeParser.plexilPlan(parser.getAST(), xml);
    }
    
    public static void main(String [] args) throws Exception 
    {
        try
	    {
		java.util.Vector arguments = getArgs(args);
		if (arguments.isEmpty()) 
		    {
			printUsage();
			exit(0);
		    }
		File output = null;
		Iterator itr = arguments.iterator();
		boolean filename = false;
		String argument = null;
		while (itr.hasNext()) 
		    {
			argument = (String) itr.next();
			if (argument.length() == 0) continue;
			if (argument.charAt(0) != '-') 
			    {
				filename = true;
				break;
			    }
			if (argument.equals("-d") || argument.equals("--debug")) 
			    {
				debugWriter = new FileWriter((String)itr.next());
				continue;
			    }
			if (argument.equals("-h") || argument.equals("--help")) 
			    {
				printUsage();
				exit(0);
			    }
			if (argument.equals("-o") || argument.equals("--output")) 
			    {
				output = new File((String) itr.next());
				continue;
			    }
			if (argument.equals("-q") || argument.equals("--quiet")) 
			    {
				quiet = true;
				continue;
			    }
			if (argument.equals("-v") || argument.equals("--version")) 
			    {
				System.out.println(version);
				System.exit(0);
			    }
			System.out.println("Error: Unknown flag: "+argument);
			printUsage();
			exit(-1);
		    }
		if (!filename) 
		    {
			System.out.println("Error: no filenames supplied.");
			printUsage();
			exit(-1);
		    }

		IXMLElement xml = new XMLElement("PlexilPlan");
		if (!quiet) 
		    {
			System.out.println(version);
			System.out.println();
			System.out.println("Translating:");
		    }
		load(xml,argument);
		while (itr.hasNext()) 
		    {
			argument = (String) itr.next();
			load(xml,argument);
		    }
		// write out XML
		if (output == null) 
		    {
			output = defaultOutputFile(argument);
		    }
		FileWriter writer = new FileWriter(output);
		XMLWriter xmlWriter = new XMLWriter(writer);
		checkXML(xml);
		xmlWriter.write(xml,true,2,true);
		if (!quiet) 
		    {
			System.out.println();
			System.out.println("Writing:");
			System.out.println("  "+output);
		    }
		//FileWriter writer = new FileWriter(output);
		//XMLWriter xmlWriter = new XMLWriter(writer);
		//xmlWriter.write(xml,true,0,true);
		if (!quiet) 
		    {
			System.out.println();
		    }
		writer.close();
		if (debugWriter != null) 
		    {
			debugWriter.close();
		    }
	    } 
	catch (Exception e) 
	    {
		System.err.println();
		e.printStackTrace();
		exit(-1);
	    }
        exit(0);
    }
   
    static void checkXML(IXMLElement xml)
    {
	//System.err.println(xml.getName());
	//xml.getAttributes().list(System.err);
	for (int i=0; i < xml.getChildrenCount(); i++)
	    checkXML(xml.getChildAtIndex(i));
    }
}
