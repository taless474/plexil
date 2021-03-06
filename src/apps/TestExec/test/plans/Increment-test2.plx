<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xsi:noNamespaceSchemaLocation="http://plexil.svn.sourceforge.net/viewvc/plexil/trunk/schema/core-plexil.xsd">
  <Node NodeType="LibraryNodeCall">
	<NodeId>Increment-test2</NodeId>
	<VariableDeclarations>
	  <DeclareVariable>
		<Name>result</Name>
		<Type>Integer</Type>
		<InitialValue>
		  <IntegerValue>0</IntegerValue>
		</InitialValue>
	  </DeclareVariable>
	</VariableDeclarations>
	<EndCondition>
	  <EQNumeric>
		<IntegerVariable>result</IntegerVariable>
		<IntegerValue>3</IntegerValue>
	  </EQNumeric>
	</EndCondition>
	<NodeBody>
	  <LibraryNodeCall>
		<NodeId>Increment</NodeId>
		<Alias>
		  <NodeParameter>x</NodeParameter>
		  <ADD>
			<IntegerValue>1</IntegerValue>
			<IntegerValue>1</IntegerValue>
		  </ADD>
		</Alias>
		<Alias>
		  <NodeParameter>result</NodeParameter>
		  <IntegerVariable>result</IntegerVariable>
		</Alias>
	  </LibraryNodeCall>
	</NodeBody>
  </Node>
</PlexilPlan>