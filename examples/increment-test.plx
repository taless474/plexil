<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations/>
   <Node NodeType="NodeList" epx="Sequence" FileName="increment-test.ple" LineNo="4"
         ColNo="1">
      <NodeId>LibraryCallTest</NodeId>
      <VariableDeclarations>
              <DeclareVariable>
                  <Name>result</Name>
                  <Type>Integer</Type>
              </DeclareVariable>
          </VariableDeclarations>
      <InvariantCondition>
         <AND>
            <NOT>
               <OR>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>LibraryCallTest__CHILD__1</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>LibraryCallTest__CHILD__2</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>LibraryCallTest__CHILD__3</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>LibraryCallTest__CHILD__4</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
               </OR>
            </NOT>
         </AND>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="LibraryNodeCall" FileName="increment-test.ple" LineNo="7" ColNo="5">
               <NodeId>LibraryCallTest__CHILD__1</NodeId>
               <NodeBody>
                  <LibraryNodeCall>
                      <NodeId>Increment</NodeId>
                      <Alias>
                          <NodeParameter>x</NodeParameter>
                          <IntegerValue>1</IntegerValue>
                      </Alias>
                      <Alias>
                          <NodeParameter>result</NodeParameter>
                          <IntegerVariable>result</IntegerVariable>
                      </Alias>
                  </LibraryNodeCall>
              </NodeBody>
            </Node>
            <Node NodeType="Command" FileName="increment-test.ple" LineNo="8" ColNo="5">
               <NodeId>LibraryCallTest__CHILD__2</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>LibraryCallTest__CHILD__1</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <NodeBody>
                  <Command>
                      <Name>
                          <StringValue>pprint</StringValue>
                      </Name>
                      <Arguments>
                          <StringValue>Increment(1) =</StringValue>
                          <IntegerVariable>result</IntegerVariable>
                      </Arguments>
                  </Command>
              </NodeBody>
            </Node>
            <Node NodeType="LibraryNodeCall" FileName="increment-test.ple" LineNo="9" ColNo="5">
               <NodeId>LibraryCallTest__CHILD__3</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>LibraryCallTest__CHILD__2</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <NodeBody>
                  <LibraryNodeCall>
                      <NodeId>Increment</NodeId>
                      <Alias>
                          <NodeParameter>x</NodeParameter>
                          <IntegerValue>2</IntegerValue>
                      </Alias>
                      <Alias>
                          <NodeParameter>result</NodeParameter>
                          <IntegerVariable>result</IntegerVariable>
                      </Alias>
                  </LibraryNodeCall>
              </NodeBody>
            </Node>
            <Node NodeType="Command" FileName="increment-test.ple" LineNo="10" ColNo="5">
               <NodeId>LibraryCallTest__CHILD__4</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>LibraryCallTest__CHILD__3</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <NodeBody>
                  <Command>
                      <Name>
                          <StringValue>pprint</StringValue>
                      </Name>
                      <Arguments>
                          <StringValue>Increment(2) =</StringValue>
                          <IntegerVariable>result</IntegerVariable>
                      </Arguments>
                  </Command>
              </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>