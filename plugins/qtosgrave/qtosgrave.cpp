// -*- coding: utf-8 -*-
// Copyright (C) 2012 Gustavo Puche, Rosen Diankov, OpenGrasp Team
//
// OpenRAVE Qt/OpenSceneGraph Viewer is licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "qtosgrave.h"

#include <openrave/utils.h>

#include "qtosg.h"

#if defined(HAVE_X11_XLIB_H) && defined(Q_WS_X11)
#include <X11/Xlib.h>
#endif

namespace qtosgrave {

OpenRAVE::ViewerBasePtr CreateQtOSGViewer(OpenRAVE::EnvironmentBasePtr penv, std::istream& sinput);

}

QtOSGRavePlugin::QtOSGRavePlugin()
{
    _interfaces[OpenRAVE::PT_Viewer].push_back("qtosg");
}

QtOSGRavePlugin::~QtOSGRavePlugin()
{
    Destroy();
}

void QtOSGRavePlugin::Destroy()
{
    // necessary since QApplication does not destroy all threads when last SoQt viewer is done
    //removePostedEvents - sometimes freezes on this function
    //QApplication::quit();
}

OpenRAVE::InterfaceBasePtr QtOSGRavePlugin::CreateInterface(OpenRAVE::InterfaceType type, const std::string& interfacename, std::istream& sinput, OpenRAVE::EnvironmentBasePtr penv)
{
    //	Debug.
    RAVELOG_VERBOSE("Initiating QTOSGRave plugin...!!!!.\n");

    switch(type) {
    case OpenRAVE::PT_Viewer:
#if defined(HAVE_X11_XLIB_H) && defined(Q_WS_X11)
        // always check viewers since DISPLAY could change
        if ( XOpenDisplay( NULL ) == NULL ) {
            RAVELOG_WARN("no display detected, so cannot load viewer");
            return InterfaceBasePtr();
        }
#endif
        if( interfacename == "qtosg" ) {
            return qtosgrave::CreateQtOSGViewer(penv, sinput);
        }
        break;
    default:
        break;
    }
    return OpenRAVE::InterfaceBasePtr();
}

const RavePlugin::InterfaceMap& QtOSGRavePlugin::GetInterfaces() const
{
    return _interfaces;
}

const std::string& QtOSGRavePlugin::GetPluginName() const
{
    static std::string pluginname = "QtOSGRavePlugin";
    return pluginname;
}

OPENRAVE_PLUGIN_API RavePlugin* CreatePlugin() {
    return new QtOSGRavePlugin();
}