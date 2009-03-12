"""
$Id: Singleton.py 712747 2008-08-16 21:53:35Z dixson3 $

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

__all__ = ['Singleton']

singletons = {}

class Singleton(object):
    
    def __new__(cls, *args, **kwargs):
        if cls in singletons:
            return singletons[cls]
        self = object.__new__(cls)
        cls.__init__(self, *args, **kwargs)
        singletons[cls] = self
        return self
