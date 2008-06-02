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

#ifndef _H_PlexilPlan
#define _H_PlexilPlan

#include <list>
#include <vector>
#include <map>
#include <string>
#include <set>
#include "Id.hh"
#include "LabelStr.hh"
#include "ExecDefs.hh"
#include "PlexilResource.hh"

#define isType(p,type) (dynamic_cast<type*>(p) != NULL)

namespace PLEXIL {

  class PlexilInterface;
  class PlexilVar;
  class PlexilArrayVar;
  class PlexilValue;
  class PlexilNodeBody;
  class PlexilVarRef;
  class PlexilNode;
  class PlexilState;
  class PlexilUpdate;
  class PlexilNodeRef;
  class PlexilInternalVar;

  DECLARE_ID(PlexilNode);
  DECLARE_ID(PlexilInterface);
  DECLARE_ID(PlexilState);
  DECLARE_ID(PlexilVar);
  DECLARE_ID(PlexilArrayVar);
  DECLARE_ID(PlexilNodeBody);
  DECLARE_ID(PlexilUpdate);
  DECLARE_ID(PlexilNodeRef);
  DECLARE_ID(PlexilInternalVar);

  typedef std::vector<const PlexilNodeId*> PlexilNodeIdSet;
  typedef std::map<double, PlexilExprId>  PlexilAliasMap;

  class PlexilParser {
  public:
    virtual PlexilNodeId parse() = 0;
    virtual ~PlexilParser(){}
  };

  class PlexilNode {
  public:
    PlexilNode();
    ~PlexilNode();

    const PlexilNodeId& getId() const {return m_id;}
    const std::string& nodeId() const {return m_nodeId;}
    const std::string& nodeType() const {return m_nodeType;}
    double priority() const {return m_priority;}
    const PlexilInterfaceId& interface() const {return m_intf;}
    const std::vector<PlexilVarId>& declarations() const {return m_declarations;}
    const std::map<std::string, PlexilExprId>& conditions() const {return m_conditions;}
    const std::string& permissions() const {return m_permissions;}
    const PlexilNodeBodyId& body() const {return m_nodeBody;}

    void setNodeId(const std::string& id) {m_nodeId = id;}
    void setNodeType(const std::string& type) {m_nodeType = type;}
    void setPriority(double priority) {m_priority = priority;}
    void setPermissions(const std::string& permissions) {m_permissions = permissions;}
    void addVariable(const PlexilVarId& var) {m_declarations.push_back(var);}
    void addCondition(const std::string& name, const PlexilExprId& expr)
    {m_conditions.insert(std::make_pair(name, expr));}
    void setBody(const PlexilNodeBodyId& body) {m_nodeBody = body;}
    void setInterface(const PlexilInterfaceId& intf) { m_intf = intf;}

    /**
     * @brief Recurse into given node and ink library calls found there in.
     */
    void link(const std::vector<PlexilNodeId>& libraries);
    void link(const std::vector<PlexilNodeId>& libraries, PlexilNodeIdSet& seen);

  private:
    PlexilNodeId m_id;
    std::string m_nodeId;
    std::string m_nodeType;
    std::string m_permissions;
    double m_priority;
    PlexilInterfaceId m_intf;
    std::vector<PlexilVarId> m_declarations;
    std::map<std::string, PlexilExprId> m_conditions;
    PlexilNodeBodyId m_nodeBody;
  };
  
  class PlexilInterface
  {
     public:
        PlexilInterface() : m_id(this) {}
        ~PlexilInterface();
        const PlexilInterfaceId& getId() const {return m_id;}
        const std::vector<PlexilVarRef*>& in() const {return m_in;}
        const std::vector<PlexilVarRef*>& inOut() const {return m_inOut;}
        const PlexilVarRef* findVar(const PlexilVarRef* target);
        const PlexilVarRef* findInVar(const PlexilVarRef* target);
        const PlexilVarRef* findInOutVar(const PlexilVarRef* target);
        const PlexilVarRef* findVar(const std::string& target);
        const PlexilVarRef* findInVar(const std::string& target);
        const PlexilVarRef* findInOutVar(const std::string& target);
        
        void addIn(PlexilVarRef* var) {m_in.push_back(var);}
        void addInOut(PlexilVarRef* var) {m_inOut.push_back(var);}
        //void addIn(PlexilVarId& var) {m_in_.push_back(var);}
        //void addInOut(PlexilVarId& var) {m_inOut_.push_back(var);}

