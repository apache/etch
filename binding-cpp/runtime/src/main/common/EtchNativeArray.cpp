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

#include "common/EtchNativeArray.h"

status_t EtchNativeArrayBase::getBase(Pos pos, capu::SmartPointer<EtchObject> &result) {

  status_t ret = ETCH_OK;
  const EtchObjectType* type = getObjectType()->getObjectComponentType();
  const EtchObjectType::EtchObjectTypeTrait trait = getObjectType()->getObjectComponentTypeTrait();
  const capu::uint32_t dims = getDim();

  switch(type->getTypeId()) {
    case EOTID_BYTE: {
      switch(trait) {
        case EtchObjectType::VALUE: {
          EtchNativeArray<capu::int8_t>* na = (EtchNativeArray<capu::int8_t>*)(this);
          if(dims == 1) {
            capu::int8_t value;
            ret = na->get(pos, &value);
            if (ret == ETCH_OK) {
              result = new EtchInt32(value);
            }
          } else {
            //multi
            capu::SmartPointer<EtchNativeArray<capu::int8_t> > subarray;
            ret = na->get(pos,subarray);
            if (ret == ETCH_OK) {
              result = subarray;
            }
          }
          break;
        }
        case EtchObjectType::POINTER: {
          EtchNativeArray<capu::int8_t*>* na = (EtchNativeArray<capu::int8_t*>*)(this);
          if(dims == 1) {
            capu::int8_t* value = NULL;
            ret = na->get(pos, &value);
            if(ret == ETCH_OK && value != NULL) {
              result = new EtchInt32(*value);
            }
          } else {
            // multi
            capu::SmartPointer<EtchNativeArray<capu::int8_t*> > subarray;
            ret = na->get(pos,subarray);
            if (ret == ETCH_OK) {
              result = subarray;
            }
          }
          break;
        }
        case EtchObjectType::SMART_POINTER: {
          EtchNativeArray<capu::SmartPointer<capu::int8_t> >* na = (EtchNativeArray<capu::SmartPointer<capu::int8_t> >*)(this);
          if(dims == 1) {
            capu::SmartPointer<capu::int8_t> value = NULL;
            ret = na->get(pos, &value);
            if(ret == ETCH_OK && value.get() != NULL) {
              result = new EtchInt32(*value.get());
            }
          } else {
            capu::SmartPointer<EtchNativeArray<capu::SmartPointer<capu::int8_t> > > subarray;
            ret = na->get(pos,subarray);
            if (ret == ETCH_OK) {
              result = subarray;
            }
          }
          break;
        }
        default: {
          return ETCH_ERROR;
        }
      }
      break;
    }

    case EOTID_OBJECT: {
      GETBASE_CAST_OBJECT_TO_NA(EtchObject)
      break;
    }
    case EOTID_INT32: {
      GETBASE_CAST_OBJECT_TO_NA(EtchInt32)
      break;
    }
    case EOTID_BOOL: {
      GETBASE_CAST_OBJECT_TO_NA(EtchBool)
      break;
    }
    case EOTID_SHORT: {
      GETBASE_CAST_OBJECT_TO_NA(EtchShort)
      break;
    }
    case EOTID_LONG: {
      GETBASE_CAST_OBJECT_TO_NA(EtchLong)
      break;
    }
    case EOTID_DOUBLE: {
      GETBASE_CAST_OBJECT_TO_NA(EtchDouble)
      break;
    }
    case EOTID_FLOAT: {
      GETBASE_CAST_OBJECT_TO_NA(EtchFloat)
      break;
    }
    case EOTID_STRING: {
      GETBASE_CAST_OBJECT_TO_NA(EtchString)
      break;
    }
    case EOTID_DATE: {
      GETBASE_CAST_OBJECT_TO_NA(EtchDate)
      break;
    }
    case EOTID_HASHTABLE: {
      switch(trait) {
        case EtchObjectType::VALUE: {
          EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > >* na = (EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > >*)(this);
          if(dims == 1) {
            EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > value;
            ret = na->get(pos, &value);
            if (ret == ETCH_OK) {
              result = new EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > (value);
            }
          } else {
            capu::SmartPointer<EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > > subarray;
            ret = na->get(pos,subarray);
            if (ret == ETCH_OK) {
              result = subarray;
            }
          }
          break;
        }
        case EtchObjectType::POINTER: {
          EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *>* na = (EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *>*)(this);
          if(dims == 1) {
            EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > * value = NULL;
            ret = na->get(pos, &value);
            if(ret == ETCH_OK && value != NULL) {
              result = new EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > (*value);
            }
          } else {
            capu::SmartPointer<EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *> > subarray;
            ret = na->get(pos,subarray);
            if (ret == ETCH_OK) {
              result = subarray;
            }
          }
          break;
        }
        case EtchObjectType::SMART_POINTER: {
          EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > >* na = (EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > >*)(this);
          if(dims == 1) {
            capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > value = NULL;
            ret = na->get(pos, &value);
            if (ret == ETCH_OK) {
              result = value;
            }
          } else {
            capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > > > subarray;
            ret = na->get(pos,subarray);
            if (ret == ETCH_OK) {
              result = subarray;
            }
          }
          break;
        }
        default: {
          return ETCH_ERROR;
        }
      }
      break;
    }
    case EOTID_SET: {
      GETBASE_CAST_OBJECT_TO_NA(EtchHashSet<capu::SmartPointer<EtchObject> > )
      break;
    }
    case EOTID_LIST: {
      GETBASE_CAST_OBJECT_TO_NA(EtchList<capu::SmartPointer<EtchObject> > )
      break;
    }
    default: {
      return ETCH_ERROR;
    }
  }
  return ret;
}

