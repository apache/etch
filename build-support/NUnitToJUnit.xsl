<?xml version="1.0" encoding="utf-8"?>
 <!--
 * Licensed to the Apache Software Foundation (ASF) under one   *
 * or more contributor license agreements.  See the NOTICE file *
 * distributed with this work for additional information        *
 * regarding copyright ownership.  The ASF licenses this file   *
 * to you under the Apache License, Version 2.0 (the            *
 * "License"); you may not use this file except in compliance   *
 * with the License.  You may obtain a copy of the License at   *
 *                                                              *
 *   http://www.apache.org/licenses/LICENSE-2.0                 *
 *                                                              *
 * Unless required by applicable law or agreed to in writing,   *
 * software distributed under the License is distributed on an  *
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
 * KIND, either express or implied.  See the License for the    *
 * specific language governing permissions and limitations      *
 * under the License.                                           *
 -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="xml" indent="yes" encoding="utf-8" version="1.0" cdata-section-elements="failure" />

    <xsl:template match="test-results/test-suite">

        <xsl:variable name="testcases" select="//test-suite[./results/test-case]"/>
        <!--
        <xsl:variable name="asmClass">
            <xsl:choose>
                <xsl:when test="$testcases"><xsl:value-of select="$testcases[1]/../../@name"/></xsl:when>
                <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        -->
        <testsuite name="NUnitTests" time="{@time}" tests="{count($testcases//test-case)}"
            errors="" failures="{count($testcases//test-case/failure)}">

            <xsl:for-each select="$testcases">

                <xsl:variable name="suite" select="."/>
                <xsl:variable name="generalfailure" select="./failure"/>
                <xsl:for-each select=".//test-case">
                
                <xsl:variable name="testClass" select="concat(substring-before(./@name, ./../../@name), ./../../@name)" />
                <xsl:variable name="testName" select="substring-after(./@name, concat($testClass,'.'))" />

                <testcase classname="{$testClass}" name="{$testName}" time="{./@time}" >
                
                 
                <xsl:if test="./failure">
                <xsl:variable name="failstack" select="count(./failure/stack-trace/*) + count(./failure/stack-trace/text())"/>
                <failure>
                    <xsl:choose>
                        <xsl:when test="$failstack &gt; 0 or not($generalfailure)">
MESSAGE: <xsl:value-of select="./failure/message"/>
+++++++++++++++++++
STACK TRACE: <xsl:value-of select="./failure/stack-trace"/>
                        </xsl:when>
                        <xsl:otherwise>
MESSAGE: <xsl:value-of select="$generalfailure/message"/>
+++++++++++++++++++
STACK TRACE: <xsl:value-of select="$generalfailure/stack-trace"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </failure>
                </xsl:if>
                </testcase>
                </xsl:for-each>
            </xsl:for-each>

        </testsuite>

    </xsl:template>

</xsl:stylesheet>
