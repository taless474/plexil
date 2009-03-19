<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp 2.0.5 on Fri Aug 24 14:22:09 2007 -->
<PlexilPlan>
  <Node NodeType="Assignment">
    <NodeId>Root</NodeId>
    <VariableDeclarations>

      <!-- initialzize the array -->

      <DeclareArray>
        <Name>a4</Name>
        <Type>Real</Type>
        <MaxSize>5</MaxSize>
        <InitialValue>
          <RealValue>5</RealValue>
          <RealValue>4</RealValue>
          <RealValue>3</RealValue>
          <RealValue>2</RealValue>
          <RealValue>1</RealValue>
        </InitialValue>
      </DeclareArray>
    </VariableDeclarations>

    <!-- verify that the array is in the expected state -->

    <PostCondition>
      <AND>
        
        <!-- the first value should be 1.1 -->

        <EQNumeric>
          <ArrayElement>
            <Name>a4</Name>
            <Index>
              <IntegerValue>0</IntegerValue>
            </Index>
          </ArrayElement>
          <RealValue>1.1</RealValue>
        </EQNumeric>    

        <!-- the last value of the array should be unknown -->
        
        <NOT>
          <IsKnown>
            <ArrayElement>
              <Name>a4</Name>
              <Index>
                <IntegerValue>4</IntegerValue>
              </Index>
            </ArrayElement>
          </IsKnown>
        </NOT>
      </AND>
    </PostCondition>

    <!-- assign array new value -->

    <NodeBody>
      <Assignment>
        <ArrayVariable>a4</ArrayVariable>
        <LookupRHS>
          <LookupNow>
            <Name>
              <StringValue>a5</StringValue>
            </Name>
          </LookupNow>
        </LookupRHS>
      </Assignment>
    </NodeBody>
  </Node>
</PlexilPlan>