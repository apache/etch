<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- 
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements. See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership. The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License. You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied. See the License for the
 specific language governing permissions and limitations
 under the License.
-->

<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:fo="http://www.w3.org/1999/XSL/Format">

<!-- Linux -->
<xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl"/>
<!-- Windows -->
<!-- <xsl:import href="/usr/share/sgml/docbook/xsl-ns-stylesheets/fo/docbook.xsl"/> -->

<!-- ***************************************************  -->
<!-- ****************  Page Settings  ******************  -->
<!-- ***************************************************  -->

<!-- Define the page to A4 -->
<xsl:param name="paper.type">A4</xsl:param>

<!-- Define Margins -->
<xsl:param name="page.margin.outer">
  <xsl:choose>
    <xsl:when test="$double.sided != 0">0.75in</xsl:when>
    <xsl:otherwise>1in</xsl:otherwise>
  </xsl:choose>
</xsl:param>
<xsl:param name="page.margin.inner">
  <xsl:choose>
    <xsl:when test="$double.sided != 0">1in</xsl:when>
    <xsl:otherwise>1in</xsl:otherwise>
  </xsl:choose>
</xsl:param>

<!-- ***************************************************  -->
<!-- ****************  Font Settings  ******************  -->
<!-- ***************************************************  -->

<!-- Use a serif font -->
<xsl:param name="title.font.family" select="'serif'"/>
<xsl:param name="body.font.family" select="'serif'"/>
<xsl:param name="symbol.font.family" select="'Symbol'"/>
<xsl:param name="dingbat.font.family">
  <xsl:value-of select="$body.font.family"/>
</xsl:param>
<xsl:param name="body.font.master">11</xsl:param>
<xsl:param name="body.font.size">
  <xsl:value-of select="$body.font.master"/><xsl:text>pt</xsl:text>
</xsl:param>

<xsl:param name="monospace.font.family" select="'monospace'"></xsl:param>
<xsl:attribute-set name="monospace.properties">
  <xsl:attribute name="font-size">80%</xsl:attribute>
</xsl:attribute-set>

<xsl:param name="body.start.indent" select="'0pc'"/>


<!-- ***************************************************  -->
<!-- ***************  Admonitions  *********************  -->
<!-- ***************************************************  -->

<!-- turn on the icon graphics for admonition elements -->
<!--<xsl:param name="admon.graphics" select="1"></xsl:param>-->


<!-- ***************************************************  -->
<!-- *****************  XSL-FPO Settings  **************  -->
<!-- ***************************************************  -->

<xsl:param name="fop1.extensions" select="1"/>
<xsl:param name="fop.extensions" select="'yes'"/>
<xsl:template match="*" mode="fop.outline"/> 

<!-- hyphenate not nedded at the moment, so disable it -->
<xsl:param name="hyphenate" select="'false'"/>



<!-- ***************************************************  -->
<!-- *************  Development Settings  **************  -->
<!-- ***************************************************  -->

<!-- select draft mode, either yes or no -->
<xsl:param name="draft.mode" select="'yes'"/>

</xsl:stylesheet>

