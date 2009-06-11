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
@set b=-b xml
@set d=-d ../../../target/generated-sources/main/etch/xml
@set i=-I .
@set n=-n

@set walf=-w all,force

@set testsdir=tests\src\main\etch
@set examplesdir=examples

@set x=%CD%\
@pushd %testsdir%

@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Async.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Bar.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Baz.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        BigIdl.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Closing.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Cuae.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Foo.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Inheritance.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i% %walf% Test1.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Test2.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Test3.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Test4.etch
@rem *** Test5.etch is a negative test ***
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Test6.etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i% %walf% TestIncludes.etch
@rem *** TestReservedWords.etch is a negative test ***
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Types.etch

@popd
@pushd %examplesdir%

@pushd chat\src\main\etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Chat.etch
@popd

@pushd distmap\src\main\etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        DistributedHashTable.etch
@popd

@pushd example\src\main\etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Example.etch
@popd

@pushd perf\src\main\etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        Perf.etch
@popd

@popd

@pushd interoptester\example\src\main\etch
@call "%x%scripts\etch-eclipse.bat" %q% %b% %n% %d% %i%        IOT.etch
@popd
