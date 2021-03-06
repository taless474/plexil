  <PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd" FileName="request.ple">
      <Node FileName="request.ple" LineNo="2" ColNo="1" NodeType="Request">
          <NodeId>RequestTest</NodeId>
          <VariableDeclarations>
              <DeclareVariable>
                  <Name>i</Name>
                  <Type>Integer</Type>
                  <InitialValue>
                      <IntegerValue>42</IntegerValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>x</Name>
                  <Type>Real</Type>
                  <InitialValue>
                      <RealValue>3.1415</RealValue>
                  </InitialValue>
              </DeclareVariable>
          </VariableDeclarations>
          <NodeBody>
              <Request>
                  <NodeRef>ParentNode</NodeRef>
                  <Pair>
                      <Name>A</Name>
                      <IntegerValue>1</IntegerValue>
                  </Pair>
                  <Pair>
                      <Name>B</Name>
                      <RealValue>2.718</RealValue>
                  </Pair>
                  <Pair>
                      <Name>C</Name>
                      <IntegerVariable>i</IntegerVariable>
                  </Pair>
                  <Pair>
                      <Name>D</Name>
                      <RealVariable>x</RealVariable>
                  </Pair>
              </Request>
          </NodeBody>
      </Node>
  </PlexilPlan>
