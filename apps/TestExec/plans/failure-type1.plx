<?xml version="1.0" encoding="UTF-8"?>
<!-- Generated by PlexiLisp 2.0.5 on Fri Apr 25 16:24:44 2008 -->
<PlexilPlan>
  <Node NodeType="NodeList" >
    <NodeId>Root</NodeId>
    <NodeBody>
      <NodeList>
        <Node NodeType="Empty" >
          <NodeId>One</NodeId>
          <PostCondition>
            <EQNumeric>
              <IntegerValue>1</IntegerValue>
              <IntegerValue>0</IntegerValue>
            </EQNumeric>
          </PostCondition>
        </Node>
        <Node NodeType="Empty" >
          <NodeId>Two</NodeId>
          <EndCondition>
            <EQInternal>
              <NodeFailureVariable>
                <NodeId>One</NodeId>
              </NodeFailureVariable>
              <NodeFailureValue>POST_CONDITION_FAILED</NodeFailureValue>
            </EQInternal>
          </EndCondition>
        </Node>
      </NodeList>
    </NodeBody>
  </Node>
</PlexilPlan>
