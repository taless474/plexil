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

#ifndef _H_PlexilResource
#define _H_PlexilResource

#include "Id.hh"

namespace PLEXIL {

  class PlexilResource;
  class PlexilExpr;
  
  DECLARE_ID(PlexilResource);
  DECLARE_ID(PlexilExpr);

  //* The data structure to hold the resource specification.
  typedef std::map<std::string, PlexilExprId> PlexilResourceMap;

  /**
   * @brief A class to represent resource specifications in commands.
   */

  class PlexilResource
  {
  public:
	//* Constructor.
    PlexilResource() : m_id(this)
	{}

	//* Destructor.
    ~PlexilResource()
	{
	  m_id.remove();
	}

    const PlexilResourceId& getId() const 
	{
	  return m_id;
	}

    void addResource(const std::string& tag, const PlexilExprId& resource)
    {
      m_resource[tag] = resource;
    }
    
    const PlexilResourceMap& getResourceMap() const
    {
      return m_resource;
    }

  private:
    PlexilResourceId m_id;
    PlexilResourceMap m_resource;
  };

}

#endif // _H_PlexilResource
