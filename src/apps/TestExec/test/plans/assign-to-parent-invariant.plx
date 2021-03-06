<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan>
<Node NodeType="NodeList">
  <NodeId>Parent</NodeId>
  <VariableDeclarations>
      <DeclareVariable>
        <Name>step_failed</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>0</BooleanValue>
        </InitialValue>
      </DeclareVariable>
  </VariableDeclarations>
  <InvariantCondition>
    <EQBoolean>
        <BooleanVariable>step_failed</BooleanVariable>
        <BooleanValue>0</BooleanValue>
    </EQBoolean>
  </InvariantCondition>
  <EndCondition>
    <EQInternal>
        <NodeStateVariable>
          <NodeId>Child</NodeId>
        </NodeStateVariable>
        <NodeStateValue>FINISHED</NodeStateValue>
    </EQInternal>
  </EndCondition>
  <NodeBody>
    <NodeList>
        <Node NodeType="Assignment">
          <NodeId>Child</NodeId>
          <Interface>
            <InOut>
              <DeclareVariable>
                <Name>step_failed</Name>
                <Type>Boolean</Type>
              </DeclareVariable>
            </InOut>
          </Interface>
          <NodeBody>
            <Assignment>
              <BooleanVariable>step_failed</BooleanVariable>
              <BooleanRHS>
                <BooleanValue>1</BooleanValue>
              </BooleanRHS>
            </Assignment>
          </NodeBody>
        </Node>
    </NodeList>
  </NodeBody>
</Node>
</PlexilPlan>
