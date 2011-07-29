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

# General

Etch documentation is based on docbook 4.5. If you would
like to generate the documentation, Linux is the preferred
way how to do it at the moment. The Windows documentation
process is currently under development and will be available
in the near future.

# Linux setup

If you would like to generate the documentation, you need the
following components.

- sudo apt-get install docbook5-xml
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

# Windows setup

If you would like to generate the documentation, you need the
following components.

- cygwin installation and the following packages
-- xslt parser
-- docbook for version 4.5
- Apache FOP http://xmlgraphics.apache.org/fop/download.html#binary

- Change still the following files:
-- Makefile; set the FOP variable to the fop binary etc. FOP=C:/etch/external/fop/1.0/fop.bat
-- Set the correct docbook import file in every xsl file (e.g. libs/etch-pdf.xsl):
   <xsl:import href="/usr/share/sgml/docbook/xsl-ns-stylesheets/fo/docbook.xsl"/>

# Links

Getting further information about docbook, see here
- http://docbook.org/tdg/en/html/docbook.html
- http://www.sagehill.net/docbookxsl/preface.html#WhatIsDocbook
- http://docbook.sourceforge.net/release/xsl/current/doc/html/
- http://docbook.sourceforge.net/release/xsl/current/doc/fo/

