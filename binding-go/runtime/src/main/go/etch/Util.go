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

import "time"
import "reflect"
import "strings" 

func WaitOnChannel(value interface{}, timeout int, channel chan interface{}) interface{} {
	timeoutchan := make(chan bool, 1)
	go func() {
		time.Sleep(1e6 * int64(timeout)) // one second
		timeoutchan <- true
	}()
	var channelValue interface{} = nil
	if value == nil {
		select {
		case channelValue = <-channel:
			return channelValue
		case <-timeoutchan:
			return nil
		}
	} else { //wait for a specific value 
		for channelValue != value {
			select {
			case channelValue = <-channel:
			case <-timeoutchan:
				return nil
			}
		}
		return channelValue
	}
	return nil
}

func EtchRecoverAll() {
	recover()
}


func ToAssignableValue(reference reflect.Value, val interface{})  interface{} {
	value := reflect.ValueOf(val)
	
	if ! strings.HasPrefix(value.Type().String(),"int") {
		return val
	}

	valueInt64 := value.Int()

	switch reference.Interface().(type) {
	case int8:
		return Toint8(valueInt64)
	case int16:
		return Toint16(valueInt64)
	case int32:
		return Toint32(valueInt64)
	case int64:
		return Toint64(valueInt64)
	}
	return val
}



func Toint8(val interface{}) int8 {
	switch val.(type) {
	case int:
		return int8(val.(int))
	case int8:
		return val.(int8)
	case int16:
		return int8(val.(int16))
	case int32:
		return int8(val.(int32))
	case int64:
		return int8(val.(int64))
	}
	return 0
}

func Toint16(val interface{}) int16 {
	switch val.(type) {
	case int:
		return int16(val.(int))
	case int8:
		return int16(val.(int8))
	case int16:
		return int16(val.(int16))
	case int32:
		return int16(val.(int32))
	case int64:
		return int16(val.(int64))
	}
	return 0
}

func Toint32(val interface{}) int32 {
	switch val.(type) {
	case int:
		return int32(val.(int))
	case int8:
		return int32(val.(int8))
	case int16:
		return int32(val.(int16))
	case int32:
		return int32(val.(int32))
	case int64:
		return int32(val.(int64))
	}
	return 0
}

func Toint64(val interface{}) int64 {
	switch val.(type) {
	case int:
		return int64(val.(int))
	case int8:
		return int64(val.(int8))
	case int16:
		return int64(val.(int16))
	case int32:
		return int64(val.(int32))
	case int64:
		return int64(val.(int64))
	}
	return 0
}
