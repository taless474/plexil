/* Copyright (c) 2006-2012, Universities Space Research Association (USRA).
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

#include "Logging.hh"
#include "PlexilExec.hh"
#include "ExecListenerHub.hh"
#include "PlanDebugListener.hh"
#include "TestExternalInterface.hh"
#include "SocketException.h"
#include "CoreExpressions.hh"
#include "Expressions.hh"
#include "Debug.hh"
#include "PlexilXmlParser.hh"
#include "Node.hh"
#include "PlexilPlan.hh"
#include "ExecTestRunner.hh"
#include "LuvListener.hh"

#include <cstring>
#include <fstream>
#include <string>

using std::endl;
using std::set;
using std::string;
using std::vector;

namespace PLEXIL {

int ExecTestRunner::run(int argc, char** argv) 
{
  string scriptName("error");
  string planName("error");
  string debugConfig("Debug.cfg");
  vector<string> libraryNames;
  vector<string> libraryPaths;
  bool luvRequest = false;
  string luvHost = LuvListener::LUV_DEFAULT_HOSTNAME();
  int luvPort = LuvListener::LUV_DEFAULT_PORT();
  bool luvBlock = false;
  string
    usage(
          "Usage: exec-test-runner -s <script> -p <plan>\n\
                        [-l <library>]*\n\
                        [-L <library-dir>]*\n\
                        [-d <debug_config_file>]\n\
                        [-v [-h <hostname>] [-n <portnumber>] [-b] ]");

  // if not enough parameters, print usage

  if (argc < 5) {
    if (argc >= 2 && strcmp(argv[1], "-h") == 0) {
      // print usage and exit
      std::cout << usage << std::endl;
      return 0;
    }
    warn("Not enough arguments.\n At least the -p and -s arguments must be provided.\n" << usage);
    return 2;
  }
  // parse out parameters

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-p") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      planName = argv[i];
    }
    else if (strcmp(argv[i], "-s") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      scriptName = argv[i];
    }
    else if (strcmp(argv[i], "-l") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      libraryNames.push_back(argv[i]);
    }
    else if (strcmp(argv[i], "-L") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      libraryPaths.push_back(argv[i]);
    }
    else if (strcmp(argv[i], "-d") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      debugConfig = string(argv[i]);
    }
    else if (strcmp(argv[i], "-v") == 0)
      luvRequest = true;
    else if (strcmp(argv[i], "-b") == 0)
      luvBlock = true;
    else if (strcmp(argv[i], "-h") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      luvHost = argv[i];
    }
    else if (strcmp(argv[i], "-n") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      std::istringstream buffer(argv[i]);
      buffer >> luvPort;
      SHOW(luvPort);
    } 
    else if (strcmp(argv[i], "-log") == 0) {
      if (argc == (++i)) {
        warn("Missing argument to the " << argv[i-1] << " option.\n"
             << usage);
        return 2;
      }
      Logging::ENABLE_LOGGING = 1;
      Logging::set_log_file_name(argv[i]);
    }
    else if (strcmp(argv[i], "-eprompt") == 0)
      Logging::ENABLE_E_PROMPT = 1;
    else if (strcmp(argv[i], "-wprompt") == 0)
      Logging::ENABLE_W_PROMPT = 1;
    else {
      warn("Unknown option '" << argv[i] << "'.  " << usage);
      return 2;
    }
  }

  // if no plan or script supplied, error out
  if (scriptName == "error") {
    warn("No -s option found.\n" << usage);
    return 2;
  }
  if (planName == "error") {
    warn("No -p option found.\n" << usage);
    return 2;
  }

  if (Logging::ENABLE_LOGGING) {

#ifdef __linux__
    Logging::print_to_log(argv, argc);
#endif
#ifdef __APPLE__
    string cmd = "user command: ";
    for (int i = 1; i < argc; ++i)
      cmd = cmd + argv[i] + " ";

    Logging::print_to_log(cmd.c_str());
#endif
  }

  // basic initialization

  std::ifstream config(debugConfig.c_str());
  if (config.good())
    DebugMessage::readConfigFile(config);

  initializeExpressions();

  // create the exec

  TestExternalInterface intf;
  PlexilExecId exec = (new PlexilExec())->getId();
  intf.setExec(exec);
  ExecListenerHub hub;
  exec->setExecListenerHub(hub.getId());

  // add the debug listener

  PlanDebugListener debug_listener;
  hub.addListener((new PlanDebugListener())->getId());

  // if a Plexil Viwer is to be attached

  if (luvRequest) {
    // create and add luv listener
    LuvListener* ll = 
      new LuvListener(luvHost, luvPort, luvBlock);
    if (ll->isConnected()) {
      hub.addListener(ll->getId());
    }
    else {
      warn("WARNING: Unable to connect to Plexil Viewer: " << endl
           << "  address: " << luvHost << ":" << luvPort << endl
           << "Execution will continue without the viewer.");
      delete ll;
    }
  }

  // if specified on command line, load libraries

  for (vector<string>::const_iterator libraryName = libraryNames.begin(); 
       libraryName != libraryNames.end();
       ++libraryName) {
    pugi::xml_document libraryXml;
    pugi::xml_parse_result parseResult = libraryXml.load_file(libraryName->c_str(), PlexilXmlParser::PUGI_PARSE_OPTIONS());
    if (parseResult.status != pugi::status_ok) {
      warn("XML error parsing library file '" << *libraryName
           << "' (offset " << parseResult.offset
           << "):\n" << parseResult.description());
      return 1;
    }

    PlexilNodeId libnode;
    try {
      libnode =
        PlexilXmlParser::parse(libraryXml.document_element().child("PlexilPlan").child("Node"));
    } 
    catch (ParserException& e) {
      warn("XML error parsing library '" << *libraryName << "':\n" << e.what());
      return 1;
    }

    exec->addLibraryNode(libnode);
  }

  // Load the plan
  {
    pugi::xml_document plan;
    pugi::xml_parse_result parseResult = plan.load_file(planName.c_str(), PlexilXmlParser::PUGI_PARSE_OPTIONS());
    if (parseResult.status != pugi::status_ok) {
      warn("XML error parsing plan file '" << planName
           << "' (offset " << parseResult.offset
           << "):\n" << parseResult.description());
      return 1;
    }

    PlexilNodeId root;
    try {
      root =
        PlexilXmlParser::parse(plan.document_element().child("Node"));
    }
    catch (ParserException& e) {
      warn("XML error parsing plan '" << planName << "':\n" << e.what());
      return 1;
    }

    {
      // Check whether all libraries for this plan are loaded
      // and try to load those that aren't
      vector<string> libs = root->getLibraryReferences();
      // N.B. libs is likely growing during this operation, 
      // so we can't use a traditional iterator.
      for (unsigned int i = 0; i < libs.size(); i++) {
        // COPY the string because its location may change out from under us!
        const std::string libname(libs[i]);

        PlexilNodeId libroot = exec->getLibrary(libname);
        if (libroot.isNoId()) {
          // Try to load the library
          libroot = PlexilXmlParser::findLibraryNode(libname, libraryPaths);
          if (libroot.isNoId()) {
            warn("Adding plan " << planName
                 << " failed because library " << libname
                 << " could not be loaded");
            return 1;
          }

          // add the library node
          exec->addLibraryNode(libroot);
        }

        // Make note of any dependencies in the library itself
        libroot->getLibraryReferences(libs);
      }
    }

    if (!exec->addPlan(root)) {
      warn("Adding plan " << planName << " failed");
      return 1;
    }
    delete (PlexilNode*) root;
    // TODO: delete library nodes
  }

  // load script

  {
    pugi::xml_document script;
    {
      pugi::xml_parse_result parseResult = script.load_file(scriptName.c_str(), PlexilXmlParser::PUGI_PARSE_OPTIONS());
      if (parseResult.status != pugi::status_ok) {
        checkParserException(false,
                             "(offset " << parseResult.offset
                             << ") XML error parsing script '" << scriptName << "': "
                             << parseResult.description());
        return 1;
      }
    }
    // execute plan

    clock_t time = clock();
    pugi::xml_node scriptElement = script.child("PLEXILScript");
    if (scriptElement.empty()) {
      warn("File '" << scriptName << "' is not a valid PLEXIL simulator script");
      return 1;
    }
    intf.run(scriptElement);
    debugMsg("Time", "Time spent in execution: " << clock() - time);
  }

  // clean up

  delete (PlexilExec*) exec;
  return 0;
}

} // namespace
