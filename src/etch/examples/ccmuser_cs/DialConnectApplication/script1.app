<Application name="script1" trigger="Metreos.Providers.Http.GotRequest" version="0.8" single="false">
  <!--//// app tree ////-->
  <global name="script1">
    <outline>
      <treenode type="evh" id="633220136509118351" level="1" text="Metreos.Providers.Http.GotRequest (trigger): OnGotRequest">
        <node type="function" name="OnGotRequest" id="633220136509118348" path="Metreos.StockTools" />
        <node type="event" name="GotRequest" id="633220136509118347" path="Metreos.Providers.Http.GotRequest" trigger="true" />
        <Properties type="hybrid">
          <ep name="url" type="literal">/UserConfig/Speeddial</ep>
        </Properties>
      </treenode>
    </outline>
    <variables>
      <treenode text="username" id="633220136509118393" vid="633220136509118392">
        <Properties type="String" initWith="Config.username">username</Properties>
      </treenode>
      <treenode text="password" id="633220136509118395" vid="633220136509118394">
        <Properties type="String" initWith="Config.password">password</Properties>
      </treenode>
    </variables>
  </global>
  <!--//// visible canvases ////-->
  <canvas type="Function" name="OnGotRequest" activetab="true" startnode="633220136509118350" treenode="633220136509118351" appnode="633220136509118348" handlerfor="633220136509118347">
    <node type="Start" id="633220136509118350" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="75" y="341">
      <linkto id="633220136509118353" type="Basic" style="Bezier" ortho="true" />
    </node>
    <node type="Action" id="633220136509118353" name="QueryByDevice" class="MaxActionNode" group="" path="Metreos.Native.CiscoDeviceList" x="253" y="341">
      <linkto id="633220136509118356" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="DeviceName" type="csharp">query["device"]</ap>
        <rd field="ResultData">results</rd>
      </Properties>
    </node>
    <node type="Action" id="633220136509118355" name="SendResponse" class="MaxActionNode" group="" path="Metreos.Providers.Http" x="445" y="532">
      <linkto id="633220136509118359" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="provider" log="On">
        <ap name="remoteHost" type="variable">remoteHost</ap>
        <ap name="responseCode" type="literal">200</ap>
        <ap name="responsePhrase" type="literal">OK</ap>
        <ap name="body" type="literal">&lt;response&gt;Error&lt;/response&gt;</ap>
        <ap name="Content-Type" type="literal">text/xml</ap>
      </Properties>
    </node>
    <node type="Action" id="633220136509118356" name="CustomCode" class="MaxCodeNode" group="Application Components" path="Metreos.StockTools" x="444" y="340">
      <linkto id="633220136509118355" type="Labeled" style="Bezier" ortho="true" label="none" />
      <linkto id="633220136509118360" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties language="csharp">
public static string Execute(Metreos.Types.Http.QueryParamCollection query, DataTable results, ref string phoneIP, LogWriter log)
{
	if(results == null || results.Rows.Count == 0)
	{
		log.Write(TraceLevel.Error, "No devices matched: " + query["device"]);
		return "none";
	}
	else
	{
		phoneIP = results.Rows[0]["IP"] as string;

		if(phoneIP == null || phoneIP == String.Empty)
		{
			log.Write(TraceLevel.Error, "No IP address for device: " + query["device"]);
			return "none";
		}
		else
		{
			return "success";
		}
	}
}
</Properties>
    </node>
    <node type="Action" id="633220136509118359" name="EndScript" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="445" y="677">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
    <node type="Action" id="633220136509118360" name="CreateExecute" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="681" y="339">
      <linkto id="633220136509118362" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="URL1" type="csharp">"Dial:" + query["to"]</ap>
        <rd field="ResultData">execute</rd>
      </Properties>
    </node>
    <node type="Action" id="633220136509118362" name="SendExecute" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="874" y="338">
      <linkto id="633220136509118355" type="Labeled" style="Bezier" ortho="true" label="default" />
      <linkto id="633220136509118363" type="Labeled" style="Bezier" ortho="true" label="Success" />
      <Properties final="false" type="native" log="On">
        <ap name="Message" type="csharp">execute.ToString()</ap>
        <ap name="URL" type="variable">phoneIP</ap>
        <ap name="Username" type="variable">username</ap>
        <ap name="Password" type="variable">password</ap>
      </Properties>
    </node>
    <node type="Action" id="633220136509118363" name="SendResponse" class="MaxActionNode" group="" path="Metreos.Providers.Http" x="1048" y="338">
      <linkto id="633220136509118368" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="provider" log="On">
        <ap name="remoteHost" type="variable">remoteHost</ap>
        <ap name="responseCode" type="literal">200</ap>
        <ap name="responsePhrase" type="literal">OK</ap>
        <ap name="body" type="literal">&lt;response&gt;Success&lt;/response&gt;</ap>
        <ap name="Content-Type" type="literal">text/xml</ap>
      </Properties>
    </node>
    <node type="Comment" id="633220136509118365" text="Respond with error." class="MaxCommentNode" group="Application Components" path="Metreos.StockTools" x="292" y="525" />
    <node type="Comment" id="633220136509118366" text="Respond with success." class="MaxCommentNode" group="Application Components" path="Metreos.StockTools" x="989" y="402" />
    <node type="Action" id="633220136509118368" name="EndScript" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="1224" y="340">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
    <node type="Variable" id="633220136509118352" name="query" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.Http.QueryParamCollection" initWith="query" refType="reference" name="Metreos.Providers.Http.GotRequest">query</Properties>
    </node>
    <node type="Variable" id="633220136509118354" name="results" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.DataTable" refType="reference">results</Properties>
    </node>
    <node type="Variable" id="633220136509118357" name="phoneIP" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" refType="reference">phoneIP</Properties>
    </node>
    <node type="Variable" id="633220136509118358" name="remoteHost" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" initWith="remoteHost" refType="reference">remoteHost</Properties>
    </node>
    <node type="Variable" id="633220136509118361" name="execute" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.CiscoIpPhone.Execute" refType="reference">execute</Properties>
    </node>
  </canvas>
  <Properties desc="">
  </Properties>
</Application>