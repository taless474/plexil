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

#include "ExecListenerFactory.hh"
#include "Debug.hh"
#include "DynamicLoader.hh"
#include "ExecListener.hh"
#include "InterfaceSchema.hh"

#include "pugixml.hpp"

namespace PLEXIL
{
  //
  // ExecListenerFactory
  //

  /**
   * @brief Creates a new ExecListener instance with the type associated with the name and
   *        the given configuration XML.
   * @param xml The configuration XML specifying the ExecListener.
   * @return The Id for the new ExecListener.
   */

  ExecListenerId 
  ExecListenerFactory::createInstance(const pugi::xml_node& xml)
  {
    // Can't do anything without the spec
    assertTrueMsg(!xml.empty(),
				  "ExecListenerFactory::createInstance: null configuration XML");

    // Get the kind of listener to make
    const char* listenerType = 
      xml.attribute(InterfaceSchema::LISTENER_TYPE_ATTR()).value();
    checkError(*listenerType != '\0',
			   "ExecListenerFactory::createInstance: no "
			   << InterfaceSchema::LISTENER_TYPE_ATTR()
			   << " attribute for listener XML:\n"
			   << *xml);

    // Make it
    return createInstance(LabelStr(listenerType), xml);
  }

  /**
   * @brief Creates a new ExecListener instance with the type associated with the name and
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the ExecListener constructor.
   * @return The Id for the new ExecListener.
   */

  ExecListenerId 
  ExecListenerFactory::createInstance(const LabelStr& name,
                                      const pugi::xml_node& xml)
  {
    std::map<double, ExecListenerFactory*>::const_iterator it = factoryMap().find(name.getKey());
    if (it == factoryMap().end()) {
	  debugMsg("ExecListenerFactory:createInstance", 
			   "Attempting to dynamically load listener type \""
			   << name.c_str() << "\"");
	  // Attempt to dynamically load library
	  const char* libCPath =
		xml.attribute(InterfaceSchema::LIB_PATH_ATTR()).value();
	  if (!DynamicLoader::loadModule(name.c_str(), libCPath)) {
		debugMsg("ExecListenerFactory:createInstance", 
				 " unable to load module for listener type \""
				 << name.c_str() << "\"");
		return ExecListenerId::noId();
	  }
	  // See if it's registered now
	  it = factoryMap().find(name.getKey());
	}

    if (it == factoryMap().end()) {
      debugMsg("ExecListenerFactory:createInstance", 
               " No exec listener factory registered for name \"" << name.c_str() << "\"");
      return ExecListenerId::noId();
    }
    ExecListenerId retval = it->second->create(xml);
    debugMsg("ExecListenerFactory:createInstance", " Created Exec listener " << name.c_str());
    return retval;
  }

  std::map<double, ExecListenerFactory*>& ExecListenerFactory::factoryMap() 
  {
    static std::map<double, ExecListenerFactory*> sl_map;
    return sl_map;
  }

  /**
   * @brief Deallocate all factories
   */
  void ExecListenerFactory::purge()
  {
    for (std::map<double, ExecListenerFactory*>::iterator it = factoryMap().begin();
         it != factoryMap().end();
         ++it)
      delete it->second;
    factoryMap().clear();
  }

  /**
   * @brief Registers an ExecListenerFactory with the specific name.
   * @param name The name by which the Exec Listener shall be known.
   * @param factory The ExecListenerFactory instance.
   */
  void ExecListenerFactory::registerFactory(const LabelStr& name, ExecListenerFactory* factory)
  {
    assertTrue(factory != NULL);
    if (factoryMap().find(name.getKey()) != factoryMap().end())
      {
        warn("Attempted to register an exec listener factory for name \""
             << name.c_str()
             << "\" twice, ignoring.");
        delete factory;
        return;
      }
    factoryMap()[name] = factory;
    debugMsg("ExecListenerFactory:registerFactory",
             " Registered exec listener factory for name \"" << name.c_str() << "\"");
  }

  bool ExecListenerFactory::isRegistered(const LabelStr& name) {
    return factoryMap().find(name) != factoryMap().end();
  }
}
