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

#ifndef EXEC_LISTENER_FILTER_FACTORY_H
#define EXEC_LISTENER_FILTER_FACTORY_H

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

  //  class ExecListener;
  //  typedef Id<ExecListener> ExecListenerId;
  class ExecListenerFilter;
  typedef Id<ExecListenerFilter> ExecListenerFilterId;

  /**
   * @brief Factory class for ExecListenerFilter instances.
   *        Implements the AbstractFactory design pattern.
   */
  class ExecListenerFilterFactory 
  {
  public:

    /**
     * @brief Creates a new ExecListenerFilter instance with the type associated with the name and
     *        the given configuration XML.
     * @param xml The configuration XML specifying the filter.
     * @return The Id for the new ExecListenerFilter.
     */

    static ExecListenerFilterId createInstance(const pugi::xml_node& xml);

    /**
     * @brief Creates a new ExecListenerFilter instance with the type associated with the name and
     *        the given configuration XML.
     * @param name The registered name for the factory.
     * @param xml The configuration XML to be passed to the ExecListenerFilter constructor.
     * @return The Id for the new ExecListenerFilter.
     */

    static ExecListenerFilterId createInstance(const LabelStr& name, 
                                               const pugi::xml_node& xml);

    /**
     * @brief Deallocate all factories
     */
    static void purge();

    const LabelStr& getName() const {return m_name;}

  protected:

    virtual ~ExecListenerFilterFactory()
    {}

    /**
     * @brief Registers an ExecListenerFilterFactory with the specific name.
     * @param name The name by which the filter shall be known.
     * @param factory The ExecListenerFilterFactory instance.
     */
    static void registerFactory(const LabelStr& name, ExecListenerFilterFactory* factory);

    /**
     * @brief Instantiates a new ExecListenerFilter of the appropriate type.
     * @param xml The configuration XML for the instantiated filter
     * @return The Id for the new ExecListenerFilter.
     */
    virtual ExecListenerFilterId create(const pugi::xml_node& xml) const = 0;

    ExecListenerFilterFactory(const LabelStr& name)
      : m_name(name)
    {
      registerFactory(m_name, this);
    }

  private:
    // Deliberately unimplemented
    ExecListenerFilterFactory();
    ExecListenerFilterFactory(const ExecListenerFilterFactory&);
    ExecListenerFilterFactory& operator=(const ExecListenerFilterFactory&);

    /**
     * @brief The map from names (LabelStr/double) to concrete ExecListenerFilterFactory instances.
     * This pattern of wrapping static data in a static method is to ensure proper loading
     * when used as a shared library.
     */
    static std::map<double, ExecListenerFilterFactory*>& factoryMap();

    const LabelStr m_name; /*!< Name used for lookup */
  };

  /**
   * @brief Concrete factory class, templated for each filter type.
   */
  template<class FilterType>
  class ConcreteExecListenerFilterFactory : public ExecListenerFilterFactory 
  {
  public:
    ConcreteExecListenerFilterFactory(const LabelStr& name)
      : ExecListenerFilterFactory(name) 
    {}

  private:
    // Deliberately unimplemented
    ConcreteExecListenerFilterFactory();
    ConcreteExecListenerFilterFactory(const ConcreteExecListenerFilterFactory&);
    ConcreteExecListenerFilterFactory& operator=(const ConcreteExecListenerFilterFactory&);

    /**
     * @brief Instantiates a new ExecListenerFilter of the appropriate type.
     * @param xml The configuration XML for the instantiated filter.
     * @return The Id for the new ExecListenerFilter.
     */

    ExecListenerFilterId create(const pugi::xml_node& xml) const
    {
      ExecListenerFilterId result = (new FilterType(xml))->getId();
      return result;
    }
  };

#define REGISTER_EXEC_LISTENER_FILTER(CLASS,NAME) {new PLEXIL::ConcreteExecListenerFilterFactory<CLASS>(PLEXIL::LabelStr(NAME));}

} // namespace PLEXIL

#endif // EXEC_LISTENER_FILTER_FACTORY_H
