<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp 2.0.5 on Wed Apr  9 16:56:55 2008 -->
<PlexilPlan>
<!-- Tests array initial values -->
  <Node NodeType="Empty" >
    <NodeId>Root</NodeId>
    <VariableDeclarations>
      <DeclareArray>
        <Name>a4</Name>
        <Type>Real</Type>
        <MaxSize>2</MaxSize>
        <InitialValue>
          <RealValue>5.0</RealValue>
          <RealValue>6.0</RealValue>
        </InitialValue>
      </DeclareArray>
    </VariableDeclarations>
    <PostCondition>
      <AND>
        <EQNumeric>
          <ArrayElement>
            <Name>a4</Name>
            <Index>
              <IntegerValue>0</IntegerValue>
            </Index>
          </ArrayElement>
          <RealValue>5.0</RealValue>
        </EQNumeric>
        <EQNumeric>
          <ArrayElement>
            <Name>a4</Name>
            <Index>
              <IntegerValue>1</IntegerValue>
            </Index>
          </ArrayElement>
          <RealValue>6.0</RealValue>
        </EQNumeric>
      </AND>
    </PostCondition>
  </Node>
</PlexilPlan>