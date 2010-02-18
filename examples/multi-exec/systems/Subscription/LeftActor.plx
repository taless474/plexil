<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations/>
   <Node NodeType="NodeList" FileName="LeftActor.ple" LineNo="7" ColNo="1">
      <NodeId>Interface</NodeId>
      <VariableDeclarations>
              <DeclareVariable>
                  <Name>calls</Name>
                  <Type>Integer</Type>
                  <InitialValue>
                      <IntegerValue>0</IntegerValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>continue</Name>
                  <Type>Boolean</Type>
                  <InitialValue>
                      <BooleanValue>true</BooleanValue>
                  </InitialValue>
              </DeclareVariable>
          </VariableDeclarations>
      <NodeBody>
         <NodeList>
                  <Node NodeType="NodeList" FileName="LeftActor.ple" LineNo="12" ColNo="17">
               <NodeId>Loop</NodeId>
               <RepeatCondition>
                  <BooleanVariable>continue</BooleanVariable>
               </RepeatCondition>
               <EndCondition>
                  <OR>
                     <EQInternal>
                                  <NodeStateVariable>
                                      <NodeId>RecMoveLeft</NodeId>
                                  </NodeStateVariable>
                                  <NodeStateValue>FINISHED</NodeStateValue>
                              </EQInternal>
                     <EQInternal>
                                  <NodeStateVariable>
                                      <NodeId>RecQuit</NodeId>
                                  </NodeStateVariable>
                                  <NodeStateValue>FINISHED</NodeStateValue>
                              </EQInternal>
                  </OR>
               </EndCondition>
               <NodeBody>
                  <NodeList>
                              <Node NodeType="NodeList" epx="Sequence" FileName="LeftActor.ple" LineNo="20"
                           ColNo="33">
                        <NodeId>RecMoveLeft</NodeId>
                        <StartCondition>
                           <LookupOnChange>
                              <Name>
                                 <Concat>
                                    <StringValue>MESSAGE__</StringValue>
                                    <StringValue>MoveLeft</StringValue>
                                 </Concat>
                              </Name>
                           </LookupOnChange>
                        </StartCondition>
                        <InvariantCondition>
                           <AND>
                              <NOT>
                                 <OR>
                                    <EQInternal>
                                       <NodeOutcomeVariable>
                                          <NodeId>RecMoveLeftCall</NodeId>
                                       </NodeOutcomeVariable>
                                       <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                    </EQInternal>
                                    <EQInternal>
                                       <NodeOutcomeVariable>
                                          <NodeId>RespondMoveLeft</NodeId>
                                       </NodeOutcomeVariable>
                                       <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                    </EQInternal>
                                 </OR>
                              </NOT>
                           </AND>
                        </InvariantCondition>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="LibraryNodeCall" FileName="LeftActor.ple" LineNo="24" ColNo="49">
                                 <NodeId>RecMoveLeftCall</NodeId>
                                 <NodeBody>
                                    <LibraryNodeCall>
                                              <NodeId>MoveLeftCommand</NodeId>
                                          </LibraryNodeCall>
                                 </NodeBody>
                              </Node>
                              <Node NodeType="Command" FileName="LeftActor.ple" LineNo="28" ColNo="49">
                                 <NodeId>RespondMoveLeft</NodeId>
                                 <StartCondition>
                                    <AND>
                                       <EQInternal>
                                          <NodeStateVariable>
                                             <NodeId>RecMoveLeftCall</NodeId>
                                          </NodeStateVariable>
                                          <NodeStateValue>FINISHED</NodeStateValue>
                                       </EQInternal>
                                    </AND>
                                 </StartCondition>
                                 <NodeBody>
                                    <Command>
                                              <Name>
                                                  <StringValue>SendMessage</StringValue>
                                              </Name>
                                              <Arguments>
                                                  <StringValue>FinishedMoveLeft</StringValue>
                                              </Arguments>
                                          </Command>
                                 </NodeBody>
                              </Node>
                           </NodeList>
                        </NodeBody>
                     </Node>
                              <Node NodeType="NodeList" epx="Sequence" FileName="LeftActor.ple" LineNo="33"
                           ColNo="33">
                        <NodeId>RecQuit</NodeId>
                        <StartCondition>
                           <LookupOnChange>
                              <Name>
                                 <Concat>
                                    <StringValue>MESSAGE__</StringValue>
                                    <StringValue>Quit</StringValue>
                                 </Concat>
                              </Name>
                           </LookupOnChange>
                        </StartCondition>
                        <InvariantCondition>
                           <AND>
                              <NOT>
                                 <OR>
                                    <EQInternal>
                                       <NodeOutcomeVariable>
                                          <NodeId>RecMoveRightCall</NodeId>
                                       </NodeOutcomeVariable>
                                       <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                    </EQInternal>
                                    <EQInternal>
                                       <NodeOutcomeVariable>
                                          <NodeId>RespondMoveRight</NodeId>
                                       </NodeOutcomeVariable>
                                       <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                    </EQInternal>
                                 </OR>
                              </NOT>
                           </AND>
                        </InvariantCondition>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="Assignment" FileName="LeftActor.ple" LineNo="37" ColNo="49">
                                 <NodeId>RecMoveRightCall</NodeId>
                                 <NodeBody>
                                    <Assignment>
                                              <BooleanVariable>continue</BooleanVariable>
                                              <BooleanRHS>
                                                  <BooleanValue>false</BooleanValue>
                                              </BooleanRHS>
                                          </Assignment>
                                 </NodeBody>
                              </Node>
                              <Node NodeType="Command" FileName="LeftActor.ple" LineNo="41" ColNo="49">
                                 <NodeId>RespondMoveRight</NodeId>
                                 <StartCondition>
                                    <AND>
                                       <EQInternal>
                                          <NodeStateVariable>
                                             <NodeId>RecMoveRightCall</NodeId>
                                          </NodeStateVariable>
                                          <NodeStateValue>FINISHED</NodeStateValue>
                                       </EQInternal>
                                    </AND>
                                 </StartCondition>
                                 <NodeBody>
                                    <Command>
                                              <Name>
                                                  <StringValue>SendMessage</StringValue>
                                              </Name>
                                              <Arguments>
                                                  <StringValue>FinishedQuit</StringValue>
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
              </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>