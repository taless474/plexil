<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan>
<Node NodeType="NodeList">
  <NodeId>SimpleDrive</NodeId>
  <NodeBody>
    <NodeList>
      <Node NodeType="Command">
        <NodeId>Drive</NodeId>
        <PostCondition>
          <EQInternal>
            <NodeCommandHandleVariable>
              <NodeId>Drive</NodeId>
            </NodeCommandHandleVariable>
            <NodeCommandHandleValue>COMMAND_SENT_TO_SYSTEM</NodeCommandHandleValue>
          </EQInternal>
        </PostCondition>
        <NodeBody>
          <Command>
            <Name><StringValue>drive</StringValue></Name>
          </Command>
        </NodeBody>
      </Node>
    </NodeList>
  </NodeBody>
</Node>
</PlexilPlan>
