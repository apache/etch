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

#ifndef __FILE_H__
#define __FILE_H__

#include "capu/Config.h"
#include "capu/Error.h"

#define FILE_INC_HEADER
#include "arch/File.inc"
#undef FILE_INC_HEADER

namespace capu {

  class File {

#define FILE_INC_MEMBER
  #include "arch/File.inc"
#undef FILE_INC_MEMBER

  public:

    /**
     * Create a new instance for a file.
     * @name of the file
     * @mode to specify the file mode
              "r"  opens file for reading.
              "w"  opens file as an empty file for writing.
              "r+" opens file for reading and writing. The file must exist.
              "w+" opens file for reading and writing. Create a new file also of old one exists.
     */
    inline File(const char* name, const char* mode);

    /**
     * return true if file is open else false
     */
    inline bool_t isOpen();

    /**
     * return true if file end was reachde
     */
    inline bool_t isEof();

    /**
     * Reads data from the stream and store it into the buffer.
     * @buffer elements to be read
     * @length of the buffer
     * @numBytes of bytes read from the stream
     * return CAPU_EINVAL if params are wrong
              CAPU_EOF    if end of stream 
              CAPU_ERROR  if invalid state or file not open
     */
    inline status_t read(char * buffer, int32_t length, int32_t* numBytes);

    /**
     * Writes the given byte buffer to the stream.
     * @buffer elements to be written
     * @length of the buffer
     * return CAPU_OK buffer could be written to the stream
     *        CAPU_ERROR otherwise
     */
    inline status_t write(char * buffer, int32_t length);

    /**
     * Writes any unwritten data to the file.
     */
    inline status_t flush();

    /**
     * Close the stream.
     *@return
     */
    inline status_t close();

    /**
     * Destruct current instance.
     */
    inline ~File();

  };

#define FILE_INC_IMPL
#include "arch/File.inc"
#undef FILE_INC_IMPL

}

#endif /* __FILE_H__ */
