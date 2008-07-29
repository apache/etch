/* $Id$
 *
 * Created by sccomer on Mar 21, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.xml;

/**
 * The tag names, etc., used by the demonstration xml message formatter.
 */
public interface XmlTags
{
	// tags
	
	/**
	 * The tag of a message. A message tag has the attributes and
	 * elements of a struct element.
	 */
	public String MESSAGE_TAG = "m";

	/**
	 * The tag of an element. An element is an item nested within an
	 * message, struct, array, or custom type. An element has a data
	 * type attribute. If the element is contained within a message,
	 * struct, or custom element, it has a key attribute. If the
	 * element is a struct or custom element, it has a struct type
	 * attribute.
	 */
	public String ELEMENT_TAG = "e";
	
	// attributes

	/**
	 * The integer type of the message.
	 */
	public String STRUCT_TYPE_ATTR = "t";

	/**
	 * The type of the element.
	 */
	public String ELEMENT_TYPE_ATTR = "d";

	/**
	 * The integer key of an element within a message or struct element.
	 */
	public String KEY_ATTR = "k";
	
	// types

	/**
	 * The element represents the null value.
	 */
	public String NULL_TYPE = "n";

	/**
	 * The element represents a boolean value, with cdata true or false.
	 */
	public String BOOLEAN_TYPE = "b";

	/**
	 * The element represents an integral type (32-bit signed). Cdata is
	 * the string format of the value.
	 */
	public String INTEGER_TYPE = "i";

	/**
	 * The element represents an integral type (64-bit signed). Cdata is
	 * the string format of the value.
	 */
	public String LONG_TYPE = "l";

	/**
	 * The element represents a floating type (32-bit signed). Cdata is
	 * the string format of the value.
	 */
	public String FLOAT_TYPE = "f";

	/**
	 * The element represents an floating type (64-bit signed). Cdata is
	 * the string format of the value.
	 */
	public String DOUBLE_TYPE = "d";

	/**
	 * The element represents a byte array type. Cdata is a space-separated
	 * list of hex values (00-ff).
	 */
	public String BYTES_TYPE = "y";

	/**
	 * The element represents a string type. Cdata is the character data
	 * of the string.
	 */
	public String STRING_TYPE = "s";
	
	/**
	 * The element represents a struct type. A struct has child elements
	 * with key attributes which represent the key / value pairs of the
	 * struct. The keys are integers.
	 */
	public String STRUCT_TYPE = "x";

	/**
	 * The element represents an array type. An array has an ordered
	 * sequence of child elements.
	 */
	public String ARRAY_TYPE = "a";

	/**
	 * The element represents a custom data type. It has a code which
	 * identifies the type and has one or more child elements which
	 * represent the internal state of the data type.
	 */
	public String CUSTOM_TYPE = "c";
}
