<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <GlobalDeclarations>
      <LibraryNodeDeclaration>
         <Name>library-node-with-array</Name>
         <Interface>
		 </Interface>
      </LibraryNodeDeclaration>
  </GlobalDeclarations>
  <Node NodeType="NodeList">
	<NodeId>library-call-with-array</NodeId>
	<VariableDeclarations>
	  <DeclareArray>
		<Name>myStates</Name>
		<Type>Real</Type>
		<MaxSize>4</MaxSize>
		<InitialValue>
		  <RealValue>70000.0</RealValue>
		  <RealValue>2000.0</RealValue>
		  <RealValue>250.0</RealValue>
		  <RealValue>0.0</RealValue>
		</InitialValue>
	  </DeclareArray>
	</VariableDeclarations>
	<EndCondition>
	  <EQNumeric>
		<ArrayElement>
		  <Name>myStates</Name>
		  <Index>
			<IntegerValue>3</IntegerValue>
		  </Index>
		</ArrayElement>
		<RealValue>1776.0</RealValue>
	  </EQNumeric>
	</EndCondition>
	<NodeBody>
	  <NodeList>
		<Node NodeType="LibraryNodeCall">
		  <NodeId>make-the-call</NodeId>
		  <NodeBody>
			<LibraryNodeCall>
			  <NodeId>library-node-with-array</NodeId>
			  <Alias>
				<NodeParameter>states</NodeParameter>
				<ArrayVariable>myStates</ArrayVariable>
			  </Alias>
			</LibraryNodeCall>
		  </NodeBody>
		</Node>
	  </NodeList>
	</NodeBody>
  </Node>
</PlexilPlan>
