  <PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/supported-plexil.xsd" FileName="resource3.ple">
      <Node FileName="resource3.ple" LineNo="1" ColNo="13" NodeType="NodeList">
          <NodeId>SimpleTask</NodeId>
          <NodeBody>
              <NodeList>
                  <Node FileName="resource3.ple" LineNo="3" ColNo="7" NodeType="Command">
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>returnValue</Name>
                              <Type>Integer</Type>
                              <InitialValue>
                                  <IntegerValue>-1</IntegerValue>
                              </InitialValue>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeId>C1</NodeId>
                      <EndCondition>
                          <EQNumeric>
                              <IntegerVariable>returnValue</IntegerVariable>
                              <IntegerValue>10</IntegerValue>
                          </EQNumeric>
                      </EndCondition>
                      <PostCondition>
                          <EQInternal>
                              <NodeCommandHandleVariable>
                                  <NodeId>C1</NodeId>
                              </NodeCommandHandleVariable>
                              <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                          </EQInternal>
                      </PostCondition>
                      <NodeBody>
                          <Command>
                              <ResourceList>
                                  <Resource>
                                      <ResourceName>
                                          <StringValue>head</StringValue>
                                      </ResourceName>
                                      <ResourcePriority>
                                          <IntegerValue>20</IntegerValue>
                                      </ResourcePriority>
                                  </Resource>
                              </ResourceList>
                              <IntegerVariable>returnValue</IntegerVariable>
                              <Name>
                                  <StringValue>c1</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="resource3.ple" LineNo="11" ColNo="7" NodeType="Command">
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>returnValue</Name>
                              <Type>Integer</Type>
                              <InitialValue>
                                  <IntegerValue>-1</IntegerValue>
                              </InitialValue>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeId>C2</NodeId>
                      <RepeatCondition>
                          <EQInternal>
                              <NodeCommandHandleVariable>
                                  <NodeId>C2</NodeId>
                              </NodeCommandHandleVariable>
                              <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
                          </EQInternal>
                      </RepeatCondition>
                      <PostCondition>
                          <EQInternal>
                              <NodeCommandHandleVariable>
                                  <NodeId>C2</NodeId>
                              </NodeCommandHandleVariable>
                              <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                          </EQInternal>
                      </PostCondition>
                      <EndCondition>
                          <EQNumeric>
                              <IntegerVariable>returnValue</IntegerVariable>
                              <IntegerValue>10</IntegerValue>
                          </EQNumeric>
                      </EndCondition>
                      <NodeBody>
                          <Command>
                              <ResourceList>
                                  <Resource>
                                      <ResourceName>
                                          <StringValue>memory</StringValue>
                                      </ResourceName>
                                      <ResourceLowerBound>
                                          <RealValue>0.3</RealValue>
                                      </ResourceLowerBound>
                                      <ResourceUpperBound>
                                          <RealValue>0.3</RealValue>
                                      </ResourceUpperBound>
                                      <ResourcePriority>
                                          <IntegerValue>25</IntegerValue>
                                      </ResourcePriority>
                                  </Resource>
                              </ResourceList>
                              <IntegerVariable>returnValue</IntegerVariable>
                              <Name>
                                  <StringValue>c2</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
                  <Node FileName="resource3.ple" LineNo="20" ColNo="7" NodeType="Command">
                      <VariableDeclarations>
                          <DeclareVariable>
                              <Name>returnValue</Name>
                              <Type>Integer</Type>
                              <InitialValue>
                                  <IntegerValue>-1</IntegerValue>
                              </InitialValue>
                          </DeclareVariable>
                      </VariableDeclarations>
                      <NodeId>C3</NodeId>
                      <PostCondition>
                          <EQInternal>
                              <NodeCommandHandleVariable>
                                  <NodeId>C3</NodeId>
                              </NodeCommandHandleVariable>
                              <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                          </EQInternal>
                      </PostCondition>
                      <EndCondition>
                          <EQNumeric>
                              <IntegerVariable>returnValue</IntegerVariable>
                              <IntegerValue>10</IntegerValue>
                          </EQNumeric>
                      </EndCondition>
                      <NodeBody>
                          <Command>
                              <ResourceList>
                                  <Resource>
                                      <ResourceName>
                                          <StringValue>vision</StringValue>
                                      </ResourceName>
                                      <ResourcePriority>
                                          <IntegerValue>30</IntegerValue>
                                      </ResourcePriority>
                                  </Resource>
                              </ResourceList>
                              <IntegerVariable>returnValue</IntegerVariable>
                              <Name>
                                  <StringValue>c3</StringValue>
                              </Name>
                          </Command>
                      </NodeBody>
                  </Node>
              </NodeList>
          </NodeBody>
      </Node>
  </PlexilPlan>