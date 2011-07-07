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

# make
MAKE=make

# mkdir
MKDIR=mkdir

# rm
RM=rm

# cp
CP=cp

# XSL Processors
XSLTPROC=/usr/bin/xsltproc

# Apache FOP
# Windows
#FOP=C:/etch/external/fop/1.0/fop.bat
# Linux
FOP=/usr/bin/fop

# IETF language tags
# Examples de for german
LANG=C

# Stylesheets
ETCH_XSL_HTML_CHUNK=libs/etch-html-chunk-cust.xsl
ETCH_XSL_PDF=libs/etch-pdf.xsl

# Build folder
BASE=build

.PHONY:


all: html pdf 


clean:
	$(RM) -rf $(BASE)

html: html-style
	if [ $(LANG) != "C" ]; then \
		@echo "documents have to translated"; \
	fi
	if find docs/$(LANG) -name "*.xml"; then \
		$(XSLTPROC) --xinclude -o $(BASE)/html/$(LANG)/index.html $(ETCH_XSL_HTML_CHUNK) docs/$(LANG)/main.xml; \
	fi

html-style:
	$(MKDIR) -p $(BASE)/html/css
	$(MKDIR) -p $(BASE)/html/images
	$(CP) libs/*.css $(BASE)/html/css


pdf:
	if [ $(LANG) != "C" ]; then \
		@echo "documents have to translated"; \
	fi
	if find docs/$(LANG) -name "*.xml"; then \
		$(XSLTPROC) --xinclude -o $(BASE)/pdf/$(LANG)/main.fo $(ETCH_XSL_PDF) docs/$(LANG)/main.xml; \
		$(FOP) -v -d -fo $(BASE)/pdf/$(LANG)/main.fo -pdf $(BASE)/pdf/$(LANG)/etch-manual.pdf; \
	fi

