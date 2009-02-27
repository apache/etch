@rem  Licensed to the Apache Software Foundation (ASF) under one   *
@rem  or more contributor license agreements.  See the NOTICE file *
@rem  distributed with this work for additional information        *
@rem  regarding copyright ownership.  The ASF licenses this file   *
@rem  to you under the Apache License, Version 2.0 (the            *
@rem  "License"); you may not use this file except in compliance   *
@rem  with the License.  You may obtain a copy of the License at   *
@rem                                                               *
@rem    http://www.apache.org/licenses/LICENSE-2.0                 *
@rem                                                               *
@rem  Unless required by applicable law or agreed to in writing,   *
@rem  software distributed under the License is distributed on an  *
@rem  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
@rem  KIND, either express or implied.  See the License for the    *
@rem  specific language governing permissions and limitations      *
@rem  under the License.                                           *
@set q=-q
@set b=-b java
@set b_xml=-b xml
@set d=-d ..\..\..\target\generated-sources\main\etch\java
@set d_test=-d ..\..\..\target\generated-sources\test\etch\java
@set d_xml=-d ..\..\..\target\generated-sources\main\etch\xml
@set d_xml_test=-d ..\..\..\target\generated-sources\test\etch\xml
@set i=-I .
@set n=-n
@set namespace_path=org\apache\etch\services\router

@set walf=-w all,force

@set testsdir=tests\src\main\etch
@set examplesdir=examples

@set x=%CD%\..\..\..\

@pushd ..\src\main\etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d% %i% -w server       EtchRouter.etch
@call %x%scripts\etch-eclipse.bat %q% %b_xml% %n% %d_xml% %i% -w both,all       EtchRouter.etch
@popd

@pushd ..\src\test\etch
@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d_test% %i% -w client,all       TestApp.etch
@call %x%scripts\etch-eclipse.bat %q% %b_xml% %n% %d_xml_test% %i% -w both,all       TestApp.etch

@call %x%scripts\etch-eclipse.bat %q% %b% %n% %d_test% %i% -w both,all       TestPlugin.etch
@call %x%scripts\etch-eclipse.bat %q% %b_xml% %n% %d_xml_test% %i% -w both,all       TestPlugin.etch

del ..\..\..\target\generated-sources\test\etch\java\%namespace_path%\test\plugin\ImplTestPluginServer.java

mkdir ..\..\..\target\runtime\applications
mkdir ..\..\..\target\runtime\plugins\TestPlugin
copy ..\..\..\target\generated-sources\main\etch\xml\%namespace_path%\EtchRouter.xml ..\..\..\target\runtime\
copy ..\..\..\target\generated-sources\test\etch\xml\%namespace_path%\test\plugin\TestPlugin.xml ..\..\..\target\runtime\plugins\TestPlugin\

@popd


