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

#ifndef _H_ExecListener
#define _H_ExecListener

#include "Id.hh"
#include "LabelStr.hh"

// Forward reference w/o namespace
class TiXmlElement;

namespace PLEXIL
{
  // Forward references
  class Node;
  typedef Id<Node> NodeId;
  class PlexilNode;
  typedef Id<PlexilNode> PlexilNodeId;
  class ExecListener;
  typedef Id<ExecListener> ExecListenerId;

  /**
   * @brief An abstract base class for notifying external agents about exec state changes.
   */
  class ExecListener 
  {
  public:

    /**
     * @brief Default constructor.
     */
    ExecListener();

    /**
     * @brief Constructor from configuration XML.
     */
    ExecListener(const TiXmlElement* xml);

    /**
     * @brief Destructor.
     */
    virtual ~ExecListener();

    //
    // API to be implemented by derived classes
    //

    /**
     * @brief Perform listener-specific initialization.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual void initialize() {}

    /**
     * @brief Perform listener-specific startup.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual void start() {}

    /**
     * @brief Perform listener-specific actions to stop.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual void stop() {}

    /**
     * @brief Perform listener-specific actions to reset to initialized state.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual void reset() {}

    /**
     * @brief Perform listener-specific actions to shut down.
     * @note Default method provided as a convenience for backward compatibility.
     */
    virtual void shutdown() {}

    /**
     * @brief Notify that a node has changed state.
     * @param prevState The old state.
     * @param node The node that has transitioned.
     * @note The current state is accessible via the node.
     */
    virtual void notifyOfTransition(const LabelStr& prevState, const NodeId& node) const = 0;

    /**
     * @brief Notify that a plan has been received by the Exec.
     * @param plan The intermediate representation of the plan.
     * @param parent The name of the parent node under which this plan will be inserted.
     */
    virtual void notifyOfAddPlan(const PlexilNodeId& plan, const LabelStr& parent) const = 0;

    /**
     * @brief Notify that a library node has been received by the Exec.
     * @param libNode The intermediate representation of the plan.
     * @note The default method is deprecated and will go away in a future release.
     */
    virtual void notifyOfAddLibrary(const PlexilNodeId& libNode) const /* = 0 */ ;

    //not sure if anybody wants this
    //void notifyOfConditionChange(const NodeId& node, const LabelStr& condition, const bool value) {}

    /**
     * @brief Get the ID of this instance.
     * @return The ID.
     */
    inline const ExecListenerId& getId() const
    { 
      return m_id; 
    }

    /**
     * @brief Get the configuration XML of this instance.
     * @return A pointer to the XML element.
     */
    inline const TiXmlElement* getXml() const
    { 
      return m_xml; 
    }

  protected:

    //
    // Member variables for use by subclasses
    //

    /**
     * @brief The configuration XML used at construction time.
     */
    const TiXmlElement* m_xml;

  private:

    /**
     * @brief The ID of this instance.
     */
    ExecListenerId m_id;
  };
}

#endif // _H_ExecListener
