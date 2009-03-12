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

__all__ = ["_Etch_RuntimeException"]

class _Etch_RuntimeException(Exception):
    serialVersionUID = 3051515541923877063L

    def __init__(self, args=None):
        super(_Etch_RuntimeException,self).__init__(args)
        self.__message = args

    def getMessage(self):
        return self.__message

    def __repr__(self):
        return "Remote side threw this runtime exception: %s" % self.__message

