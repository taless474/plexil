<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <Node NodeType="Assignment">
    <NodeId>StartConditionCheck</NodeId>
      <VariableDeclarations>
      <DeclareVariable>
        <Name>foo</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>0</IntegerValue>
        </InitialValue>
      </DeclareVariable>
      </VariableDeclarations>
      <StartCondition>
	<GE>
	  <LookupOnChange>
	    <Name><StringValue>some_state</StringValue></Name>
	    <Tolerance><RealValue>1.0</RealValue></Tolerance>
	  </LookupOnChange>
	  <RealValue>3.0</RealValue>
	</GE>
      </StartCondition>
    <NodeBody>
      <Assignment>
	<IntegerVariable>foo</IntegerVariable>
	<NumericRHS>
	  <IntegerValue>3</IntegerValue>
	</NumericRHS>
      </Assignment>
    </NodeBody>
  </Node>
</PlexilPlan>
