@rem Licensed to the Apache Software Foundation (ASF) under one
@rem or more contributor license agreements. See the NOTICE file
@rem distributed with this work for additional information
@rem regarding copyright ownership. The ASF licenses this file
@rem to you under the Apache License, Version 2.0 (the
@rem "License"); you may not use this file except in compliance
@rem with the License. You may obtain a copy of the License at
@rem
@rem   http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing,
@rem software distributed under the License is distributed on an
@rem "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
@rem KIND, either express or implied. See the License for the
@rem specific language governing permissions and limitations
@rem under the License.
@echo OFF
echo postBuild.bat start
echo postBuild.bat copying jenkhash.lib debug symbols to lib directory ...

if exist .\debug\vc80.pdb goto debug
if exist .\release\vc80.pdb goto release
echo no debug symbol file found - assuming C7 embedded symbols
goto done  

:release
xcopy .\release\vc80.pdb ..\lib /y /f /i
del .\release\vc80.pdb 
goto done

:debug
xcopy .\debug\vc80.pdb ..\lib /y /f /i
del .\debug\vc80.pdb

:done
echo postBuild.bat exit

