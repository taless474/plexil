  <PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/supported-plexil.xsd" FileName="test/for-test.ple">
      <GlobalDeclarations>
          <CommandDeclaration>
              <Name>foo</Name>
          </CommandDeclaration>
      </GlobalDeclarations>
      <NODE FileName="test/for-test.ple" LineNo="4" ColNo="1" NodeType="NodeList">
          <NodeId>ForTestContainer</NodeId>
          <NodeBody>
              <NodeList>
                  <For FileName="test/for-test.ple" LineNo="7" ColNo="3">
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>j</Name>
                              <Type>Integer</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeId>ForTest</NodeId>
                      <LoopVariable>
                          <DeclareVariable>
                              <Name>i</Name>
                              <Type>Integer</Type>
                              <InitialValue>
                                  <IntegerValue>0</IntegerValue>
                              </InitialValue>
                          </DeclareVariable>
                      </LoopVariable>
                      <Condition>
                          <LT>
                              <IntegerVariable>i</IntegerVariable>
                              <IntegerValue>1</IntegerValue>
                          </LT>
                      </Condition>
                      <LoopVariableUpdate>
                          <ADD>
                              <IntegerVariable>i</IntegerVariable>
                              <IntegerValue>1</IntegerValue>
                          </ADD>
                      </LoopVariableUpdate>
                      <Action>
                          <NODE FileName="test/for-test.ple" LineNo="10" ColNo="7" NodeType="Command">
                              <NodeId>ForTest__CHILD__2</NodeId>
                              <NodeBody>
                                  <Command>
                                      <Name>
                                          <StringValue>foo</StringValue>
                                      </Name>
                                  </Command>
                              </NodeBody>
                          </NODE>
                      </Action>
                  </For>
                  <NODE FileName="test/for-test.ple" LineNo="16" ColNo="3" NodeType="NodeList">
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>y</Name>
                              <Type>Real</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeId>ConfusingForTest</NodeId>
                      <NodeBody>
                          <NodeList>
                              <For FileName="test/for-test.ple" LineNo="19" ColNo="5">
                                  <NodeId>ConfusingForTest__CHILD__1</NodeId>
                                  <LoopVariable>
                                      <DeclareVariable>
                                          <Name>x</Name>
                                          <Type>Real</Type>
                                          <InitialValue>
                                              <RealValue>0</RealValue>
                                          </InitialValue>
                                      </DeclareVariable>
                                  </LoopVariable>
                                  <Condition>
                                      <LT>
                                          <RealVariable>x</RealVariable>
                                          <IntegerValue>1</IntegerValue>
                                      </LT>
                                  </Condition>
                                  <LoopVariableUpdate>
                                      <ADD>
                                          <RealVariable>x</RealVariable>
                                          <RealValue>0.5</RealValue>
                                      </ADD>
                                  </LoopVariableUpdate>
                                  <Action>
                                      <NODE FileName="test/for-test.ple" LineNo="21" ColNo="9" NodeType="Command">
                                          <NodeId>ConfusingForTest__CHILD__1__CHILD__2</NodeId>
                                          <NodeBody>
                                              <Command>
                                                  <Name>
                                                      <StringValue>foo</StringValue>
                                                  </Name>
                                              </Command>
                                          </NodeBody>
                                      </NODE>
                                  </Action>
                              </For>
                          </NodeList>
                      </NodeBody>
                  </NODE>
              </NodeList>
          </NodeBody>
      </NODE>
  </PlexilPlan>
