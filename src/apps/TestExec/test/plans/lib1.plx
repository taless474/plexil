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
               <DeclareVariable LineNo="2" ColNo="24">
                  <Name>li</Name>
                  <Type>Integer</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="3" ColNo="24">
                  <Name>lr</Name>
                  <Type>Real</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="4" ColNo="24">
                  <Name>ls</Name>
                  <Type>String</Type>
               </DeclareVariable>
            </In>
         </Interface>
      </LibraryNodeDeclaration>
      <LibraryNodeDeclaration LineNo="5" ColNo="0">
         <Name>LibTest2</Name>
         <Interface LineNo="5" ColNo="24">
            <In>
               <DeclareVariable LineNo="5" ColNo="24">
                  <Name>y</Name>
                  <Type>Real</Type>
               </DeclareVariable>
               <DeclareVariable LineNo="5" ColNo="35">
                  <Name>z</Name>
                  <Type>Real</Type>
               </DeclareVariable>
            </In>
         </Interface>
      </LibraryNodeDeclaration>
      <CommandDeclaration LineNo="6" ColNo="0">
         <Name>foo</Name>
         <Parameter>
            <Type>Boolean</Type>
         </Parameter>
         <Parameter>
            <Type>Integer</Type>
         </Parameter>
         <Parameter>
            <Type>Real</Type>
         </Parameter>
         <Parameter>
            <Type>String</Type>
         </Parameter>
      </CommandDeclaration>
   </GlobalDeclarations>
   <Node NodeType="Command" LineNo="14" ColNo="3">
      <NodeId>LibTest1</NodeId>
      <Interface>
         <In>
            <DeclareVariable LineNo="10" ColNo="6">
               <Name>lb</Name>
               <Type>Boolean</Type>
            </DeclareVariable>
            <DeclareVariable LineNo="11" ColNo="6">
               <Name>li</Name>
               <Type>Integer</Type>
            </DeclareVariable>
            <DeclareVariable LineNo="12" ColNo="6">
               <Name>lr</Name>
               <Type>Real</Type>
            </DeclareVariable>
            <DeclareVariable LineNo="13" ColNo="6">
               <Name>ls</Name>
               <Type>String</Type>
            </DeclareVariable>
         </In>
      </Interface>
      <NodeBody>
         <Command>
            <Name>
               <StringValue>foo</StringValue>
            </Name>
            <Arguments LineNo="14" ColNo="8">
               <BooleanVariable>lb</BooleanVariable>
               <IntegerVariable>li</IntegerVariable>
               <RealVariable>lr</RealVariable>
               <StringVariable>ls</StringVariable>
            </Arguments>
         </Command>
      </NodeBody>
   </Node>
</PlexilPlan>