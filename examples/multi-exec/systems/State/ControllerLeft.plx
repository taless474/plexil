<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations/>
   <Node NodeType="NodeList" epx="Sequence" FileName="ControllerLeft.ple" LineNo="5"
         ColNo="1">
      <NodeId>Controller</NodeId>
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
      <InvariantCondition>
         <AND>
            <NOT>
               <OR>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>CheckMoveLeftState</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>Loop</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
               </OR>
            </NOT>
         </AND>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="NodeList" epx="Sequence" FileName="ControllerLeft.ple" LineNo="11"
                  ColNo="9">
               <NodeId>CheckMoveLeftState</NodeId>
               <InvariantCondition>
                  <AND>
                     <NOT>
                        <OR>
                           <EQInternal>
                              <NodeOutcomeVariable>
                                 <NodeId>SendRequest</NodeId>
                              </NodeOutcomeVariable>
                              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                           </EQInternal>
                           <EQInternal>
                              <NodeOutcomeVariable>
                                 <NodeId>GetTrue</NodeId>
                              </NodeOutcomeVariable>
                              <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                           </EQInternal>
                        </OR>
                     </NOT>
                  </AND>
               </InvariantCondition>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="Command" FileName="ControllerLeft.ple" LineNo="14" ColNo="25">
                        <NodeId>SendRequest</NodeId>
                        <NodeBody>
                           <Command>
                              <Name>
                                 <StringValue>SendMessage</StringValue>
                              </Name>
                              <Arguments>
                                 <StringValue>GetShouldMoveLeft</StringValue>
                              </Arguments>
                           </Command>
                        </NodeBody>
                     </Node>
                     <Node NodeType="NodeList" epx="Sequence" FileName="ControllerLeft.ple" LineNo="18"
                           ColNo="25">
                        <NodeId>GetTrue</NodeId>
                        <StartCondition>
                           <AND>
                              <EQInternal>
                                 <NodeStateVariable>
                                    <NodeId>SendRequest</NodeId>
                                 </NodeStateVariable>
                                 <NodeStateValue>FINISHED</NodeStateValue>
                              </EQInternal>
                              <LookupOnChange>
                                 <Name>
                                    <Concat>
                                       <StringValue>MESSAGE__</StringValue>
                                       <StringValue>ShouldMoveLeft::true</StringValue>
                                    </Concat>
                                 </Name>
                              </LookupOnChange>
                           </AND>
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
                                          <NodeId>Increment</NodeId>
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
                        <SkipCondition>
                           <LookupOnChange>
                              <Name>
                                 <Concat>
                                    <StringValue>MESSAGE__</StringValue>
                                    <StringValue>ShouldMoveLeft::false</StringValue>
                                 </Concat>
                              </Name>
                           </LookupOnChange>
                        </SkipCondition>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="LibraryNodeCall" FileName="ControllerLeft.ple" LineNo="23" ColNo="41">
                                 <NodeId>RecMoveLeftCall</NodeId>
                                 <NodeBody>
                                    <LibraryNodeCall>
                                       <NodeId>MoveLeftCommand</NodeId>
                                    </LibraryNodeCall>
                                 </NodeBody>
                              </Node>
                              <Node NodeType="Assignment" FileName="ControllerLeft.ple" LineNo="27" ColNo="41">
                                 <NodeId>Increment</NodeId>
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
                                    <Assignment>
                                       <IntegerVariable>calls</IntegerVariable>
                                       <NumericRHS>
                                          <ADD>
                                             <IntegerVariable>calls</IntegerVariable>
                                             <IntegerValue>1</IntegerValue>
                                          </ADD>
                                       </NumericRHS>
                                    </Assignment>
                                 </NodeBody>
                              </Node>
                              <Node NodeType="Command" FileName="ControllerLeft.ple" LineNo="31" ColNo="41">
                                 <NodeId>RespondMoveLeft</NodeId>
                                 <StartCondition>
                                    <AND>
                                       <EQInternal>
                                          <NodeStateVariable>
                                             <NodeId>Increment</NodeId>
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
                                          <StringValue>ResetMoveLeft</StringValue>
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
            <Node NodeType="NodeList" FileName="ControllerLeft.ple" LineNo="37" ColNo="9">
               <NodeId>Loop</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>CheckMoveLeftState</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <RepeatCondition>
                  <BooleanVariable>continue</BooleanVariable>
               </RepeatCondition>
               <EndCondition>
                  <OR>
                     <EQInternal>
                          <NodeStateVariable>
                              <NodeId>RecUpdate</NodeId>
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
                      <Node NodeType="NodeList" epx="Sequence" FileName="ControllerLeft.ple" LineNo="43"
                           ColNo="25">
                        <NodeId>RecUpdate</NodeId>
                        <StartCondition>
                           <LookupOnChange>
                              <Name>
                                 <Concat>
                                    <StringValue>MESSAGE__</StringValue>
                                    <StringValue>Update::ShouldMoveLeft::true</StringValue>
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
                                          <NodeId>Increment</NodeId>
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
                              <Node NodeType="LibraryNodeCall" FileName="ControllerLeft.ple" LineNo="47" ColNo="41">
                                 <NodeId>RecMoveLeftCall</NodeId>
                                 <NodeBody>
                                    <LibraryNodeCall>
                                      <NodeId>MoveLeftCommand</NodeId>
                                    </LibraryNodeCall>
                                 </NodeBody>
                              </Node>
                              <Node NodeType="Assignment" FileName="ControllerLeft.ple" LineNo="51" ColNo="41">
                                 <NodeId>Increment</NodeId>
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
                                    <Assignment>
                                      <IntegerVariable>calls</IntegerVariable>
                                      <NumericRHS>
                                          <ADD>
                                              <IntegerVariable>calls</IntegerVariable>
                                              <IntegerValue>1</IntegerValue>
                                          </ADD>
                                      </NumericRHS>
                                    </Assignment>
                                 </NodeBody>
                              </Node>
                              <Node NodeType="Command" FileName="ControllerLeft.ple" LineNo="55" ColNo="41">
                                 <NodeId>RespondMoveLeft</NodeId>
                                 <StartCondition>
                                    <AND>
                                       <EQInternal>
                                          <NodeStateVariable>
                                             <NodeId>Increment</NodeId>
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
                                          <StringValue>ResetMoveLeft</StringValue>
                                      </Arguments>
                                    </Command>
                                 </NodeBody>
                              </Node>
                           </NodeList>
                        </NodeBody>
                     </Node>
                      <Node NodeType="Assignment" FileName="ControllerLeft.ple" LineNo="60" ColNo="25">
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
                        <NodeBody>
                           <Assignment>
                                  <BooleanVariable>continue</BooleanVariable>
                                  <BooleanRHS>
                                      <BooleanValue>false</BooleanValue>
                                  </BooleanRHS>
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