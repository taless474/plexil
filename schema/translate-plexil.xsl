<?xml version="1.0" encoding="ISO-8859-1"?>

<!--
* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

<xsl:transform version="2.0"
               xmlns:tr="extended-plexil-translator"
               xmlns:xs="http://www.w3.org/2001/XMLSchema"
               xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
               exclude-result-prefixes="xs">

  <xsl:output method="xml" indent="yes"/>

  <!-- To Do incorporate into run-te (?) add checks for all node
       states?  add all node failure types to transition diagrams
       Implementation:  copy comments (hard) use copy-of in local
       variables -->

  <!-- This is the "overriding copy idiom", from "XSLT Cookbook" by
       Sal Mangano.  It is the identity transform, covering all
       elements that are not explicitly handled elsewhere. -->

  <xsl:template match= "node() | @*">
    <xsl:copy>
      <xsl:apply-templates select= "@* | node()"/>
    </xsl:copy>
  </xsl:template>

  <!-- Abstraction for Action constructs.  Unfortunately, this is not
       allowed in 'select' attributes, so the actions are enumerated
       there. -->

  <xsl:key name="action"
           match="Node|Concurrence|Sequence|UncheckedSequence|Try|If|While|For|OnMessage|
                  OnCommand|Wait|SynchronousCommand"
           use="."/>

  <!-- Entry point -->
  <xsl:template match="PlexilPlan">
    <PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
      <!-- 0 or 1 expected -->
      <xsl:copy-of select="GlobalDeclarations"/>
      <!-- 1 expected -->
      <xsl:apply-templates select="key('action', *)"/>
    </PlexilPlan>
  </xsl:template>

  <xsl:template match="Node">
    <xsl:call-template name="translate-node" />
  </xsl:template>

  <xsl:template match="Node" mode="ordered">
    <xsl:call-template name="translate-node">
      <xsl:with-param name="ordered" select="'true'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="translate-node">
    <xsl:param name="ordered" />
    <Node>
      <!-- Parts that are copied directly -->
      <xsl:copy-of select="@NodeType" />
      <xsl:copy-of select="@FileName" />
      <xsl:copy-of select="@LineNo" />
      <xsl:copy-of select="@ColNo" />
      <xsl:copy-of select="NodeId" />
      <!-- <xsl:copy-of select="VariableDeclarations" /> -->
      <xsl:apply-templates select="VariableDeclarations"/>
      <xsl:copy-of select="Priority" />
      <xsl:copy-of select="Permissions" />
      <xsl:copy-of select="Interface" />
      <!-- Handle start condition -->
      <xsl:choose>
        <xsl:when test="$ordered">
          <xsl:call-template name="ordered-start-condition" />
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates select="StartCondition" />
        </xsl:otherwise>
      </xsl:choose>
      <!-- Elements that may need translation -->
      <xsl:apply-templates select="RepeatCondition|PreCondition|PostCondition|
                                   InvariantCondition|ExitCondition|EndCondition" />
      <xsl:apply-templates select="NodeBody" />
    <!-- Handle skip condition -->
    <xsl:choose>
      <xsl:when test="$ordered">
        <xsl:call-template name="ordered-skip-condition" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="SkipCondition" />
      </xsl:otherwise>
    </xsl:choose>
    </Node>
  </xsl:template>

  <xsl:template name="ordered-start-condition">
    <xsl:choose>
      <xsl:when
          test="preceding-sibling::Node|preceding-sibling::Sequence|
                preceding-sibling::UncheckedSequence|preceding-sibling::If|
                preceding-sibling::While|preceding-sibling::For|
                preceding-sibling::Try|preceding-sibling::Concurrence|
                preceding-sibling::OnCommand|preceding-sibling::OnMessage|
                preceding-sibling::SynchronousCommand|preceding-sibling::Wait">
        <StartCondition>
          <AND>
            <xsl:choose>
              <xsl:when test="preceding-sibling::*[1]/NodeId">
                <xsl:call-template name="node-finished">
                  <xsl:with-param name="id"
                                  select="preceding-sibling::*[1]/NodeId" />
                </xsl:call-template>
              </xsl:when>
              <xsl:otherwise>
                <xsl:call-template name="node-finished">
                  <xsl:with-param name="id"
                                  select="tr:node-id(preceding-sibling::*[1])" />
                </xsl:call-template>
              </xsl:otherwise>
            </xsl:choose>
            <xsl:apply-templates select="StartCondition/*" />
          </AND>
        </StartCondition>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="StartCondition" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="ordered-skip-condition">
    <xsl:if test= "SkipCondition">
      <xsl:choose>
        <xsl:when
            test="preceding-sibling::Node|preceding-sibling::Sequence|
                  preceding-sibling::UncheckedSequence|preceding-sibling::If|
                  preceding-sibling::While|preceding-sibling::For|
                  preceding-sibling::Try|preceding-sibling::Concurrence|
                  preceding-sibling::OnCommand|preceding-sibling::OnMessage|
                  preceding-sibling::SynchronousCommand|preceding-sibling::Wait">
          <SkipCondition>
            <AND>
              <xsl:choose>
                <xsl:when test="preceding-sibling::*[1]/NodeId">
                  <xsl:call-template name="node-finished">
                    <xsl:with-param name="id"
                                    select="preceding-sibling::*[1]/NodeId" />
                  </xsl:call-template>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:call-template name="node-finished">
                    <xsl:with-param name="id"
                                    select="tr:node-id(preceding-sibling::*[1])" />
                  </xsl:call-template>
                </xsl:otherwise>
              </xsl:choose>
              <xsl:apply-templates select="SkipCondition/*" />
            </AND>
          </SkipCondition>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates select="SkipCondition" />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:template>

  <xsl:template match="UncheckedSequence">
    <Node NodeType="NodeList" epx="UncheckedSequence">
      <xsl:call-template name="translate-nose-clauses" />
      <xsl:call-template name="sequence-body" />
    </Node>
  </xsl:template>

  <xsl:template match="UncheckedSequence" mode="ordered">
    <Node NodeType="NodeList" epx="UncheckedSequence">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
      </xsl:call-template>
      <xsl:call-template name="sequence-body" />
    </Node>
  </xsl:template>

  <xsl:template name="sequence-body">
    <NodeBody>
      <NodeList>
        <xsl:for-each select="child::* intersect key('action', *)">
          <xsl:apply-templates select="." mode="ordered" />
        </xsl:for-each>
      </NodeList>
    </NodeBody>
  </xsl:template>


  <xsl:template match="Sequence">
    <Node NodeType="NodeList" epx="Sequence">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="success-invariant" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="sequence-body" />
    </Node>
  </xsl:template>

  <xsl:template match="Sequence" mode="ordered">
    <Node NodeType="NodeList" epx="Sequence">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
        <xsl:with-param name="success-invariant" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="sequence-body" />
    </Node>
  </xsl:template>

  <xsl:template match="Concurrence">
    <Node NodeType="NodeList" epx="Concurrence">
      <xsl:call-template name="translate-nose-clauses" />
      <xsl:call-template name="concurrent-body" />
    </Node>
  </xsl:template>

  <xsl:template match="Concurrence" mode="ordered">
    <Node NodeType="NodeList" epx="Concurrence">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
      </xsl:call-template>
      <xsl:call-template name="concurrent-body" />
    </Node>
  </xsl:template>

  <xsl:template name="concurrent-body">
    <NodeBody>
      <NodeList>
        <xsl:for-each select="child::* intersect key('action', *)">
          <xsl:apply-templates select="." />
        </xsl:for-each>
      </NodeList>
    </NodeBody>
  </xsl:template>


  <xsl:template match="Try">
    <Node NodeType="NodeList" epx="Try">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="try-clauses" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="sequence-body" />
    </Node>
  </xsl:template>

  <xsl:template match="Try" mode="ordered">
    <Node NodeType="NodeList" epx="Try">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
        <xsl:with-param name="try-clauses" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="sequence-body" />
    </Node>
  </xsl:template>


  <xsl:template match="If">
    <Node NodeType="NodeList" epx="If">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="declare-test" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="if-body" />
    </Node>
  </xsl:template>

  <xsl:template match="If" mode="ordered">
    <Node NodeType="NodeList" epx="If">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
        <xsl:with-param name="declare-test" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="if-body" />
    </Node>
  </xsl:template>

  <xsl:template name="if-body">
    <xsl:variable name="setup-node-id" select="tr:prefix('IfSetup')" />
    <xsl:variable name="true-node-id" select="tr:prefix('IfThenCase')" />
    <xsl:variable name="false-node-id" select="tr:prefix('IfElseCase')" />
    <NodeBody>
      <NodeList>
        <xsl:call-template name="retest">
          <xsl:with-param name="id" select="$setup-node-id" />
        </xsl:call-template>
        <Node NodeType="NodeList" epx="aux">
          <NodeId>
            <xsl:value-of select="tr:prefix('IfBody')" />
          </NodeId>
          <StartCondition>
            <xsl:call-template name="node-finished">
              <xsl:with-param name="id" select="$setup-node-id" />
            </xsl:call-template>
          </StartCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="NodeList" epx="Then">
                <NodeId>
                  <xsl:value-of select="$true-node-id" />
                </NodeId>
                <StartCondition>
                  <BooleanVariable>
                    <xsl:value-of select="tr:prefix('test')" />
                  </BooleanVariable>
                </StartCondition>
                <SkipCondition>
                  <xsl:call-template name= "variable-unknown-or-false">
                    <xsl:with-param name= "var" select= "tr:prefix('test')"/>
                  </xsl:call-template>
                </SkipCondition>
                <NodeBody>
                  <NodeList>
                    <xsl:apply-templates select="Then/*" />
                  </NodeList>
                </NodeBody>
              </Node>
              <xsl:if test="Else">
                <Node NodeType="NodeList" epx="Else">
                  <NodeId>
                    <xsl:value-of select="$false-node-id" />
                  </NodeId>
                  <StartCondition>
                    <xsl:call-template name= "variable-unknown-or-false">
                      <xsl:with-param name= "var" select= "tr:prefix('test')"/>
                    </xsl:call-template>
                  </StartCondition>
                  <SkipCondition>
                    <BooleanVariable>
                      <xsl:value-of select="tr:prefix('test')" />
                    </BooleanVariable>
                  </SkipCondition>
                  <NodeBody>
                    <NodeList>
                      <xsl:apply-templates select="Else/*" />
                    </NodeList>
                  </NodeBody>
                </Node>
              </xsl:if>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </xsl:template>

  <xsl:template name= "variable-unknown-or-false">
    <xsl:param name= "var"/>
    <OR>
      <NOT>
        <IsKnown>
          <BooleanVariable>
            <xsl:value-of select="$var" />
          </BooleanVariable>
        </IsKnown>
      </NOT>
      <NOT>
        <BooleanVariable>
          <xsl:value-of select="$var" />
        </BooleanVariable>
      </NOT>
    </OR>
  </xsl:template>

  <xsl:template match="While">
    <Node NodeType="NodeList" epx="While">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="declare-test" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="while-body" />
    </Node>
  </xsl:template>

  <xsl:template match="While" mode="ordered">
    <Node NodeType="NodeList" epx="While">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
        <xsl:with-param name="declare-test" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="while-body" />
    </Node>
  </xsl:template>

  <xsl:template name="while-body">
    <xsl:variable name="setup-id" select="tr:prefix('WhileSetup')" />
    <xsl:variable name="retest-id" select="tr:prefix('WhileRetest')" />
    <xsl:variable name="true-node-id" select="tr:prefix('WhileTrue')" />
    <xsl:variable name="action-node-id" select="tr:prefix('WhileAction')" />
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="aux">
          <NodeId>
            <xsl:value-of select="tr:prefix('WhileBody')" />
          </NodeId>
          <NodeBody>
            <NodeList>
              <xsl:call-template name="retest">
                <xsl:with-param name="id" select="$setup-id" />
              </xsl:call-template>
              <Node NodeType="NodeList" epx="aux">
                <NodeId>
                  <xsl:value-of select="$true-node-id" />
                </NodeId>
                <StartCondition>
                  <AND>
                    <xsl:call-template name="node-finished">
                      <xsl:with-param name="id" select="$setup-id" />
                    </xsl:call-template>
                    <BooleanVariable>
                      <xsl:value-of select="tr:prefix('test')" />
                    </BooleanVariable>
                  </AND>
                </StartCondition>
                <SkipCondition>
                  <NOT>
                    <BooleanVariable>
                      <xsl:value-of select="tr:prefix('test')" />
                    </BooleanVariable>
                  </NOT>
                </SkipCondition>
                <RepeatCondition>
                  <BooleanVariable>
                    <xsl:value-of select="tr:prefix('test')" />
                  </BooleanVariable>
                </RepeatCondition>
                <NodeBody>
                  <NodeList>
                    <Node NodeType="NodeList" epx="aux">
                      <NodeId>
                        <xsl:value-of select="$action-node-id" />
                      </NodeId>
                      <NodeBody>
                        <NodeList>
                          <xsl:apply-templates
                            select="Action/*" />
                        </NodeList>
                      </NodeBody>
                    </Node>
                    <xsl:call-template name="retest">
                      <xsl:with-param name="id" select="$retest-id" />
                      <xsl:with-param name="predecessor"
                        select="$action-node-id" />
                    </xsl:call-template>
                  </NodeList>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </xsl:template>


  <xsl:template name="retest">
    <!--
      Assigns to the (predeclared) variable 'test' the value of the
      element 'Condition'. This node is reused in several forms.
    -->
    <xsl:param name="id" />
    <xsl:param name="predecessor" />
    <Node NodeType="Assignment" epx="aux">
      <NodeId>
        <xsl:value-of select="$id" />
      </NodeId>
      <xsl:if test="$predecessor">
        <StartCondition>
          <xsl:call-template name="node-finished">
            <xsl:with-param name="id" select="$predecessor" />
          </xsl:call-template>
        </StartCondition>
      </xsl:if>
      <NodeBody>
        <Assignment>
          <BooleanVariable>
            <xsl:value-of select="tr:prefix('test')" />
          </BooleanVariable>
          <BooleanRHS>
            <xsl:apply-templates select="Condition/*" />
          </BooleanRHS>
        </Assignment>
      </NodeBody>
    </Node>
  </xsl:template>


  <xsl:template match="For">
    <Node NodeType="NodeList" epx="For">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="declare-for" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="for-body" />
    </Node>
  </xsl:template>

  <xsl:template match="For" mode="ordered">
    <Node NodeType="NodeList" epx="For">
      <xsl:call-template name="translate-nose-clauses">
        <xsl:with-param name="mode" select="'ordered'" />
        <xsl:with-param name="declare-for" select="'true'" />
      </xsl:call-template>
      <xsl:call-template name="for-body" />
    </Node>
  </xsl:template>

  <xsl:template name="for-body">
    <xsl:variable name="setup-node-id" select="tr:prefix('ForSetup')" />
    <xsl:variable name="loop-node-id" select="tr:prefix('ForLoop')" />
    <xsl:variable name="do-node-id" select="tr:prefix('ForDo')" />
    <NodeBody>
      <NodeList>
        <Node NodeType="NodeList" epx="aux">
          <NodeId>
            <xsl:value-of select="$loop-node-id" />
          </NodeId>
          <RepeatCondition>
            <xsl:apply-templates select="Condition/*" />
          </RepeatCondition>
          <NodeBody>
            <NodeList>
              <Node NodeType="NodeList" epx="aux">
                <NodeId>
                  <xsl:value-of select="$do-node-id" />
                </NodeId>
                <NodeBody>
                  <NodeList>
                    <xsl:apply-templates select="Action/*" />
                  </NodeList>
                </NodeBody>
              </Node>
              <Node NodeType="Assignment" epx="aux">
                <NodeId>
                  <xsl:value-of select="tr:prefix('ForLoopUpdater')" />
                </NodeId>
                <StartCondition>
                  <xsl:call-template name="node-finished">
                    <xsl:with-param name="id" select="$do-node-id" />
                  </xsl:call-template>
                </StartCondition>
                <NodeBody>
                  <Assignment>
                    <xsl:choose>
                      <xsl:when
                        test="LoopVariable/DeclareVariable/Type = 'Integer'">
                        <IntegerVariable>
                          <xsl:value-of
                            select="LoopVariable/DeclareVariable/Name" />
                        </IntegerVariable>
                      </xsl:when>
                      <xsl:when
                        test="LoopVariable/DeclareVariable/Type = 'Real'">
                        <RealVariable>
                          <xsl:value-of
                            select="LoopVariable/DeclareVariable/Name" />
                        </RealVariable>
                      </xsl:when>
                      <xsl:otherwise>
                        <error>Illegal loop variable type in For</error>
                      </xsl:otherwise>
                    </xsl:choose>
                    <NumericRHS>
                      <xsl:copy-of select="LoopVariableUpdate/*" />
                    </NumericRHS>
                  </Assignment>
                </NodeBody>
              </Node>
            </NodeList>
          </NodeBody>
        </Node>
      </NodeList>
    </NodeBody>
  </xsl:template>


  <!-- Wait -->

  <xsl:template match="Wait">
    <Node NodeType="Empty" epx="Wait">
      <xsl:call-template name="basic-clauses"/>
      <!-- ;<xsl:copy-of select="VariableDeclarations"/> -->
      <xsl:apply-templates select="VariableDeclarations"/>
      <xsl:apply-templates select="StartCondition"/>
      <xsl:apply-templates select="RepeatCondition"/>
      <xsl:apply-templates select="PreCondition"/>
      <xsl:apply-templates select="PostCondition"/>
      <xsl:apply-templates select="InvariantCondition"/>
      <xsl:apply-templates select="ExitCondition"/>
      <xsl:call-template name= "wait-end-condition"/>
      <xsl:apply-templates select="SkipCondition"/>
    </Node>
  </xsl:template>

  <xsl:template match="Wait" mode= "ordered">
    <Node NodeType="Empty" epx="Wait">
      <xsl:call-template name="basic-clauses"/>
      <!-- <xsl:copy-of select="VariableDeclarations"/> -->
      <xsl:apply-templates select="VariableDeclarations"/>
      <xsl:call-template name="ordered-start-condition"/>
      <xsl:apply-templates select="RepeatCondition"/>
      <xsl:apply-templates select="PreCondition"/>
      <xsl:apply-templates select="PostCondition"/>
      <xsl:apply-templates select="InvariantCondition"/>
      <xsl:apply-templates select="ExitCondition"/>
      <xsl:call-template name= "wait-end-condition"/>
      <xsl:call-template name="ordered-skip-condition"/>
    </Node>
  </xsl:template>

  <xsl:template name= "wait-end-condition">
    <EndCondition>
      <OR>
        <xsl:apply-templates select="EndCondition/*"/>
        <xsl:call-template name= "timed-out">
          <xsl:with-param name= "element" select= "Units/*"/>
        </xsl:call-template>
      </OR>
    </EndCondition>
  </xsl:template>

  <xsl:template name= "timed-out">
    <xsl:param name= "element"/>
    <GE>
      <LookupOnChange>
        <Name>
          <StringValue>time</StringValue>
        </Name>
        <xsl:choose>
          <xsl:when test= "Tolerance">
            <xsl:apply-templates select="Tolerance"/>
          </xsl:when>
          <xsl:otherwise>
            <Tolerance>
              <RealValue>1.0</RealValue>
            </Tolerance>
          </xsl:otherwise>
        </xsl:choose>
      </LookupOnChange>
      <ADD>
        <xsl:apply-templates select="$element"/>
        <NodeTimepointValue>
          <xsl:call-template name= "insert-node-id"/>
          <NodeStateValue>EXECUTING</NodeStateValue>
          <Timepoint>START</Timepoint>
        </NodeTimepointValue>
      </ADD>
    </GE>
  </xsl:template>

  <xsl:template match="SynchronousCommand">
    <xsl:choose>
      <xsl:when test= "Command/IntegerVariable|
                       Command/RealVariable|
                       Command/BooleanVariable|
                       Command/StringVariable|
                       Command/ArrayVariable">
        <xsl:call-template name= "command-with-return"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name= "command-without-return"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="SynchronousCommand" mode= "ordered">
    <xsl:choose>
      <xsl:when test= "Command/IntegerVariable|
                       Command/RealVariable|
                       Command/BooleanVariable|
                       Command/StringVariable|
                       Command/ArrayVariable">
        <xsl:call-template name= "command-with-return">
          <xsl:with-param name= "ordered" select= "true"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name= "command-without-return">
          <xsl:with-param name= "ordered" select= "true"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- NOTE: the following two templates could be refactored a bit! -->

  <xsl:template name="command-with-return">
    <xsl:param name= "ordered"/>
    <xsl:variable name= "return" select= "tr:prefix('return')"/>
    <!-- Hack to save array name, iff command's return is an array -->
    <xsl:variable name= "array_name" select= "Command/ArrayVariable"/>
    <xsl:variable name= "decl">
      <xsl:choose>
        <xsl:when test= "Command/IntegerVariable">
          <IntegerVariable><xsl:value-of select="$return"/></IntegerVariable>
        </xsl:when>
        <xsl:when test= "Command/RealVariable">
          <RealVariable><xsl:value-of select="$return"/></RealVariable>
        </xsl:when>
        <xsl:when test= "Command/StringVariable">
          <StringVariable><xsl:value-of select="$return"/></StringVariable>
        </xsl:when>
        <xsl:when test= "Command/BooleanVariable">
          <BooleanVariable><xsl:value-of select="$return"/></BooleanVariable>
        </xsl:when>
        <xsl:when test= "Command/ArrayVariable">
          <ArrayVariable><xsl:value-of select="$return"/></ArrayVariable>
        </xsl:when>
        <xsl:otherwise>
          <error>Unrecognized variable type in SynchronousCommand</error>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name= "known-test">
      <xsl:choose>
        <xsl:when test= "not(Command/ArrayVariable)">
          <IsKnown><xsl:copy-of select= "$decl"/></IsKnown>
        </xsl:when>
        <xsl:otherwise>
          <IsKnown>
            <ArrayElement>
              <Name><xsl:value-of select= "$return"/></Name>
              <Index><IntegerValue>0</IntegerValue></Index>
          </ArrayElement>
          </IsKnown>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <Node NodeType="NodeList" epx="SynchronousCommand">
      <xsl:choose>
        <xsl:when test= "$ordered">
          <xsl:call-template name= "standard-preamble-ordered"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name= "standard-preamble"/>
        </xsl:otherwise>
      </xsl:choose>
      <NodeBody>
        <NodeList>
          <Node NodeType="NodeList" epx="aux">
            <NodeId>
              <xsl:value-of select="tr:prefix('SynchronousCommandAux')" />
            </NodeId>
            <VariableDeclarations>
              <xsl:choose>
                <xsl:when test= "Command/IntegerVariable">
                  <xsl:call-template name= "declare-variable">
                    <xsl:with-param name= "name" select= "$return"/>
                    <xsl:with-param name= "type" select= "'Integer'"/>
                  </xsl:call-template>
                </xsl:when>
                <xsl:when test= "Command/RealVariable">
                  <xsl:call-template name= "declare-variable">
                    <xsl:with-param name= "name" select= "$return"/>
                    <xsl:with-param name= "type" select= "'Real'"/>
                  </xsl:call-template>
                </xsl:when>
                <xsl:when test= "Command/StringVariable">
                  <xsl:call-template name= "declare-variable">
                    <xsl:with-param name= "name" select= "$return"/>
                    <xsl:with-param name= "type" select= "'String'"/>
                  </xsl:call-template>
                </xsl:when>
                <xsl:when test= "Command/BooleanVariable">
                  <xsl:call-template name= "declare-variable">
                    <xsl:with-param name= "name" select= "$return"/>
                    <xsl:with-param name= "type" select= "'Boolean'"/>
                  </xsl:call-template>
                </xsl:when>
                <xsl:when test= "Command/ArrayVariable">
                  <DeclareArray>
                    <Name><xsl:value-of select= "$return"/></Name>
                    <!-- A royal hack!  Couldn't find a more compact expression that worked. -->
                    <xsl:choose>
                      <!-- First see if the array we are proxying is local -->
                      <xsl:when test= "VariableDeclarations/DeclareArray[Name = $array_name][last()]">
                        <xsl:copy-of select= "VariableDeclarations/DeclareArray[Name = $array_name][last()]/Type"/>
                        <xsl:copy-of select= "VariableDeclarations/DeclareArray[Name = $array_name][last()]/MaxSize"/>
                        <!-- Otherwise find it in the closest ancestor -->
                      </xsl:when>
                      <xsl:when test= "ancestor::*/VariableDeclarations/DeclareArray[Name = $array_name][last()]">
                        <xsl:copy-of select= "ancestor::*/VariableDeclarations/DeclareArray[Name = $array_name][last()]/Type"/>
                        <xsl:copy-of select= "ancestor::*/VariableDeclarations/DeclareArray[Name = $array_name][last()]/MaxSize"/>
                      </xsl:when>
                    </xsl:choose>
                  </DeclareArray>
                </xsl:when>
                <xsl:otherwise>
                  <error>Unrecognized variable type in SynchronousCommand</error>
                </xsl:otherwise>
              </xsl:choose>
            </VariableDeclarations>
            <xsl:if test= "Timeout">
              <InvariantCondition>
                <xsl:call-template name= "timed-out">
                  <xsl:with-param name= "element" select= "Timeout/*"/>
                </xsl:call-template>
              </InvariantCondition>
            </xsl:if>
            <NodeBody>
              <NodeList>
                <Node NodeType= "Command" epx= "aux">
                  <NodeId>
                    <xsl:value-of select="tr:prefix('SynchronousCommandCommand')" />
                  </NodeId>
                  <EndCondition>
                    <xsl:copy-of select= "$known-test"/>
                  </EndCondition>
                  <NodeBody>
                    <Command>
                      <xsl:copy-of select= "Command/ResourceList"/>
                      <xsl:copy-of select= "$decl"/>
                      <xsl:copy-of select= "Command/Name"/>
                      <xsl:copy-of select= "Command/Arguments"/>
                    </Command>
                  </NodeBody>
                </Node>
                <Node NodeType= "Assignment" epx= "aux">
                  <NodeId>
                    <xsl:value-of select="tr:prefix('SynchronousCommandAssignment')" />
                  </NodeId>
                  <StartCondition>
                    <xsl:call-template name= "node-finished">
                      <xsl:with-param name= "id" select="tr:prefix('SynchronousCommandCommand')"/>
                    </xsl:call-template>
                  </StartCondition>
                  <NodeBody>
                    <Assignment>
                      <xsl:copy-of select= "Command/IntegerVariable|
                                            Command/RealVariable|
                                            Command/StringVariable|
                                            Command/BooleanVariable|
                                            Command/ArrayVariable"/>
                      <xsl:choose>
                        <xsl:when test= "Command/IntegerVariable|
                                         Command/RealVariable">
                          <NumericRHS><xsl:copy-of select="$decl"/></NumericRHS>
                        </xsl:when>
                        <xsl:when test= "Command/StringVariable">
                          <StringRHS><xsl:copy-of select="$decl"/></StringRHS>
                        </xsl:when>
                        <xsl:when test= "Command/BooleanVariable">
                          <BooleanRHS><xsl:copy-of select="$decl"/></BooleanRHS>
                        </xsl:when>
                        <xsl:when test= "Command/ArrayVariable">
                          <ArrayRHS><xsl:copy-of select="$decl"/></ArrayRHS>
                        </xsl:when>
                        <xsl:otherwise>
                          <error>Unrecognized variable type in SynchronousCommand</error>
                        </xsl:otherwise>
                      </xsl:choose>
                    </Assignment>
                  </NodeBody>
                </Node>
              </NodeList>
            </NodeBody>
          </Node>
        </NodeList>
      </NodeBody>
    </Node>
  </xsl:template>

  <xsl:template name="command-without-return">
    <xsl:param name= "ordered"/>
    <Node NodeType="NodeList" epx="SynchronousCommand">
      <xsl:choose>
        <xsl:when test= "$ordered">
          <xsl:call-template name= "standard-preamble-ordered"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name= "standard-preamble"/>
        </xsl:otherwise>
      </xsl:choose>
      <NodeBody>
        <NodeList>
          <Node NodeType="NodeList" epx="aux">
            <NodeId>
              <xsl:value-of select="tr:prefix('SynchronousCommandAux')" />
            </NodeId>
            <xsl:if test= "Timeout">
              <InvariantCondition>
                <xsl:call-template name= "timed-out">
                  <xsl:with-param name= "element" select= "Timeout/*"/>
                </xsl:call-template>
              </InvariantCondition>
            </xsl:if>
            <NodeBody>
              <NodeList>
                <Node NodeType= "Command" epx= "aux">
                  <NodeId>
                    <xsl:value-of select="tr:prefix('SynchronousCommandCommand')" />
                  </NodeId>
                  <EndCondition>
                    <OR>
                      <EQInternal>
                        <NodeCommandHandleVariable>
                          <NodeId>
                            <xsl:value-of select="tr:prefix('SynchronousCommandCommand')" />
                          </NodeId>
                        </NodeCommandHandleVariable>
                        <NodeCommandHandleValue>COMMAND_SUCCESS</NodeCommandHandleValue>
                      </EQInternal>
                      <EQInternal>
                        <NodeCommandHandleVariable>
                          <NodeId>
                            <xsl:value-of select="tr:prefix('SynchronousCommandCommand')" />
                          </NodeId>
                        </NodeCommandHandleVariable>
                        <NodeCommandHandleValue>COMMAND_FAILED</NodeCommandHandleValue>
                      </EQInternal>
                      <EQInternal>
                        <NodeCommandHandleVariable>
                          <NodeId>
                            <xsl:value-of select="tr:prefix('SynchronousCommandCommand')" />
                          </NodeId>
                        </NodeCommandHandleVariable>
                        <NodeCommandHandleValue>COMMAND_DENIED</NodeCommandHandleValue>
                      </EQInternal>
                    </OR>
                  </EndCondition>
                  <NodeBody>
                    <Command>
                      <xsl:copy-of select= "Command/ResourceList"/>
                      <xsl:copy-of select= "Command/Name"/>
                      <xsl:copy-of select= "Command/Arguments"/>
                    </Command>
                  </NodeBody>
                </Node>
              </NodeList>
            </NodeBody>
          </Node>
        </NodeList>
      </NodeBody>
    </Node>
  </xsl:template>

  <xsl:template name= "standard-preamble">
    <xsl:call-template name="basic-clauses"/>
    <xsl:apply-templates select="StartCondition"/>
    <xsl:apply-templates select="RepeatCondition"/>
    <xsl:apply-templates select="PreCondition"/>
    <xsl:apply-templates select="PostCondition"/>
    <xsl:apply-templates select="InvariantCondition"/>
    <xsl:apply-templates select="ExitCondition"/>
    <xsl:apply-templates select="EndCondition"/>
    <xsl:apply-templates select="SkipCondition"/>
    <!-- <xsl:copy-of select="VariableDeclarations"/> -->
    <xsl:apply-templates select="VariableDeclarations"/>
  </xsl:template>

  <xsl:template name= "standard-preamble-ordered">
    <xsl:call-template name="basic-clauses"/>
    <xsl:call-template name="ordered-start-condition"/>
    <xsl:apply-templates select="RepeatCondition"/>
    <xsl:apply-templates select="PreCondition"/>
    <xsl:apply-templates select="PostCondition"/>
    <xsl:apply-templates select="InvariantCondition"/>
    <xsl:apply-templates select="ExitCondition"/>
    <xsl:call-template name= "wait-end-condition"/>
    <xsl:call-template name="ordered-skip-condition"/>
    <!-- <xsl:copy-of select="VariableDeclarations"/> -->
    <xsl:apply-templates select="VariableDeclarations"/>
  </xsl:template>


  <!-- Action support -->

  <!-- This template translates the basic node/action clauses and is
       parameterized to handle specific kinds of actions.  Beginning to refactor
       and plan to eventually eliminate this unwieldy template!
  -->
  <xsl:template name="translate-nose-clauses">
    <xsl:param name="mode" />
    <xsl:param name="declare-test" />
    <xsl:param name="declare-for" />
    <xsl:param name="success-invariant" />
    <xsl:param name="try-clauses" />
    <xsl:param name="skip-basic-clauses"/>
    <xsl:param name="skip-variable-declarations"/>
    <!-- Conditionally include the basic clauses -->
    <xsl:if test="$skip-basic-clauses != 'true'">
      <xsl:call-template name="basic-clauses"/>
    </xsl:if>
    <!-- Special case translations -->
    <!-- Conditionally include the variable declarations -->
    <xsl:if test="$skip-variable-declarations != 'true'">
      <xsl:choose>
        <xsl:when test="$declare-test">
          <!-- declare a "test" variable in addition to existing -->
          <VariableDeclarations>
            <!-- <xsl:copy-of select="VariableDeclarations/*" /> -->
            <xsl:apply-templates select="VariableDeclarations/*"/>
            <xsl:call-template name="declare-variable">
              <xsl:with-param name="name" select="tr:prefix('test')" />
              <xsl:with-param name="type" select="'Boolean'" />
            </xsl:call-template>
          </VariableDeclarations>
        </xsl:when>
        <xsl:when test="$declare-for">
          <!-- declare the for loop's variable -->
          <VariableDeclarations>
            <!-- <xsl:copy-of select="VariableDeclarations/*" /> -->
            <xsl:apply-templates select="VariableDeclarations/*"/>
            <xsl:copy-of select="LoopVariable/*" />
          </VariableDeclarations>
        </xsl:when>
        <xsl:otherwise>
          <!-- <xsl:copy-of select="VariableDeclarations" /> -->
          <xsl:apply-templates select="VariableDeclarations"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <!-- Handle start condition -->
    <xsl:choose>
      <xsl:when test="$mode = 'ordered'">
        <xsl:call-template name="ordered-start-condition" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="StartCondition" />
      </xsl:otherwise>
    </xsl:choose>
    <!-- Translate remaining conditions, handling special cases -->
    <xsl:apply-templates select="RepeatCondition" />
    <xsl:apply-templates select="PreCondition" />
    <xsl:apply-templates select="ExitCondition" />
    <xsl:choose>
      <xsl:when test="$try-clauses">
        <PostCondition>
          <AND>
            <xsl:apply-templates select="PostCondition/*" />
            <OR>
              <xsl:for-each select="child::* intersect key('action', *)">
                <xsl:apply-templates select="."
                  mode="success-check" />
              </xsl:for-each>
            </OR>
          </AND>
        </PostCondition>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="PostCondition" />
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="$success-invariant">
        <InvariantCondition>
          <AND>
            <xsl:apply-templates select="InvariantCondition/*" />
            <NOT>
              <OR>
                <xsl:for-each select="child::* intersect key('action', *)">
                  <xsl:apply-templates select="."
                    mode="failure-check" />
                </xsl:for-each>
              </OR>
            </NOT>
          </AND>
        </InvariantCondition>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="InvariantCondition" />
      </xsl:otherwise>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="$try-clauses">
        <xsl:if test="key('action', *) | EndCondition/*">
          <EndCondition>
            <OR>
              <xsl:apply-templates select="EndCondition/*" />
              <xsl:for-each select="child::* intersect key('action', *)">
                <xsl:apply-templates select="."
                  mode="success-check" />
              </xsl:for-each>
                <AND>
                  <xsl:for-each select="child::* intersect key('action', *)">
                    <xsl:apply-templates select="."
                                         mode="finished-check" />
                  </xsl:for-each>
                </AND>
            </OR>
          </EndCondition>
        </xsl:if>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="EndCondition" />
      </xsl:otherwise>
    </xsl:choose>
    <!-- Handle skip condition -->
    <xsl:choose>
      <xsl:when test="$mode = 'ordered'">
        <xsl:call-template name="ordered-skip-condition" />
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="SkipCondition" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name= "basic-clauses">
    <!-- Copy attributes first -->
    <xsl:copy-of select="@FileName" />
    <xsl:copy-of select="@LineNo" />
    <xsl:copy-of select="@ColNo" />
    <!-- Then handle NodeId -->
    <xsl:call-template name="insert-node-id" />
    <!-- Copy clauses that don't need translation -->
    <xsl:copy-of select="Comment" />
    <xsl:copy-of select="Priority" />
    <xsl:copy-of select="Permissions" />
    <xsl:copy-of select="Interface" />
  </xsl:template>


  <xsl:template
    match="StartCondition|RepeatCondition|PreCondition|
                      PostCondition|InvariantCondition|EndCondition|
                      ExitCondition|SkipCondition">
    <xsl:element name="{name()}">
      <xsl:apply-templates select="*" />
    </xsl:element>
  </xsl:template>

  <xsl:template
    match="Node|Concurrence|Sequence|UncheckedSequence|Try|If|While|For|
           OnCommand|OnMessage|Wait|SynchronousCommand"
    mode="failure-check">
    <xsl:choose>
      <xsl:when test="NodeId">
        <xsl:call-template name="node-failed">
          <xsl:with-param name="id" select="NodeId" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="node-failed">
          <xsl:with-param name="id" select="tr:node-id(.)" />
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template
    match="Node|Concurrence|Sequence|UncheckedSequence|Try|If|While|For|
           OnCommand|OnMessage|Wait|SynchronousCommand"
    mode="success-check">
    <xsl:choose>
      <xsl:when test="NodeId">
        <xsl:call-template name="node-succeeded">
          <xsl:with-param name="id" select="NodeId" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="node-succeeded">
          <xsl:with-param name="id" select="tr:node-id(.)" />
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template
    match="Node|Concurrence|Sequence|UncheckedSequence|Try|If|While|For|
           OnCommand|OnMessage|Wait|SynchronousCommand"
    mode="finished-check">
    <xsl:choose>
      <xsl:when test="NodeId">
        <xsl:call-template name="node-finished">
          <xsl:with-param name="id" select="NodeId" />
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="node-finished">
          <xsl:with-param name="id" select="tr:node-id(.)" />
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="insert-node-id">
    <xsl:param name="node" select="." />
    <!-- Supply missing NodeId or copy existing one -->
    <xsl:choose>
      <xsl:when test="not($node/NodeId)">
        <NodeId>
          <xsl:value-of select="tr:node-id($node)" />
        </NodeId>
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy-of select="$node/NodeId" />
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="declare-variable">
    <xsl:param name="name" />
    <xsl:param name="type" />
    <xsl:param name="init-value" />
    <DeclareVariable>
      <Name>
        <xsl:value-of select="$name" />
      </Name>
      <Type>
        <xsl:value-of select="$type" />
      </Type>
      <xsl:if test="$init-value">
        <InitialValue>
          <xsl:element name="{concat($type, 'Value')}">
            <xsl:value-of select="$init-value" />
          </xsl:element>
        </InitialValue>
      </xsl:if>
    </DeclareVariable>
  </xsl:template>

  <!-- Boolean Expressions -->

  <!-- These expressions are translated recursively. -->
  <xsl:template match="IsKnown|GT|GE|LT|LE|EQNumeric|EQInternal|EQString|
                       NENumeric|NEInternal|NEString|OR|XOR|AND|NOT|EQBoolean|NEBoolean">
    <xsl:element name="{name()}">
      <xsl:apply-templates select="*" />
    </xsl:element>
  </xsl:template>

  <!-- These expressions are deep copied. (But must also be processed
       for dates and durations) -->
  <xsl:template match="BooleanVariable|BooleanValue|LookupOnChange|LookupNow|ArrayElement">
    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="Finished">
    <xsl:call-template name="node-finished" />
  </xsl:template>


  <xsl:template match="IterationEnded">
    <xsl:call-template name="node-iteration-ended" />
  </xsl:template>

  <xsl:template match="Executing">
    <xsl:call-template name="node-executing" />
  </xsl:template>

  <xsl:template match="Waiting">
    <xsl:call-template name="node-waiting" />
  </xsl:template>

  <xsl:template match="Inactive">
    <xsl:call-template name="node-inactive" />
  </xsl:template>

  <xsl:template match="Succeeded">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-succeeded" />
    </AND>
  </xsl:template>

  <xsl:template match="IterationSucceeded">
    <AND>
      <xsl:call-template name="node-iteration-ended" />
      <xsl:call-template name="node-succeeded" />
    </AND>
  </xsl:template>

  <xsl:template match="Failed">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-failed" />
    </AND>
  </xsl:template>

  <xsl:template match="Interrupted">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-interrupted" />
    </AND>
  </xsl:template>

  <xsl:template match="IterationFailed">
    <AND>
      <xsl:call-template name="node-iteration-ended" />
      <xsl:call-template name="node-failed" />
    </AND>
  </xsl:template>

  <xsl:template match="Skipped">
    <!-- NOTE: implies that node is in state FINISHED. -->
    <xsl:call-template name="node-skipped" />
  </xsl:template>

  <xsl:template match="InvariantFailed">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-invariant-failed" />
    </AND>
  </xsl:template>

  <xsl:template match="PreconditionFailed">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-precondition-failed" />
    </AND>
  </xsl:template>

  <xsl:template match="PostconditionFailed">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-postcondition-failed" />
    </AND>
  </xsl:template>

  <xsl:template match="ParentFailed">
    <AND>
      <xsl:call-template name="node-finished" />
      <xsl:call-template name="node-parent-failed" />
    </AND>
  </xsl:template>

  <!--
    Support for message passing between executives
  -->

  <!-- Warning:  This one might be obsolete. -->
  <xsl:template match="MessageReceived">
    <LookupOnChange>
      <Name>
        <Concat>
          <StringValue>MESSAGE__</StringValue>
          <xsl:copy-of select="*" />
        </Concat>
      </Name>
    </LookupOnChange>
  </xsl:template>

  <xsl:template match="OnMessage">
    <xsl:variable name="Msg_staging">
      <xsl:call-template name="OnMessage-staging" />
    </xsl:variable>
    <xsl:apply-templates select="$Msg_staging" />
  </xsl:template>

  <xsl:template match="OnMessage" mode="ordered">
    <xsl:variable name="Msg_staging">
      <xsl:call-template name="OnMessage-staging" />
    </xsl:variable>
    <xsl:apply-templates select="$Msg_staging"
      mode="ordered" />
  </xsl:template>

  <xsl:template name="OnMessage-staging">
    <Sequence>
      <xsl:copy-of select="@FileName" />
      <xsl:copy-of select="@LineNo" />
      <xsl:copy-of select="@ColNo" />
      <VariableDeclarations>
        <DeclareVariable>
          <Name>
            <xsl:value-of select="tr:prefix('hdl')" />
          </Name>
          <Type>String</Type>
        </DeclareVariable>
      </VariableDeclarations>
      <xsl:copy-of select="NodeId" />
      <!-- Find parent node and set invariant, if exists -->
      <xsl:variable name="parent_id">
        <xsl:call-template name="parent-id-value" />
      </xsl:variable>
      <xsl:if test="not($parent_id='')">
        <InvariantCondition>
          <AND>
            <EQInternal>
              <NodeStateVariable>
                <NodeId>
                  <xsl:value-of select="$parent_id" />
                </NodeId>
              </NodeStateVariable>
              <NodeStateValue>EXECUTING</NodeStateValue>
            </EQInternal>
          </AND>
        </InvariantCondition>
      </xsl:if>
      <!-- Msg wait node -->
      <xsl:variable name="hdl_dec">
        <StringVariable>
          <xsl:value-of select="tr:prefix('hdl')" />
        </StringVariable>
      </xsl:variable>
      <xsl:call-template name="run-wait-command">
        <xsl:with-param name="command" select="'ReceiveMessage'" />
        <xsl:with-param name="dest" select="$hdl_dec" />
        <xsl:with-param name="args" select="Message/*" />
      </xsl:call-template>
      <!-- Action for this message -->
      <Node NodeType="NodeList" epx="aux">
        <NodeId>
          <xsl:value-of
            select="concat(tr:prefix('MsgAction'), '_', Name/StringValue/text())" />
        </NodeId>
        <NodeBody>
          <NodeList>
            <xsl:copy-of select="key('action', *)" />
          </NodeList>
        </NodeBody>
      </Node>
    </Sequence>
  </xsl:template>

  <xsl:template match="OnCommand">
    <xsl:variable name="Cmd_staging">
      <xsl:call-template name="OnCommand-staging" />
    </xsl:variable>
    <xsl:apply-templates select="$Cmd_staging" />
  </xsl:template>

  <xsl:template match="OnCommand" mode="ordered">
    <xsl:variable name="Cmd_staging">
      <xsl:call-template name="OnCommand-staging">
        <xsl:with-param name="ordered" select="'true'"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:apply-templates select="$Cmd_staging"
      mode="ordered" />
  </xsl:template>

  <xsl:template name="OnCommand-staging">
    <xsl:param name="ordered"/>
    <Sequence>
      <xsl:copy-of select="@FileName" />
      <xsl:copy-of select="@LineNo" />
      <xsl:copy-of select="@ColNo" />
      <VariableDeclarations>
        <!-- <xsl:copy-of select="VariableDeclarations/DeclareVariable" /> -->
        <xsl:apply-templates select="VariableDeclarations/DeclareVariable"/>
        <!-- Arrays are variables too -->
        <!-- <xsl:copy-of select="VariableDeclarations/DeclareArray"/> -->
        <xsl:apply-templates select="VariableDeclarations/DeclareArray"/>
        <DeclareVariable>
          <Name>
            <xsl:value-of select="tr:prefix('hdl')" />
          </Name>
          <Type>String</Type>
        </DeclareVariable>
      </VariableDeclarations>
      <!-- Handle the OnCommand node conditions -->
      <xsl:choose>
        <xsl:when test="$ordered">
          <xsl:call-template name="translate-nose-clauses">
            <xsl:with-param name="skip-basic-clauses" select="'true'"/>
            <xsl:with-param name="skip-variable-declarations" select="'true'"/>
            <xsl:with-param name="mode" select="'ordered'"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="translate-nose-clauses">
            <xsl:with-param name="skip-basic-clauses" select="'true'"/>
            <xsl:with-param name="skip-variable-declarations" select="'true'"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
      <!-- Find parent node and set invariant, if exists -->
      <xsl:variable name="parent_id">
        <xsl:call-template name="parent-id-value" />
      </xsl:variable>
      <!-- This invariant condition can create an out-of-scope node reference,
           e.g. when the OnCommand occurs inside a While.  The purpose and usefulness
           of this condition is questionable to begin with, so we're trying without
           it...
      <xsl:if test="not($parent_id='')">
        <InvariantCondition>
          <AND>
            <EQInternal>
              <NodeStateVariable>
                <NodeId>
                  <xsl:value-of select="$parent_id" />
                </NodeId>
              </NodeStateVariable>
              <NodeStateValue>EXECUTING</NodeStateValue>
            </EQInternal>
          </AND>
        </InvariantCondition>
      </xsl:if> -->
      <xsl:copy-of select="NodeId" />
      <!-- Cmd wait node -->
      <xsl:variable name="hdl_dec">
        <StringVariable>
          <xsl:value-of select="tr:prefix('hdl')" />
        </StringVariable>
      </xsl:variable>
      <xsl:variable name="arg_dec">
        <StringValue>
          <xsl:value-of select="Name/StringValue" />
        </StringValue>
      </xsl:variable>
      <xsl:call-template name="run-wait-command">
        <xsl:with-param name="command" select="'ReceiveCommand'" />
        <xsl:with-param name="dest" select="$hdl_dec" />
        <xsl:with-param name="args" select="$arg_dec" />
      </xsl:call-template>
      <!-- Cmd get parameters nodes -->
      <xsl:for-each select="VariableDeclarations/DeclareVariable | VariableDeclarations/DeclareArray">
        <Node NodeType="Command" epx="aux">
          <NodeId>
            <xsl:value-of
              select="concat(tr:prefix('CmdGetParam'), '_', Name/text())" />
          </NodeId>
          <EndCondition>
            <IsKnown>
              <xsl:choose>
                <xsl:when test="MaxSize"> <!-- Arrays -->
                  <ArrayElement>
                    <Name><xsl:value-of select="Name"/></Name>
                    <Index><IntegerValue>0</IntegerValue></Index>
                  </ArrayElement>
                </xsl:when>
                <xsl:otherwise> <!-- Scalars -->
                  <xsl:element name='{concat(Type/text(), "Variable")}'>
                    <xsl:value-of select="Name/text()" />
                  </xsl:element>
                </xsl:otherwise>
              </xsl:choose>
            </IsKnown>
          </EndCondition>
          <NodeBody>
            <Command>
              <xsl:choose>
                <xsl:when test="MaxSize"> <!-- Arrays -->
                  <ArrayVariable><xsl:value-of select="Name"/></ArrayVariable>
                </xsl:when>
                <xsl:otherwise> <!-- Scalars -->
                  <xsl:element name='{concat(Type/text(), "Variable")}'>
                    <xsl:value-of select="Name/text()" />
                  </xsl:element>
                </xsl:otherwise>
              </xsl:choose>
              <Name>
                <StringValue>GetParameter</StringValue>
              </Name>
              <Arguments>
                <StringVariable>
                  <xsl:value-of select="tr:prefix('hdl')" />
                </StringVariable>
                <IntegerValue>
                  <xsl:value-of select="position() - 1" />
                </IntegerValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
      </xsl:for-each>
      <!-- Action for this command -->
      <Node NodeType="NodeList" epx="aux">
        <NodeId>
          <xsl:value-of
            select="concat(tr:prefix('CmdAction'), '_', Name/StringValue/text())" />
        </NodeId>
        <NodeBody>
          <NodeList>
            <xsl:apply-templates select="key('action', *)"
              mode="oncommand-mode" />
          </NodeList>
        </NodeBody>
      </Node>
      <!--  Insert return value command if not present -->
      <xsl:if
        test="not(.//Command/Name/StringValue/text() = 'SendReturnValue')">
        <Node NodeType="Command" epx="aux">
          <NodeId>
            <xsl:value-of select="tr:prefix('CmdReturn')" />
          </NodeId>
          <NodeBody>
            <Command>
              <Name>
                <StringValue>SendReturnValue</StringValue>
              </Name>
              <Arguments>
                <StringVariable>
                  <xsl:value-of select="tr:prefix('hdl')" />
                </StringVariable>
                <BooleanValue>true</BooleanValue>
              </Arguments>
            </Command>
          </NodeBody>
        </Node>
      </xsl:if>
    </Sequence>
  </xsl:template>

  <!--
    Recursive template that attempts to find and insert the NodeId of
    the parent action to the given node
  -->
  <xsl:template name="parent-id-value">
    <xsl:param name="start_path" select="." />
    <xsl:choose>
      <!-- Insert NodeId via insert-node-id template -->
      <xsl:when test="key('action', $start_path/..)">
        <xsl:variable name="id">
          <xsl:call-template name="insert-node-id">
            <xsl:with-param name="node" select="$start_path/.." />
          </xsl:call-template>
        </xsl:variable>
        <xsl:value-of select="$id/NodeId" />
      </xsl:when>
      <!-- If not an action, go up a level and try again, if one exists -->
      <xsl:when test="$start_path/..">
        <xsl:call-template name="parent-id-value">
          <xsl:with-param name="start_path" select="$start_path/.." />
        </xsl:call-template>
      </xsl:when>
    </xsl:choose>

  </xsl:template>


  <xsl:template name="run-wait-command">
    <xsl:param name="command" />
    <xsl:param name="dest" />
    <xsl:param name="args" />
    <Node NodeType="Command" epx="aux">
      <NodeId>
        <xsl:copy-of select="tr:prefix('CmdWait')" />
      </NodeId>
      <EndCondition>
        <IsKnown>
          <xsl:copy-of select="$dest" />
        </IsKnown>
      </EndCondition>
      <NodeBody>
        <Command>
          <xsl:copy-of select="$dest" />
          <Name>
            <StringValue>
              <xsl:copy-of select="$command" />
            </StringValue>
          </Name>
          <Arguments>
            <xsl:copy-of select="$args" />
          </Arguments>
        </Command>
      </NodeBody>
    </Node>
  </xsl:template>

  <xsl:template match="Command" mode="oncommand-mode">
    <Command>
      <xsl:copy-of select="node()[./local-name()!='Arguments'] " />
      <xsl:choose>
        <xsl:when test="Name/StringValue/text()='SendReturnValue'">
          <Arguments>
            <StringVariable>
              <xsl:value-of select="tr:prefix('hdl')" />
            </StringVariable>
            <xsl:copy-of select="Arguments/node()" />
          </Arguments>
        </xsl:when>
        <xsl:otherwise>
          <xsl:copy-of select="Arguments" />
        </xsl:otherwise>
      </xsl:choose>
    </Command>
  </xsl:template>

  <xsl:template match="*" mode="oncommand-mode">
    <xsl:copy>
      <xsl:copy-of select="@*" />
      <xsl:apply-templates mode="oncommand-mode" />
    </xsl:copy>
  </xsl:template>

  <xsl:template name="node-finished">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-state-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="state" select="'FINISHED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-iteration-ended">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-state-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="state" select="'ITERATION_ENDED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-executing">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-state-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="state" select="'EXECUTING'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-waiting">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-state-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="state" select="'WAITING'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-inactive">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-state-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="state" select="'INACTIVE'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-state-check">
    <xsl:param name="id" />
    <xsl:param name="state" />
    <EQInternal>
      <NodeStateVariable>
        <NodeId>
          <xsl:value-of select="$id" />
        </NodeId>
      </NodeStateVariable>
      <NodeStateValue>
        <xsl:value-of select="$state" />
      </NodeStateValue>
    </EQInternal>
  </xsl:template>

  <xsl:template name="node-failed">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-outcome-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="outcome" select="'FAILURE'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-interrupted">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-outcome-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="outcome" select="'INTERRUPTED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-succeeded">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-outcome-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="outcome" select="'SUCCESS'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-skipped">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-outcome-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="outcome" select="'SKIPPED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-outcome-check">
    <xsl:param name="id" />
    <xsl:param name="outcome" />
    <EQInternal>
      <NodeOutcomeVariable>
        <NodeId>
          <xsl:value-of select="$id" />
        </NodeId>
      </NodeOutcomeVariable>
      <NodeOutcomeValue>
        <xsl:value-of select="$outcome" />
      </NodeOutcomeValue>
    </EQInternal>
  </xsl:template>


  <xsl:template name="node-invariant-failed">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-failure-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="failure" select="'INVARIANT_CONDITION_FAILED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-precondition-failed">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-failure-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="failure" select="'PRE_CONDITION_FAILED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-postcondition-failed">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-failure-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="failure" select="'POST_CONDITION_FAILED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-parent-failed">
    <xsl:param name="id" select="*" />
    <xsl:call-template name="node-failure-check">
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="failure" select="'PARENT_FAILED'" />
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="node-failure-check">
    <xsl:param name="id" />
    <xsl:param name="failure" />
    <EQInternal>
      <NodeFailureVariable>
        <NodeId>
          <xsl:value-of select="$id" />
        </NodeId>
      </NodeFailureVariable>
      <NodeFailureValue>
        <xsl:value-of select="$failure" />
      </NodeFailureValue>
    </EQInternal>
  </xsl:template>


  <!-- Generic Lookup form -->
  <xsl:template match= "Lookup">
    <xsl:choose>
      <xsl:when test= "ancestor::Command|ancestor::Assignment|ancestor::Update|
                       ancestor::Command|ancestor::PreCondition|
                       ancestor::PostCondition|ancestor::InvariantCondition">
        <LookupNow>
          <Name>
            <xsl:apply-templates select= "Name/*"/>
          </Name>
          <xsl:if test= "Arguments">
            <Arguments>
              <xsl:apply-templates select= "Arguments/*"/>
            </Arguments>
          </xsl:if>
        </LookupNow>
      </xsl:when>
      <xsl:otherwise>
        <LookupOnChange>
          <Name>
            <xsl:apply-templates select= "Name/*"/>
          </Name>
          <xsl:apply-templates select="Tolerance"/>
          <xsl:if test= "Arguments">
            <Arguments>
              <xsl:apply-templates select= "Arguments/*"/>
            </Arguments>
          </xsl:if>
        </LookupOnChange>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <!-- Dates and Durations (handled rather naively as real values) -->

  <!-- Some Epochs http://en.wikipedia.org/wiki/Epoch_(reference_date) -->

  <!-- Julian day 2415021 (DJD+1?) -->
  <xsl:variable name="UTC" select="xs:dateTime('1900-01-01T00:00:00Z')"/>

  <!-- Julian day 2440587.5 -->
  <xsl:variable name="Unix" select="xs:dateTime('1970-01-01T00:00:00Z')"/>

  <!-- Julian day 2448250 -->
  <xsl:variable name="CPS" select="xs:dateTime('1990-12-24T00:00:00Z')"/>

  <!-- Julian date 2451545.0 TT -->
  <xsl:variable name="J2000" select="xs:dateTime('2000-01-01T11:58:55.816Z')"/>

  <!-- Use this epoch -->
  <xsl:variable name="epoch" select="$Unix"/>

  <xsl:template match="Type[.='Date' or .='Duration']">
    <!-- Dates and Durations are represented as "real" values -->
    <Type>Real</Type>
  </xsl:template>

  <xsl:template match="DurationVariable|DateVariable">
    <!-- Dates and Durations are represented as "real" variables -->
    <RealVariable><xsl:value-of select="."/></RealVariable>
  </xsl:template>

  <xsl:template match="DateValue">
    <!-- A Date is the number of seconds since the start of the epoch used on this platform -->
    <RealValue>
      <xsl:value-of select="tr:seconds(xs:dateTime(.) - xs:dateTime($epoch))"/>
    </RealValue>
  </xsl:template>

  <xsl:template match="DurationValue">
    <!-- A Duration is the number of seconds in the ISO 8601 duration -->
    <RealValue><xsl:value-of select="tr:seconds(.)"/></RealValue>
  </xsl:template>

  <!-- Return the (total) number of seconds in and ISO 8601 duration -->
  <xsl:function name="tr:seconds">
    <xsl:param name="duration"/>
    <xsl:value-of select="xs:dayTimeDuration($duration) div xs:dayTimeDuration('PT1.0S')"/>
  </xsl:function>

  <!-- Functions -->

  <!-- Computes a unique NodeID -->
  <xsl:function name="tr:node-id">
    <xsl:param name="node" />
    <xsl:value-of
      select="tr:prefix(concat(name($node), '_', generate-id($node)))" />
  </xsl:function>

  <!-- Prefix names of some generated nodes and variables -->
  <xsl:function name="tr:prefix">
    <xsl:param name="name" />
    <xsl:value-of select="concat('ep2cp_', $name)" />
  </xsl:function>

</xsl:transform>
