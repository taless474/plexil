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

#include "AdapterFactory.hh"
#include "InterfaceAdapter.hh"
#include "AdapterExecInterface.hh"
#include "tinyxml.h"

namespace PLEXIL
{
  /**
   * @brief Creates a new InterfaceAdapter instance with the type associated with the name and
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the InterfaceAdapter constructor.
   * @return The Id for the new InterfaceAdapter.  May not be unique.
   */

  InterfaceAdapterId 
  AdapterFactory::createInstance(const LabelStr& name,
                                 const TiXmlElement* xml,
                                 AdapterExecInterface& execInterface)
  {
    bool dummy;
    return createInstance(name, xml, execInterface, dummy);
  }


  /**
   * @brief Creates a new InterfaceAdapter instance with the type associated with the name and
   *        the given configuration XML.
   * @param name The registered name for the factory.
   * @param xml The configuration XML to be passed to the InterfaceAdapter constructor.
   * @param wasCreated Reference to a boolean variable;
   *                   variable will be set to true if new object created, false otherwise.
   * @return The Id for the new InterfaceAdapter.  If wasCreated is set to false, is not unique.
   */

  InterfaceAdapterId 
  AdapterFactory::createInstance(const LabelStr& name,
                                 const TiXmlElement* xml,
                                 AdapterExecInterface& execInterface,
                                 bool& wasCreated)
  {
    std::map<double, AdapterFactory*>::const_iterator it = factoryMap().find(name.getKey());
    assertTrueMsg(it != factoryMap().end(),
		  "Error: No adapter factory registered for name \"" << name.c_str() << "\".");
    InterfaceAdapterId retval = it->second->create(xml, execInterface, wasCreated);
    debugMsg("AdapterFactory:createInstance", " Created adapter " << name.c_str());
    return retval;
  }

  std::map<double, AdapterFactory*>& AdapterFactory::factoryMap() 
  {
    static std::map<double, AdapterFactory*> sl_map;
    return sl_map;
  }

  /**
   * @brief Deallocate all factories
   */
  void AdapterFactory::purge()
  {
    for (std::map<double, AdapterFactory*>::iterator it = factoryMap().begin();
         it != factoryMap().end();
         ++it)
      delete it->second;
    factoryMap().clear();
  }

  /**
   * @brief Registers an AdapterFactory with the specific name.
   * @param name The name by which the Adapter shall be known.
   * @param factory The AdapterFactory instance.
   */
  void AdapterFactory::registerFactory(const LabelStr& name, AdapterFactory* factory)
  {
    assertTrue(factory != NULL);
    if (factoryMap().find(name.getKey()) != factoryMap().end())
      {
	warn("Attempted to register an adapter factory for name \""
             << name.c_str()
             << "\" twice, ignoring.");
        delete factory;
        return;
      }
    factoryMap()[name.getKey()] = factory;
    debugMsg("AdapterFactory:registerFactory",
             " Registered adapter factory for name \"" << name.c_str() << "\"");
  }

}