/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util;

/**
 * A pretty cool hash function which has few collisions.
 */
public class Hash
{
    /**
     * Computes the hash value of the name to be used as the id when
     * constructing an IdName.
     * @param name the name of the type or field.
     * @return a hash of name in the unicode character encoding which is
     * very likely to be unique over reasonable name spaces. Collisions
     * should be very unlikely as they will force the user to change the
     * name.
     */
    public static int hash( String name )
    {
    	// combination of djb2 and sdbm hash algorithm:
        int hash = 5381;
        int n = name.length();
        for (int i = 0; i < n; i++)
        {
        	char c = name.charAt( i );
        	int h6 = hash << 6;
        	hash = (h6 << 10) + h6 - hash + c;
        }
        return hash;
    }
}
