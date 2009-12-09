<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="plexilJul31.xsd">
  <Node NodeType="NodeList">
    <NodeId>TestArrays</NodeId>
    <VariableDeclarations>

      <!-- an integer (good old X) -->

      <DeclareVariable>
        <Name>x</Name>
        <Type>Integer</Type>
        <InitialValue>
          <IntegerValue>4</IntegerValue>
        </InitialValue>
      </DeclareVariable>

      <!-- an array of strings -->

      <DeclareArray>
        <Name>myStringArray</Name>
        <Type>String</Type>
        <MaxSize>3</MaxSize>
        <InitialValue>
          <StringValue>string 1</StringValue>
          <StringValue>string 2</StringValue>
        </InitialValue>
      </DeclareArray>

      <!-- an array of reals -->

      <DeclareArray>
        <Name>myRealArray</Name>
        <Type>Real</Type>
        <MaxSize>3</MaxSize>
        <InitialValue>
          <RealValue>1.1</RealValue>
          <RealValue>2.2</RealValue>
        </InitialValue>
      </DeclareArray>

      <!-- an array of integers -->

      <DeclareArray>
        <Name>myIntegerArray</Name>
        <Type>Integer</Type>
        <MaxSize>3</MaxSize>
        <InitialValue>
          <IntegerValue>1</IntegerValue>
          <IntegerValue>2</IntegerValue>
        </InitialValue>
      </DeclareArray>

      <!-- an array of booleans -->

      <DeclareArray>
        <Name>myBooleanArray</Name>
        <Type>Boolean</Type>
        <MaxSize>3</MaxSize>
        <InitialValue>
          <BooleanValue>1</BooleanValue>
          <BooleanValue>0</BooleanValue>
        </InitialValue>
      </DeclareArray>
    </VariableDeclarations>
      
    <!-- a start condition exersizing arrays -->

    <StartCondition>
      <AND>

        <!-- known string -->

        <EQString>
          <ArrayElement>
            <Name>myStringArray</Name>
            <Index>
              <IntegerValue>0</IntegerValue>
            </Index>
          </ArrayElement>
          <StringValue>string 1</StringValue>
        </EQString>

        <!-- unknown string -->

        <NOT>
          <IsKnown>
            <ArrayElement>
            <Name>myStringArray</Name>
            <Index>
              <IntegerValue>2</IntegerValue>
            </Index>
            </ArrayElement>
          </IsKnown>
        </NOT>

        <!-- known real -->

        <EQNumeric>
          <ArrayElement>
            <Name>myRealArray</Name>
            <Index>
              <IntegerValue>0</IntegerValue>
            </Index>
          </ArrayElement>
          <RealValue>1.1</RealValue>
        </EQNumeric>

        <!-- unknown real -->

        <NOT>
          <IsKnown>
            <ArrayElement>
            <Name>myRealArray</Name>
            <Index>
              <IntegerValue>2</IntegerValue>
            </Index>
            </ArrayElement>
          </IsKnown>
        </NOT>

        <!-- known integer -->

        <EQNumeric>
          <ArrayElement>
            <Name>myIntegerArray</Name>
            <Index>
              <IntegerValue>0</IntegerValue>
            </Index>
          </ArrayElement>
          <IntegerValue>1</IntegerValue>
        </EQNumeric>

        <!-- unknown integer -->

        <NOT>
          <IsKnown>
            <ArrayElement>
            <Name>myIntegerArray</Name>
            <Index>
              <IntegerValue>2</IntegerValue>
            </Index>
            </ArrayElement>
          </IsKnown>
        </NOT>

        <!-- known boolean -->

        <ArrayElement>
          <Name>myBooleanArray</Name>
          <Index>
            <IntegerValue>0</IntegerValue>
          </Index>
        </ArrayElement>

        <!-- known boolean -->

        <NOT>
          <ArrayElement>
            <Name>myBooleanArray</Name>
            <Index>
              <IntegerValue>1</IntegerValue>
            </Index>
          </ArrayElement>
        </NOT>

        <!-- unknown boolean -->

        <NOT>
          <IsKnown>
            <ArrayElement>
            <Name>myBooleanArray</Name>
            <Index>
              <IntegerValue>2</IntegerValue>
            </Index>
            </ArrayElement>
          </IsKnown>
        </NOT>
      </AND>
    </StartCondition>
    <NodeBody>
      <NodeList>

        <!-- lookup a state -->

        <Node NodeType="Empty">
          <NodeId>testIntStateVariable</NodeId>
          <StartCondition>
            <EQNumeric>
              <LookupOnChange>
                <StateName>stateInteger</StateName>
              </LookupOnChange>
              <IntegerValue>100</IntegerValue>
            </EQNumeric>
          </StartCondition>
        </Node>
        
        <!-- execute a command which returns an array -->
        
        <Node NodeType="Command">
          <NodeId>callBoolArrayCommand</NodeId>
          <NodeBody>
            <Command>
              <ArrayVariable>myBooleanArray</ArrayVariable>
              <CommandName>boolArrayCommand</CommandName>
            </Command>
          </NodeBody>
        </Node>
        
        <!-- the test that the command executed and the 
             boolean array values updated -->

        <Node NodeType="Empty">
          <NodeId>testCallBoolArrayCommand</NodeId>
          <StartCondition>
            <AND>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeId>callBoolArrayCommand</NodeId>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
              </EQInternal>
              <ArrayElement>
                <Name>myBooleanArray</Name>
                <Index>
                  <IntegerValue>1</IntegerValue>
                </Index>
              </ArrayElement>
            </AND>
            </StartCondition>
        </Node>
        
        <!-- call a function which returns an array -->
    
        <Node NodeType="FunctionCall">
          <NodeId>callStringArrayFunction</NodeId>
          <NodeBody>
            <FunctionCall>
              <ArrayVariable>myStringArray</ArrayVariable>
              <FunctionName>stringArrayFunction</FunctionName>
            </FunctionCall>
          </NodeBody>
        </Node>
        
        <!-- the test that the function executed and
             the string array values updated -->

        <Node NodeType="Empty">
          <NodeId>testCallStringArrayFunction</NodeId>
          <StartCondition>
            <AND>
              <EQInternal>
                <NodeOutcomeVariable>
                  <NodeId>callStringArrayFunction</NodeId>
                </NodeOutcomeVariable>
                <NodeOutcomeValue>SUCCESS</NodeOutcomeValue>
              </EQInternal>
              <EQString>
                <ArrayElement>
                  <Name>myStringArray</Name>
                  <Index>
                    <IntegerValue>1</IntegerValue>
                  </Index>
                </ArrayElement>
                <StringValue>new string 2</StringValue>
              </EQString>
            </AND>
            </StartCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>