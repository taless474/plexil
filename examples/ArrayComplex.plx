<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Sequence" FileName="ArrayComplex.ple" LineNo="2"
         ColNo="1">
      <NodeId>ArrayComplex</NodeId>
      <VariableDeclarations>
              <DeclareArray>
                  <Name>foo</Name>
                  <Type>Real</Type>
                  <MaxSize>4</MaxSize>
                  <InitialValue>
                      <RealValue>0.0</RealValue>
                      <RealValue>0.0</RealValue>
                      <RealValue>0.0</RealValue>
                      <RealValue>0.0</RealValue>
                  </InitialValue>
              </DeclareArray>
              <DeclareArray>
                  <Name>bar</Name>
                  <Type>Real</Type>
                  <MaxSize>30</MaxSize>
              </DeclareArray>
          </VariableDeclarations>
      <InvariantCondition>
         <AND>
            <NOT>
               <OR>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>Initialize</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>ArrayComplex__CHILD__2</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
               </OR>
            </NOT>
         </AND>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="NodeList" epx="For" FileName="ArrayComplex.ple" LineNo="7" ColNo="1">
               <NodeId>Initialize</NodeId>
               <VariableDeclarations>
                  <DeclareVariable>
                      <Name>i</Name>
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
                              <RealVariable>i</RealVariable>
                              <RealValue>30.0</RealValue>
                           </LT>
                        </RepeatCondition>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="NodeList" epx="aux">
                                 <NodeId>ep2cp_ForDo</NodeId>
                                 <NodeBody>
                                    <NodeList>
                                       <Node NodeType="NodeList" epx="Sequence" FileName="ArrayComplex.ple" LineNo="9"
                                             ColNo="9">
                                          <NodeId>Initialize__CHILD__2</NodeId>
                                          <InvariantCondition>
                                             <AND>
                                                <NOT>
                                                   <OR>
                                                      <EQInternal>
                                                         <NodeOutcomeVariable>
                                                            <NodeId>BarInit</NodeId>
                                                         </NodeOutcomeVariable>
                                                         <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                                      </EQInternal>
                                                   </OR>
                                                </NOT>
                                             </AND>
                                          </InvariantCondition>
                                          <NodeBody>
                                             <NodeList>
                                                <Node NodeType="Assignment" FileName="ArrayComplex.ple" LineNo="12" ColNo="17">
                                                   <NodeId>BarInit</NodeId>
                                                   <NodeBody>
                                                      <Assignment>
                                                         <ArrayElement>
                                                            <Name>bar</Name>
                                                            <Index>
                                                               <RealVariable>i</RealVariable>
                                                            </Index>
                                                         </ArrayElement>
                                                         <NumericRHS>
                                                            <RealValue>0.0</RealValue>
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
                                       <RealVariable>i</RealVariable>
                                       <NumericRHS>
                                          <ADD>
                                             <RealVariable>i</RealVariable>
                                             <IntegerValue>1</IntegerValue>
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
            <Node NodeType="NodeList" epx="For" FileName="ArrayComplex.ple" LineNo="17" ColNo="1">
               <NodeId>ArrayComplex__CHILD__2</NodeId>
               <VariableDeclarations>
                  <DeclareVariable>
                      <Name>i</Name>
                      <Type>Real</Type>
                      <InitialValue>
                          <RealValue>0.0</RealValue>
                      </InitialValue>
                  </DeclareVariable>
               </VariableDeclarations>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>Initialize</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="NodeList" epx="aux">
                        <NodeId>ep2cp_ForLoop</NodeId>
                        <RepeatCondition>
                           <LT>
                              <RealVariable>i</RealVariable>
                              <RealValue>10.0</RealValue>
                           </LT>
                        </RepeatCondition>
                        <NodeBody>
                           <NodeList>
                              <Node NodeType="NodeList" epx="aux">
                                 <NodeId>ep2cp_ForDo</NodeId>
                                 <NodeBody>
                                    <NodeList>
                                       <Node NodeType="NodeList" epx="Sequence" FileName="ArrayComplex.ple" LineNo="19"
                                             ColNo="1">
                                          <NodeId>ArrayComplex__CHILD__2__CHILD__2</NodeId>
                                          <InvariantCondition>
                                             <AND>
                                                <NOT>
                                                   <OR>
                                                      <EQInternal>
                                                         <NodeOutcomeVariable>
                                                            <NodeId>SimpleArrayAssignment</NodeId>
                                                         </NodeOutcomeVariable>
                                                         <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                                      </EQInternal>
                                                      <EQInternal>
                                                         <NodeOutcomeVariable>
                                                            <NodeId>SimpleArrayAssignment2</NodeId>
                                                         </NodeOutcomeVariable>
                                                         <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                                      </EQInternal>
                                                      <EQInternal>
                                                         <NodeOutcomeVariable>
                                                            <NodeId>SimpleArrayAssignment3</NodeId>
                                                         </NodeOutcomeVariable>
                                                         <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                                      </EQInternal>
                                                      <EQInternal>
                                                         <NodeOutcomeVariable>
                                                            <NodeId>SimpleArrayAssignment4</NodeId>
                                                         </NodeOutcomeVariable>
                                                         <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                                                      </EQInternal>
                                                   </OR>
                                                </NOT>
                                             </AND>
                                          </InvariantCondition>
                                          <NodeBody>
                                             <NodeList>
                                                <Node NodeType="Assignment" FileName="ArrayComplex.ple" LineNo="22" ColNo="3">
                                                   <NodeId>SimpleArrayAssignment</NodeId>
                                                   <NodeBody>
                                                      <Assignment>
                                                         <ArrayElement>
                                                            <Name>foo</Name>
                                                            <Index>
                                                               <IntegerValue>1</IntegerValue>
                                                            </Index>
                                                         </ArrayElement>
                                                         <NumericRHS>
                                                            <ADD>
                                                               <ArrayElement>
                                                                  <Name>foo</Name>
                                                                  <Index>
                                                                     <IntegerValue>1</IntegerValue>
                                                                  </Index>
                                                               </ArrayElement>
                                                               <RealValue>1.0</RealValue>
                                                            </ADD>
                                                         </NumericRHS>
                                                      </Assignment>
                                                   </NodeBody>
                                                </Node>
                                                <Node NodeType="Assignment" FileName="ArrayComplex.ple" LineNo="26" ColNo="3">
                                                   <NodeId>SimpleArrayAssignment2</NodeId>
                                                   <StartCondition>
                                                      <AND>
                                                         <EQInternal>
                                                            <NodeStateVariable>
                                                               <NodeId>SimpleArrayAssignment</NodeId>
                                                            </NodeStateVariable>
                                                            <NodeStateValue>FINISHED</NodeStateValue>
                                                         </EQInternal>
                                                      </AND>
                                                   </StartCondition>
                                                   <NodeBody>
                                                      <Assignment>
                                                         <ArrayElement>
                                                            <Name>bar</Name>
                                                            <Index>
                                                               <IntegerValue>2</IntegerValue>
                                                            </Index>
                                                         </ArrayElement>
                                                         <NumericRHS>
                                                            <ADD>
                                                               <ArrayElement>
                                                                  <Name>bar</Name>
                                                                  <Index>
                                                                     <IntegerValue>2</IntegerValue>
                                                                  </Index>
                                                               </ArrayElement>
                                                               <RealValue>2.0</RealValue>
                                                            </ADD>
                                                         </NumericRHS>
                                                      </Assignment>
                                                   </NodeBody>
                                                </Node>
                                                <Node NodeType="Assignment" FileName="ArrayComplex.ple" LineNo="30" ColNo="3">
                                                   <NodeId>SimpleArrayAssignment3</NodeId>
                                                   <StartCondition>
                                                      <AND>
                                                         <EQInternal>
                                                            <NodeStateVariable>
                                                               <NodeId>SimpleArrayAssignment2</NodeId>
                                                            </NodeStateVariable>
                                                            <NodeStateValue>FINISHED</NodeStateValue>
                                                         </EQInternal>
                                                      </AND>
                                                   </StartCondition>
                                                   <NodeBody>
                                                      <Assignment>
                                                         <ArrayElement>
                                                            <Name>foo</Name>
                                                            <Index>
                                                               <IntegerValue>3</IntegerValue>
                                                            </Index>
                                                         </ArrayElement>
                                                         <NumericRHS>
                                                            <ADD>
                                                               <ArrayElement>
                                                                  <Name>foo</Name>
                                                                  <Index>
                                                                     <IntegerValue>3</IntegerValue>
                                                                  </Index>
                                                               </ArrayElement>
                                                               <RealValue>3.0</RealValue>
                                                            </ADD>
                                                         </NumericRHS>
                                                      </Assignment>
                                                   </NodeBody>
                                                </Node>
                                                <Node NodeType="Assignment" FileName="ArrayComplex.ple" LineNo="34" ColNo="3">
                                                   <NodeId>SimpleArrayAssignment4</NodeId>
                                                   <StartCondition>
                                                      <AND>
                                                         <EQInternal>
                                                            <NodeStateVariable>
                                                               <NodeId>SimpleArrayAssignment3</NodeId>
                                                            </NodeStateVariable>
                                                            <NodeStateValue>FINISHED</NodeStateValue>
                                                         </EQInternal>
                                                      </AND>
                                                   </StartCondition>
                                                   <NodeBody>
                                                      <Assignment>
                                                         <ArrayElement>
                                                            <Name>bar</Name>
                                                            <Index>
                                                               <IntegerValue>15</IntegerValue>
                                                            </Index>
                                                         </ArrayElement>
                                                         <NumericRHS>
                                                            <ADD>
                                                               <ArrayElement>
                                                                  <Name>bar</Name>
                                                                  <Index>
                                                                     <IntegerValue>15</IntegerValue>
                                                                  </Index>
                                                               </ArrayElement>
                                                               <RealValue>4.0</RealValue>
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
                                       <RealVariable>i</RealVariable>
                                       <NumericRHS>
                                          <ADD>
                                             <RealVariable>i</RealVariable>
                                             <IntegerValue>1</IntegerValue>
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
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>