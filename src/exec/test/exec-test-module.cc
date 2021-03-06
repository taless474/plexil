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

#include "exec-test-module.hh"

#include "Array.hh"
#include "BooleanVariable.hh"
#include "Calculables.hh"
#include "CoreExpressions.hh"
#include "Debug.hh"
#include "ExecConnector.hh"
#include "ExecDefs.hh"
#include "Expression.hh"
#include "ExpressionFactory.hh"
#include "Expressions.hh"
#include "ExternalInterface.hh"
#include "Lookup.hh"
#include "Node.hh"
#include "NodeFactory.hh"
#include "PlexilExec.hh"
#include "PlexilPlan.hh"
#include "StateCache.hh"
#include "TestSupport.hh"
#include "Variables.hh"
#include "XMLUtils.hh"

#include <iostream>
#include <map>
#include <cmath>

using namespace PLEXIL;

class AllocationExpression : public Expression {
public:
  AllocationExpression(PlexilExpr* /* expr */, const NodeId /* node */) : Expression() {
    setValue(10);
  }
  void print(std::ostream& s) const { s << "AllocationExpression"; }
private:
  bool checkValue(const double /* val */){return true;}
};

class TestListener : public ExpressionListener {
public:
  TestListener(bool& changed) : ExpressionListener(), m_changed(changed) {}
  void notifyValueChanged(const ExpressionId& /* expression */) { m_changed = true;}
private:
  bool& m_changed;
};

class BaseExpressionTest {
public:
  static bool test() {
    runTest(testBasicAllocation);
    runTest(testPublication);
    runTest(testLocking);
    return true;
  }
private:
  static bool testBasicAllocation() {
    PlexilValue data(PLEXIL::INTEGER);
    data.setName("foo");
    ExpressionId exp = ExpressionFactory::createInstance(LabelStr("AllocationExpression"),
                             data.getId());
    assertTrue(exp.isValid());
    assertTrue(!exp->isActive());
    exp->activate();
    assertTrue(exp->getValue() == 10);
    assertTrue(exp->toString() == "AllocationExpression");

    delete (Expression*) exp;

    return true;
  }
  static bool testPublication() {
    PlexilValue data(PLEXIL::INTEGER);
    data.setName("foo");
    ExpressionId exp =
      ExpressionFactory::createInstance(LabelStr("AllocationExpression"), data.getId());
    assertTrue(exp.isValid());
    if (exp->isActive())
      exp->deactivate();
    bool changed = false;
    ExpressionListenerId foo = (new TestListener(changed))->getId();
    assertTrue(foo.isValid());
    assertTrue(!foo->isActive());
    exp->addListener(foo);
    foo->activate();
    assertTrue(foo->isActive());
    //assertTrue(!foo->isActive());
    exp->setValue(10);
    assertTrue(!changed);
    exp->activate();
    assertTrue(exp->isActive());
    assertTrue(foo->isActive());
    exp->setValue(20);
    assertTrue(changed);

    exp->removeListener(foo);
    delete (ExpressionListener*) foo;
    delete (Expression*) exp;
    return true;
  }
  static bool testLocking() {
    BooleanVariable var;
    var.activate();
    bool changed = false;
    ExpressionListenerId listener = (new TestListener(changed))->getId();
    listener->activate();
    var.addListener(listener);

    //active, unlocked
    var.setValue(BooleanVariable::TRUE_VALUE());
    assertTrue(changed);
    assertTrue(var.getValue() == BooleanVariable::TRUE_VALUE());

    //inactive, unlocked
    changed = false;
    var.deactivate();
    var.setValue(BooleanVariable::FALSE_VALUE());
    assertTrue(!changed);
    assertTrue(var.getValue() == BooleanVariable::UNKNOWN());

    //active, locked
    var.activate();
    assertTrue(!changed);
    var.lock();
    var.setValue(BooleanVariable::TRUE_VALUE());
    assertTrue(!changed);
    assertTrue(var.getValue() == BooleanVariable::FALSE_VALUE());
    var.unlock();
    assertTrue(changed);
    assertTrue(var.getValue() == BooleanVariable::TRUE_VALUE());

    var.removeListener(listener);
    delete (ExpressionListener*) listener;
    return true;
  }
};

class CoreExpressionsTest {
public:
  static bool test() {
    runTest(testLogicalNegation);
    runTest(testConjunction);
    runTest(testDisjunction);
    runTest(testExclusiveDisjunction);
    runTest(testConcatenation);
    runTest(testEquality);
    runTest(testInequality);

    runTest(testIntegerEquality);
    runTest(testIntegerInequality);
    runTest(testIntegerLessThan);
    runTest(testIntegerLessEqual);
    runTest(testIntegerGreaterThan);
    runTest(testIntegerGreaterEqual);

    runTest(testIntegerAddition);
    runTest(testIntegerSubtraction);
    runTest(testIntegerMultiplication);
    runTest(testIntegerDivision);
    runTest(testIntegerMaximum);
    runTest(testIntegerMinimum);

    return true;
  }
private:
  static bool testConjunction() {
    //test basic three-valued AND semantics
    Conjunction c1(BooleanVariable::FALSE_EXP(), BooleanVariable::FALSE_EXP());
    c1.activate();
    Conjunction c2(BooleanVariable::FALSE_EXP(), BooleanVariable::UNKNOWN_EXP());
    c2.activate();
    Conjunction c3(BooleanVariable::FALSE_EXP(), BooleanVariable::TRUE_EXP());
    c3.activate();
    Conjunction c4(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::FALSE_EXP());
    c4.activate();
    Conjunction c5(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::UNKNOWN_EXP());
    c5.activate();
    Conjunction c6(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::TRUE_EXP());
    c6.activate();
    Conjunction c7(BooleanVariable::TRUE_EXP(), BooleanVariable::FALSE_EXP());
    c7.activate();
    Conjunction c8(BooleanVariable::TRUE_EXP(), BooleanVariable::UNKNOWN_EXP());
    c8.activate();
    Conjunction c9(BooleanVariable::TRUE_EXP(), BooleanVariable::TRUE_EXP());
    c9.activate();
    assertTrue(c1.getValue() == BooleanVariable::FALSE_VALUE());
    assertTrue(c2.getValue() == BooleanVariable::FALSE_VALUE());
    assertTrue(c3.getValue() == BooleanVariable::FALSE_VALUE());
    assertTrue(c4.getValue() == BooleanVariable::FALSE_VALUE());
    assertTrue(c5.getValue() == BooleanVariable::UNKNOWN());
    assertTrue(c6.getValue() == BooleanVariable::UNKNOWN());
    assertTrue(c7.getValue() == BooleanVariable::FALSE_VALUE());
    assertTrue(c8.getValue() == BooleanVariable::UNKNOWN());
    assertTrue(c9.getValue() == BooleanVariable::TRUE_VALUE());

    //test proper responses to changes in subexpressions
    ExpressionId expr1 = (new BooleanVariable())->getId();
    ExpressionId expr2 = (new BooleanVariable())->getId();
    expr1->activate();
    expr2->activate();
    bool changed = false;
    ExpressionListenerId listener = (new TestListener(changed))->getId();
    listener->activate();

    ExpressionId c10 = (new Conjunction(expr1, expr2))->getId();
    c10->addListener(listener);
    c10->activate();

    assertTrue(c10->getValue() == BooleanVariable::UNKNOWN()); //conjunction is unknown
    expr1->setValue(BooleanVariable::FALSE_VALUE()); //this should make it false
    assertTrue(changed);
    assertTrue(c10->getValue() == BooleanVariable::FALSE_VALUE());
    changed = false;
    expr2->setValue(BooleanVariable::TRUE_VALUE()); //this should have no effect
    assertTrue(!changed);
    assertTrue(c10->getValue() == BooleanVariable::FALSE_VALUE());
    changed = false;
    expr1->setValue(BooleanVariable::TRUE_VALUE()); //this should make it true
    assertTrue(changed);
    assertTrue(c10->getValue() == BooleanVariable::TRUE_VALUE());

    c10->removeListener(listener);
    delete (Conjunction*) c10;
    delete (BooleanVariable*) expr1;
    delete (BooleanVariable*) expr2;
    delete (TestListener*) listener;

    return true;
  }

  static bool testDisjunction()
  {
    //test basic three-valued OR semantics
    Disjunction d1(BooleanVariable::FALSE_EXP(), BooleanVariable::FALSE_EXP());
    Disjunction d2(BooleanVariable::FALSE_EXP(), BooleanVariable::UNKNOWN_EXP());
    Disjunction d3(BooleanVariable::FALSE_EXP(), BooleanVariable::TRUE_EXP());
    Disjunction d4(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::FALSE_EXP());
    Disjunction d5(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::UNKNOWN_EXP());
    Disjunction d6(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::TRUE_EXP());
    Disjunction d7(BooleanVariable::TRUE_EXP(), BooleanVariable::FALSE_EXP());
    Disjunction d8(BooleanVariable::TRUE_EXP(), BooleanVariable::UNKNOWN_EXP());
    Disjunction d9(BooleanVariable::TRUE_EXP(), BooleanVariable::TRUE_EXP());
    d1.activate();
    assertTrue(d1.getValue() == BooleanVariable::FALSE_VALUE());
    d2.activate();
    assertTrue(d2.getValue() == BooleanVariable::UNKNOWN());
    d3.activate();
    assertTrue(d3.getValue() == BooleanVariable::TRUE_VALUE());
    d4.activate();
    assertTrue(d4.getValue() == BooleanVariable::UNKNOWN());
    d5.activate();
    assertTrue(d5.getValue() == BooleanVariable::UNKNOWN());
    d6.activate();
    assertTrue(d6.getValue() == BooleanVariable::TRUE_VALUE());
    d7.activate();
    assertTrue(d7.getValue() == BooleanVariable::TRUE_VALUE());
    d8.activate();
    assertTrue(d8.getValue() == BooleanVariable::TRUE_VALUE());
    d9.activate();
    assertTrue(d9.getValue() == BooleanVariable::TRUE_VALUE());

    //test proper responses to changes in subexpressions

    ExpressionId expr1 = (new BooleanVariable())->getId();
    ExpressionId expr2 = (new BooleanVariable())->getId();
    expr1->activate();
    expr2->activate();
    bool changed = false;
    ExpressionListenerId listener = (new TestListener(changed))->getId();
    listener->activate();

    ExpressionId d10 = (new Disjunction(expr1, expr2))->getId();
    d10->activate();
    d10->addListener(listener);

    assertTrue(d10->getValue() == BooleanVariable::UNKNOWN());
    expr1->setValue(BooleanVariable::FALSE_VALUE()); //should have no effect
    assertTrue(!changed);
    assertTrue(d10->getValue() == BooleanVariable::UNKNOWN());
    changed = false;
    expr2->setValue(BooleanVariable::FALSE_VALUE()); //should be false
    assertTrue(changed);
    assertTrue(d10->getValue() == BooleanVariable::FALSE_VALUE());
    changed = false;
    expr1->setValue(BooleanVariable::TRUE_VALUE()); //should be true
    assertTrue(changed);
    assertTrue(d10->getValue() == BooleanVariable::TRUE_VALUE());

    d10->removeListener(listener);
    delete (Disjunction*) d10;
    delete (BooleanVariable*) expr1;
    delete (BooleanVariable*) expr2;
    delete (TestListener*) listener;

    return true;
  }

