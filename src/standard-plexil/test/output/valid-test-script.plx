<PLEXILScript>
  <InitialState>
    <State name="time" type="real">
      <Value>0.0</Value>
    </State>
    <State name="target_in_view" type="bool">
      <Value>false</Value>
    </State>
    <State name="temperature" type="real">
      <Value>0.0</Value>
    </State>
  </InitialState>
  <Script>
    <State name="time" type="real">
      <Value>1.0</Value>
    </State>
    <State name="time" type="real">
      <Value>2.0</Value>
    </State>
    <CommandAck name="take_navcam" type="string">
      <Result>COMMAND_SUCCESS</Result>
    </CommandAck>
    <CommandAck name="rover_stop" type="string">
      <Result>COMMAND_SUCCESS</Result>
    </CommandAck>
    <State name="time" type="real">
      <Value>3.0</Value>
    </State>
    <CommandAck name="turn_on_heater" type="string">
      <Param type="real">10.0</Param>
      <Result>COMMAND_SUCCESS</Result>
    </CommandAck>
    <State name="time" type="real">
      <Value>10.0</Value>
    </State>
    <State name="target_in_view" type="bool">
      <Value>true</Value>
    </State>
    <CommandAck name="rover_drive" type="string">
      <Param type="real">10.0</Param>
      <Param type="real">20.0</Param>
      <Result>COMMAND_SUCCESS</Result>
    </CommandAck>
    <State name="time" type="real">
      <Value>11.0</Value>
    </State>
  </Script>
</PLEXILScript>