     private:
        PlexilInterfaceId m_id;
//        std::vector<PlexilVarId>   m_in_;
//        std::vector<PlexilVarId>   m_inOut_;
        std::vector<PlexilVarRef*> m_in;
        std::vector<PlexilVarRef*> m_inOut;
  };

  class PlexilExpr {
  public:
    PlexilExpr() : m_id(this){}
    virtual ~PlexilExpr() {m_id.remove();}
    const PlexilExprId& getId() const {return m_id;}
    const std::string& name() const {return m_name;}
    const std::vector<PlexilExprId>& subExprs() const {return m_subExprs;}

    void setName(const std::string& name)  {m_name = name;}
    void addSubExpr(PlexilExprId expr) {m_subExprs.push_back(expr);}
  private:
    PlexilExprId m_id;
    std::string m_name;
    std::vector<PlexilExprId> m_subExprs;
  };

   class VarType : public std::string
   {
      public:
         VarType(const char * type) : std::string(type) {}
         VarType(const std::string& type) : std::string(type) {}
         VarType() : std::string("unknown") {}
         PlexilType plexilType() const;
   };

   class PlexilVarRef : public PlexilExpr
   {
      public:
         PlexilVarRef() : PlexilExpr(), m_typed(false),
                          m_defaultValue(PlexilExprId::noId()) {}
         bool typed() const {return m_typed;}
         const VarType& type() const {return m_type;}
         const PlexilExprId& defaultValue() const {return m_defaultValue;}
         
         void setDefaultValue(const PlexilExprId& defaultValue)
         {m_defaultValue = defaultValue;}
         void setType(const std::string& type) {setType(VarType(type));}
         void setType(const VarType& type) {m_type = type; m_typed = true;}
      private:
         bool m_typed;
         VarType m_type;
         PlexilExprId m_defaultValue;
   };

  class PlexilOp : public PlexilExpr {
  public:
    PlexilOp() : PlexilExpr() {}
    const std::string& getOp() const {return m_op;}

    void setOp(const std::string& op) {m_op = op; setName(op);}
  private:
    std::string m_op;
  };

  class PlexilArrayElement : public PlexilExpr {
  public:
    PlexilArrayElement();

    const std::string& getArrayName() const
    {
      return m_arrayName;
    }

    void setArrayName(const std::string& name);

  private:
    std::string m_arrayName;
  };

  class PlexilState {
  public:
    PlexilState() : m_id(this) {}
   ~PlexilState() {m_id.remove(); m_nameExpr.remove();}
    const PlexilStateId& getId() const {return m_id;}
    const std::vector<PlexilExprId>& args() const {return m_args;}
    const std::string& name() const;
    const PlexilExprId& nameExpr() const {return m_nameExpr;}

    void addArg(const PlexilExprId& arg) {m_args.push_back(arg);}
    void setName(const std::string& name);
    void setNameExpr(const PlexilExprId& nameExpr) {m_nameExpr = nameExpr;}
  private:
    PlexilStateId m_id;
    PlexilExprId m_nameExpr;
    std::vector<PlexilExprId> m_args;
  };

  class PlexilLookup : public PlexilExpr {
  public:
    PlexilLookup() : PlexilExpr() {}

    const PlexilStateId& state() const {return m_state;}
    void setState(const PlexilStateId& state) {m_state = state;}
  private:
    PlexilStateId m_state;
  };


  class PlexilLookupNow : public PlexilLookup {
  public:
    PlexilLookupNow() : PlexilLookup() {setName("LookupNow");}
  };

  class PlexilChangeLookup : public PlexilLookup {
  public:
    PlexilChangeLookup() : PlexilLookup() {setName("LookupOnChange");}
    const std::vector<PlexilExprId>& tolerances() const {return m_tolerances;}
    void addTolerance(const PlexilExprId& tolerance) {m_tolerances.push_back(tolerance);}
  private:
    std::vector<PlexilExprId> m_tolerances;
  };

  class PlexilFrequencyLookup : public PlexilLookup {
  public:
    PlexilFrequencyLookup()
      : PlexilLookup() {setName("LookupWithFrequency");}
    const PlexilExprId& lowFreq() const {return m_lowFreq;}
    const PlexilExprId& highFreq() const {return m_highFreq;}

    void setLowFreq(PlexilExprId freq) {m_lowFreq = freq;}
    void setHighFreq(PlexilExprId freq) {m_highFreq = freq;}
  private:
    PlexilExprId m_lowFreq;
    PlexilExprId m_highFreq;
  };

  class PlexilValue : public PlexilExpr {
  public:
    PlexilValue(const VarType& type, const std::string& value = "UNKNOWN");
    const VarType& type() const {return m_type;}
    const std::string& value() const {return m_value;}
  private:
    VarType m_type;
    std::string m_value;
  };

