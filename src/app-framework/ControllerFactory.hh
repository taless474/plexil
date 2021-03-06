/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

#ifndef CONTROLLER_FACTORY_H
#define CONTROLLER_FACTORY_H

#include "Id.hh"
#include "LabelStr.hh"
#include <map>

// Forward reference
namespace pugi
{
  class xml_node;
}

namespace PLEXIL
{
  //
  // Forward declarations
  //

  class ExecController;
  typedef Id<ExecController> ExecControllerId;

  class ExecApplication;

  /**
   * @brief Factory class for ExecController instances.  
   *        Implements the AbstractFactory design pattern.
   */
  class ControllerFactory 
  {
  public:

    /**
     * @brief Creates a new ExecController instance as specified by
     *        the given configuration XML.
     * @param xml The configuration XML to be passed to the ExecController constructor.
     * @param app Reference to the parent ExecApplication instance.
     * @return The Id for the new ExecController.  May not be unique.
     */

    static ExecControllerId createInstance(const pugi::xml_node& xml,
                                             ExecApplication& app);


    /**
     * @brief Creates a new ExecController instance with the type associated with the name and
     *        the given configuration XML.
     * @param name The registered name for the factory.
     * @param xml The configuration XML to be passed to the ExecController constructor.
     * @param app Reference to the parent ExecApplication instance.
     * @return The Id for the new ExecController.  May not be unique.
     */

    static ExecControllerId createInstance(const LabelStr& name, 
                                             const pugi::xml_node& xml,
                                             ExecApplication& app);

    /**
     * @brief Creates a new ExecController instance with the type associated with the name and
     *        the given configuration XML.
     * @param name The registered name for the factory.
     * @param xml The configuration XML to be passed to the ExecController constructor.
     * @param app Reference to the parent ExecApplication instance.
     * @param wasCreated Reference to a boolean variable;
     *                   variable will be set to true if new object created, false otherwise.
     * @return The Id for the new ExecController.  If wasCreated is set to false, is not unique.
     */

    static ExecControllerId createInstance(const LabelStr& name,
                                             const pugi::xml_node& xml,
                                             ExecApplication& app,
                                             bool& wasCreated);

    /**
     * @brief Checks whether or not the given ControllerFactory is registered.
     * @param name The registered name for the factory
     * @return True if the factory is registered, false otherwise
     */

    static bool isRegistered(const LabelStr& name);

    /**
     * @brief Deallocate all factories
     */
    static void purge();

    const LabelStr& getName() const {return m_name;}

  protected:
    virtual ~ControllerFactory()
    {}

    /**
     * @brief Registers an ControllerFactory with the specific name.
     * @param name The name by which the controller shall be known.
     * @param factory The ControllerFactory instance.
     */
    static void registerFactory(const LabelStr& name, ControllerFactory* factory);

    /**
     * @brief Instantiates a new ExecController of the appropriate type.
     * @param xml The configuration XML for the instantiated controller.
     * @param app Reference to the parent ExecApplication instance.
     * @param wasCreated Reference to a boolean variable;
     *                   variable will be set to true if new object created, false otherwise.
     * @return The Id for the new ExecController.
     */
    virtual ExecControllerId create(const pugi::xml_node& xml,
                                      ExecApplication& app,
                                      bool& wasCreated) const = 0;

    ControllerFactory(const LabelStr& name)
      : m_name(name)
    {
      registerFactory(m_name, this);
    }

  private:
    // Deliberately unimplemented
    ControllerFactory();
    ControllerFactory(const ControllerFactory&);
    ControllerFactory& operator=(const ControllerFactory&);

    /**
     * @brief The map from names (LabelStr/double) to concrete ControllerFactory instances.
     * This pattern of wrapping static data in a static method is to ensure proper loading
     * when used as a shared library.
     */
    static std::map<double, ControllerFactory*>& factoryMap();

    const LabelStr m_name; /*!< Name used for lookup */
  };

  /**
   * @brief Concrete factory class, templated for each controller type.
   */
  template<class ControllerType>
  class ConcreteControllerFactory : public ControllerFactory 
  {
  public:
    ConcreteControllerFactory(const LabelStr& name)
      : ControllerFactory(name) 
    {}

  private:
    // Deliberately unimplemented
    ConcreteControllerFactory();
    ConcreteControllerFactory(const ConcreteControllerFactory&);
    ConcreteControllerFactory& operator=(const ConcreteControllerFactory&);

    /**
     * @brief Instantiates a new ExecController of the appropriate type.
     * @param xml The configuration XML for the instantiated controller.
     * @param app Reference to the parent ExecApplication instance.
     * @param wasCreated Reference to a boolean variable;
     *                   variable will be set to true if new object created, false otherwise.
     * @return The Id for the new ExecController.
     */

    ExecControllerId create(const pugi::xml_node& xml,
                              ExecApplication& app,
                              bool& wasCreated) const
    {
      ExecControllerId result = (new ControllerType(app, xml))->getId();
      wasCreated = true;
      return result;
    }
  };

#define REGISTER_CONTROLLER(CLASS,NAME) {new PLEXIL::ConcreteControllerFactory<CLASS>(*(new PLEXIL::LabelStr(NAME)));}

} // namespace PLEXIL

#endif // CONTROLLER_FACTORY_H
