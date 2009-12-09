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

#include "EmptyNodeStateManager.hh"
#include "Debug.hh"
#include "Node.hh"
#include "Expression.hh"
#include "CoreExpressions.hh"

namespace PLEXIL
{
  class EmptyNodeExecutingStateComputer : public StateComputer 
  {
  public:
    EmptyNodeExecutingStateComputer() : StateComputer()
    {}
    const LabelStr& getDestState(NodeId& node)
    {
      checkError(node->getType() == Node::EMPTY(),
		 "Expected empty node, got " <<
		 node->getType().toString());
      checkError(node->getState() == StateVariable::EXECUTING(),
		 "Node " << node->getNodeId().toString() << " in state " <<
		 node->getState().toString() << " not EXECUTING.");
      checkError(node->pairActive(Node::ANCESTOR_INVARIANT_CONDITION()),
		 "Ancestor invariant for " << node->getNodeId().toString() << " is inactive.");
      checkError(node->pairActive(Node::INVARIANT_CONDITION()),
		 "Invariant for " << node->getNodeId().toString() << " is inactive.");
      checkError(node->pairActive(Node::END_CONDITION()),
		 "End for " << node->getNodeId().toString() << " is inactive.");

      if (node->getCondition(Node::ANCESTOR_INVARIANT_CONDITION())->getValue() ==
          BooleanVariable::FALSE())
      {
         debugMsg("Node:getDestState", "Destination: FINISHED. Ancestor invariant false.");
         return StateVariable::FINISHED();
      }
      else if(node->getCondition(Node::INVARIANT_CONDITION())->getValue() ==
	      BooleanVariable::FALSE())
      {
         debugMsg("Node:getDestState", "Destination: ITERATION_ENDED.  Invariant false.");
         return StateVariable::ITERATION_ENDED();
      }
      else if(node->getCondition(Node::END_CONDITION())->getValue() ==
	      BooleanVariable::TRUE())
	{
	  debugMsg("Node:getDestState", "Destination: ITERATION_ENDED.  End condition true.");
	  return StateVariable::ITERATION_ENDED();
	}
      return StateVariable::NO_STATE();
    }
  };

  class EmptyNodeExecutingTransitionHandler : public TransitionHandler
  {
  public:
    EmptyNodeExecutingTransitionHandler() : TransitionHandler()
    {}

    void transitionFrom(NodeId& node, const LabelStr& destState) {
      checkError(node->getType() == Node::EMPTY(),
		 "Expected empty node, got " <<
		 node->getType().toString());
      checkError(node->getState() == StateVariable::EXECUTING(),
		 "In state '" << node->getState().toString() << "', not EXECUTING.");
      checkError(destState == StateVariable::FINISHED() ||
		 destState == StateVariable::ITERATION_ENDED(),
		 "Attempting to transition to invalid state '" << destState.toString() << "'");

      if (node->getCondition(Node::ANCESTOR_INVARIANT_CONDITION())->getValue() ==
          BooleanVariable::FALSE())
      {
         node->getOutcomeVariable()->setValue(OutcomeVariable::FAILURE());
         node->getFailureTypeVariable()->setValue(FailureVariable::PARENT_FAILED());
      }
      else if (node->getCondition(Node::INVARIANT_CONDITION())->getValue() ==
               BooleanVariable::FALSE())
      {
         node->getOutcomeVariable()->setValue(OutcomeVariable::FAILURE());
         node->getFailureTypeVariable()->setValue(FailureVariable::INVARIANT_CONDITION_FAILED());
      }
      else if(node->getCondition(Node::END_CONDITION())->getValue() == BooleanVariable::TRUE()) {
	if(node->getCondition(Node::POST_CONDITION())->getValue() == BooleanVariable::TRUE())
	  node->getOutcomeVariable()->setValue(OutcomeVariable::SUCCESS());
	else {
	  node->getOutcomeVariable()->setValue(OutcomeVariable::FAILURE());
	  node->getFailureTypeVariable()->setValue(FailureVariable::POST_CONDITION_FAILED());
	}
      }
      else {
	checkError(ALWAYS_FAIL, "Shouldn't get here.");
      }

      deactivatePair(node, Node::ANCESTOR_INVARIANT_CONDITION());
      deactivatePair(node, Node::INVARIANT_CONDITION());
      deactivatePair(node, Node::END_CONDITION());
      deactivatePair(node, Node::POST_CONDITION());
    }

    void transitionTo(NodeId& node, const LabelStr& destState)
    {
      checkError(node->getType() == Node::EMPTY(),
		 "Expected empty node, got " <<
		 node->getType().toString());

      checkError(destState == StateVariable::EXECUTING(),
		 "Attempting to transition to inavlid state '" << destState.toString() << "'.");

      activatePair(node, Node::ANCESTOR_INVARIANT_CONDITION());
      activatePair(node, Node::INVARIANT_CONDITION());
      activatePair(node, Node::END_CONDITION());
      activatePair(node, Node::POST_CONDITION());
      node->findVariable(Node::STATE())->setValue(destState);
    }
  };

  EmptyNodeStateManager::EmptyNodeStateManager() : DefaultStateManager()
  {
    addStateComputer(StateVariable::EXECUTING(), (new EmptyNodeExecutingStateComputer())->getId());
    addTransitionHandler(StateVariable::EXECUTING(),
			 (new EmptyNodeExecutingTransitionHandler())->getId());
  }
}