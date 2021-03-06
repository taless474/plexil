<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="For">
      <NodeId>ep2cp_For_d1e3</NodeId>
      <VariableDeclarations>
         <DeclareVariable>
            <Name>count</Name>
            <Type>Real</Type>
            <InitialValue>
               <RealValue>0.0</RealValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <NodeBody>
         <NodeList>
            <Node NodeType="NodeList" epx="aux">
               <NodeId>ep2cp_ForLoop</NodeId>
               <RepeatCondition>
                  <LT>
                     <RealVariable>count</RealVariable>
                     <RealValue>1.0</RealValue>
                  </LT>
               </RepeatCondition>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="NodeList" epx="aux">
                        <NodeId>ep2cp_ForDo</NodeId>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="Empty">
                                 <NodeId>One</NodeId>
                              </Node>
                           </NodeList>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" epx="aux">
                        <NodeId>ep2cp_ForLoopUpdater</NodeId>
                        <StartCondition>
                           <EQInternal>
                              <NodeStateVariable>
                                 <NodeId>ep2cp_ForDo</NodeId>
                              </NodeStateVariable>
                              <NodeStateValue>FINISHED</NodeStateValue>
                           </EQInternal>
                        </StartCondition>
                        <NodeBody>
                           <Assignment>
                              <RealVariable>count</RealVariable>
                              <NumericRHS>
                                 <ADD>
                                    <RealVariable>count</RealVariable>
                                    <RealValue>0.2</RealValue>
                                 </ADD>
                              </NumericRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                  </NodeList>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>