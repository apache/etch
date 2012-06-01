/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version
 * 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ETCHTYPECODES_H__
#define __ETCHTYPECODES_H__
#include "common/EtchConfig.h"

/**
 * Values denoting types of encoded values. There are two value ranges:
 * 
 * -128 to -65: codes for defined types.
 * -64 to 127: tiny integer values.
 * 
 * Tiny integers are completely encoded within the type byte.
 */
class EtchTypeCode {
public:
  /**
   * A code denoting a null value.
   */
  static const capu::int8_t Null = -128;

  /**
   * A code denoting no value, which is different than NULL. For
   * example, an array is a sequence of values (some of which may
   * be NULL), terminated by a NONE.
   */
  static const capu::int8_t NONE = -127;

  //////////////////////////////////////////////////
  // scalar values (but, also see tiny int below) //
  //////////////////////////////////////////////////

  /**
   * A code denoting a false boolean value.
   */
  static const capu::int8_t BOOLEAN_FALSE = -126;

  /**
   * A code denoting a true boolean value.
   */
  static const capu::int8_t BOOLEAN_TRUE = -125;

  /**
   * A code denoting a signed byte.
   */
  static const capu::int8_t BYTE = -124;

  /**
   * A code denoting a two byte signed short, msb first.
   */
  static const capu::int8_t SHORT = -123;

  /**
   * A code denoting a four byte signed integer, msb first.
   */
  static const capu::int8_t INT = -122;

  /**
   * A code denoting an eight byte signed long, msb first.
   */
  static const capu::int8_t LONG = -121;

  /**
   * A code denoting a four byte ieee floating format number (single,
   * written as an INT).
   */
  static const capu::int8_t FLOAT = -120;

  /**
   * A code denoting an eight byte ieee floating format number (double,
   * written as a LONG).
   */
  static const capu::int8_t DOUBLE = -119;

  ////////////
  // arrays //
  ////////////

  // BOOLS, SHORTS, INTS, LONGS, FLOATS, and DOUBLES are
  // reserved for future use.

  //	/**
  //	 * A code denoting an array of booleans.
  //	 */
  //	static capu::int8_t BOOLS = -118;

  /**
   * A code denoting an array of bytes.
   */
  static const capu::int8_t BYTES = -117;

  //	/**
  //	 * A code denoting an array of shorts.
  //	 */
  //	static capu::int8_t SHORTS = -116;

  //	/**
  //	 * A code denoting an array of ints.
  //	 */
  //	static capu::int8_t INTS = -115;

  //	/**
  //	 * A code denoting an array of longs.
  //	 */
  //	static capu::int8_t LONGS = -114;

  //	/**
  //	 * A code denoting an array of ieee format singles.
  //	 */
  //	static capu::int8_t FLOATS = -113;

  //	/**
  //	 * A code denoting an array of ieee format doubles.
  //	 */
  //	static capu::int8_t DOUBLES = -112;

  /**
   * A code denoting a sequence of values.
   */
  static const capu::int8_t ARRAY = -111;

  /////////////
  // strings //
  /////////////

  /**
   * A code denoting an empty string.
   */
  static const capu::int8_t EMPTY_STRING = -110;

  /**
   * A code denoting a utf-8 encoded string.
   */
  static const capu::int8_t STRING = -109;

  /////////////////////
  // structured data //
  /////////////////////

  /**
   * A code denoting a custom value (struct, exception, enum, extern)
   * from a value factory. An associated value identifies the specific
   * type. The format on the wire of STRUCT and CUSTOM are the same.
   * Emit CUSTOM instead of STRUCT, and accept STRUCT as if it were
   * CUSTOM.
   */
  static const capu::int8_t STRUCT = -108;

  /**
   * A code denoting a custom value (struct, exception, enum, extern)
   * from a value factory. An associated value identifies the specific
   * type. The format on the wire of STRUCT and CUSTOM are the same.
   * Emit CUSTOM instead of STRUCT, and accept STRUCT as if it were
   * CUSTOM.
   */
  static const capu::int8_t CUSTOM = -107;

  //////////
  // misc //
  //////////

  /**
   * A code denoting that any value is ok (as long as we know how
   * to serialize it). Dynamic methods should be applied to determine
   * the type. This type never appears on the wire.
   */
  static const capu::int8_t ANY = -106;

  ///////////////////
  // tiny integers //
  ///////////////////

  /**
   * Minimum "tiny" integer.
   */
  static const capu::int8_t MIN_TINY_INT = -64;

  /**
   * Maximum "tiny" integer.
   */
  static const capu::int8_t MAX_TINY_INT = 127;
};

#endif /* ETCHTYPECODES_H */

