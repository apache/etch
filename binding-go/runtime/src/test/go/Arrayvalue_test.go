/*
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
package goetchtest

import . "etch"
import "fmt"

type dummyStruct struct {
	x int
}

func TestArrayValue() {

	Log("*************************************************\n")
	ar := []byte{1,2,3}
	av := NewArrayValueWith(ar, 42, nil)
	Log(fmt.Sprint(av) + "\n")
	Log(fmt.Sprint(av.ArrayVal) + "\n")

	fav := FromArrayValue(av)
	Log(fmt.Sprint(fav) + "\n")

	switch fav.(type) {
	case []byte: Log("BYTE ARRAY\n")
        default: Error("Wrong type of array \n")
	}


	Log("*************************************************\n")

	iarray := make([]interface{}, 3)
	nav := NewArrayValueWith(iarray, 42, nil)
	nav.Set(0, int(1))
	nav.Set(1, int(2))
	nav.Set(2, int(3))
	nfav := FromArrayValue(nav)
	Log(fmt.Sprint(nfav) + "\n")	

	Log(fmt.Sprint(nfav.([]interface{})[1]) + "\n")	

	switch nfav.(type) {
	case []interface{}: Log("INTERFACE ARRAY\n")
        default: Error("Wrong type of array \n")
	}
	
	Log("*************************************************\n")
	iarray2 := make([]*dummyStruct, 3)

	nav2 := NewArrayValueWith(iarray2, 42, nil)
	nav2.Set(0, &dummyStruct{42})
	nav2.Set(1, &dummyStruct{43})
	nav2.Set(2, &dummyStruct{44})

	nfav2 := FromArrayValue(nav2)
	Log(fmt.Sprint(nav2) + "\n")	
	Log(fmt.Sprint(nav2.Get(1)) + "\n")	
	Log(fmt.Sprint(nfav2) + "\n")	

	switch nfav2.(type) {
	case []*dummyStruct: Log("DUMMY ARRAY\n")
        default: Error("Wrong type of array \n")
	}
	Log("*************************************************\n")
	Log("Arrayvalue test done\n")
}