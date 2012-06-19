# -*- coding: utf-8 -*-
# Copyright (C) 2011 Rosen Diankov <rosen.diankov@gmail.com>
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
from common_test_openrave import *

@with_destroy
def test_pluginloading():
    RaveInitialize(load_all_plugins=False)
    assert(RaveLoadPlugin('ikfastsolvers'))
    assert(RaveLoadPlugin('libikfastsolvers'))
    env=Environment()
    assert(RaveCreateProblem(env,'ikfast') is not None)


class RunTutorialExample(object):
    def __init__(self,name,docname,args=[]):
        self.name=name
        self.args=args
        self.description = 'test_programs.tutorialexample.%s.%s'%(name,docname)
            
    def __call__(self):
        # turn off trajectory validation for now
        #python 
        RaveSetDebugLevel(RaveGetDebugLevel())
        example = getattr(examples,self.name)
        example.run(args=self.args+['--testmode',"--viewer="])


# test all scripts in source/tutorials/openravepy_examples/*.py
def test_tutorialexamples():
    dir=os.path.join('..','docs','source','tutorials','openravepy_examples')
    