  static bool testExclusiveDisjunction()
  {
    //test basic three-valued XOR semantics
    ExclusiveDisjunction d1(BooleanVariable::FALSE_EXP(), BooleanVariable::FALSE_EXP());
    ExclusiveDisjunction d2(BooleanVariable::FALSE_EXP(), BooleanVariable::UNKNOWN_EXP());
    ExclusiveDisjunction d3(BooleanVariable::FALSE_EXP(), BooleanVariable::TRUE_EXP());
    ExclusiveDisjunction d4(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::FALSE_EXP());
    ExclusiveDisjunction d5(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::UNKNOWN_EXP());
    ExclusiveDisjunction d6(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::TRUE_EXP());
    ExclusiveDisjunction d7(BooleanVariable::TRUE_EXP(), BooleanVariable::FALSE_EXP());
    ExclusiveDisjunction d8(BooleanVariable::TRUE_EXP(), BooleanVariable::UNKNOWN_EXP());
    ExclusiveDisjunction d9(BooleanVariable::TRUE_EXP(), BooleanVariable::TRUE_EXP());
    d1.activate();
    assertTrue(d1.getValue() == BooleanVariable::FALSE_VALUE());
    d2.activate();
    assertTrue(d2.getValue() == BooleanVariable::UNKNOWN());
    d3.activate();
    assertTrue(d3.getValue() == BooleanVariable::TRUE_VALUE());
    d4.activate();
    assertTrue(d4.getValue() == BooleanVariable::UNKNOWN());
    d5.activate();
    assertTrue(d5.getValue() == BooleanVariable::UNKNOWN());
    d6.activate();
    assertTrue(d6.getValue() == BooleanVariable::UNKNOWN());
    d7.activate();
    assertTrue(d7.getValue() == BooleanVariable::TRUE_VALUE());
    d8.activate();
    assertTrue(d8.getValue() == BooleanVariable::UNKNOWN());
    d9.activate();
    assertTrue(d9.getValue() == BooleanVariable::FALSE_VALUE());

    //test proper responses to changes in subexpressions

    ExpressionId expr1 = (new BooleanVariable())->getId();
    ExpressionId expr2 = (new BooleanVariable())->getId();
    expr1->activate();
    expr2->activate();
    bool changed = false;
    ExpressionListenerId listener = (new TestListener(changed))->getId();
    listener->activate();

    ExpressionId d10 = (new ExclusiveDisjunction(expr1, expr2))->getId();
    d10->activate();
    d10->addListener(listener);

    assertTrue(d10->getValue() == BooleanVariable::UNKNOWN());
    expr1->setValue(BooleanVariable::FALSE_VALUE()); //should have no effect
    assertTrue(!changed);
    assertTrue(d10->getValue() == BooleanVariable::UNKNOWN());
    changed = false;
    expr2->setValue(BooleanVariable::FALSE_VALUE()); //should be false
    assertTrue(changed);
    assertTrue(d10->getValue() == BooleanVariable::FALSE_VALUE());
    changed = false;
    expr1->setValue(BooleanVariable::TRUE_VALUE()); //should be true
    assertTrue(changed);
    assertTrue(d10->getValue() == BooleanVariable::TRUE_VALUE());

    d10->removeListener(listener);
    delete (ExclusiveDisjunction*) d10;
    delete (BooleanVariable*) expr1;
    delete (BooleanVariable*) expr2;
    delete (TestListener*) listener;

    return true;
  }

private:
  static bool testConcatenation() {
   //test basic three-valued AND semantics
   LabelStr ls1("string one");
   LabelStr ls2("string too");
   LabelStr ls3("string onetoo");
   LabelStr ls4("toostring one");
   LabelStr ls5("");
   LabelStr ls6("string 23.45");
   LabelStr ls7("23.4523.45");
   LabelStr ls8("string ");
   LabelStr ls9("too");

   StringVariable s1 = std::string("string ");
   StringVariable s2 = std::string("one");
   StringVariable s3 = std::string("too");
   StringVariable s4 = std::string("");
   StringVariable s5 = std::string("23.45");
   
   ExpressionId expr1 = s1.getId();
    ExpressionId expr2 = s2.getId();
    ExpressionId expr3 = s3.getId();
    ExpressionId expr4 = s4.getId();
    ExpressionId expr5 = s5.getId();

    expr1->activate();
    expr2->activate();
    expr3->activate();
    expr4->activate();
    expr5->activate();

   Concatenation c1 (expr1, expr2);
   c1.activate();
   Concatenation c2 (expr1, expr3);
   c2.activate();
   Concatenation c3 (c1.getId(), expr3);
   c3.activate();
   Concatenation c4 (expr3, c1.getId());
   c4.activate();
   Concatenation c5 (expr4, expr4);
   c5.activate();
   Concatenation c6 (expr4, c1.getId());
   c6.activate();
   Concatenation c7 (c1.getId(), expr4);
   c7.activate();
   Concatenation c8 (expr1, expr5);
   c8.activate();
   Concatenation c9 (expr5, expr5);
   c9.activate();

    assertTrue(c1.getValue() == ls1.getKey());  
    assertTrue(c2.getValue() == ls2.getKey());
    assertTrue(c3.getValue() == ls3.getKey());
    assertTrue(c4.getValue() == ls4.getKey());
    assertTrue(c5.getValue() == ls5.getKey());
    assertTrue(c6.getValue() == ls1.getKey());
    assertTrue(c7.getValue() == ls1.getKey());
    assertTrue(c8.getValue() == ls6.getKey());
    assertTrue(c9.getValue() == ls7.getKey());

    // Test response to UNKNOWN sub-expression(s)
   Concatenation c10(expr5, Expression::UNKNOWN_EXP());
   c10.activate();
   Concatenation c11 (Expression::UNKNOWN_EXP(), expr3);
   c11.activate();
   Concatenation c12 (Expression::UNKNOWN_EXP(), Expression::UNKNOWN_EXP());
   c12.activate();

    assertTrue(c10.getValue() == Expression::UNKNOWN());
    assertTrue(c11.getValue() == Expression::UNKNOWN());
    assertTrue(c12.getValue() == Expression::UNKNOWN());

   //test proper responses to changes in subexpressions
    ExpressionId expr10 = (new StringVariable(std::string("")))->getId(); 
    ExpressionId expr11 = (new StringVariable(std::string("")))->getId();
    expr10->activate();
    expr11->activate();
    bool changed = false;
    ExpressionListenerId listener = (new TestListener(changed))->getId();
    listener->activate();

    ExpressionId c13 = (new Concatenation(expr10, expr11))->getId();
    c13->activate();
    c13->addListener(listener);
    assertTrue(c13->getValue() == ls5.getKey());
    changed = false;
    expr11->setValue(ls1.getKey()); 
    assertTrue(changed);
    assertTrue(c13->getValue() == ls1.getKey());
    changed = false;
    expr10->setValue(ls8.getKey());
    expr11->setValue(ls9.getKey());
    assertTrue(changed);
    assertTrue(c13->getValue() == ls2.getKey());

    c13->removeListener(listener);
    delete (Concatenation*) c13;
    delete (StringVariable*) expr10;
    delete (StringVariable*) expr11;
    delete (TestListener*) listener;

    return true;
  }

  static bool testLogicalNegation()
  {
    LogicalNegation n1(BooleanVariable::UNKNOWN_EXP());
    LogicalNegation n2(BooleanVariable::FALSE_EXP());
    LogicalNegation n3(BooleanVariable::TRUE_EXP());
    n1.activate();
    assertTrue(n1.getValue() == BooleanVariable::UNKNOWN());
    n2.activate();
    assertTrue(n2.getValue() == BooleanVariable::TRUE_VALUE());
    n3.activate();
    assertTrue(n3.getValue() == BooleanVariable::FALSE_VALUE());
    return true;
  }

