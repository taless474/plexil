<?xml version="1.0" encoding="UTF-8"?>
<!-- test plan -->
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	    xsi:noNamespaceSchemaLocation="PLEXIL_schema_outdated.xsd">
  <Node NodeType="Assignment">
    <NodeId>Three_Min_Timer</NodeId>
    <EndCondition> 
      <GT>
	<LookupOnChange>
	  <StateName>time</StateName>
	  <Tolerance><RealValue>180</RealValue></Tolerance>
	</LookupOnChange>
	<ADD>
	  <NodeTimepointValue>
	    <NodeRef dir="self">Three_Min_Timer</NodeRef>
	    <NodeStateValue>EXECUTING</NodeStateValue>
	    <Timepoint>START</Timepoint>
	  </NodeTimepointValue>
	  <IntegerValue>180</IntegerValue>
	</ADD>
      </GT>
    </EndCondition> 
    <VariableDeclarations> 
      <DeclareVariable>
        <Name>hack</Name>
        <Type>Boolean</Type>
      </DeclareVariable>
</VariableDeclarations> 
    <NodeBody>
      <Assignment>
	<BooleanVariable>hack</BooleanVariable>
	<BooleanRHS><BooleanValue>1</BooleanValue></BooleanRHS>
      </Assignment>
    </NodeBody>
  </Node> 
</PlexilPlan>