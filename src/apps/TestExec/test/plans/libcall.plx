<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations LineNo="1" ColNo="0">
      <LibraryNodeDeclaration LineNo="1" ColNo="0">
         <Name>LibTest1</Name>
         <Interface LineNo="1" ColNo="24">
            <In>
               <DeclareVariable LineNo="1" ColNo="24">
                  <Name>lb</Name>
                  <Type>Boolean</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="1" ColNo="39">
                  <Name>li</Name>
                  <Type>Integer</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="2" ColNo="24">
                  <Name>lr</Name>
                  <Type>Real</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="2" ColNo="36">
                  <Name>ls</Name>
                  <Type>String</Type>
               </DeclareVariable>
            </In>
         </Interface>
      </LibraryNodeDeclaration>
      <LibraryNodeDeclaration LineNo="3" ColNo="0">
         <Name>LibTest2</Name>
         <Interface LineNo="3" ColNo="24">
            <In>
               <DeclareVariable LineNo="3" ColNo="24">
                  <Name>y</Name>
                  <Type>Real</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="3" ColNo="35">
                  <Name>z</Name>
                  <Type>Real</Type>
               </DeclareVariable>
            </In>
         </Interface>
      </LibraryNodeDeclaration>
   </GlobalDeclarations>
   <Node NodeType="NodeList" epx="Sequence" LineNo="7" ColNo="2">
      <NodeId>List</NodeId>
      <VariableDeclarations>
         <DeclareVariable LineNo="7" ColNo="2">
            <Name>b</Name>
            <Type>Boolean</Type>
            <InitialValue>
               <BooleanValue>true</BooleanValue>
            </InitialValue>
         </DeclareVariable>
         <DeclareVariable LineNo="8" ColNo="2">
            <Name>i</Name>
            <Type>Integer</Type>
            <InitialValue>
               <IntegerValue>1966</IntegerValue>
            </InitialValue>
         </DeclareVariable>
         <DeclareVariable LineNo="9" ColNo="2">
            <Name>r</Name>
            <Type>Real</Type>
            <InitialValue>
               <RealValue>3.1415</RealValue>
            </InitialValue>
         </DeclareVariable>
         <DeclareVariable LineNo="10" ColNo="2">
            <Name>s</Name>
            <Type>String</Type>
            <InitialValue>
               <StringValue>hello</StringValue>
            </InitialValue>
         </DeclareVariable>
      </VariableDeclarations>
      <InvariantCondition>
         <AND>
            <NOT>
               <OR>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>CallLibTest1</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>CallLibTest2</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>FAILURE</NodeOutcomeValue>
                  </EQInternal>
               </OR>
            </NOT>
         </AND>
      </InvariantCondition>
      <NodeBody>
         <NodeList>
            <Node NodeType="LibraryNodeCall">
               <NodeId>CallLibTest1</NodeId>
               <StartCondition>
                  <EQInternal>
                     <NodeOutcomeVariable>
                        <NodeId>CallLibTest2</NodeId>
                     </NodeOutcomeVariable>
                     <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
                  </EQInternal>
               </StartCondition>
               <NodeBody>
                  <LibraryNodeCall>
                     <NodeId>LibTest1</NodeId>
                     <Alias>
                        <NodeParameter>lb</NodeParameter>
                        <BooleanVariable>b</BooleanVariable>
                     </Alias>
                     <Alias>
                        <NodeParameter>li</NodeParameter>
                        <IntegerVariable>i</IntegerVariable>
                     </Alias>
                     <Alias>
                        <NodeParameter>lr</NodeParameter>
                        <RealVariable>r</RealVariable>
                     </Alias>
                     <Alias>
                        <NodeParameter>ls</NodeParameter>
                        <StringVariable>s</StringVariable>
                     </Alias>
                  </LibraryNodeCall>
               </NodeBody>
            </Node>
            <Node NodeType="LibraryNodeCall">
               <NodeId>CallLibTest2</NodeId>
               <StartCondition>
                  <AND>
                     <EQInternal>
                        <NodeStateVariable>
                           <NodeId>CallLibTest1</NodeId>
                        </NodeStateVariable>
                        <NodeStateValue>FINISHED</NodeStateValue>
                     </EQInternal>
                  </AND>
               </StartCondition>
               <NodeBody>
                  <LibraryNodeCall>
                     <NodeId>LibTest2</NodeId>
                     <Alias>
                        <NodeParameter>y</NodeParameter>
                        <RealVariable>r</RealVariable>
                     </Alias>
                     <Alias>
                        <NodeParameter>z</NodeParameter>
                        <RealVariable>r</RealVariable>
                     </Alias>
                  </LibraryNodeCall>
               </NodeBody>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>