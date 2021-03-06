  <PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd" FileName="command-test.ple">
      <GlobalDeclarations>
          <CommandDeclaration>
              <Name>bletch</Name>
              <Return Type="Real">_Command_return_1</Return>
              <Parameter Type="String">s</Parameter>
          </CommandDeclaration>
          <CommandDeclaration>
              <Name>baz</Name>
              <Return Type="Boolean">_Command_return_1</Return>
          </CommandDeclaration>
          <CommandDeclaration>
              <Name>foo</Name>
          </CommandDeclaration>
          <CommandDeclaration>
              <Name>bar</Name>
              <Parameter Type="Integer">i</Parameter>
          </CommandDeclaration>
      </GlobalDeclarations>
      <Node FileName="command-test.ple" LineNo="7" ColNo="1" NodeType="NodeList">
          <NodeId>CommandTest</NodeId>
          <VariableDeclarations>
              <DeclareVariable>
                  <Name>fooName</Name>
                  <Type>String</Type>
                  <InitialValue>
                      <StringValue>foo</StringValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>bazName</Name>
                  <Type>String</Type>
                  <InitialValue>
                      <StringValue>baz</StringValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>bletchName</Name>
                  <Type>String</Type>
                  <InitialValue>
                      <StringValue>bletch</StringValue>
                  </InitialValue>
              </DeclareVariable>
          </VariableDeclarations>
          <NodeBody>
              <NodeList>
                  <Node FileName="command-test.ple" LineNo="15" ColNo="3" NodeType="Command">
                      <NodeId>NoArgsNoReturnTest</NodeId>
                      <NodeBody>
                          <Command>
                              <Name>
                                  <StringValue>foo</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="20" ColNo="3" NodeType="Command">
                      <NodeId>ArgsElidedNoReturnTest</NodeId>
                      <NodeBody>
                          <Command>
                              <Name>
                                  <StringValue>foo</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="25" ColNo="3" NodeType="Command">
                      <NodeId>OneArgNoReturnTest</NodeId>
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>n</Name>
                              <Type>Integer</Type>
                              <InitialValue>
                                  <IntegerValue>2</IntegerValue>
                              </InitialValue>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeBody>
                          <Command>
                              <Name>
                                  <StringValue>bar</StringValue>
                              </Name>
                              <Arguments>
                                  <IntegerVariable>n</IntegerVariable>
                              </Arguments>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="31" ColNo="3" NodeType="Command">
                      <NodeId>NoArgOneReturnTest</NodeId>
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>b</Name>
                              <Type>Boolean</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeBody>
                          <Command>
                              <BooleanVariable>b</BooleanVariable>
                              <Name>
                                  <StringValue>baz</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="37" ColNo="3" NodeType="Command">
                      <NodeId>ArgsElidedOneReturnTest</NodeId>
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>b</Name>
                              <Type>Boolean</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeBody>
                          <Command>
                              <BooleanVariable>b</BooleanVariable>
                              <Name>
                                  <StringValue>baz</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="43" ColNo="3" NodeType="Command">
                      <NodeId>OneArgOneReturnTest</NodeId>
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>r</Name>
                              <Type>Real</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeBody>
                          <Command>
                              <RealVariable>r</RealVariable>
                              <Name>
                                  <StringValue>bletch</StringValue>
                              </Name>
                              <Arguments>
                                  <StringValue>Foo!</StringValue>
                              </Arguments>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="49" ColNo="3" NodeType="Command">
                      <NodeId>VarRefAsCmdNameTest</NodeId>
                      <NodeBody>
                          <Command>
                              <Name>
                                  <StringVariable>fooName</StringVariable>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="54" ColNo="3" NodeType="Command">
                      <NodeId>VarRefAsCmdNameArgsElidedTest</NodeId>
                      <NodeBody>
                          <Command>
                              <Name>
                                  <StringVariable>fooName</StringVariable>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="59" ColNo="3" NodeType="Command">
                      <NodeId>VarRefAsCmdNameWithRtnValTest</NodeId>
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>r</Name>
                              <Type>Real</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeBody>
                          <Command>
                              <RealVariable>r</RealVariable>
                              <Name>
                                  <StringVariable>bletchName</StringVariable>
                              </Name>
                              <Arguments>
                                  <StringValue>Bletch!</StringValue>
                              </Arguments>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="command-test.ple" LineNo="65" ColNo="3" NodeType="Command">
                      <NodeId>VarRefAsCmdNameWithRtnValArgsElidedTest</NodeId>
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>b</Name>
                              <Type>Boolean</Type>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeBody>
                          <Command>
                              <BooleanVariable>b</BooleanVariable>
                              <Name>
                                  <StringVariable>bazName</StringVariable>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
              </NodeList>
          </NodeBody>
      </Node>
  </PlexilPlan>
