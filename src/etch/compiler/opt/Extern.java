/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.opt;

import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;
import etch.compiler.ast.Name;
import etch.compiler.ast.Opt;

/**
 * Option which sets the language specific attributes of an extern
 * declaration.
 */
public class Extern extends Opt
{
	/**
	 * Constructs the Extern.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException 
	 */
	public Extern( Name name, Token... args ) throws ParseException
	{
		super( null, name.token.beginLine );
		addType( etch.compiler.ast.Extern.class );
		
		// usage: @Extern(language, name, nameImport, serializer, serializerImport)
		// example: @Extern(java, "java.util.Date", "", "etch.bindings.java.util.DateSerializer", "")
		
		if (args.length != 5)
			throw new ParseException( String.format(
				"usage: @Extern(language, \"xname\", \"xnameImport\", \"serializer\", \"serializerImport\") at line %d",
				name.token.beginLine ) );
		
		language = args[0];
		xname = args[1];
		xnameImport = args[2];
		serializer = args[3];
		serializerImport = args[4];
		
		if (language.kind != EtchGrammarConstants.ID)
			throw new ParseException( String.format(
				"Extern expected language to be <ID> at line %d",
				language.beginLine ) );
		
		if (xname.kind != EtchGrammarConstants.STR)
			throw new ParseException( String.format(
				"Extern expected xname to be <STR> at line %d",
				xname.beginLine ) );
		
		if (xnameImport.kind != EtchGrammarConstants.STR)
			throw new ParseException( String.format(
				"Extern expected xnameImport to be <STR> at line %d",
				xnameImport.beginLine ) );
		
		if (serializer.kind != EtchGrammarConstants.STR)
			throw new ParseException( String.format(
				"Extern expected serializer to be <STR> at line %d",
				serializer.beginLine ) );
		
		if (serializerImport.kind != EtchGrammarConstants.STR)
			throw new ParseException( String.format(
				"Extern expected serializerImport to be <STR> at line %d",
				serializerImport.beginLine ) );
	}
	
	@Override
	public String name()
	{
		return "Extern."+language.image;
	}
	
	/**
	 * The language of this extern opt.
	 */
	public final Token language;
	
	/**
	 * The name of the external class.
	 */
	public final Token xname;
	
	/**
	 * How to make the external class available in the environment.
	 */
	public final Token xnameImport;
	
	/**
	 * The name of the serializer class.
	 */
	public final Token serializer;

	/**
	 * How to make the serializer name available in the environment.
	 */
	public final Token serializerImport;

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@Extern %s %s %s %s %s\n", indent, language, xname, xnameImport, serializer, serializerImport );
	}

	@Override
	public String toString()
	{
		return String.format( "@Extern %s %s %s %s %s", language, xname, xnameImport, serializer, serializerImport );
	}

	public Token getLanguage() {
		return language;
	}

	public Token getSerializer() {
		return serializer;
	}

	public Token getSerializerImport() {
		return serializerImport;
	}

	public Token getXname() {
		return xname;
	}

	public Token getXnameImport() {
		return xnameImport;
	}
}
