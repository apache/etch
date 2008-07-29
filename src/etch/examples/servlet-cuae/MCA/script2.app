<Application name="script2" trigger="Metreos.Providers.Http.GotRequest" version="0.8" single="false">
  <!--//// app tree ////-->
  <global name="script2">
    <outline>
      <treenode type="evh" id="633228059323866026" level="1" text="Metreos.Providers.Http.GotRequest (trigger): OnGotRequest">
        <node type="function" name="OnGotRequest" id="633228059323866023" path="Metreos.StockTools" />
        <node type="event" name="GotRequest" id="633228059323866022" path="Metreos.Providers.Http.GotRequest" trigger="true" />
        <Properties type="hybrid">
          <ep name="url" type="literal">/ChoiceMade</ep>
        </Properties>
      </treenode>
    </outline>
    <variables />
  </global>
  <!--//// visible canvases ////-->
  <canvas type="Function" name="OnGotRequest" activetab="true" startnode="633228059323866025" treenode="633228059323866026" appnode="633228059323866023" handlerfor="633228059323866022">
    <node type="Start" id="633228059323866025" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="158" y="338">
      <linkto id="633228059323866027" type="Basic" style="Bezier" ortho="true" />
    </node>
    <node type="Action" id="633228059323866027" name="SelectionMade" class="MaxActionNode" group="" path="Cisco.Providers.TwoWay" x="350" y="337">
      <linkto id="633228059323866031" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="provider" log="On">
        <ap name="Selection" type="csharp">query["selection"]</ap>
      </Properties>
    </node>
    <node type="Action" id="633228059323866031" name="CreateText" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="504" y="339">
      <linkto id="633228059323866034" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Text" type="literal">Thanks for playing!</ap>
        <rd field="ResultData">text</rd>
      </Properties>
    </node>
    <node type="Action" id="633228059323866033" name="EndScript" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="804" y="344">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
    <node type="Action" id="633228059323866034" name="SendResponse" class="MaxActionNode" group="" path="Metreos.Providers.Http" x="662" y="342">
      <linkto id="633228059323866033" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="provider" log="On">
        <ap name="remoteHost" type="variable">remoteHost</ap>
        <ap name="responseCode" type="literal">200</ap>
        <ap name="body" type="csharp">text.ToString()</ap>
        <ap name="Content-Type" type="literal">text/xml</ap>
      </Properties>
    </node>
    <node type="Variable" id="633228059323866028" name="query" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.Http.QueryParamCollection" initWith="query" refType="reference">query</Properties>
    </node>
    <node type="Variable" id="633228059323866030" name="remoteHost" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" initWith="remoteHost" refType="reference">remoteHost</Properties>
    </node>
    <node type="Variable" id="633228059323866032" name="text" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.CiscoIpPhone.Text" refType="reference">text</Properties>
    </node>
  </canvas>
  <Properties desc="">
  </Properties>
</Application>