<?xml version="1.0" encoding="UTF-8"?>
<PlexilPlan xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xmlns:tr="extended-plexil-translator">
   <Node NodeType="NodeList" epx="Concurrence" FileName="DriveToTarget.ple" LineNo="4"
         ColNo="1">
      <NodeId>DriveToTarget</NodeId>
      <VariableDeclarations>
              <DeclareVariable>
                  <Name>drive_done</Name>
                  <Type>Boolean</Type>
                  <InitialValue>
                      <BooleanValue>false</BooleanValue>
                  </InitialValue>
              </DeclareVariable>
              <DeclareVariable>
                  <Name>timeout</Name>
                  <Type>Boolean</Type>
                  <InitialValue>
                      <BooleanValue>false</BooleanValue>
                  </InitialValue>
              </DeclareVariable>
          </VariableDeclarations>
      <NodeBody>
         <NodeList>
            <Node NodeType="Command" FileName="DriveToTarget.ple" LineNo="9" ColNo="5">
               <NodeId>Drive</NodeId>
               <NodeBody>
                  <Command>
                      <Name>
                          <StringValue>rover_drive</StringValue>
                      </Name>
                      <Arguments>
                          <IntegerValue>10</IntegerValue>
                      </Arguments>
                  </Command>
              </NodeBody>
            </Node>
            <Node NodeType="NodeList" epx="Concurrence" FileName="DriveToTarget.ple" LineNo="14"
                  ColNo="5">
               <NodeId>StopForTimeout</NodeId>
               <StartCondition>
                  <GE>
                     <LookupOnChange>
                          <Name>
                              <StringValue>time</StringValue>
                          </Name>
                      </LookupOnChange>
                     <IntegerValue>10</IntegerValue>
                  </GE>
               </StartCondition>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="Command" FileName="DriveToTarget.ple" LineNo="18" ColNo="9">
                        <NodeId>Stop</NodeId>
                        <NodeBody>
                           <Command>
                              <Name>
                                 <StringValue>rover_stop</StringValue>
                              </Name>
                           </Command>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" FileName="DriveToTarget.ple" LineNo="22" ColNo="9">
                        <NodeId>SetTimeoutFlag</NodeId>
                        <NodeBody>
                           <Assignment>
                              <BooleanVariable>timeout</BooleanVariable>
                              <BooleanRHS>
                                 <BooleanValue>true</BooleanValue>
                              </BooleanRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                  </NodeList>
               </NodeBody>
            </Node>
            <Node NodeType="NodeList" epx="Concurrence" FileName="DriveToTarget.ple" LineNo="28"
                  ColNo="5">
               <NodeId>StopForTarget</NodeId>
               <StartCondition>
                  <LookupOnChange>
                      <Name>
                          <StringValue>target_in_view</StringValue>
                      </Name>
                  </LookupOnChange>
               </StartCondition>
               <SkipCondition>
                  <BooleanVariable>timeout</BooleanVariable>
               </SkipCondition>
               <NodeBody>
                  <NodeList>
                     <Node NodeType="Command" FileName="DriveToTarget.ple" LineNo="33" ColNo="9">
                        <NodeId>Stop</NodeId>
                        <NodeBody>
                           <Command>
                              <Name>
                                 <StringValue>rover_stop</StringValue>
                              </Name>
                           </Command>
                        </NodeBody>
                     </Node>
                     <Node NodeType="Assignment" FileName="DriveToTarget.ple" LineNo="37" ColNo="9">
                        <NodeId>SetDriveFlag</NodeId>
                        <NodeBody>
                           <Assignment>
                              <BooleanVariable>drive_done</BooleanVariable>
                              <BooleanRHS>
                                 <BooleanValue>true</BooleanValue>
                              </BooleanRHS>
                           </Assignment>
                        </NodeBody>
                     </Node>
                  </NodeList>
               </NodeBody>
            </Node>
            <Node NodeType="Command" FileName="DriveToTarget.ple" LineNo="43" ColNo="5">
               <NodeId>TakeNavcam</NodeId>
               <StartCondition>
                  <BooleanVariable>timeout</BooleanVariable>
               </StartCondition>
               <NodeBody>
                  <Command>
                      <Name>
                          <StringValue>take_navcam</StringValue>
                      </Name>
                  </Command>
              </NodeBody>
               <SkipCondition>
                  <BooleanVariable>drive_done</BooleanVariable>
               </SkipCondition>
            </Node>
            <Node NodeType="Command" FileName="DriveToTarget.ple" LineNo="50" ColNo="5">
               <NodeId>TakePancam</NodeId>
               <StartCondition>
                  <BooleanVariable>drive_done</BooleanVariable>
               </StartCondition>
               <NodeBody>
                  <Command>
                      <Name>
                          <StringValue>take_pancam</StringValue>
                      </Name>
                  </Command>
              </NodeBody>
               <SkipCondition>
                  <BooleanVariable>timeout</BooleanVariable>
               </SkipCondition>
            </Node>
            <Node NodeType="Command" FileName="DriveToTarget.ple" LineNo="57" ColNo="5">
               <NodeId>Heater</NodeId>
               <StartCondition>
                  <LT>
                     <LookupOnChange>
                          <Name>
                              <StringValue>temperature</StringValue>
                          </Name>
                      </LookupOnChange>
                     <IntegerValue>0</IntegerValue>
                  </LT>
               </StartCondition>
               <EndCondition>
                  <GE>
                     <LookupOnChange>
                          <Name>
                              <StringValue>temperature</StringValue>
                          </Name>
                      </LookupOnChange>
                     <IntegerValue>10</IntegerValue>
                  </GE>
               </EndCondition>
               <RepeatCondition>
                  <BooleanValue>true</BooleanValue>
               </RepeatCondition>
               <NodeBody>
                  <Command>
                      <Name>
                          <StringValue>turn_on_heater</StringValue>
                      </Name>
                  </Command>
              </NodeBody>
               <SkipCondition>
                  <BooleanVariable>timeout</BooleanVariable>
               </SkipCondition>
            </Node>
         </NodeList>
      </NodeBody>
   </Node>
</PlexilPlan>