  <PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/supported-plexil.xsd" FileName="real-exp-test.ple">
      <Node FileName="real-exp-test.ple" LineNo="1" ColNo="1" NodeType="NodeList">
          <VariableDeclarations>
              <DeclareVariable>
                  <Name>foo</Name>
                  <Type>Real</Type>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>bar</Name>
                  <Type>Real</Type>
                  <InitialValue>
                      <RealValue>3.14</RealValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>baz</Name>
                  <Type>Real</Type>
                  <InitialValue>
                      <RealValue>2</RealValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareArray>
                  <Name>bletch</Name>
                  <Type>Real</Type>
                  <MaxSize>1</MaxSize>
              </DeclareArray>
              <DeclareArray>
                  <Name>blorf</Name>
                  <Type>Real</Type>
                  <MaxSize>3</MaxSize>
                  <InitialValue>
                      <RealValue>2.71828</RealValue>
                  </InitialValue>
              </DeclareArray>
              <DeclareArray>
                  <Name>blurp</Name>
                  <Type>Real</Type>
                  <MaxSize>5</MaxSize>
                  <InitialValue>
                      <RealValue>5.1</RealValue>
                      <RealValue>4.2</RealValue>
                      <RealValue>3.3</RealValue>
                      <RealValue>2</RealValue>
                      <RealValue>1</RealValue>
                  </InitialValue>
              </DeclareArray>
          </VariableDeclarations>
          <NodeId>_GLOBAL_CONTEXT___CHILD__1</NodeId>
          <NodeBody>
              <NodeList>
                  <Node FileName="real-exp-test.ple" LineNo="4" ColNo="3" NodeType="Assignment">
                      <NodeId>_GLOBAL_CONTEXT___CHILD__1__CHILD__1</NodeId>
                      <NodeBody>
                          <Assignment>
                              <RealVariable>foo</RealVariable>
                              <NumericRHS>
                                  <RealValue>2.0</RealValue>
                              </NumericRHS>
                          </Assignment>
                      </NodeBody>
                  </Node>
                  <Node FileName="real-exp-test.ple" LineNo="5" ColNo="3" NodeType="Assignment">
                      <NodeId>_GLOBAL_CONTEXT___CHILD__1__CHILD__2</NodeId>
                      <NodeBody>
                          <Assignment>
                              <RealVariable>bar</RealVariable>
                              <NumericRHS>
                                  <ArrayElement>
                                      <Name>blorf</Name>
                                      <Index>
                                          <RealVariable>foo</RealVariable>
                                      </Index>
                                  </ArrayElement>
                              </NumericRHS>
                          </Assignment>
                      </NodeBody>
                  </Node>
                  <Node FileName="real-exp-test.ple" LineNo="6" ColNo="3" NodeType="Assignment">
                      <NodeId>_GLOBAL_CONTEXT___CHILD__1__CHILD__3</NodeId>
                      <NodeBody>
                          <Assignment>
                              <RealVariable>foo</RealVariable>
                              <NumericRHS>
                                  <ADD>
                                      <MUL>
                                          <RealValue>4.1</RealValue>
                                          <RealVariable>bar</RealVariable>
                                      </MUL>
                                      <IntegerValue>3</IntegerValue>
                                  </ADD>
                              </NumericRHS>
                          </Assignment>
                      </NodeBody>
                  </Node>
              </NodeList>
          </NodeBody>
      </Node>
  </PlexilPlan>
