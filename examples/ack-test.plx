<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations/>
   <Node NodeType="NodeList" epx="Sequence" FileName="ack-test.ple" LineNo="4" ColNo="1">
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
                        <NodeId>Ack1</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>Print1</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
               </OR>
            </NOT>
         </AND>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="LibraryNodeCall" FileName="ack-test.ple" LineNo="9" ColNo="11">
               <NodeId>Ack1</NodeId>
               <NodeBody>
                  <LibraryNodeCall>
                      <NodeId>Ack</NodeId>
                      <Alias>
                          <NodeParameter>m</NodeParameter>
                          <IntegerValue>0</IntegerValue>
                      </Alias>
                      <Alias>
                          <NodeParameter>n</NodeParameter>
                          <IntegerValue>0</IntegerValue>
                      </Alias>
                      <Alias>
                          <NodeParameter>result</NodeParameter>
                          <IntegerVariable>result</IntegerVariable>
                      </Alias>
                  </LibraryNodeCall>
              </NodeBody>
            </Node>
            <Node NodeType="Command" FileName="ack-test.ple" LineNo="10" ColNo="13">
               <NodeId>Print1</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>Ack1</NodeId>
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
                          <StringValue>Ack(0,0) =</StringValue>
                          <IntegerVariable>result</IntegerVariable>
                      </Arguments>
                  </Command>
              </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>