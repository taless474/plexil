<?xml version="1.0" encoding="utf-8"?>
<!-- Generated by PlexiLisp 2.0.4 on Tue Jul 31 15:58:30 2007 -->
<PlexilPlan>
  <Node NodeType="NodeList">
    <NodeId>Root</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>done</Name>
        <Type>Boolean</Type>
        <InitialValue>
          <BooleanValue>false</BooleanValue>
        </InitialValue>
      </DeclareVariable>
    </VariableDeclarations>
    <EndCondition>
      <BooleanVariable>done</BooleanVariable>
    </EndCondition>
    <NodeBody>
      <NodeList>
        <Node NodeType="Assignment">
          <NodeId>set</NodeId>
          <RepeatCondition>
            <BooleanValue>1</BooleanValue>
          </RepeatCondition>
          <NodeBody>
            <Assignment>
              <BooleanVariable>done</BooleanVariable>
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