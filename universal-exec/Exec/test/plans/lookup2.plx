<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp 2.0.5 on Mon Feb 11 15:44:39 2008 -->
<PlexilPlan>
  <Node NodeType="Assignment" >
    <NodeId>Root</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>x</Name>
        <Type>Integer</Type>
      </DeclareVariable>
      <DeclareVariable>
        <Name>state-name</Name>
        <Type>String</Type>
        <InitialValue>
          <StringValue>foo</StringValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <Assignment>
        <IntegerVariable>x</IntegerVariable>
        <NumericRHS>
          <LookupNow>
            <Name>
              <StringVariable>state-name</StringVariable>
            </Name>
          </LookupNow>
        </NumericRHS>
      </Assignment>
    </NodeBody>
  </Node>
</PlexilPlan>
