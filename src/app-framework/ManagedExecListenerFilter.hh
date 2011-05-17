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

#ifndef _H_ManagedExecListenerFilter
#define _H_ManagedExecListenerFilter

#include "ExecListenerFilter.hh"

class TiXmlElement;

namespace PLEXIL {

  class InterfaceManagerBase;

  /**
   * @brief An abstract base class, derived from ExecListenerFilter, which allows
   *        automatic instantiation from configuration data by ManagedExecListener.
   * @see Class ExecListenerFilter
   */
  class ManagedExecListenerFilter :
    public ExecListenerFilter
  {
  public:
    /**
     * @brief Constructor from configuration XML.
     */
    ManagedExecListenerFilter(const TiXmlElement* xml, InterfaceManagerBase & mgr);

    /**
     * @brief Destructor from configuration XML.
     */
    virtual ~ManagedExecListenerFilter();

    /**
     * @brief Get the configuration XML of this instance.
     * @return A pointer to the XML element.
     */
    const TiXmlElement* getXml() const
    {
      return m_xml;
    }

    /**
     * @brief Get the interface manager.
     * @return A reference to the manager.
     */
    InterfaceManagerBase& getManager() const
    {
      return m_manager;
    }

  private:
    //
    // Deliberately unimplemented
    //
    ManagedExecListenerFilter();
    ManagedExecListenerFilter(const ManagedExecListenerFilter &);
    ManagedExecListenerFilter& operator=(const ManagedExecListenerFilter &);

    //
    // Member variables
    //

    /**
     * @brief The configuration XML used at construction time.
     */
    const TiXmlElement* m_xml;

    /**
     * @brief The InterfaceManager instance that owns (the listener which owns) this filter.
     * @note Provided for access to the Exec core and other shared data.
     */
    InterfaceManagerBase & m_manager;
  };

} // Namespace


#endif