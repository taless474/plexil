<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Concurrence" LineNo="2" ColNo="0">
      <NodeId>NonLocalExit</NodeId>
      <VariableDeclarations>
            <DeclareVariable LineNo="4" ColNo="2">
                <Name>quitLooping</Name>
                <Type>Boolean</Type>
                <InitialValue>
                    <BooleanValue>false</BooleanValue>
                </InitialValue>
            </DeclareVariable>
            <DeclareVariable LineNo="5" ColNo="2">
                <Name>loopCount</Name>
                <Type>Integer</Type>
                <InitialValue>
                    <IntegerValue>0</IntegerValue>
                </InitialValue>
            </DeclareVariable>
        </VariableDeclarations>
      <NodeBody>
         <NodeList>
            <Node NodeType="Assignment" LineNo="11" ColNo="4">
               <NodeId>theLoop</NodeId>
               <RepeatCondition>
                  <LT>
                     <IntegerVariable>loopCount</IntegerVariable>
                     <IntegerValue>5</IntegerValue>
                  </LT>
               </RepeatCondition>
               <ExitCondition>
                  <BooleanVariable>quitLooping</BooleanVariable>
               </ExitCondition>
               <NodeBody>
                  <Assignment>
                    <IntegerVariable>loopCount</IntegerVariable>
                    <NumericRHS>
                        <ADD LineNo="11" ColNo="26">
                            <IntegerVariable>loopCount</IntegerVariable>
                            <IntegerValue>1</IntegerValue>
                        </ADD>
                    </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
            <Node NodeType="Assignment" LineNo="17" ColNo="4">
               <NodeId>theSpoiler</NodeId>
               <StartCondition>
                  <EQNumeric>
                     <IntegerVariable>loopCount</IntegerVariable>
                     <IntegerValue>2</IntegerValue>
                  </EQNumeric>
               </StartCondition>
               <NodeBody>
                  <Assignment>
                    <BooleanVariable>quitLooping</BooleanVariable>
                    <BooleanRHS>
                        <BooleanValue>true</BooleanValue>
                    </BooleanRHS>
                  </Assignment>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>