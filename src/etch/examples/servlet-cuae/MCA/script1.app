<Application name="script1" trigger="Metreos.Providers.TimerFacility.TimerFire" version="0.8" single="false">
  <!--//// app tree ////-->
  <global name="script1">
    <outline>
      <treenode type="evh" id="633228059323865963" level="1" text="Metreos.Providers.TimerFacility.TimerFire (trigger): OnTimerFire">
        <node type="function" name="OnTimerFire" id="633228059323865960" path="Metreos.StockTools" />
        <node type="event" name="TimerFire" id="633228059323865959" path="Metreos.Providers.TimerFacility.TimerFire" trigger="true" />
        <Properties type="hybrid">
        </Properties>
      </treenode>
    </outline>
    <variables />
  </global>
  <!--//// visible canvases ////-->
  <canvas type="Function" name="OnTimerFire" startnode="633228059323865962" treenode="633228059323865963" appnode="633228059323865960" handlerfor="633228059323865959">
    <node type="Start" id="633228059323865962" name="Start" class="MaxStartNode" group="Metreos.StockTools" path="Metreos.StockTools" x="32" y="32" />
  </canvas>
  <Properties desc="">
  </Properties>
</Application>