  static bool testEquality() {
    Equality e1(BooleanVariable::TRUE_EXP(), BooleanVariable::FALSE_EXP());
    Equality e2(BooleanVariable::TRUE_EXP(), BooleanVariable::TRUE_EXP());
    Equality e3(BooleanVariable::TRUE_EXP(), BooleanVariable::UNKNOWN_EXP());
    Equality e4(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::UNKNOWN_EXP());

    e1.activate();
    assertTrue(e1.getValue() == BooleanVariable::FALSE_VALUE());
    e2.activate();
    assertTrue(e2.getValue() == BooleanVariable::TRUE_VALUE());
    e3.activate();
    assertTrue(e3.getValue() == BooleanVariable::UNKNOWN());
    e4.activate();
    assertTrue(e4.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testInequality()
  {
    Inequality i1(BooleanVariable::TRUE_EXP(), BooleanVariable::FALSE_EXP());
    Inequality i2(BooleanVariable::TRUE_EXP(), BooleanVariable::TRUE_EXP());
    Inequality i3(BooleanVariable::TRUE_EXP(), BooleanVariable::UNKNOWN_EXP());
    Inequality i4(BooleanVariable::FALSE_EXP(), BooleanVariable::UNKNOWN_EXP());
    Inequality i5(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::TRUE_EXP());
    Inequality i6(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::FALSE_EXP());
    Inequality i7(BooleanVariable::UNKNOWN_EXP(), BooleanVariable::UNKNOWN_EXP());

    i1.activate();
    assertTrue(i1.getValue() == BooleanVariable::TRUE_VALUE());
    i2.activate();
    assertTrue(i2.getValue() == BooleanVariable::FALSE_VALUE());
    i3.activate();
    assertTrue(i3.getValue() == BooleanVariable::UNKNOWN());
    i4.activate();
    assertTrue(i4.getValue() == BooleanVariable::UNKNOWN());
    i5.activate();
    assertTrue(i5.getValue() == BooleanVariable::UNKNOWN());
    i6.activate();
    assertTrue(i6.getValue() == BooleanVariable::UNKNOWN());
    i7.activate();
    assertTrue(i7.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  //
  // Integer arithmetic expressions
  //

  static bool testIntegerEquality()
  {
    Equality e1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    Equality e2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Equality e3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    Equality e4(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Equality e5(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Equality e6(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Equality e7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    e1.activate();
    assertTrue(e1.getValue() == BooleanVariable::TRUE_VALUE());
    e2.activate();
    assertTrue(e2.getValue() == BooleanVariable::FALSE_VALUE());
    e3.activate();
    assertTrue(e3.getValue() == BooleanVariable::FALSE_VALUE());
    e4.activate();
    assertTrue(e4.getValue() == BooleanVariable::TRUE_VALUE());
    e5.activate();
    assertTrue(e5.getValue() == BooleanVariable::UNKNOWN());
    e6.activate();
    assertTrue(e6.getValue() == BooleanVariable::UNKNOWN());
    e7.activate();
    assertTrue(e7.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerInequality()
  {
    Inequality i1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    Inequality i2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Inequality i3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    Inequality i4(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Inequality i5(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Inequality i6(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Inequality i7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    i1.activate();
    assertTrue(i1.getValue() == BooleanVariable::FALSE_VALUE());
    i2.activate();
    assertTrue(i2.getValue() == BooleanVariable::TRUE_VALUE());
    i3.activate();
    assertTrue(i3.getValue() == BooleanVariable::TRUE_VALUE());
    i4.activate();
    assertTrue(i4.getValue() == BooleanVariable::FALSE_VALUE());
    i5.activate();
    assertTrue(i5.getValue() == BooleanVariable::UNKNOWN());
    i6.activate();
    assertTrue(i6.getValue() == BooleanVariable::UNKNOWN());
    i7.activate();
    assertTrue(i7.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerLessThan()
  {
    LessThan l1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    LessThan l2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    LessThan l3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    LessThan l4(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    LessThan l5(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    LessThan l6(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());
    
    l1.activate();
    assertTrue(l1.getValue() == BooleanVariable::FALSE_VALUE());
    l2.activate();
    assertTrue(l2.getValue() == BooleanVariable::TRUE_VALUE());
    l3.activate();
    assertTrue(l3.getValue() == BooleanVariable::FALSE_VALUE());
    l4.activate();
    assertTrue(l4.getValue() == BooleanVariable::UNKNOWN());
    l5.activate();
    assertTrue(l5.getValue() == BooleanVariable::UNKNOWN());
    l6.activate();
    assertTrue(l6.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerLessEqual()
  {
    LessEqual l1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    LessEqual l2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    LessEqual l3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    LessEqual l4(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    LessEqual l5(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    LessEqual l6(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    l1.activate();
    assertTrue(l1.getValue() == BooleanVariable::TRUE_VALUE());
    l2.activate();
    assertTrue(l2.getValue() == BooleanVariable::TRUE_VALUE());
    l3.activate();
    assertTrue(l3.getValue() == BooleanVariable::FALSE_VALUE());
    l4.activate();
    assertTrue(l4.getValue() == BooleanVariable::UNKNOWN());
    l5.activate();
    assertTrue(l5.getValue() == BooleanVariable::UNKNOWN());
    l6.activate();
    assertTrue(l6.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerGreaterThan()
  {
    GreaterThan g1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    GreaterThan g2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    GreaterThan g3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    GreaterThan g4(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    GreaterThan g5(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    GreaterThan g6(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    g1.activate();
    assertTrue(g1.getValue() == BooleanVariable::FALSE_VALUE());
    g2.activate();
    assertTrue(g2.getValue() == BooleanVariable::FALSE_VALUE());
    g3.activate();
    assertTrue(g3.getValue() == BooleanVariable::TRUE_VALUE());
    g4.activate();
    assertTrue(g4.getValue() == BooleanVariable::UNKNOWN());
    g5.activate();
    assertTrue(g5.getValue() == BooleanVariable::UNKNOWN());
    g6.activate();
    assertTrue(g6.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerGreaterEqual()
  {
    GreaterEqual g1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    GreaterEqual g2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    GreaterEqual g3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    GreaterEqual g4(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    GreaterEqual g5(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    GreaterEqual g6(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    g1.activate();
    assertTrue(g1.getValue() == BooleanVariable::TRUE_VALUE());
    g2.activate();
    assertTrue(g2.getValue() == BooleanVariable::FALSE_VALUE());
    g3.activate();
    assertTrue(g3.getValue() == BooleanVariable::TRUE_VALUE());
    g4.activate();
    assertTrue(g4.getValue() == BooleanVariable::UNKNOWN());
    g5.activate();
    assertTrue(g5.getValue() == BooleanVariable::UNKNOWN());
    g6.activate();
    assertTrue(g6.getValue() == BooleanVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerAddition()
  {
    Addition a1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    Addition a2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Addition a3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    Addition a4(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Addition a5(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Addition a6(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Addition a7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    a1.activate();
    assertTrue(a1.getValue() == 0.0);
    a2.activate();
    assertTrue(a2.getValue() == 1.0);
    a3.activate();
    assertTrue(a3.getValue() == 1.0);
    a4.activate();
    assertTrue(a4.getValue() == 2.0);
    a5.activate();
    assertTrue(a5.getValue() == IntegerVariable::UNKNOWN());
    a6.activate();
    assertTrue(a6.getValue() == IntegerVariable::UNKNOWN());
    a7.activate();
    assertTrue(a7.getValue() == IntegerVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerSubtraction()
  {
    Subtraction s1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    Subtraction s2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Subtraction s3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    Subtraction s4(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Subtraction s5(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Subtraction s6(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Subtraction s7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    s1.activate();
    assertTrue(s1.getValue() == 0.0);
    s2.activate();
    assertTrue(s2.getValue() == -1.0);
    s3.activate();
    assertTrue(s3.getValue() == 1.0);
    s4.activate();
    assertTrue(s4.getValue() == 0.0);
    s5.activate();
    assertTrue(s5.getValue() == IntegerVariable::UNKNOWN());
    s6.activate();
    assertTrue(s6.getValue() == IntegerVariable::UNKNOWN());
    s7.activate();
    assertTrue(s7.getValue() == IntegerVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerMultiplication()
  {
    Multiplication m1(IntegerVariable::ZERO_EXP(), IntegerVariable::ZERO_EXP());
    Multiplication m2(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Multiplication m3(IntegerVariable::ONE_EXP(), IntegerVariable::ZERO_EXP());
    Multiplication m4(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Multiplication m5(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::ONE_EXP());
    Multiplication m6(IntegerVariable::ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Multiplication m7(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Multiplication m8(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Multiplication m9(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Multiplication m10(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ZERO_EXP());

    m1.activate();
    assertTrue(m1.getValue() == 0.0);
    m2.activate();
    assertTrue(m2.getValue() == 0.0);
    m3.activate();
    assertTrue(m3.getValue() == 0.0);
    m4.activate();
    assertTrue(m4.getValue() == 1.0);
    m5.activate();
    assertTrue(m5.getValue() == -1.0);
    m6.activate();
    assertTrue(m6.getValue() == -1.0);
    m7.activate();
    assertTrue(m7.getValue() == 1.0);
    m8.activate();
    assertTrue(m8.getValue() == IntegerVariable::UNKNOWN());
    m9.activate();
    assertTrue(m9.getValue() == IntegerVariable::UNKNOWN());
    m10.activate();
    assertTrue(m10.getValue() == IntegerVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerDivision()
  {
    Division d1(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Division d2(IntegerVariable::ZERO_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Division d3(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Division d4(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::ONE_EXP());
    Division d5(IntegerVariable::ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Division d6(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Division d7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Division d8(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Division d9(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ONE_EXP());

    d1.activate();
    assertTrue(d1.getValue() == 0.0);
    d2.activate();
    assertTrue(d2.getValue() == 0.0);
    d3.activate();
    assertTrue(d3.getValue() == 1.0);
    d4.activate();
    assertTrue(d4.getValue() == -1.0);
    d5.activate();
    assertTrue(d5.getValue() == -1.0);
    d6.activate();
    assertTrue(d6.getValue() == 1.0);
    d7.activate();
    assertTrue(d7.getValue() == IntegerVariable::UNKNOWN());
    d8.activate();
    assertTrue(d8.getValue() == IntegerVariable::UNKNOWN());
    d9.activate();
    assertTrue(d9.getValue() == IntegerVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerMaximum()
  {
    Maximum m1(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Maximum m2(IntegerVariable::ZERO_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Maximum m3(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Maximum m4(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::ONE_EXP());
    Maximum m5(IntegerVariable::ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Maximum m6(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Maximum m7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Maximum m8(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Maximum m9(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ONE_EXP());

    m1.activate();
    assertTrue(m1.getValue() == 1.0);
    m2.activate();
    assertTrue(m2.getValue() == 0.0);
    m3.activate();
    assertTrue(m3.getValue() == 1.0);
    m4.activate();
    assertTrue(m4.getValue() == 1.0);
    m5.activate();
    assertTrue(m5.getValue() == 1.0);
    m6.activate();
    assertTrue(m6.getValue() == -1.0);
    m7.activate();
    assertTrue(m7.getValue() == IntegerVariable::UNKNOWN());
    m8.activate();
    assertTrue(m8.getValue() == IntegerVariable::UNKNOWN());
    m9.activate();
    assertTrue(m9.getValue() == IntegerVariable::UNKNOWN());
    return true;
  }

  static bool testIntegerMinimum()
  {
    Minimum m1(IntegerVariable::ZERO_EXP(), IntegerVariable::ONE_EXP());
    Minimum m2(IntegerVariable::ZERO_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Minimum m3(IntegerVariable::ONE_EXP(), IntegerVariable::ONE_EXP());
    Minimum m4(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::ONE_EXP());
    Minimum m5(IntegerVariable::ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Minimum m6(IntegerVariable::MINUS_ONE_EXP(), IntegerVariable::MINUS_ONE_EXP());
    Minimum m7(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::UNKNOWN_EXP());
    Minimum m8(IntegerVariable::ZERO_EXP(), IntegerVariable::UNKNOWN_EXP());
    Minimum m9(IntegerVariable::UNKNOWN_EXP(), IntegerVariable::ONE_EXP());

    m1.activate();
    assertTrue(m1.getValue() == 0.0);
    m2.activate();
    assertTrue(m2.getValue() == -1.0);
    m3.activate();
    assertTrue(m3.getValue() == 1.0);
    m4.activate();
    assertTrue(m4.getValue() == -1.0);
    m5.activate();
    assertTrue(m5.getValue() == -1.0);
    m6.activate();
    assertTrue(m6.getValue() == -1.0);
    m7.activate();
    assertTrue(m7.getValue() == IntegerVariable::UNKNOWN());
    m8.activate();
    assertTrue(m8.getValue() == IntegerVariable::UNKNOWN());
    m9.activate();
    assertTrue(m9.getValue() == IntegerVariable::UNKNOWN());
    return true;
  }

};

class TestInterface : public ExternalInterface 
{
public:
  static Id<TestInterface>& instance()
  {
    return s_instanceTestInterface;
  }

  TestInterface()
    : ExternalInterface(),
      m_listener(*this) 
  {
    s_instanceTestInterface = (Id<TestInterface>) this->getId();
    m_listener.activate();
  }

  ~TestInterface() 
  {
    if (s_instanceTestInterface == this->getId())
      s_instanceTestInterface = Id<TestInterface>::noId();

    for (std::set<ExpressionId>::iterator it = m_exprs.begin(); it != m_exprs.end(); ++it)
      (*it)->removeListener(m_listener.getId());
  }

  void setCache(const StateCacheId& cache) 
  {
    m_cache = cache;
  }

  double lookupNow(const State& state) {
    if (state.first == LabelStr("test1")) {
      return 0.0;
    }
    else if (state.first == LabelStr("test2")) {
      check_error(state.second.size() == 1);
      LabelStr param(state.second[0]);
      if (param == LabelStr("high")) return 1.0;
      else if (param == LabelStr("low")) return -1.0;
    }
    else if (state.first == LabelStr("time")) {
      return 0.0;
    }
    else {
      return m_changingExprs[state.first]->getValue();
    }
    std::cerr << "ERROR (shouldn't happen): reached end of lookupNow()"
              << std::endl;
    return 0.0;
  }


  void subscribe(const State& /* state */)
  {
  }

  void unsubscribe(const State& /* state */)
  {
  }

  void setThresholds(const State& /* state */, double /* hi */, double /* lo */)
  {
  }

  void batchActions(std::list<CommandId>& /* commands */)
  {
  }

  void updatePlanner(std::list<UpdateId>& /* updates */)
  {
  }

  void invokeAbort(const CommandId& /* cmd */)
  {
  }

  double currentTime()
  {
    return 0.0;
  }

  void watch(const LabelStr& name, ExpressionId expr) {
    if (m_exprs.find(expr) == m_exprs.end()) {
      expr->addListener(m_listener.getId());
      m_exprs.insert(expr);
    }
    m_changingExprs.insert(std::pair<double, ExpressionId>(name, expr));
    m_exprsToStateName.insert(std::make_pair(expr, name));
  }

  void unwatch(const LabelStr& name, ExpressionId expr) {
    if (m_exprs.find(expr) != m_exprs.end()) {
      m_exprs.erase(expr);
      expr->removeListener(m_listener.getId());
    }
    m_changingExprs.erase(name);
    m_exprsToStateName.erase(expr);
  }

protected:
  friend class ChangeListener;

  void internalExecuteCommand(const LabelStr& /* name */,
                  const std::list<double>& /* args */,
                  ExpressionId /* dest */)
  {}

  void internalInvokeAbort(const LabelStr& /* name */,
               const std::list<double>& /* args */, 
               ExpressionId /* dest */)
  {}

  void notifyValueChanged(ExpressionId expression)
  {
    std::multimap<ExpressionId, double>::const_iterator it = m_exprsToStateName.find(expression);
    while(it != m_exprsToStateName.end() && it->first == expression) {
      State st(it->second, std::vector<double>());
      m_cache->updateState(st, expression->getValue());
      ++it;
    }
  }

private:
  class ChangeListener : public ExpressionListener {
  public:
    ChangeListener(TestInterface& intf) : ExpressionListener(), m_intf(intf) {}
    void notifyValueChanged(const ExpressionId& expression) {m_intf.notifyValueChanged(expression);}
  protected:
  private:
    TestInterface& m_intf;
  };

  static Id<TestInterface> s_instanceTestInterface;

  std::set<ExpressionId> m_exprs;
  std::map<double, ExpressionId> m_changingExprs; //map of names to expressions being watched
  std::multimap<ExpressionId, double> m_exprsToStateName; //make of watched expressions to their state names
  std::multimap<ExpressionId, ExpressionId> m_listeningExprs; //map of changing expressions to listening expressions
  std::map<ExpressionId, double> m_tolerances; //map of dest expressions to tolerances
  std::map<ExpressionId, double> m_cachedValues; //cache of the previously returned values (dest expression, value pairs)
  ChangeListener m_listener;
  StateCacheId m_cache;
};

Id<TestInterface> TestInterface::s_instanceTestInterface = Id<TestInterface>::noId();

class LookupTestExecConnector : public ExecConnector {
public:
  LookupTestExecConnector() : ExecConnector() 
  {
    m_cache.setExternalInterface(TestInterface::instance()->getId()); // static_cast didn't work here, grumble
  }

  void notifyNodeConditionChanged(NodeId /* node */) {}
  void handleConditionsChanged(const NodeId& /* node */, NodeState /* newState */) {}
  void enqueueAssignment(const AssignmentId& /* assign */) {}
  void enqueueAssignmentForRetraction(const AssignmentId& /* assign */) {}
  void enqueueCommand(const CommandId& /* cmd */) {}
  void enqueueUpdate(const UpdateId& /* update */) {}
  void notifyExecuted(const NodeId& /* node */) {}
  void markRootNodeFinished(const NodeId& /* node */) {}
  const StateCacheId& getStateCache() {return m_cache.getId();}
  const ExternalInterfaceId& getExternalInterface() 
  {
    return TestInterface::instance()->getId(); // static_cast didn't work here, grumble
  }
  const ExecListenerHubId& getExecListenerHub() const { return ExecListenerHubId::noId(); }

private:
  StateCache m_cache;
  
};

class LookupTestNodeConnector : public NodeConnector {
public:
  LookupTestNodeConnector() : NodeConnector() {}
  const VariableId& findVariable(const PlexilVarRef* /* var */)
  { return VariableId::noId(); }
  const VariableId& findVariable(const LabelStr& /* name */, bool /* recursive */ = false)
  { return VariableId::noId(); }
  const NodeId& getNode() const { return NodeId::noId(); }
  const ExecConnectorId& getExec() const { return m_exec.getId(); }
  const ExecListenerHubId& getExecListenerHub() const { return m_exec.getExecListenerHub(); }
private:
  LookupTestExecConnector m_exec;
};

class LookupsTest {
public:
  static bool test() {
    TestInterface foo;
    runTest(lookupNow);
    runTest(lookupOnChange);
    return true;
  }
private:
  static bool lookupNow() {
    LookupTestNodeConnector node;
    TestInterface::instance()->setCache(node.getExec()->getStateCache());
    PlexilStateId state1 = (new PlexilState())->getId();
    state1->setName("test1");

    PlexilStateId state2 = (new PlexilState())->getId();
    state2->setName("test2");
    state2->addArg((new PlexilValue(PLEXIL::STRING, "high"))->getId());

    PlexilStateId state3 = (new PlexilState())->getId();
    state3->setName("test2");
    state3->addArg((new PlexilValue(PLEXIL::STRING, "low"))->getId());

    PlexilLookupNow test1;
    test1.setState(state1);

    PlexilLookupNow test2;
    test2.setState(state2);

    PlexilLookupNow test3;
    test3.setState(state3);

    //need a fake NodeConnector that has a fake ExecConnector that will return a StateCache implementation
    LookupNow l1(test1.getId(), node.getId());
    LookupNow l2(test2.getId(), node.getId());
    LookupNow l3(test3.getId(), node.getId());

    node.getExec()->getStateCache()->handleQuiescenceStarted();
    l1.activate();
    l2.activate();
    l3.activate();

    assertTrue(l1.getValue() == 0.0);
    assertTrue(l2.getValue() == 1.0);
    assertTrue(l3.getValue() == -1.0);

    return true;
  }

  static bool lookupOnChange() {
    PlexilStateId state1 = (new PlexilState())->getId();
    state1->setName("changeTest");
    PlexilChangeLookup test1;
    test1.setState(state1);

    PlexilStateId state2 = (new PlexilState())->getId();
    state2->setName("changeWithToleranceTest");
    PlexilChangeLookup test2;
    test2.setState(state2);
    test2.addTolerance((new PlexilValue(PLEXIL::REAL, "0.5"))->getId());


    RealVariable watchVar(0.0);
    watchVar.activate();
    TestInterface::instance()->watch(LabelStr("changeTest"), watchVar.getId());
    TestInterface::instance()->watch(LabelStr("changeWithToleranceTest"), watchVar.getId());

    LookupTestNodeConnector node;
    TestInterface::instance()->setCache(node.getExec()->getStateCache());
    LookupOnChange l1(test1.getId(), node.getId());
    LookupOnChange l2(test2.getId(), node.getId());

    assertTrue(l1.getValue() == Expression::UNKNOWN());
    assertTrue(l2.getValue() == Expression::UNKNOWN());

    node.getExec()->getStateCache()->handleQuiescenceStarted();
    l1.activate();
    assertTrue(l1.getValue() == 0.0);
    l2.activate();
    assertTrue(l2.getValue() == 0.0);

    node.getExec()->getStateCache()->handleQuiescenceEnded();

    watchVar.setValue(0.1);
    assertTrue(l1.getValue() == 0.1);
    assertTrue(l2.getValue() == 0.0);

    watchVar.setValue(0.6);
    assertTrue(l1.getValue() == 0.6);
    assertTrue(l2.getValue() == 0.6);

    node.getExec()->getStateCache()->handleQuiescenceStarted();
    l1.deactivate();
    node.getExec()->getStateCache()->handleQuiescenceEnded();

    watchVar.setValue(0.7);
    assertTrue(l1.getValue() == Expression::UNKNOWN());
    assertTrue(l2.getValue() == 0.6);
    watchVar.setValue(1.1);
    assertTrue(l1.getValue() == Expression::UNKNOWN());
    assertTrue(l2.getValue() == 1.1);

    TestInterface::instance()->unwatch(LabelStr("changeTest"), watchVar.getId());
    TestInterface::instance()->unwatch(LabelStr("changeWithToleranceTest"), watchVar.getId());

    return true;
  }
};

class ExpressionsTest {
public:
  static bool test() {
    //runTest(testAbsoluteValue);
    runTest(testSqrt);
    runTest(testIsKnown);
    return true;
  }

private:
  static bool testAbsoluteValue() {
    AbsoluteValue abs1(RealVariable::ZERO_EXP());
    AbsoluteValue abs2(RealVariable::ONE_EXP());
    AbsoluteValue abs3(RealVariable::MINUS_ONE_EXP());
    AbsoluteValue abs4(RealVariable::UNKNOWN_EXP());

    assertTrue(abs1.getValue() == 0.0);
    assertTrue(abs2.getValue() == 1.0);
    assertTrue(abs3.getValue() == 1.0);
    assertTrue(abs4.getValue() == Expression::UNKNOWN());
    return true;
  }
  static bool testSqrt() {
    return true;
  } 
 static bool testIsKnown() {
     IsKnown isKnown1(BooleanVariable::UNKNOWN_EXP());
     IsKnown isKnown2(BooleanVariable::FALSE_EXP());
     assertTrue ( isKnown1.recalculate() == false );
     assertTrue ( isKnown2.recalculate() == true );
    return true;
  }
};

class ExternalInterfaceTest {
public:
  static bool test() {
    return true;
  }
private:
};

// For Boolean variable/condition tests
#define IDX_UNKNOWN 0
#define IDX_FALSE 1
#define IDX_TRUE 2

class TransitionExecConnector : public ExecConnector {
public:
  TransitionExecConnector() : ExecConnector(), m_executed(false) {}
  void notifyNodeConditionChanged(NodeId /* node */) {}
  void handleConditionsChanged(const NodeId& /* node */, NodeState /* newState */) {}
  void enqueueAssignment(const AssignmentId& /* assign */) {}
  void enqueueAssignmentForRetraction(const AssignmentId& /* assign */) {}
  void enqueueCommand(const CommandId& /* cmd */) {}
  void enqueueUpdate(const UpdateId& /* update */) {}
  // Replacement for handleNeedsExecution()
  void notifyExecuted(const NodeId& node) {assertTrue(node->getState() == EXECUTING_STATE); m_executed = true;}
  void markRootNodeFinished(const NodeId& /* node */) {}
  const StateCacheId& getStateCache() {return StateCacheId::noId();}
  const ExternalInterfaceId& getExternalInterface() {return ExternalInterfaceId::noId();}
  const ExecListenerHubId& getExecListenerHub() const { return ExecListenerHubId::noId(); }
  bool executed() {return m_executed;}
private:
  bool m_executed;
};

class StateTransitionsTest 
{
public:
  static bool test() 
  {
    runTest(inactiveDestTest);
    runTest(inactiveTransTest);
    runTest(waitingDestTest);
    runTest(waitingTransTest);
    runTest(iterationEndedDestTest);
    runTest(iterationEndedTransTest);
    runTest(finishedDestTest);
    runTest(finishedTransTest);
    runTest(listExecutingDestTest);
    runTest(listExecutingTransTest);
    runTest(listFailingDestTest);
    runTest(listFailingTransTest);
    runTest(listFinishingDestTest);
    runTest(listFinishingTransTest);
    runTest(bindingExecutingDestTest);
    runTest(bindingExecutingTransTest);
    runTest(bindingFailingDestTest);
    runTest(bindingFailingTransTest);
    runTest(commandExecutingDestTest);
    runTest(commandExecutingTransTest);
    runTest(commandFailingDestTest);
    runTest(commandFailingTransTest);
    runTest(commandFinishingDestTest);
    runTest(commandFinishingTransTest);
    runTest(updateExecutingDestTest);
    runTest(updateExecutingTransTest);
    runTest(updateFailingDestTest);
    runTest(updateFailingTransTest);
    return true;
  }

private:
  
  static bool inactiveDestTest() 
  {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", INACTIVE_STATE, con.getId(), NodeId::noId());
    NodeId nodes[4] =
      {NodeFactory::createNode(Node::ASSIGNMENT(), "testAssignment", INACTIVE_STATE, con.getId(), parent),
       NodeFactory::createNode(Node::COMMAND(), "testCommand", INACTIVE_STATE, con.getId(), parent),
       NodeFactory::createNode(Node::LIST(), "testList", INACTIVE_STATE, con.getId(), parent),
       NodeFactory::createNode(Node::UPDATE(), "testUpdate", INACTIVE_STATE, con.getId(), parent)
      };

    NodeState states[7] = {INACTIVE_STATE,
                           WAITING_STATE,
                           EXECUTING_STATE,
                           FINISHING_STATE,
                           FINISHED_STATE,
                           FAILING_STATE,
                           ITERATION_ENDED_STATE};

    for (int i = 0; i < 4; ++i) {
      NodeId node = nodes[i];
      for (size_t s = 0; s < 7; ++s) {
        parent->setState(states[s]);
        NodeState destState = node->getDestState();
        if (destState != node->getState()) {
          debugMsg("UnitTest:inactiveDestTest",
                   " Node " << node->getNodeId().toString()
                   << " Parent state: " << StateVariable::nodeStateName(states[s]).toString()
                   << " Dest: " << StateVariable::nodeStateName(destState).toString());
          if (states[s] == FINISHED_STATE) {
            assertTrue(destState == FINISHED_STATE);
          }
          else if (states[s] == EXECUTING_STATE) {
            assertTrue(destState == WAITING_STATE);
          }
        }
        else {
          assertTrue(destState == NO_NODE_STATE);
        }
      }
    }
    for (int i = 0; i < 4; i++) {
      delete (Node*) nodes[i];
    }
    delete (Node*) parent;
    return true;
  }

  static bool inactiveTransTest() 
  {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", INACTIVE_STATE, con.getId(), NodeId::noId());

    LabelStr types[5] = {Node::ASSIGNMENT(), Node::COMMAND(), Node::LIBRARYNODECALL(), Node::LIST(), Node::UPDATE()};
    NodeState states[7] = {INACTIVE_STATE,
                           WAITING_STATE,
                           EXECUTING_STATE,
                           FINISHING_STATE,
                           FINISHED_STATE,
                           FAILING_STATE,
                           ITERATION_ENDED_STATE};
    
    for (size_t s = 0; s < 7; ++s) {
      for (int i = 0; i < 5; i++) {
        NodeId node = NodeFactory::createNode(types[i], "test", INACTIVE_STATE, con.getId(), parent);
        parent->setState(states[s]);
      
        if (node->canTransition()) {
          node->transition(node->getDestState());
          NodeState state = node->getState();
          debugMsg("UnitTest:inactiveTransTest",
                   " Node type " << types[i].toString()
                   << " Parent state " << StateVariable::nodeStateName(states[s]).toString()
                   << " Node state " << StateVariable::nodeStateName(state).toString());
          if (states[s] == FINISHED_STATE) {
            assertTrue(state == FINISHED_STATE);
            assertTrue(node->getOutcome() == OutcomeVariable::SKIPPED());
          }
          else if (states[s] == EXECUTING_STATE) {
            assertTrue(state == WAITING_STATE);
            // These are activated by the parent node on transition to EXECUTING.
            //assertTrue(node->isAncestorEndConditionActive());
            //assertTrue(node->isAncestorExitConditionActive());
            //assertTrue(node->isAncestorInvariantConditionActive());
            assertTrue(node->isExitConditionActive());
            assertTrue(node->isPreConditionActive());
            assertTrue(node->isSkipConditionActive());
            assertTrue(node->isStartConditionActive());
          }
          else {
            assertTrue(false);
          }
        }
        delete (Node*) node;
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool waitingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", WAITING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        node->getExitCondition()->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
          for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
            node->getAncestorEndCondition()->setValue(values[ancestorEnd]);
            for (int skip = 0; skip < 3; ++skip) {
              node->getSkipCondition()->setValue(values[skip]);
              for (int start = 0; start < 3; ++start) {
                node->getStartCondition()->setValue(values[start]);
                for (int pre = 0; pre < 3; ++pre) {
                  node->getPreCondition()->setValue(values[pre]);

                  NodeState destState = node->getDestState();
                  debugMsg("UnitTest:waitingDestTest: Destination",
                           " state is " << StateVariable::nodeStateName(destState).toString());
                  if (ancestorExit == IDX_TRUE
                      || exit == IDX_TRUE
                      || ancestorInvariant == IDX_FALSE
                      || ancestorEnd == IDX_TRUE
                      || skip == IDX_TRUE) {
                    assertTrue(destState == FINISHED_STATE);
                  }
                  else if (start == IDX_TRUE) {
                    if (pre == IDX_TRUE) {
                      assertTrue(destState == EXECUTING_STATE);
                    }
                    else {
                      assertTrue(destState == ITERATION_ENDED_STATE);
                    }
                  }
                  else {
                    assertTrue(destState == NO_NODE_STATE);
                  }
                }
              }
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool waitingTransTest() 
  {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr types[4] = {Node::ASSIGNMENT(), Node::COMMAND(), Node::LIST(), Node::UPDATE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
            for (int skip = 0; skip < 3; ++skip) {
              for (int start = 0; start < 3; ++start) {
                for (int pre = 0; pre < 3; ++pre) {
                  for (int i = 0; i < 4; i++) {
                    NodeId node = NodeFactory::createNode(types[i], "test", WAITING_STATE, con.getId(), parent);
                    node->getAncestorExitCondition()->setValue(values[ancestorExit]);
                    node->getExitCondition()->setValue(values[exit]);
                    node->getSkipCondition()->setValue(values[skip]);
                    node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
                    node->getAncestorEndCondition()->setValue(values[ancestorEnd]);
                    node->getStartCondition()->setValue(values[start]);
                    node->getPreCondition()->setValue(values[pre]);

                    debugMsg("UnitTest:waitingTransition",
                             "Testing node type " << types[i].toString() << " with "
                             << " ancestor exit = " << values[ancestorExit]
                             << " ancestor invariant = " << values[ancestorInvariant]
                             << " ancestor end = " << values[ancestorEnd]
                             << " skip = " << values[skip]
                             << " start = " << values[start]
                             << " pre = " << values[pre] 
                             );

                    if (node->canTransition()) {
                      node->transition(node->getDestState());
                      NodeState state = node->getState();

                      if (ancestorExit == IDX_TRUE
                          || exit == IDX_TRUE
                          || ancestorInvariant == IDX_FALSE
                          || ancestorEnd == IDX_TRUE
                          || skip == IDX_TRUE) {
                        assertTrue(state == FINISHED_STATE);
                        assertTrue(node->getOutcome() == OutcomeVariable::SKIPPED());
                      }
                      else if (start == IDX_TRUE) {
                        assertTrue(node->isAncestorExitConditionActive());
                        assertTrue(node->isAncestorInvariantConditionActive());
                        if (pre == IDX_TRUE) {
                          assertTrue(state == EXECUTING_STATE);
                          assertTrue(node->isEndConditionActive());
                          assertTrue(node->isExitConditionActive());
                          assertTrue(node->isInvariantConditionActive());
                          if (types[i] == Node::ASSIGNMENT()) {
                            assertTrue(node->isActionCompleteConditionActive());
                          }
                          assertTrue(con.executed());
                        }
                        else {
                          assertTrue(state == ITERATION_ENDED_STATE);
                          assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                          assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PRE_CONDITION_FAILED());
                          assertTrue(node->isRepeatConditionActive());
                          assertTrue(node->isAncestorEndConditionActive());
                        }
                      }
                      else {
                        assertTrue(false);
                      }
                    }
                    delete (Node*) node;
                  }
                }
              }
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool iterationEndedDestTest() {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", ITERATION_ENDED_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
        for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
          node->getAncestorEndCondition()->setValue(values[ancestorEnd]);
          for (int repeat = 0; repeat < 3; ++repeat) {
            node->getRepeatCondition()->setValue(values[repeat]);

            NodeState destState = node->getDestState();

            if (ancestorExit == IDX_TRUE
                || ancestorInvariant == IDX_FALSE
                || ancestorEnd == IDX_TRUE
                || repeat == IDX_FALSE) {
              assertTrue(destState == FINISHED_STATE);
            }
            else if (repeat == IDX_TRUE) {
              assertTrue(destState == WAITING_STATE);
            }
            else {
              assertTrue(destState == NO_NODE_STATE);
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool iterationEndedTransTest() 
  {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());

    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr types[4] = {Node::ASSIGNMENT(), Node::COMMAND(), Node::LIST(), Node::UPDATE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
        for (int ancestorEnd = 0; ancestorEnd < 3; ++ancestorEnd) {
          for (int repeat = 0; repeat < 3; ++repeat) {
            for (int i = 0; i < 4; i++) {
              NodeId node = NodeFactory::createNode(types[i], "test", ITERATION_ENDED_STATE, con.getId(), parent);
              node->getAncestorExitCondition()->setValue(values[ancestorExit]);
              node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
              node->getAncestorEndCondition()->setValue(values[ancestorEnd]);
              node->getRepeatCondition()->setValue(values[repeat]);

              debugMsg("UnitTest:iterationEndedTransition",
                       "Testing node type " << types[i].toString() << " with "
                       << " ancestor exit = " << values[ancestorExit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " ancestor end = " << values[ancestorEnd]
                       << " repeat = " << values[repeat]
                       );

              if (node->canTransition()) {
                node->transition(node->getDestState());
                NodeState state = node->getState();

                // TODO: check to make sure the reset happened here
                if (ancestorExit == IDX_TRUE) {
                  assertTrue(state == FINISHED_STATE);
                  assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                  assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                }
                else if (ancestorInvariant == IDX_FALSE || ancestorEnd == IDX_TRUE || repeat == IDX_FALSE) {
                  assertTrue(state == FINISHED_STATE);
                  if (ancestorInvariant == IDX_FALSE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                  }
                }
                else if (repeat == IDX_TRUE) {
                  assertTrue(state == WAITING_STATE);
                  assertTrue(node->getStartCondition()->isActive());
                }
                else {
                  assertTrue(false);
                }
              }
              delete (Node*) node;
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool finishedDestTest() {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", INACTIVE_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", FINISHED_STATE, con.getId(), parent);
    NodeState states[7] = {INACTIVE_STATE,
                           WAITING_STATE,
                           EXECUTING_STATE,
                           FINISHING_STATE,
                           FINISHED_STATE,
                           FAILING_STATE,
                           ITERATION_ENDED_STATE};

    for (size_t s = 0; s < 7; ++s) {
      parent->setState(states[s]);
      NodeState destState = node->getDestState();
      if (states[s] == WAITING_STATE) {
        assertTrue(destState == INACTIVE_STATE);
      }
      else {
        assertTrue(destState == NO_NODE_STATE);
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool finishedTransTest() {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", INACTIVE_STATE, con.getId(), NodeId::noId());

    NodeState states[7] = {INACTIVE_STATE,
                           WAITING_STATE,
                           EXECUTING_STATE,
                           FINISHING_STATE,
                           FINISHED_STATE,
                           FAILING_STATE,
                           ITERATION_ENDED_STATE};
    LabelStr types[4] = {Node::ASSIGNMENT(), Node::COMMAND(), Node::LIST(), Node::UPDATE()};

    for (size_t s = 0; s < 7; ++s) {
      for (int i = 0; i < 4; ++i) {
        NodeId node = NodeFactory::createNode(types[i], "test", FINISHED_STATE, con.getId(), parent);
        parent->setState(states[s]);

        debugMsg("UnitTest:finishedTransition",
                 "Testing node type " << types[i].toString()
                 << " with parent state = " << StateVariable::nodeStateName(states[s]).toString());

        if (node->canTransition()) {
          node->transition(node->getDestState());
          NodeState state = node->getState();

          if (states[s] == WAITING_STATE) {
            assertTrue(state == INACTIVE_STATE);
          }
          else {
            assertTrue(false);
          }
        }
        delete (Node*) node;
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool listExecutingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::LIST(), "test", EXECUTING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        node->getExitCondition()->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
          for (int invariant = 0; invariant < 3; ++invariant) {
            node->getInvariantCondition()->setValue(values[invariant]);
            for (int end = 0; end < 3; ++end) {
              node->getEndCondition()->setValue(values[end]);

              NodeState destState = node->getDestState();

              if (ancestorExit == IDX_TRUE
                  || exit == IDX_TRUE
                  || ancestorInvariant == IDX_FALSE 
                  || invariant == IDX_FALSE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (end == IDX_TRUE) {
                assertTrue(destState == FINISHING_STATE);
              }
              else {
                assertTrue(destState == NO_NODE_STATE);
              }
            }
          }
        }
      }
    }

    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool listExecutingTransTest() {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int invariant = 0; invariant < 3; ++invariant) {
            for (int end = 0; end < 3; ++end) {
              NodeId node = NodeFactory::createNode(Node::LIST(), "test", EXECUTING_STATE, con.getId(), parent);
              node->getAncestorExitCondition()->setValue(values[ancestorExit]);
              node->getExitCondition()->setValue(values[exit]);
              node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
              node->getInvariantCondition()->setValue(values[invariant]);
              node->getEndCondition()->setValue(values[end]);

              debugMsg("UnitTest:listExecutingTransition",
                       "Testing ListNode with"
                       << " ancestor exit = " << values[ancestorExit]
                       << " exit = " << values[exit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " invariant = " << values[invariant]
                       << " end = " << values[end]);

              if (node->canTransition()) {
                node->transition(node->getDestState());
                NodeState state = node->getState();
                assertTrue(node->getActionCompleteCondition()->isActive());
                if (ancestorExit == IDX_TRUE || exit == IDX_TRUE) {
                  assertTrue(state == FAILING_STATE);
                  assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                  if (ancestorExit == IDX_TRUE) {
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                  }
                  else if (exit == IDX_TRUE) {
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::EXITED());
                  }
                }
                else if (ancestorInvariant == IDX_FALSE || invariant == IDX_FALSE) {
                  assertTrue(state == FAILING_STATE);
                  assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                  if (ancestorInvariant == IDX_FALSE) {
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                  }
                  else if (invariant == IDX_FALSE) {
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::INVARIANT_CONDITION_FAILED());
                  }
                }
                else if (end == IDX_TRUE) {
                  assertTrue(state == FINISHING_STATE);
                  assertTrue(node->getInvariantCondition()->isActive());
                  assertTrue(node->isAncestorInvariantConditionActive());
                }
                else {
                  assertTrue(false);
                }
              }
              delete (Node*) node;
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool listFailingDestTest() {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::LIST(), "test", FAILING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    double failureTypes[2] = {FailureVariable::PRE_CONDITION_FAILED(), FailureVariable::PARENT_FAILED()};

    for (int children = 0; children < 3; ++children) {
      node->getActionCompleteCondition()->setValue(values[children]);
      for (int failure = 0; failure < 2; ++failure) {
        node->getFailureTypeVariable()->setValue(failureTypes[failure]);
        NodeState destState = node->getDestState();

        if (children == IDX_TRUE) {
          if (failure == 0) {
            assertTrue(destState == ITERATION_ENDED_STATE);
          }
          else if (failure == 1) {
            assertTrue(destState == FINISHED_STATE);
          }
        }
        else {
          assertTrue(destState == NO_NODE_STATE);
        }
      }
    }

    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool listFailingTransTest() {
    TransitionExecConnector con;
    NodeId parent =
      NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());

    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr failureType[2] = {FailureVariable::INVARIANT_CONDITION_FAILED(), FailureVariable::PARENT_FAILED()};

    for (int children = 0; children < 3; ++children) {
      for (int i = 0; i < 2; ++i) {
        NodeId node = NodeFactory::createNode(Node::LIST(), "test", FAILING_STATE, con.getId(), parent);
        node->getActionCompleteCondition()->setValue(values[children]);
        node->getFailureTypeVariable()->setValue(failureType[i]);

        debugMsg("UnitTest:listFailingTrans",
                 "Testing with children waiting or finished = " << values[children] << " failure type = " << failureType[i].toString());

        if (node->canTransition()) {
          node->transition(node->getDestState());
          NodeState state = node->getState();

          if (children == IDX_TRUE) {
            if (i == 0) {
              assertTrue(state == ITERATION_ENDED_STATE);
              assertTrue(node->isAncestorInvariantConditionActive());
              assertTrue(node->isAncestorEndConditionActive());
              assertTrue(node->isRepeatConditionActive());
            }
            else if (i == 1) {
              assertTrue(state == FINISHED_STATE);
            }
            else {
              assertTrue(false);
            }
          }
          else {
            assertTrue(false);
          }
        }
        delete (Node*) node;
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool listFinishingDestTest()
 {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::LIST(), "test", FINISHING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        node->getExitCondition()->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
          for (int invariant = 0; invariant < 3; ++invariant) {
            node->getInvariantCondition()->setValue(values[invariant]);
            for (int children = 0; children < 3; ++children) {
              node->getActionCompleteCondition()->setValue(values[children]);
              for (int post = 0; post < 3; ++post) {
                node->getPostCondition()->setValue(values[post]);
                NodeState destState = node->getDestState();

                debugMsg("UnitTest:listFinishingDest",
                         "Testing NodeList with"
                         << " ancestor exit = " << values[ancestorExit]
                         << " exit = " << values[exit]
                         << " ancestor invariant = " << values[ancestorInvariant]
                         << " invariant = " << values[invariant]
                         << " children waiting or finished = " << values[children] 
                         << " post = " << values[post]
                         << "\n Got dest " << StateVariable::nodeStateName(destState).toString());

                if (ancestorExit == IDX_TRUE
                    || exit == IDX_TRUE
                    || ancestorInvariant == IDX_FALSE
                    || invariant == IDX_FALSE) {
                  assertTrue(destState == FAILING_STATE);
                }
                else if (children == IDX_TRUE) {
                  assertTrue(destState == ITERATION_ENDED_STATE);
                }
                else {
                  assertTrue(destState == NO_NODE_STATE);
                }
              }
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool listFinishingTransTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());

    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int invariant = 0; invariant < 3; ++invariant) {
            for (int children = 0; children < 3; ++children) {
              for (int post = 0; post < 3; ++post) {
                NodeId node = NodeFactory::createNode(Node::LIST(), "test", FINISHING_STATE, con.getId(), parent);
                node->getAncestorExitCondition()->setValue(values[ancestorExit]);
                node->getExitCondition()->setValue(values[exit]);
                node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
                node->getInvariantCondition()->setValue(values[invariant]);
                node->getActionCompleteCondition()->setValue(values[children]);
                node->getPostCondition()->setValue(values[post]);

                debugMsg("UnitTest:listFinishingTrans",
                         "Testing NodeList with"
                         << " ancestor exit = " << values[ancestorExit]
                         << " exit = " << values[exit]
                         << " ancestor invariant = " << values[ancestorInvariant]
                         << " invariant = " << values[invariant]
                         << " children waiting or finished = " << values[children]
                         << " post = " << values[post]);

                if (node->canTransition()) {
                  node->transition(node->getDestState());
                  NodeState state = node->getState();

                  if (ancestorExit == IDX_TRUE || exit == IDX_TRUE) {
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                    if (ancestorExit == IDX_TRUE) {
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                    }
                    else if (exit == IDX_TRUE) {
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::EXITED());
                    }
                    assertTrue(node->getActionCompleteCondition()->isActive());
                  }
                  else if (ancestorInvariant == IDX_FALSE || invariant == IDX_FALSE) {
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                    if (ancestorInvariant == IDX_FALSE) {
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                    }
                    else if (invariant == IDX_FALSE) {
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::INVARIANT_CONDITION_FAILED());
                    }
                    assertTrue(node->getActionCompleteCondition()->isActive());
                  }
                  else if (children == IDX_TRUE) {
                    assertTrue(state == ITERATION_ENDED_STATE);
                    assertTrue(node->isAncestorInvariantConditionActive());
                    assertTrue(node->isAncestorExitConditionActive());
                    assertTrue(node->isAncestorEndConditionActive());
                    assertTrue(node->isRepeatConditionActive());
                    if (post == IDX_TRUE) {
                      assertTrue(node->getOutcome() == OutcomeVariable::SUCCESS());
                    }
                    else {
                      assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::POST_CONDITION_FAILED());
                    }
                  }
                  else {
                    assertTrue(false);
                  }
                }
                delete (Node*) node;
              }
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool bindingExecutingDestTest() 
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", EXECUTING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
      node->getActionCompleteCondition()->setValue(values[actionComplete]);
      for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
        node->getAncestorExitCondition()->setValue(values[ancestorExit]);
        for (int exit = 0; exit < 3; ++exit) {
          node->getExitCondition()->setValue(values[exit]);
          for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
            node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
            for (int invariant = 0; invariant < 3; ++invariant) {
              node->getInvariantCondition()->setValue(values[invariant]);
              for (int end = 0; end < 3; ++end) {
                node->getEndCondition()->setValue(values[end]);
                for (int post = 0; post < 3; ++post) {
                  node->getPostCondition()->setValue(values[post]);

                  NodeState destState = node->getDestState();
                  if (actionComplete != IDX_TRUE) {
                    assertTrue(destState == NO_NODE_STATE);
                  }
                  else if (ancestorExit == IDX_TRUE) {
                    assertTrue(destState == FAILING_STATE);
                  }
                  else if (exit == IDX_TRUE) {
                    assertTrue(destState == FAILING_STATE);
                  }
                  else if (ancestorInvariant == IDX_FALSE) {
                    assertTrue(destState == FAILING_STATE);
                  }
                  else if (invariant == IDX_FALSE) {
                    assertTrue(destState == FAILING_STATE);
                  }
                  else if (end == IDX_TRUE) {
                    assertTrue(destState == ITERATION_ENDED_STATE);
                  }
                  else {
                    assertTrue(destState == NO_NODE_STATE);
                  }
                }
              }
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool bindingExecutingTransTest() 
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());

    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr type = Node::ASSIGNMENT();

    for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
      for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
        for (int exit = 0; exit < 3; ++exit) {
          for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
            for (int invariant = 0; invariant < 3; ++invariant) {
              for (int end = 0; end < 3; ++end) {
                for (int post = 0; end < 3; ++end) {
                  NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", EXECUTING_STATE, con.getId(), parent);
                  node->getActionCompleteCondition()->setValue(values[actionComplete]);
                  node->getAncestorExitCondition()->setValue(values[ancestorExit]);
                  node->getExitCondition()->setValue(values[exit]);
                  node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
                  node->getInvariantCondition()->setValue(values[invariant]);
                  node->getEndCondition()->setValue(values[end]);
                  node->getPostCondition()->setValue(values[post]);

                  debugMsg("UnitTest:bindingExecutingTransition",
                           "Testing " << Node::ASSIGNMENT().toString() << " with"
                           << " action complete = " << values[actionComplete]
                           << " ancestor exit = " << values[ancestorExit]
                           << " exit = " << values[exit]
                           << " ancestor invariant = " << values[ancestorInvariant]
                           << " invariant = " << values[invariant]
                           << " end = " << values[end]
                           << " post = " << values[post]
                           );

                  if (node->canTransition()) {
                    node->transition(node->getDestState());
                    NodeState state = node->getState();

                    if (ancestorExit == IDX_TRUE || exit == IDX_TRUE) {
                      assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                      if (ancestorExit == IDX_TRUE) {
                        assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                        assertTrue(state == FAILING_STATE);
                        assertTrue(node->isAbortCompleteConditionActive());
                      }
                      else if (exit == IDX_TRUE) {
                        assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::EXITED());
                        assertTrue(state == FAILING_STATE);
                        assertTrue(node->isAbortCompleteConditionActive());
                      }
                    }
                    else if (ancestorInvariant == IDX_FALSE || invariant == IDX_FALSE) {
                      assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                      if (ancestorInvariant == IDX_FALSE) {
                        assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                        assertTrue(state == FAILING_STATE);
                        assertTrue(node->isAbortCompleteConditionActive());
                      }
                      else if (invariant == IDX_FALSE) {
                        assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::INVARIANT_CONDITION_FAILED());
                        assertTrue(state == FAILING_STATE);
                        assertTrue(node->isAbortCompleteConditionActive());
                      }
                    }
                    else if (end == IDX_TRUE) {
                      assertTrue(state == ITERATION_ENDED_STATE);
                      assertTrue(node->isAncestorEndConditionActive());
                      assertTrue(node->isAncestorExitConditionActive());
                      assertTrue(node->isAncestorInvariantConditionActive());
                      assertTrue(node->isRepeatConditionActive());
                      if (post == IDX_TRUE) {
                        assertTrue(node->getOutcome() == OutcomeVariable::SUCCESS());
                      }
                      else {
                        assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                        assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::POST_CONDITION_FAILED());
                      }
                    }
                    else {
                      assertTrue(false);
                    }
                  }
                  delete (Node*) node;
                }
              }
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool bindingFailingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", FAILING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr failureType[4] = {FailureVariable::INVARIANT_CONDITION_FAILED(),
                               FailureVariable::PARENT_FAILED(),
                               FailureVariable::EXITED(),
                               FailureVariable::PARENT_EXITED()};

    for (int abortComplete = 0; abortComplete < 3; ++abortComplete) {
      node->getAbortCompleteCondition()->setValue(values[abortComplete]);
      for (int failure = 0; failure < 4; ++ failure) {
        node->getFailureTypeVariable()->setValue(failureType[failure].getKey());

        NodeState destState = node->getDestState();
        if (abortComplete == IDX_TRUE) {
          if (failureType[failure] == FailureVariable::PARENT_FAILED()
              || failureType[failure] == FailureVariable::PARENT_EXITED()) {
            assertTrue(destState == FINISHED_STATE);
          }
          else {
            assertTrue(destState == ITERATION_ENDED_STATE);
          }
        }
        else {
          assertTrue(destState == NO_NODE_STATE);
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool bindingFailingTransTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr failureType[4] = {FailureVariable::INVARIANT_CONDITION_FAILED(),
                               FailureVariable::PARENT_FAILED(),
                               FailureVariable::EXITED(),
                               FailureVariable::PARENT_EXITED()};

    for (int children = 0; children < 3; ++children) {
      for (int failure = 0; failure < 2; ++failure) {
        NodeId node = NodeFactory::createNode(Node::ASSIGNMENT(), "test", FAILING_STATE, con.getId(), parent);
        node->getActionCompleteCondition()->setValue(values[children]);
        node->getFailureTypeVariable()->setValue(failureType[failure]);

        debugMsg("UnitTest:listFailingTrans",
                 "Testing with children waiting or finished = " << values[children] << " failure type = " << failureType[failure].toString());

        if (node->canTransition()) {
          node->transition(node->getDestState());
          NodeState state = node->getState();

          if (children == IDX_TRUE) {
            if (failureType[failure] == FailureVariable::PARENT_FAILED()
                || failureType[failure] == FailureVariable::PARENT_EXITED()) {
              assertTrue(state == FINISHED_STATE);
            }
            else {
              assertTrue(state == ITERATION_ENDED_STATE);
              assertTrue(node->isAncestorInvariantConditionActive());
              assertTrue(node->isAncestorEndConditionActive());
              assertTrue(node->isRepeatConditionActive());
            }
          }
          else {
            assertTrue(false);
          }
        }
        delete (Node*) node;
      }
    }
    delete (Node*) parent;
    return true;
  }

  //
  // Command nodes
  // 

  static bool commandExecutingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::COMMAND(), "test", EXECUTING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        node->getExitCondition()->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
          for (int invariant = 0; invariant < 3; ++invariant) {
            node->getInvariantCondition()->setValue(values[invariant]);
            for (int end = 0; end < 3; ++end) {
              node->getEndCondition()->setValue(values[end]);

              NodeState destState = node->getDestState();
              if (ancestorExit == IDX_TRUE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (exit == IDX_TRUE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (ancestorInvariant == IDX_FALSE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (invariant == IDX_FALSE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (end == IDX_TRUE) {
                assertTrue(destState == FINISHING_STATE);
              }
              else {
                assertTrue(destState == NO_NODE_STATE);
              }
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool commandExecutingTransTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());

    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int invariant = 0; invariant < 3; ++invariant) {
            for (int end = 0; end < 3; ++end) {
              NodeId node = NodeFactory::createNode(Node::COMMAND(), "test", EXECUTING_STATE, con.getId(), parent);
              node->getAncestorExitCondition()->setValue(values[ancestorExit]);
              node->getExitCondition()->setValue(values[exit]);
              node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
              node->getInvariantCondition()->setValue(values[invariant]);
              node->getEndCondition()->setValue(values[end]);
              debugMsg("UnitTest:actionExecutingTransition",
                       "Testing Command with"
                       << " ancestor exit = " << values[ancestorExit]
                       << " exit = " << values[exit]
                       << " ancestor invariant = " << values[ancestorInvariant]
                       << " invariant = " << values[invariant]
                       << " end = " << values[end]
                       );

              if (node->canTransition()) {
                node->transition(node->getDestState());
                NodeState state = node->getState();
                if (ancestorExit == IDX_TRUE) {
                  assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                  assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                  assertTrue(state == FAILING_STATE);
                  assertTrue(node->isAbortCompleteConditionActive());
                }
                else if (exit == IDX_TRUE) {
                  assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                  assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::EXITED());
                  assertTrue(state == FAILING_STATE);
                  assertTrue(node->isAbortCompleteConditionActive());
                }
                else if (ancestorInvariant == IDX_FALSE) {
                  assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                  assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                  assertTrue(state == FAILING_STATE);
                  assertTrue(node->isAbortCompleteConditionActive());
                }
                else if (invariant == IDX_FALSE) {
                  assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                  assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::INVARIANT_CONDITION_FAILED());
                  assertTrue(state == FAILING_STATE);
                  assertTrue(node->isAbortCompleteConditionActive());
                }
                else if (end == IDX_TRUE) {
                  assertTrue(state == FINISHING_STATE);
                  assertTrue(node->isActionCompleteConditionActive());
                  assertTrue(node->isAncestorExitConditionActive());
                  assertTrue(node->isAncestorInvariantConditionActive());
                  assertTrue(node->isExitConditionActive());
                  assertTrue(node->isInvariantConditionActive());
                  assertTrue(node->isPostConditionActive());
                }
                else {
                  assertTrue(false);
                }
              }
              delete (Node*) node;
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool commandFailingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::COMMAND(), "test", FAILING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    double failureTypes[4] = {FailureVariable::INVARIANT_CONDITION_FAILED(),
                              FailureVariable::PARENT_FAILED(),
                              FailureVariable::EXITED(),
                              FailureVariable::PARENT_EXITED()};
    for (int abortComplete = 0; abortComplete < 3; ++abortComplete) {
      node->getAbortCompleteCondition()->setValue(values[abortComplete]);
      for (int failure = 0; failure < 4; ++failure) {
        node->getFailureTypeVariable()->setValue(failureTypes[failure]);
        NodeState destState = node->getDestState();
        if (abortComplete == IDX_TRUE) {
          if (failureTypes[failure] == FailureVariable::PARENT_FAILED()
              || failureTypes[failure] == FailureVariable::PARENT_EXITED()) {
            assertTrue(destState == FINISHED_STATE);
          }
          else {
            assertTrue(destState == ITERATION_ENDED_STATE);
          }
        }
        else {
          assertTrue(destState == NO_NODE_STATE);
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool commandFailingTransTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr failureTypes[4] = {FailureVariable::INVARIANT_CONDITION_FAILED(),
                                FailureVariable::PARENT_FAILED(),
                                FailureVariable::EXITED(),
                                FailureVariable::PARENT_EXITED()};

    for (int abort = 0; abort < 3; ++abort) {
      for (int failure = 0; failure < 4; ++failure) {
        NodeId node = NodeFactory::createNode(Node::COMMAND(), "test", FAILING_STATE, con.getId(), parent);
        node->getAbortCompleteCondition()->setValue(values[abort]);
        node->getFailureTypeVariable()->setValue(failureTypes[failure]);

        debugMsg("UnitTest:finishedTransition",
                 "Testing Command with"
                 << " abort complete = " << values[abort]
                 << " failure type = " << failureTypes[failure].toString());

        if (node->canTransition()) {
          node->transition(node->getDestState());
          NodeState state = node->getState();

          if (abort == IDX_TRUE) {
            if (failureTypes[failure] == FailureVariable::PARENT_FAILED()
                || failureTypes[failure] == FailureVariable::PARENT_EXITED()) {
              assertTrue(state == FINISHED_STATE);
            }
            else {
              assertTrue(state == ITERATION_ENDED_STATE);
              assertTrue(node->isRepeatConditionActive());
              assertTrue(node->isAncestorEndConditionActive());
              assertTrue(node->isAncestorExitConditionActive());
              assertTrue(node->isAncestorInvariantConditionActive());
            }
          }
          else {
            assertTrue(false);
          }
        }
        delete (Node*) node;
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool commandFinishingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::COMMAND(), "test", FINISHING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        node->getExitCondition()->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
          for (int invariant = 0; invariant < 3; ++invariant) {
            node->getInvariantCondition()->setValue(values[invariant]);
            for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
              node->getActionCompleteCondition()->setValue(values[actionComplete]);

              NodeState destState = node->getDestState();
              if (ancestorExit == IDX_TRUE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (exit == IDX_TRUE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (ancestorInvariant == IDX_FALSE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (invariant == IDX_FALSE) {
                assertTrue(destState == FAILING_STATE);
              }
              else if (actionComplete == IDX_TRUE) {
                assertTrue(destState == ITERATION_ENDED_STATE);
              }
              else {
                assertTrue(destState == NO_NODE_STATE);
              }
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool commandFinishingTransTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int invariant = 0; invariant < 3; ++invariant) {
            for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
              for (int post = 0; post < 3; ++post) {
                NodeId node = NodeFactory::createNode(Node::COMMAND(), "test", FINISHING_STATE, con.getId(), parent);
                node->getAncestorExitCondition()->setValue(values[ancestorExit]);
                node->getExitCondition()->setValue(values[exit]);
                node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
                node->getInvariantCondition()->setValue(values[invariant]);
                node->getActionCompleteCondition()->setValue(values[actionComplete]);
                node->getPostCondition()->setValue(values[post]);
                debugMsg("UnitTest:actionExecutingTransition",
                         "Testing Command with"
                         << " ancestor exit = " << values[ancestorExit]
                         << " exit = " << values[exit]
                         << " ancestor invariant = " << values[ancestorInvariant]
                         << " invariant = " << values[invariant]
                         << " action complete = " << values[actionComplete]
                         << " post = " << values[post]
                         );

                if (node->canTransition()) {
                  node->transition(node->getDestState());
                  NodeState state = node->getState();
                  if (ancestorExit == IDX_TRUE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isAbortCompleteConditionActive());
                  }
                  else if (exit == IDX_TRUE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::EXITED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isAbortCompleteConditionActive());
                  }
                  else if (ancestorInvariant == IDX_FALSE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isAbortCompleteConditionActive());
                  }
                  else if (invariant == IDX_FALSE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::INVARIANT_CONDITION_FAILED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isAbortCompleteConditionActive());
                  }
                  else if (actionComplete == IDX_TRUE) {
                    assertTrue(state == ITERATION_ENDED_STATE);
                    assertTrue(node->isAncestorEndConditionActive());
                    assertTrue(node->isAncestorExitConditionActive());
                    assertTrue(node->isAncestorInvariantConditionActive());
                    assertTrue(node->isRepeatConditionActive());
                    if (post == IDX_TRUE) {
                      assertTrue(node->getOutcome() == OutcomeVariable::SUCCESS());
                    }
                    else {
                      assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::POST_CONDITION_FAILED());
                    }
                  }
                  else {
                    assertTrue(false);
                  }
                }
                delete (Node*) node;
              }
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  //
  // Update nodes
  //

  static bool updateExecutingDestTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::UPDATE(), "test", EXECUTING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      node->getAncestorExitCondition()->setValue(values[ancestorExit]);
      for (int exit = 0; exit < 3; ++exit) {
        node->getExitCondition()->setValue(values[exit]);
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
          for (int invariant = 0; invariant < 3; ++invariant) {
            node->getInvariantCondition()->setValue(values[invariant]);
            for (int end = 0; end < 3; ++end) {
              node->getEndCondition()->setValue(values[end]);
              for (int post = 0; post < 3; ++post) {
                node->getPostCondition()->setValue(values[post]);

                NodeState destState = node->getDestState();
                if (ancestorExit == IDX_TRUE) {
                  assertTrue(destState == FAILING_STATE);
                }
                else if (exit == IDX_TRUE) {
                  assertTrue(destState == FAILING_STATE);
                }
                else if (ancestorInvariant == IDX_FALSE) {
                  assertTrue(destState == FAILING_STATE);
                }
                else if (invariant == IDX_FALSE) {
                  assertTrue(destState == FAILING_STATE);
                }
                else if (end == IDX_TRUE) {
                  assertTrue(destState == ITERATION_ENDED_STATE);
                }
                else {
                  assertTrue(destState == NO_NODE_STATE);
                }
              }
            }
          }
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool updateExecutingTransTest()
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};

    for (int ancestorExit = 0; ancestorExit < 3; ++ancestorExit) {
      for (int exit = 0; exit < 3; ++exit) {
        for (int ancestorInvariant = 0; ancestorInvariant < 3; ++ancestorInvariant) {
          for (int invariant = 0; invariant < 3; ++invariant) {
            for (int end = 0; end < 3; ++end) {
              for (int post = 0; post < 3; ++post) {
                NodeId node = NodeFactory::createNode(Node::UPDATE(), "test", EXECUTING_STATE, con.getId(), parent);
                node->getAncestorExitCondition()->setValue(values[ancestorExit]);
                node->getExitCondition()->setValue(values[exit]);
                node->getAncestorInvariantCondition()->setValue(values[ancestorInvariant]);
                node->getInvariantCondition()->setValue(values[invariant]);
                node->getEndCondition()->setValue(values[end]);
                node->getPostCondition()->setValue(values[post]);
                debugMsg("UnitTest:actionExecutingTransition",
                         "Testing Update with"
                         << " ancestor exit = " << values[ancestorExit]
                         << " exit = " << values[exit]
                         << " ancestor invariant = " << values[ancestorInvariant]
                         << " invariant = " << values[invariant]
                         << " end = " << values[end]
                         << " post = " << values[post]
                         );

                if (node->canTransition()) {
                  node->transition(node->getDestState());
                  NodeState state = node->getState();
                  if (ancestorExit == IDX_TRUE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_EXITED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isActionCompleteConditionActive());
                  }
                  else if (exit == IDX_TRUE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::INTERRUPTED());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::EXITED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isActionCompleteConditionActive());
                  }
                  else if (ancestorInvariant == IDX_FALSE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::PARENT_FAILED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isActionCompleteConditionActive());
                  }
                  else if (invariant == IDX_FALSE) {
                    assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                    assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::INVARIANT_CONDITION_FAILED());
                    assertTrue(state == FAILING_STATE);
                    assertTrue(node->isActionCompleteConditionActive());
                  }
                  else if (end == IDX_TRUE) {
                    assertTrue(state == ITERATION_ENDED_STATE);
                    assertTrue(node->isAncestorEndConditionActive());
                    assertTrue(node->isAncestorExitConditionActive());
                    assertTrue(node->isAncestorInvariantConditionActive());
                    if (post == IDX_TRUE) {
                      assertTrue(node->getOutcome() == OutcomeVariable::SUCCESS());
                    }
                    else {
                      assertTrue(node->getOutcome() == OutcomeVariable::FAILURE());
                      assertTrue(node->getFailureTypeVariable()->getValue() == FailureVariable::POST_CONDITION_FAILED());
                    }
                  }
                  else {
                    assertTrue(false);
                  }
                }
                delete (Node*) node;
              }
            }
          }
        }
      }
    }
    delete (Node*) parent;
    return true;
  }

  static bool updateFailingDestTest() 
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    NodeId node = NodeFactory::createNode(Node::UPDATE(), "test", FAILING_STATE, con.getId(), parent);
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    double failureTypes[4] = {FailureVariable::INVARIANT_CONDITION_FAILED(),
                              FailureVariable::PARENT_FAILED(),
                              FailureVariable::EXITED(),
                              FailureVariable::PARENT_EXITED()};
    for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
      node->getActionCompleteCondition()->setValue(values[actionComplete]);
      for (int failure = 0; failure < 4; ++failure) {
        node->getFailureTypeVariable()->setValue(failureTypes[failure]);
        NodeState destState = node->getDestState();
        if (actionComplete == IDX_TRUE) {
          if (failureTypes[failure] == FailureVariable::PARENT_FAILED()
              || failureTypes[failure] == FailureVariable::PARENT_EXITED()) {
            assertTrue(destState == FINISHED_STATE);
          }
          else {
            assertTrue(destState == ITERATION_ENDED_STATE);
          }
        }
        else {
          assertTrue(destState == NO_NODE_STATE);
        }
      }
    }
    delete (Node*) node;
    delete (Node*) parent;
    return true;
  }

  static bool updateFailingTransTest() 
  {
    TransitionExecConnector con;
    NodeId parent = NodeFactory::createNode(Node::LIST(), "testParent", EXECUTING_STATE, con.getId(), NodeId::noId());
    double values[3] = {Expression::UNKNOWN(), BooleanVariable::FALSE_VALUE(), BooleanVariable::TRUE_VALUE()};
    LabelStr failureTypes[4] = {FailureVariable::INVARIANT_CONDITION_FAILED(),
                                FailureVariable::PARENT_FAILED(),
                                FailureVariable::EXITED(),
                                FailureVariable::PARENT_EXITED()};

    for (int actionComplete = 0; actionComplete < 3; ++actionComplete) {
      for (int failure = 0; failure < 4; ++failure) {
        NodeId node = NodeFactory::createNode(Node::UPDATE(), "test", FAILING_STATE, con.getId(), parent);
        node->getActionCompleteCondition()->setValue(values[actionComplete]);
        node->getFailureTypeVariable()->setValue(failureTypes[failure]);

        debugMsg("UnitTest:finishedTransition",
                 "Testing Update with"
                 << " action complete = " << values[actionComplete]
                 << " failure type = " << failureTypes[failure].toString());

        if (node->canTransition()) {
          node->transition(node->getDestState());
          NodeState state = node->getState();

          if (actionComplete == IDX_TRUE) {
            if (failureTypes[failure] == FailureVariable::PARENT_FAILED()
                || failureTypes[failure] == FailureVariable::PARENT_EXITED()) {
              assertTrue(state == FINISHED_STATE);
            }
            else {
              assertTrue(state == ITERATION_ENDED_STATE);
              assertTrue(node->isRepeatConditionActive());
              assertTrue(node->isAncestorEndConditionActive());
              assertTrue(node->isAncestorExitConditionActive());
              assertTrue(node->isAncestorInvariantConditionActive());
            }
          }
          else {
            assertTrue(false);
          }
        }
        delete (Node*) node;
      }
    }
    delete (Node*) parent;
    return true;
  }

};

class CacheTestInterface : public ExternalInterface {
public:
  CacheTestInterface(const StateCacheId& cache)
  : ExternalInterface(), m_cache(cache), m_lookupNowCalled(false)
  {}

  double lookupNow(const State& state) 
  {
    m_lookupNowCalled = true;
    return m_values[state];
  }

  void subscribe(const State& /* state */)
  {
  }

  void unsubscribe(const State& /* state */)
  {
  }

  void setThresholds(const State& /* state */, double /* hi */, double /* lo */)
  {
  }

  void batchActions(std::list<CommandId>& /* commands */)
  {
  }

  void updatePlanner(std::list<UpdateId>& /* updates */)
  {
  }

  void invokeAbort(const CommandId& /* cmd */)
  {
  }

  double currentTime()
  {
    return 0.0;
  }

  bool lookupNowCalled() {return m_lookupNowCalled;}
  void clearLookupNowCalled() {m_lookupNowCalled = false;}
  void setValue(const State& state, const double& value, StateCacheId cache, bool update = true) {
    std::map<State, double>::iterator it = m_values.find(state);
    if (it == m_values.end())
      m_values.insert(std::make_pair(state, value));
    else
      it->second = value;
    if (update)
      cache->updateState(state, value);
  }
protected:
private:
  std::map<State, double> m_values;
  StateCacheId m_cache;
  bool m_lookupNowCalled;
};

class StateCacheTest {
public:
  static bool test() {
    runTest(testLookupNow);
    runTest(testChangeLookup);
    return true;
  }
private:
  static bool testLookupNow() {
    StateCache cache;
    CacheTestInterface iface(cache.getId());
    cache.setExternalInterface(iface.getId());

    IntegerVariable destVar;
    destVar.activate();

    State st(LabelStr("foo"), std::vector<double>());

    iface.setValue(st, 1, cache.getId(), false);
    cache.handleQuiescenceStarted();

    //single lookup for new state
    assertTrue(destVar.getValue() == Expression::UNKNOWN());
    cache.registerLookupNow(destVar.getId(), st);
    assertTrue(iface.lookupNowCalled());
    assertTrue(destVar.getValue() == 1);
    cache.unregisterLookupNow(destVar.getId());

    //re-lookup for same state in same quiescence
    iface.setValue(st, 2, cache.getId(), false);
    iface.clearLookupNowCalled();
    cache.registerLookupNow(destVar.getId(), st);
    assertTrue(!iface.lookupNowCalled());
    assertTrue(destVar.getValue() == 1);
    cache.unregisterLookupNow(destVar.getId());

    //re-lookup for same state in next quiescence
    cache.handleQuiescenceEnded();
    cache.handleQuiescenceStarted();
    cache.registerLookupNow(destVar.getId(), st);
    assertTrue(iface.lookupNowCalled());
    assertTrue(destVar.getValue() == 2);
    cache.unregisterLookupNow(destVar.getId());

    // *** TODO: Add test for updating LookupNow that 
    // *** remains active across multiple quiescence cycles

    return true;
  }

  static bool testChangeLookup() {
    StateCache cache;
    CacheTestInterface iface(cache.getId());
    cache.setExternalInterface(iface.getId());

    IntegerVariable destVar1, destVar2;
    destVar1.activate();
    destVar2.activate();

    State st(LabelStr("foo"), std::vector<double>());

    //lookup
    iface.setValue(st, 1, cache.getId(), false);
    cache.handleQuiescenceStarted();
    cache.registerChangeLookup(destVar1.getId(), st, 1);
    assertTrue(destVar1.getValue() == 1);
    cache.registerChangeLookup(destVar2.getId(), st, 2);
    assertTrue(destVar2.getValue() == 1);
    cache.handleQuiescenceEnded();

    //update value
    iface.setValue(st, 2, cache.getId());
    assertTrue(destVar1.getValue() == 2);
    assertTrue(destVar2.getValue() == 1);

    //lookupNow triggering change
    IntegerVariable nowDestVar;
    nowDestVar.activate();
    iface.setValue(st, 3, cache.getId(), false);
    cache.handleQuiescenceStarted();
    cache.handleQuiescenceEnded();
    cache.handleQuiescenceStarted();
    cache.registerLookupNow(nowDestVar.getId(), st);
    assertTrue(nowDestVar.getValue() == 3);
    assertTrue(destVar1.getValue() == 3);
    assertTrue(destVar2.getValue() == 3);

    //unregister
    cache.unregisterLookupNow(nowDestVar.getId());
    cache.unregisterChangeLookup(destVar2.getId());
    cache.handleQuiescenceEnded();
    iface.setValue(st, 5, cache.getId());
    assertTrue(destVar2.getValue() == 3);
    assertTrue(destVar1.getValue() == 5);
    return true;
  }
};


void ExecModuleTests::runTests() {
  REGISTER_EXPRESSION(AllocationExpression, AllocationExpression);
  REGISTER_EXPRESSION(Conjunction, AND);
  REGISTER_EXPRESSION(Disjunction, OR);
  REGISTER_EXPRESSION(ExclusiveDisjunction, XOR);
  REGISTER_EXPRESSION(LogicalNegation, NOT);
  REGISTER_EXPRESSION(IsKnown, IsKnown);
  REGISTER_EXPRESSION(Equality, EQ);
  REGISTER_EXPRESSION(Equality, EQNumeric);
  REGISTER_EXPRESSION(Equality, EQBoolean);
  REGISTER_EXPRESSION(Equality, EQString);
  REGISTER_EXPRESSION(Inequality, NE);
  REGISTER_EXPRESSION(Inequality, NENumeric);
  REGISTER_EXPRESSION(Inequality, NEBoolean);
  REGISTER_EXPRESSION(Inequality, NEString);
  REGISTER_EXPRESSION(LessThan, LT);
  REGISTER_EXPRESSION(LessEqual, LE);
  REGISTER_EXPRESSION(GreaterThan, GT);
  REGISTER_EXPRESSION(GreaterEqual, GE);
  REGISTER_EXPRESSION(Addition, ADD);
  REGISTER_EXPRESSION(Subtraction, SUB);
  REGISTER_EXPRESSION(Multiplication, MUL);
  REGISTER_EXPRESSION(Division, DIV);
  REGISTER_EXPRESSION(ArrayVariable, Array);
  REGISTER_CONSTANT_EXPRESSION(ArrayVariable, ArrayValue);
  REGISTER_EXPRESSION(BooleanVariable, Boolean);
  REGISTER_CONSTANT_EXPRESSION(BooleanVariable, BooleanValue);
  REGISTER_EXPRESSION(IntegerVariable, Integer);
  REGISTER_CONSTANT_EXPRESSION(IntegerVariable, IntegerValue);
  REGISTER_EXPRESSION(RealVariable, Real);
  REGISTER_CONSTANT_EXPRESSION(RealVariable, RealValue);
  REGISTER_EXPRESSION(StringVariable, String);
  REGISTER_CONSTANT_EXPRESSION(StringVariable, StringValue);
  REGISTER_EXPRESSION(StateVariable, NodeStateValue);
  REGISTER_EXPRESSION(OutcomeVariable, NodeOutcomeValue);
  REGISTER_EXPRESSION(InternalCondition, EQInternal);
  REGISTER_EXPRESSION(InternalCondition, NEInternal);
  REGISTER_EXPRESSION(LookupNow, LookupNow);
  REGISTER_EXPRESSION(LookupOnChange, LookupOnChange);
  REGISTER_EXPRESSION(AbsoluteValue, ABS);
  REGISTER_EXPRESSION(TimepointVariable, NodeTimepointValue);
  //these are to make sure that the id count gets captured properly
  BooleanVariable::FALSE_EXP();
  BooleanVariable::TRUE_EXP();
  BooleanVariable::UNKNOWN_EXP();
  IntegerVariable::ZERO_EXP();
  IntegerVariable::ONE_EXP();
  IntegerVariable::MINUS_ONE_EXP();
  IntegerVariable::UNKNOWN_EXP();
  RealVariable::ZERO_EXP();
  RealVariable::ONE_EXP();
  RealVariable::MINUS_ONE_EXP();
  RealVariable::UNKNOWN_EXP();
  StateVariable::EXECUTING_EXP();

  runTestSuite(ExpressionsTest::test);
  runTestSuite(BaseExpressionTest::test);
  runTestSuite(CoreExpressionsTest::test);
  runTestSuite(LookupsTest::test);
  runTestSuite(StateTransitionsTest::test);
  runTestSuite(StateCacheTest::test);
  std::cout << "Finished" << std::endl;
}