status_t EtchNativeArrayBase::setBase(Pos pos, capu::SmartPointer<EtchObject> data) {

  status_t ret = ETCH_OK;
  const EtchObjectType* type = getObjectType()->getObjectComponentType();
  const EtchObjectType::EtchObjectTypeTrait trait = getObjectType()->getObjectComponentTypeTrait();
  const capu::int32_t dims = getDim();

  switch(type->getTypeId()) {
    case EOTID_BYTE: {
      switch(trait) {
        case EtchObjectType::VALUE: {
          EtchNativeArray<capu::int8_t>* na = (EtchNativeArray<capu::int8_t>*)(this);
          if(dims == 1) {
            //unboxing of native type
            capu::SmartPointer<EtchInt32> value = capu::smartpointer_cast<EtchInt32> (data);
            capu::int8_t nativeValue = value->get();
            return na->set(pos, nativeValue);
          } else {
            //multi
            capu::SmartPointer<EtchNativeArray<capu::int8_t> > subarray = capu::smartpointer_cast<EtchNativeArray<capu::int8_t> >(data);
            return na->set(pos,subarray);
          }
          break;
        }
        case EtchObjectType::POINTER: {
          EtchNativeArray<capu::int8_t*>* na = (EtchNativeArray<capu::int8_t*>*)(this);
          if(dims == 1) {
            capu::SmartPointer<EtchInt32> value = capu::smartpointer_cast<EtchInt32> (data);
            capu::int32_t native32Value = value->get();
            capu::int8_t* nativeValue = (capu::int8_t*) &native32Value;
            return na->set(pos, nativeValue);
          } else {
            // multi
            capu::SmartPointer<EtchNativeArray<capu::int8_t*> > subarray = capu::smartpointer_cast<EtchNativeArray<capu::int8_t*> >(data);
            return na->set(pos,subarray.get());
          }
          break;
        }
        case EtchObjectType::SMART_POINTER: {
          EtchNativeArray<capu::SmartPointer<capu::int8_t> >* na = (EtchNativeArray<capu::SmartPointer<capu::int8_t> >*)(this);
          if(dims == 1) {
            capu::SmartPointer<EtchInt32> value = capu::smartpointer_cast<EtchInt32> (data);
            if (value.get() != NULL) {
              capu::int32_t nativeInt32Value = value->get();
              //value has to be copied to avoid deletion by smartpointer
              capu::SmartPointer<capu::int8_t> newValue = new capu::int8_t(nativeInt32Value);
              return na->set(pos, newValue);
            }
          } else {
            capu::SmartPointer<EtchNativeArray<capu::SmartPointer<capu::int8_t> > > subarray = capu::smartpointer_cast<EtchNativeArray<capu::SmartPointer<capu::int8_t> > >(data);
            return na->set(pos,subarray.get());
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case EOTID_OBJECT: {
      SETBASE_CAST_OBJECT_TO_NA(EtchObject)
      break;
    }
    case EOTID_INT32: {
      SETBASE_CAST_OBJECT_TO_NA(EtchInt32)
      break;
    }
    case EOTID_BOOL: {
      SETBASE_CAST_OBJECT_TO_NA(EtchBool)
      break;
    }
    case EOTID_SHORT: {
      SETBASE_CAST_OBJECT_TO_NA(EtchShort)
      break;
    }
    case EOTID_LONG: {
      SETBASE_CAST_OBJECT_TO_NA(EtchLong)
      break;
    }
    case EOTID_DOUBLE: {
      SETBASE_CAST_OBJECT_TO_NA(EtchDouble)
      break;
    }
    case EOTID_FLOAT: {
      SETBASE_CAST_OBJECT_TO_NA(EtchFloat)
      break;
    }
    case EOTID_STRING: {
      SETBASE_CAST_OBJECT_TO_NA(EtchString)
      break;
    }
    case EOTID_DATE: {
      SETBASE_CAST_OBJECT_TO_NA(EtchDate)
      break;
    }case EOTID_HASHTABLE: {
      switch(trait) {
        case EtchObjectType::VALUE: {
          EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > >* na = (EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > >*)(this);
          if(dims == 1) {
            capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > value = capu::smartpointer_cast<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > (data);
            EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >  *newValue = value.get();
            if (newValue != NULL) {
              return na->set(pos, *newValue);
            }
          } else {
            capu::SmartPointer<EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > > subarray = capu::smartpointer_cast<EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > >(data);
            return na->set(pos,subarray.get());
          }
          break;
        }
        case EtchObjectType::POINTER: {
          EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *>* na = (EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *>*)(this);
          if(dims == 1) {
            capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > value = capu::smartpointer_cast<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > (data);
            EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *newValue = value.get();
            return na->set(pos, newValue);
          } else {
            capu::SmartPointer<EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *> > subarray = capu::smartpointer_cast<EtchNativeArray<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > *> >(data);
            return na->set(pos,subarray.get());
          }
          break;
        }
        case EtchObjectType::SMART_POINTER: {
          EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > >* na = (EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > >*)(this);
          if(dims == 1) {
            capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > value = capu::smartpointer_cast<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > (data);
            return na->set(pos, value);
          } else {
            capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > > > subarray = capu::smartpointer_cast<EtchNativeArray<capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > > >(data);
            return na->set(pos,subarray.get());
          }
          break;
        }
        default: {
          break;
        }
      }
      break;
    }
    case EOTID_SET: {
      SETBASE_CAST_OBJECT_TO_NA(EtchHashSet<capu::SmartPointer<EtchObject> > )
      break;
    }
    case EOTID_LIST: {
      SETBASE_CAST_OBJECT_TO_NA(EtchList<capu::SmartPointer<EtchObject> > )
      break;
    }
  }
  return ret;
}