   class PlexilArrayValue : public PlexilValue
   {
      public:
         PlexilArrayValue(const VarType& type, unsigned maxSize,
                          const std::vector<std::string>& values);
         const std::vector<std::string>& values() const {return m_values;}
         unsigned maxSize() const {return m_maxSize;}
      private:
         unsigned m_maxSize;
         std::vector<std::string> m_values;
   };

  class PlexilVar {
  public:
    PlexilVar(const std::string& name, const VarType& type,
	      const std::string& value = "UNKNOWN");
    PlexilVar(const std::string& name, const VarType& type,
	      PlexilValue* value);
    virtual ~PlexilVar();
    const PlexilVarId& getId() const {return m_id;}
    const std::string& name() const {return m_name;}
    const VarType& type() const {return value()->type();}
    PlexilValue* value() const {return m_value;}
  private:
    PlexilVarId m_id;
    std::string m_name;
    PlexilValue* m_value;
  };
  
   class PlexilArrayVar : public PlexilVar
   {
      public:
         PlexilArrayVar(const std::string& name, 
                        const VarType& type, 
                        const unsigned maxSize, 
                        std::vector<std::string>& values);
         ~PlexilArrayVar();
         const VarType& type() const {return m_type;}
         //const std::vector<PlexilValue*>& values() const {return m_values;}
      private:
         VarType m_type;
         unsigned m_maxSize;
   };
  
  class PlexilNodeBody {
  public:
    PlexilNodeBody() : m_id(this) {}
    virtual ~PlexilNodeBody() {m_id.remove();}
    const PlexilNodeBodyId& getId() const {return m_id;}
  private:
    PlexilNodeBodyId m_id;
  };

  class PlexilActionBody : public PlexilNodeBody {
  public:
    PlexilActionBody() : PlexilNodeBody() {}
    const std::vector<PlexilExpr*>& dest() const {return m_dest;}

    void addDestVar(PlexilExpr* ref) {m_dest.push_back(ref);}
  private:
    std::vector<PlexilExpr*> m_dest;
  };

  //rhs may need to end up being a list!
  class PlexilAssignmentBody : public PlexilActionBody {
  public:
    PlexilAssignmentBody() : PlexilActionBody() {}
    const PlexilExprId& RHS() const {return m_rhs;}
    const VarType& type() const {return m_type;}

    void setRHS(const PlexilExprId& rhs) {m_rhs = rhs;}
    void setType(VarType& type) {m_type = type;}
  private:
    PlexilExprId m_rhs;
    VarType m_type;
  };

  class PlexilCommandBody : public PlexilActionBody {
  public:
    PlexilCommandBody() : PlexilActionBody() {}
    const PlexilStateId& state() const {return m_state;}
    const std::vector<PlexilResourceId>& getResource() const {return m_resource;}

    void setState(const PlexilStateId& state) {m_state = state;}
    void setResource(const std::vector<PlexilResourceId>& resource) {m_resource=resource;}
  private:
    PlexilStateId m_state;
    std::vector<PlexilResourceId> m_resource;
  };

  class PlexilFunctionCallBody : public PlexilActionBody {
  public:
    PlexilFunctionCallBody() : PlexilActionBody() {}
    const PlexilStateId& state() const {return m_state;}

    void setState(const PlexilStateId& state) {m_state = state;}
  private:
    PlexilStateId m_state;
  };

  class PlexilNodeRef {
  public:
    enum Direction {
      SELF = 0,
      PARENT,
      CHILD,
      SIBLING,
      NO_DIR
    };
    PlexilNodeRef() :  m_id(this), m_dir(NO_DIR) {}
    ~PlexilNodeRef() {m_id.remove();}
    const PlexilNodeRefId& getId() const {return m_id;}
    const Direction& dir() const {return m_dir;}
    const std::string& name() const {return m_name;}

    void setDir(const Direction& dir) {m_dir = dir;}
    void setName(const std::string& name) {m_name = name;}
  private:
    PlexilNodeRefId m_id;
    Direction m_dir;
    std::string m_name;
  };

  class PlexilInternalVar : public PlexilVarRef {
  public:
    PlexilInternalVar() : PlexilVarRef() {}
    const PlexilNodeRefId& ref() const {return m_ref;}

    void setRef(const PlexilNodeRefId& ref) {m_ref = ref;}
  private:
    PlexilNodeRefId m_ref;
  };

