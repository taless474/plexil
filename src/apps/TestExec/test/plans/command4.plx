<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp 2.0.5 on Thu Feb  7 19:41:00 2008 -->
<PlexilPlan>
<!-- Tests command with return value -->
  <Node NodeType="NodeList" >
    <NodeId>Root</NodeId>
    <VariableDeclarations>
      <DeclareVariable>
        <Name>result-string</Name>
        <Type>String</Type>
      </DeclareVariable>
    </VariableDeclarations>
    <NodeBody>
      <NodeList>
        <Node NodeType="Command" >
          <NodeId>StringCommand</NodeId>
          <NodeBody>
            <Command>
              <StringVariable>result-string</StringVariable>
              <Name>
                <LookupNow>
                  <Name>
                    <StringValue>CommandName</StringValue>
                  </Name>
                </LookupNow>
              </Name>
            </Command>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>