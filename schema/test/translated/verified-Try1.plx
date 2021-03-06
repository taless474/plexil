<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Try">
      <NodeId>Root</NodeId>
      <VariableDeclarations>
         <DeclareVariable>
            <Name>foo</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>0</IntegerValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <PostCondition>
         <AND>
            <OR>
               <EQInternal>
                  <NodeOutcomeVariable>
                     <NodeId>One</NodeId>
                  </NodeOutcomeVariable>
                  <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
               </EQInternal>
               <EQInternal>
                  <NodeOutcomeVariable>
                     <NodeId>Two</NodeId>
                  </NodeOutcomeVariable>
                  <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
               </EQInternal>
            </OR>
         </AND>
      </PostCondition>
      <EndCondition>
         <OR>
            <EQInternal>
               <NodeOutcomeVariable>
                  <NodeId>One</NodeId>
               </NodeOutcomeVariable>
               <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
            <EQInternal>
               <NodeOutcomeVariable>
                  <NodeId>Two</NodeId>
               </NodeOutcomeVariable>
               <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
            </EQInternal>
            <AND>
               <EQInternal>
                  <NodeStateVariable>
                     <NodeId>One</NodeId>
                  </NodeStateVariable>
                  <NodeStateValue>FINISHED</NodeStateValue>
               </EQInternal>
               <EQInternal>
                  <NodeStateVariable>
                     <NodeId>Two</NodeId>
                  </NodeStateVariable>
                  <NodeStateValue>FINISHED</NodeStateValue>
               </EQInternal>
            </AND>
         </OR>
      </EndCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="Assignment">
               <NodeId>One</NodeId>
               <PostCondition>
                  <EQNumeric>
                     <IntegerVariable>foo</IntegerVariable>
                     <IntegerValue>3</IntegerValue>
                  </EQNumeric>
               </PostCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>foo</IntegerVariable>
                     <NumericRHS>
                        <IntegerValue>3</IntegerValue>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
            <Node NodeType="Assignment">
               <NodeId>Two</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>One</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <PostCondition>
                  <EQNumeric>
                     <IntegerVariable>foo</IntegerVariable>
                     <IntegerValue>6</IntegerValue>
                  </EQNumeric>
               </PostCondition>
               <NodeBody>
                  <Assignment>
                     <IntegerVariable>foo</IntegerVariable>
                     <NumericRHS>
                        <IntegerValue>6</IntegerValue>
                     </NumericRHS>
                  </Assignment>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>