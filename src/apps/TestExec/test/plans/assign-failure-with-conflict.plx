<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Concurrence" LineNo="2" ColNo="0">
      <NodeId>AssignFailureWithConflict</NodeId>
      <VariableDeclarations>
         <DeclareVariable LineNo="4" ColNo="2">
            <Name>n</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>0</IntegerValue>
            </InitialValue>
         </DeclareVariable>
         <DeclareVariable LineNo="5" ColNo="2">
            <Name>spoil</Name>
            <Type>Boolean</Type>
            <InitialValue>
               <BooleanValue>false</BooleanValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <PostCondition>
         <EQNumeric>
            <IntegerVariable>n</IntegerVariable>
            <IntegerValue>2</IntegerValue>
         </EQNumeric>
      </PostCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="Assignment" LineNo="12" ColNo="4">
               <NodeId>theFailure</NodeId>
               <Priority>2</Priority>
               <InvariantCondition>
                  <NOT>
                     <BooleanVariable>spoil</BooleanVariable>
                  </NOT>
               </InvariantCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>n</IntegerVariable>
                     <NumericRHS>
                        <ADD LineNo="12" ColNo="10">
                           <IntegerVariable>n</IntegerVariable>
                           <IntegerValue>1</IntegerValue>
                        </ADD>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
            <Node NodeType="Assignment" LineNo="16" ColNo="2">
               <NodeId>theSpoiler</NodeId>
               <NodeBody>
                  <Assignment>
                     <BooleanVariable>spoil</BooleanVariable>
                     <BooleanRHS>
                        <BooleanValue>true</BooleanValue>
                     </BooleanRHS>
                  </Assignment>
               </NodeBody>
            </Node>
            <Node NodeType="Assignment" LineNo="22" ColNo="4">
               <NodeId>theConflict</NodeId>
               <Priority>1</Priority>
               <StartCondition>
                  <BooleanVariable>spoil</BooleanVariable>
               </StartCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>n</IntegerVariable>
                     <NumericRHS>
                        <IntegerValue>2</IntegerValue>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>