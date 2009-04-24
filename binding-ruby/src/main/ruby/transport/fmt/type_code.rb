module TypeCode
    
#  /**
#   * A code denoting a null value.
#   */
  NULL = -1;
#  /**
#   * A code denoting a false boolean value.
#   */
  BOOLEAN_FALSE = -2;
#  /**
#   * A code denoting a true boolean value.
#   */
  BOOLEAN_TRUE = -3;
#  /**
#   * A code denoting a signed byte.
#   */
  BYTE1 = -4;
#  /**
#   * A code denoting a single byte signed short.
#   */
  SHORT1 = -5;
#  /**
#   * A code denoting a two byte signed short, lsb first.
#   */
  SHORT2 = -6;
#  /**
#   * A code denoting a single byte signed integer.
#   */
  INT1 = -7;
#  /**
#   * A code denoting a two byte signed integer, lsb first.
#   */
  INT2 = -8;
#  /**
#   * A code denoting a four byte signed integer, lsb first.
#   */
  INT4 = -9;
#  /**
#   * A code denoting a single byte signed long.
#   */
  LONG1 = -10;
#  /**
#   * A code denoting a two byte signed long, lsb first.
#   */
  LONG2 = -11;
#  /**
#   * A code denoting a four byte signed long, lsb first.
#   */
  LONG4 = -12;
#  /**
#   * A code denoting an eight byte signed long, lsb first.
#   */
  LONG8 = -13;
#  /**
#   * A code denoting a four byte ieee floating format number.
#   */
  FLOAT4 = -14;
#  /**
#   * A code denoting an eight byte ieee floating format number.
#   */
  FLOAT8 = -15;
#  /**
#   * A code denoting an array of bytes.
#   */
  BYTES = -16;
#  /**
#   * A code denoting an empty string.
#   */
  EMPTY_STRING = -17;
#  /**
#   * A code denoting a utf-8 encoded string.
#   */
  STRING = -18;
#  /**
#   * A code denoting a sequence of key / value pairs.
#   */
  STRUCT = -19;
#  /**
#   * A code denoting a sequence of values.
#   */
  ARRAY = -20;
#  /**
#   * A code denoting a custom value from a value factory. An integer
#   * value follows which identifies the specific type.
#   */
  CUSTOM = -21;
#  /**
#   * A code denoting no value, which is different than NULL. For
#   * example, an array is a sequence of values (some of which may
#   * be NULL), terminated by a NONE.
#   */
  NONE = -22;

#  ////////////////////
#  // SMALL INTEGERS //
#  ////////////////////
#  
#  /**
#   * Minimum "small" integer.
#   */
  MIN_SMALL_INT = -16;
#  /**
#   * Maximum "small" integer. Small integers are encoded asis
#   * (with embedded type code)
#   */
  MAX_SMALL_INT = 31;
#  /**
#   * Positive sentinal mask.
#   */
  PSMASK = 0x80;
#  /**
#   * Positive sentinal value.
#   */
  PSVALUE = 0x00;
#  /**
#   * Shift for positive data type value.
#   */
  PDTSHIFT = 5;
#  /**
#   * Mask for positive value.
#   */
  PVMASK = 31;
#  /**
#   * Negative sentinal mask.
#   */
  NSMASK = 0xC0;
#  /**
#   * Negative sentinal value.
#   */
  NSVALUE = 0x80;
#  /**
#   * Shift for negative data type value.
#   */
  NDTSHIFT = 4;
#  /**
#   * Mask for negative value.
#   */
  NVMASK = 15;
#  /**
#   * Mask for data type value.
#   */
  DTMASK = 0x03;
#  /**
#   * Data type value for small integer from byte.
#   */
  BYTE_DT = 0;
#  /**
#   * Data type value for small integer from short.
#   */
  SHORT_DT = 1;
#  /**
#   * Data type value for small integer from int.
#   */
  INT_DT = 2;
#  /**
#   * Data type value for small integer from long.
#   */
  LONG_DT = 3;    
end