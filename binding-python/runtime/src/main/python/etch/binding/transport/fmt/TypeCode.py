"""
#  Licensed to the Apache Software Foundation (ASF) under one   *
#  or more contributor license agreements.  See the NOTICE file *
#  distributed with this work for additional information        *
#  regarding copyright ownership.  The ASF licenses this file   *
#  to you under the Apache License, Version 2.0 (the            *
#  "License"); you may not use this file except in compliance   *
#  with the License.  You may obtain a copy of the License at   *
#                                                               *
#    http://www.apache.org/licenses/LICENSE-2.0                 *
#                                                               *
#  Unless required by applicable law or agreed to in writing,   *
#  software distributed under the License is distributed on an  *
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY       *
#  KIND, either express or implied.  See the License for the    *
#  specific language governing permissions and limitations      *
#  under the License. 
"""
from __future__ import absolute_import

class TypeCode:
    """
    Values denoting types of encoded values.
    """
    
    NULL = -128
    """A null value"""
    
    NONE = -127
    """A code denoting no value, which is different from NULL."""
    
    BOOLEAN_FALSE = -126
    """A false boolean value"""
    
    BOOLEAN_TRUE = -125
    """A true boolean value"""
    
    BYTE = -124
    """A signed byte"""
    
    SHORT = -123
    """A single Byte signed short"""
    
    INT = -122
    """A single byte signed integer"""
    
    LONG = -121
    """A single byte signed long, msb first"""
    
    FLOAT = -120
    """A four byte IEEE floating format number"""
    
    DOUBLE = -119
    """A eight byte IEEE floating format number"""
    
    BOOLS = -118
    """An array of bytes"""

    BYTES = -117
    """An array of bytes"""

    SHORTS = -116
    """An array of bytes"""
    
    INTS = -115
    """An array of bytes"""
    
    LONGS = -114
    """An array of bytes"""
    
    FLOATS = -113
    """An array of bytes"""
    
    DOUBLES = -112
    """An array of bytes"""
    
    ARRAY = -111
    """A sequence of values"""
    
    EMPTY_STRING = -110
    """An empty string"""

    STRING = -109
    """A UTF-8 encoded string"""
    
    STRUCT = -108
    """A sequence of key/value pairs"""

    CUSTOM = -107
    """A custom value from a value factory."""
    
    ANY = -106
    """Any value"""
    
    MIN_TINY_INT = -64
    """Minimum small integer"""
    
    MAX_TINY_INT = 127
    """Maximum small integer"""
