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

#ifdef FILE_INC_HEADER
#endif

#ifdef FILE_INC_MEMBER
  private:
    FILE*   mHandle;
    bool_t  mIsOpen;
#endif

#ifdef FILE_INC_IMPL

  inline File::File(const char* name, const char* mode) :
    mHandle(NULL) ,
    mIsOpen(false) {

    // try to open file
    mHandle  = fopen(name, mode);
    if(mHandle != NULL) {
      mIsOpen = true;
    }
  }

  inline bool_t File::isOpen() {
    return mIsOpen;
  }

  inline bool_t File::isEof() {
    if(mHandle == NULL) {
      return false;
    }
    return (feof(mHandle) != 0);
  }

  inline status_t File::read(char * buffer, int32_t length, int32_t* numBytes) {
    if(buffer == NULL) {
      return CAPU_EINVAL;
    }
    if(mHandle == NULL) {
      return CAPU_ERROR;
    }

    size_t result = fread(buffer, 1, length, mHandle);
    if(result == length) {
        if(numBytes != NULL) {
          *numBytes = result;
        }
        return CAPU_OK;
    }
    if(feof(mHandle)) {
      if(numBytes != NULL) {
        *numBytes = result;
      }
      return CAPU_EOF;
    }

    return CAPU_ERROR;
  }

  inline status_t File::write(char * buffer, int32_t length) {
    if(buffer == NULL) {
      return CAPU_EINVAL;
    }
    if(mHandle == NULL) {
      return CAPU_ERROR;
    }

    size_t result = fwrite(buffer, 1, length, mHandle);
    if(result != length) {
      return CAPU_ERROR;
    }
    return CAPU_OK;
  }

  inline status_t File::flush() {
    if(mHandle != NULL) {
      int error = fflush(mHandle);
      if(error == 0) {
        return CAPU_OK;
      }
    }
    return CAPU_ERROR;
  }

  inline status_t File::close() {
    if(mHandle != NULL) {
      fclose(mHandle);
      mHandle = NULL;
      mIsOpen = false;
      return CAPU_OK;
    }
    return CAPU_ERROR;
  }

  inline File::~File() {
    if(mHandle != NULL) {
      fclose(mHandle);
    }
  }

#endif
