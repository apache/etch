"""
$Id$

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
from .Messagezier import *
from ..support.FreePool import *
from ..support.QueuedPool import *
from ...util.Resources import *

class TransportHelper(object):
    """
    Class to help construct transport stacks
    """
    
    ## Resources
    
    QUEUED_POOL = "QUEUED_POOL"
    """The Pool used to execute queued async receiver messages."""
    
    FREE_POOL = "FREE_POOL"
    """The Pool used to execute free async receiver messages."""
    
    BINARY = "binary"
    """Binary transport format"""
    
    XML = "xml"
    """Xml transport format"""
    
    ## Utilities
    
    @staticmethod
    def initResources(resources):
        """
        Initializes standard resources.
        
        @param resources - some initial values for resources. May be None to 
        accept all the defaults.
        @return - copy of resources initialized with default values for standard
        items.
        @raises Exception
        """
        if resources == None:
            resources = Resources()
        else:
            resources = Resources(resources)
        
        if not TransportHelper.QUEUED_POOL in resources:
            resources[TransportHelper.QUEUED_POOL] = QueuedPool()
        
        if not TransportHelper.FREE_POOL in resources:
            resources[TransportHelper.FREE_POOL] = FreePool()
        
        if not Messagizer.FORMAT in resources:
            resources[Messagizer.FORMAT] = TransportHelper.BINARY
        
        return resources
            