  class PlexilOutcomeVar : public PlexilInternalVar {
  public:
    PlexilOutcomeVar() : PlexilInternalVar() {setName("outcome");}
  };

  class PlexilFailureVar : public PlexilInternalVar {
  public:
    PlexilFailureVar() : PlexilInternalVar() {setName("failure_type");}
  };

  class PlexilStateVar : public PlexilInternalVar {
  public:
    PlexilStateVar() : PlexilInternalVar() {setName("state");}
  };

  class PlexilCommandHandleVar : public PlexilInternalVar {
  public:
    PlexilCommandHandleVar() : PlexilInternalVar() {setName("command_handle");}
  };

  class PlexilTimepointVar : public PlexilInternalVar {
  public:
    PlexilTimepointVar() : PlexilInternalVar() {setName("@Timepoint");}
    const std::string& state() const {return m_state;}
    const std::string& timepoint() const {return m_timepoint;}

    void setState(const std::string& state) {m_state = state;}
    void setTimepoint(const std::string& timepoint) {m_timepoint = timepoint;}
  private:
    std::string m_state;
    std::string m_timepoint;
  };

// PlexilFunctionBody : public PlexilActionBody {
//   PlexilState function();
// }

  class PlexilUpdate {
  public:
    PlexilUpdate() : m_id(this) {}
    ~PlexilUpdate() {m_id.remove();}
    const PlexilUpdateId& getId() const {return m_id;}
    const std::vector<std::pair<std::string, PlexilExprId> >& pairs() const {return m_map;}

    void addPair(const std::string& name, const PlexilExprId& value)
    { m_map.push_back(std::make_pair(name, value));}
  private:
    PlexilUpdateId m_id;
    std::vector<std::pair<std::string, PlexilExprId> > m_map;
  };


  class PlexilUpdateBody : public PlexilNodeBody {
  public:
    PlexilUpdateBody() : PlexilNodeBody() {}
    const PlexilUpdateId& update() const {return m_update;}

    void setUpdate(const PlexilUpdateId& update) {m_update = update;}
  private:
    PlexilUpdateId m_update;
  };

  class PlexilRequestBody : public PlexilNodeBody {
  public:
    PlexilRequestBody() : PlexilNodeBody() {}
    const PlexilNodeRefId& parent() const {return m_parent;}
    const PlexilUpdateId& update() const {return m_update;}

    void setParent(PlexilNodeRefId parent) {m_parent = parent;}
    void setUpdate(PlexilUpdateId update) {m_update = update;}
  private:
    PlexilNodeRefId m_parent;
    PlexilUpdateId m_update;
  };

  class PlexilListBody : public PlexilNodeBody 
  {
     public:
        PlexilListBody() : PlexilNodeBody() {}
        void addChild(const PlexilNodeId& child) {m_children.push_back(child);}
        bool replaceChild(const PlexilNodeId& oldChild,
                                   const PlexilNodeId& newChild)
        {
           for(std::vector<PlexilNodeId>::iterator child = m_children.begin();
               child != m_children.end(); ++child)
           {
              if (*child == oldChild)
              {
                 *child = newChild;
                 return true;
              }
           }
           return false;
        }
        const std::vector<PlexilNodeId>& children() const {return m_children;}
     private:
        std::vector<PlexilNodeId> m_children;
  };
      // library node call body

  class PlexilLibNodeCallBody : public PlexilNodeBody 
  {
     public:

           // construct a library node call body with the name of the
           // library node
        
        PlexilLibNodeCallBody(std::string libNodeName) : PlexilNodeBody() 
        {
           setLibNodeName(libNodeName);
        }
           // getter for library node name

        const std::string& libNodeName() const {return m_libNodeName;}

           // getter for library node

        const PlexilNodeId& libNode() const {return m_libNode;}

           // getter for all aliases
        
        const PlexilAliasMap& aliases() const
        {
           return m_aliases;
        }
           // add an alias pair to the library call

        void addAlias(std::string param, PlexilExprId value)
        {
           PlexilExprId &alias = m_aliases[LabelStr(param)];
           checkError(!alias.isId(), "Alias '" << param
                      << "' apears more then once in call to "
                      << m_libNodeName);
           alias = value;
        }
           // setter for library node name

        void setLibNodeName(std::string libNodeName) 
        {
           m_libNodeName = libNodeName;
        }
           // setter for the called library node

        void setLibNode(const PlexilNodeId& libNode) {m_libNode = libNode;}

     private:
        std::string m_libNodeName;
        PlexilNodeId m_libNode;
        PlexilAliasMap m_aliases;
  };
}
#endif
