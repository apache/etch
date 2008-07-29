<Application name="PushToPhone" trigger="Cisco.Providers.TwoWay.PushMessage" version="0.8" single="false">
  <!--//// app tree ////-->
  <global name="PushToPhone">
    <outline>
      <treenode type="evh" id="633228059323865973" level="1" text="Cisco.Providers.TwoWay.PushMessage (trigger): OnPushMessage">
        <node type="function" name="OnPushMessage" id="633228059323865970" path="Metreos.StockTools" />
        <node type="event" name="PushMessage" id="633228059323865969" path="Cisco.Providers.TwoWay.PushMessage" trigger="true" />
        <Properties type="triggering">
        </Properties>
      </treenode>
      <treenode type="evh" id="633228506200261062" level="2" text="Metreos.Providers.Http.GotRequest: OnGotRequest">
        <node type="function" name="OnGotRequest" id="633228506200261059" path="Metreos.StockTools" />
        <node type="event" name="GotRequest" id="633228506200261058" path="Metreos.Providers.Http.GotRequest" />
        <references />
        <Properties type="hybrid">
          <ep name="url" type="literal">/PushToPhone</ep>
        </Properties>
      </treenode>
      <treenode type="evh" id="633228506200261091" level="2" text="Metreos.Providers.Http.SessionExpired: OnSessionExpired">
        <node type="function" name="OnSessionExpired" id="633228506200261088" path="Metreos.StockTools" />
        <node type="event" name="SessionExpired" id="633228506200261087" path="Metreos.Providers.Http.SessionExpired" />
        <references />
        <Properties type="nonTriggering">
        </Properties>
      </treenode>
    </outline>
    <variables>
      <treenode text="phoneIP" id="633228506200261135" vid="633228059323865990">
        <Properties type="String" initWith="Config.PhoneIP">phoneIP</Properties>
      </treenode>
      <treenode text="phoneUser" id="633228506200261137" vid="633228059323865992">
        <Properties type="String" initWith="Config.PhoneUser">phoneUser</Properties>
      </treenode>
      <treenode text="phonePass" id="633228506200261139" vid="633228059323865994">
        <Properties type="String" initWith="Config.PhonePass">phonePass</Properties>
      </treenode>
      <treenode text="cuaeIP" id="633228506200261141" vid="633228059323866016">
        <Properties type="String" initWith="Config.AppServerIP">cuaeIP</Properties>
      </treenode>
      <treenode text="g_msg" id="633228506200261143" vid="633228506200261093">
        <Properties type="String">g_msg</Properties>
      </treenode>
    </variables>
  </global>
  <!--//// visible canvases ////-->
  <canvas type="Function" name="OnPushMessage" activetab="true" startnode="633228059323865972" treenode="633228059323865973" appnode="633228059323865970" handlerfor="633228059323865969">
    <node type="Start" id="633228059323865972" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="236" y="407">
      <linkto id="633228506200261095" type="Basic" style="Bezier" ortho="true" />
    </node>
    <node type="Action" id="633228506200261056" name="SendEvent" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="549" y="399">
      <linkto id="633228506200261057" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="appControl" log="On">
        <ap name="Host" type="literal">anotherscript</ap>
        <ap name="Url" type="literal">/fakehttp</ap>
        <ap name="parentRoutingGuid" type="variable">routingGuid</ap>
        <ap name="EventName" type="literal">Metreos.Providers.Http.GotRequest</ap>
      </Properties>
    </node>
    <node type="Action" id="633228506200261057" name="EndFunction" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="753" y="397">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
    <node type="Action" id="633228506200261095" name="Assign" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="339" y="398">
      <linkto id="633228506200261056" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Value" type="variable">msg</ap>
        <rd field="ResultData">g_msg</rd>
      </Properties>
    </node>
    <node type="Comment" id="633228506200261096" text="Workaround for odd deadlock issue with SendExecute... we have to trigger a seperate script to push the 'Execute' command to the phone.&#xD;&#xA;The other script points the phone to come back to the secondary HTTP request handler in this script." class="MaxCommentNode" group="Application Components" path="Metreos.StockTools" x="252" y="296" />
    <node type="Variable" id="633228059323865975" name="msg" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" initWith="message" refType="reference" name="Cisco.Providers.TwoWay.PushMessage">msg</Properties>
    </node>
    <node type="Variable" id="633228059323865976" name="text" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.CiscoIpPhone.Text" refType="reference">text</Properties>
    </node>
    <node type="Variable" id="633228506200261162" name="routingGuid" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" initWith="RoutingGuid" refType="reference">routingGuid</Properties>
    </node>
  </canvas>
  <canvas type="Function" name="OnGotRequest" startnode="633228506200261061" treenode="633228506200261062" appnode="633228506200261059" handlerfor="633228506200261058">
    <node type="Start" id="633228506200261061" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="186" y="319">
      <linkto id="633228506200261075" type="Basic" style="Bezier" ortho="true" />
    </node>
    <node type="Action" id="633228506200261075" name="CreateText" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="325.97525" y="316">
      <linkto id="633228506200261078" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Text" type="variable">g_msg</ap>
        <rd field="ResultData">text</rd>
      </Properties>
    </node>
    <node type="Action" id="633228506200261077" name="EndScript" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="922.9752" y="317">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
    <node type="Action" id="633228506200261078" name="AddSoftKeyItem" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="460.97525" y="316">
      <linkto id="633228506200261079" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Name" type="literal">Water</ap>
        <ap name="Position" type="literal">1</ap>
        <ap name="URL" type="csharp">"http://" + cuaeIP + ":8000/ChoiceMade?selection=Water"</ap>
        <rd field="ResultData">text</rd>
      </Properties>
    </node>
    <node type="Action" id="633228506200261079" name="AddSoftKeyItem" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="581.9752" y="314">
      <linkto id="633228506200261080" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Name" type="literal">Coke</ap>
        <ap name="Position" type="literal">2</ap>
        <ap name="URL" type="csharp">"http://" + cuaeIP + ":8000/ChoiceMade?selection=Coke"</ap>
        <rd field="ResultData">text</rd>
      </Properties>
    </node>
    <node type="Action" id="633228506200261080" name="AddSoftKeyItem" class="MaxActionNode" group="" path="Metreos.Native.CiscoIpPhone" x="695.9752" y="315">
      <linkto id="633228506200261097" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="native" log="On">
        <ap name="Name" type="literal">HotDog</ap>
        <ap name="Position" type="literal">3</ap>
        <ap name="URL" type="csharp">"http://" + cuaeIP + ":8000/ChoiceMade?selection=HotDog"</ap>
        <rd field="ResultData">text</rd>
      </Properties>
    </node>
    <node type="Action" id="633228506200261097" name="SendResponse" class="MaxActionNode" group="" path="Metreos.Providers.Http" x="817" y="318">
      <linkto id="633228506200261077" type="Labeled" style="Bezier" ortho="true" label="default" />
      <Properties final="false" type="provider" log="On">
        <ap name="remoteHost" type="variable">remoteHost</ap>
        <ap name="responseCode" type="literal">200</ap>
        <ap name="responsePhrase" type="literal">OK</ap>
        <ap name="body" type="csharp">text.ToString()</ap>
        <ap name="Content-Type" type="literal">text/xml</ap>
      </Properties>
    </node>
    <node type="Variable" id="633228506200261098" name="remoteHost" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="String" initWith="remoteHost" refType="reference">remoteHost</Properties>
    </node>
    <node type="Variable" id="633228506200261099" name="text" class="MaxRecumbentVariableNode" group="Application Components" path="Metreos.StockTools" x="0" y="0">
      <Properties type="Metreos.Types.CiscoIpPhone.Text" initWith="" refType="reference">text</Properties>
    </node>
  </canvas>
  <canvas type="Function" name="OnSessionExpired" startnode="633228506200261090" treenode="633228506200261091" appnode="633228506200261088" handlerfor="633228506200261087">
    <node type="Start" id="633228506200261090" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="32" y="32">
      <linkto id="633228506200261092" type="Basic" style="Bezier" ortho="true" />
    </node>
    <node type="Action" id="633228506200261092" name="EndScript" class="MaxActionNode" group="" path="Metreos.ApplicationControl" x="565" y="194">
      <Properties final="true" type="appControl" log="On">
      </Properties>
    </node>
  </canvas>
  <Properties desc="">
  </Properties>
</Application>