"""
$Id: Hash.py 712748 2008-08-17 05:58:09Z dixson3 $

# Copyright 2007-2008 Cisco Systems Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy
# of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#
"""
from __future__ import absolute_import
from ..python.Types import Integer

class Hash(object):

    @staticmethod
    def hash(name):
        """
        Computes the hash value of the name to be used as the id
        when constructing an IdName.

        @param name    the name of the type or field
        @return        a hash of name
        """
        hash   = Integer(5381)
        for c in name:
            h6    = hash << 6
            hash  = (h6 << 10) + h6 - hash + ord(c)
        return hash