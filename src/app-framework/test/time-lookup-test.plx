<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations LineNo="1" ColNo="5">
      <StateDeclaration LineNo="1" ColNo="5">
         <Name>time</Name>
         <Return>
            <Name>_return_0</Name>
            <Type>Real</Type>
         </Return>
      </StateDeclaration>
   </GlobalDeclarations>
   <Node NodeType="Empty" LineNo="5" ColNo="2">
      <NodeId>time_lookup_test</NodeId>
      <EndCondition>
         <GE>
            <LookupOnChange>
               <Name>
                  <StringValue>time</StringValue>
               </Name>
               <Tolerance>
                  <RealValue>1.0</RealValue>
               </Tolerance>
            </LookupOnChange>
            <ADD LineNo="5" ColNo="70">
               <NodeTimepointValue>
                  <NodeId>time_lookup_test</NodeId>
                  <NodeStateValue>EXECUTING</NodeStateValue>
                  <Timepoint>START</Timepoint>
               </NodeTimepointValue>
               <IntegerValue>5</IntegerValue>
            </ADD>
         </GE>
      </EndCondition>
   </Node>
</PlexilPlan>