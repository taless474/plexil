<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Sequence">
      <NodeId>ep2cp_Sequence_d1e3</NodeId>
      <InvariantCondition>
         <AND>
            <NOT>
               <OR>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>A</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>B</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
               </OR>
            </NOT>
         </AND>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="NodeList" epx="SynchronousCommand" FileName="foo.ple" LineNo="104"
                  ColNo="1">
               <NodeId>A</NodeId>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="NodeList" epx="aux">
                        <NodeId>ep2cp_SynchronousCommandAux</NodeId>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="Command" epx="aux">
                                 <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                 <EndCondition>
                                    <OR>
                                       <EQInternal>
                                          <NodeCommandHandleVariable>
                                             <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                          </NodeCommandHandleVariable>
                                          <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                                       </EQInternal>
                                       <EQInternal>
                                          <NodeCommandHandleVariable>
                                             <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                          </NodeCommandHandleVariable>
                                          <NodeCommandHandleValue>COMMAND_FAILED</NodeCommandHandleValue>
                                       </EQInternal>
                                       <EQInternal>
                                          <NodeCommandHandleVariable>
                                             <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                          </NodeCommandHandleVariable>
                                          <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
                                       </EQInternal>
                                    </OR>
                                 </EndCondition>
                                 <NodeBody>
                                    <Command>
                                       <ResourceList>
                                          <Resource>
                                             <ResourceName>
                                                <StringValue>arm</StringValue>
                                             </ResourceName>
                                             <ResourcePriority>
                                                <IntegerValue>3</IntegerValue>
                                             </ResourcePriority>
                                          </Resource>
                                       </ResourceList>
                                       <Name>
                                          <StringValue>FOO</StringValue>
                                       </Name>
                                       <Arguments>
                                          <StringValue>this</StringValue>
                                          <StringValue>that</StringValue>
                                       </Arguments>
                                    </Command>
                                 </NodeBody>
                              </Node>
                           </NodeList>
                        </NodeBody>
                     </Node>
                  </NodeList>
               </NodeBody>
            </Node>
            <Node NodeType="NodeList" epx="SynchronousCommand">
               <NodeId>B</NodeId>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="NodeList" epx="aux">
                        <NodeId>ep2cp_SynchronousCommandAux</NodeId>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="Command" epx="aux">
                                 <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                 <EndCondition>
                                    <OR>
                                       <EQInternal>
                                          <NodeCommandHandleVariable>
                                             <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                          </NodeCommandHandleVariable>
                                          <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                                       </EQInternal>
                                       <EQInternal>
                                          <NodeCommandHandleVariable>
                                             <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                          </NodeCommandHandleVariable>
                                          <NodeCommandHandleValue>COMMAND_FAILED</NodeCommandHandleValue>
                                       </EQInternal>
                                       <EQInternal>
                                          <NodeCommandHandleVariable>
                                             <NodeId>ep2cp_SynchronousCommandCommand</NodeId>
                                          </NodeCommandHandleVariable>
                                          <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
                                       </EQInternal>
                                    </OR>
                                 </EndCondition>
                                 <NodeBody>
                                    <Command>
                                       <ResourceList/>
                                       <Name>
                                          <StringValue>BAR</StringValue>
                                       </Name>
                                    </Command>
                                 </NodeBody>
                              </Node>
                           </NodeList>
                        </NodeBody>
                     </Node>
                  </NodeList>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>