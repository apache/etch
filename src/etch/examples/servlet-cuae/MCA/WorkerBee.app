<Application name="WorkerBee" trigger="Metreos.Providers.Http.GotRequest" version="0.8" single="false">
  <!--//// app tree ////-->
  <global name="WorkerBee">
    <outline>
      <treenode type="evh" id="633228506200260934" level="1" text="Metreos.Providers.Http.GotRequest (trigger): OnGotRequest">
        <node type="function" name="OnGotRequest" id="633228506200260931" path="Metreos.StockTools" />
        <node type="event" name="GotRequest" id="633228506200260930" path="Metreos.Providers.Http.GotRequest" trigger="true" />
        <Properties type="hybrid">
          <ep name="host" type="literal">anotherscript</ep>
          <ep name="url" type="literal">/fakehttp</ep>
        </Properties>
      </treenode>
    </outline>
    <variables>
      <treenode text="phoneUser" id="633228506200261107" vid="633228506200260935">
        <Properties type="String" initWith="Config.PhoneUser">phoneUser</Properties>
      </treenode>
      <treenode text="phoneIP" id="633228506200261109" vid="633228506200260937">
        <Properties type="String" initWith="Config.PhoneIP">phoneIP</Properties>
      </treenode>
      <treenode text="phonePass" id="633228506200261111" vid="633228506200260939">
        <Properties type="String" initWith="Config.PhonePass">phonePass</Properties>
      </treenode>
      <treenode text="cuaeIP" id="633228506200261113" vid="633228506200260941">
        <Properties type="String" initWith="Config.AppServerIP">cuaeIP</Properties>
      </treenode>
    </variables>
  </global>
  <!--//// visible canvases ////-->
  <canvas type="Function" name="OnGotRequest" activetab="true" startnode="633228506200260933" treenode="633228506200260934" appnode="633228506200260931" handlerfor="633228506200260930">
    <node type="Start" id="633228506200260933" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="174" y="321">
      <linkto id="633228506200261030" type="Basic" style="Bezier" ortho="true" />
    </node>
    <node type="Action" id="633228506200261020" name="SendExecute" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="599.9752" y="323">
      <linkto id="633228506200261031" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Message" type="csharp">execute.ToString()</ap>
        <ap name="URL" type="variable">phoneIP</ap>
        <ap name="Username" type="variable">phoneUser</ap>
        <ap name="Password" type="variable">phonePass</ap>
      </Properties>
    </node>
    <node type="Action" id="633228506200261030" name="CreateExecute" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="421" y="323">
      <linkto id="633228506200261020" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="URL1" type="csharp">"http://" + cuaeIP + ":8000/PushToPhone?metreosSessionId=" + parentRoutingGuid</ap>
        <rd field="ResultData">execute</rd>
      </Properties>
    </node>
    <node type="Action" id="633228506200261031" name="EndScript" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="803" y="325">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
    <node type="Variable" id="633228506200261029" name="parentRoutingGuid" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" initWith="parentRoutingGuid" refType="reference">parentRoutingGuid</Properties>
    </node>
    <node type="Variable" id="633228506200261032" name="execute" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.CiscoIpPhone.Execute" initWith="" refType="reference">execute</Properties>
    </node>
  </canvas>
  <Properties desc="">
  </Properties>
</Application>