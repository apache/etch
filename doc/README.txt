# 
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements. See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership. The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License. You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied. See the License for the
# specific language governing permissions and limitations
# under the License.
#

Currently only Linux is supported to generate the documentation
based on docbook. For doing it with windows the Makefile has to
be adjusted.

If you would like to generate the documentation, you need the
following components.
- sudo apt-get install docbook
- sudo apt-get install docbook-xsl
- sudo apt-get install docbook-xsl-doc-pdf
- sudo apt-get install xsltproc
- sudo apt-get install fop 

Docbook reference can be installed via
- sudo apt-get install docbook-defguide

Debian does not support the fop hyphenation feature out of
the box. If you get some fop hyphenation errors while you
generate docbook pdf, see the following links for more
information.
- http://offo.sourceforge.net/hyphenation/fop-stable/installation.html
- http://xmlgraphics.apache.org/fop/1.0/hyphenation.html 

Getting further information, see here
- http://docbook.org/tdg/en/html/docbook.html
- http://www.sagehill.net/docbookxsl/preface.html#WhatIsDocbook
- http://docbook.sourceforge.net/release/xsl/current/doc/html/
- http://docbook.sourceforge.net/release/xsl/current/doc/fo/

