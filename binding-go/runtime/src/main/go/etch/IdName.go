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
package etch

type IdName struct {
	Id   int32
	Name string
}

func NewIdName(id int32, name string) *IdName {
	return &IdName{id, name}
}

func NewIdNameByName(name string) *IdName {
	return &IdName{hash(name), name}
}


func hash(val string) int32 {
	hash := 5381
	n := len(val)
	for i := 0; i < n; i++ {
		c := int(val[i])
		h6 := hash << 6
		hash = (h6 << 10) + h6 - hash + c
	}
	return int32(hash)

}
