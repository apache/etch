<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <!-- This file is a customization layer for HTML only -->
  
  <!-- Imports -->
  <xsl:import href="/usr/share/xml/docbook/stylesheet/nwalsh/xhtml/chunk.xsl"/>

  <!-- requires DocBook XSL 1.69.1a -->
  <xsl:param name="generate.revhistory.link" select="1"/>
  <xsl:param name="toc.max.depth" select="2"/>
  <xsl:param name="chunker.output.indent" select="'yes'"/>
  <xsl:param name="body.font.master" select="10"/>
  <xsl:param name="html.stylesheet" select="'../../css/etch-book.css'"/>
  <xsl:param name="shade.verbatim" select="0"/>
  <xsl:param name="draft.mode" select="'yes'"/>
  <xsl:param name="chunk.first.sections" select="'1'"/>


  <xsl:param name="use.id.as.filename" select="1"/>

</xsl:stylesheet>
