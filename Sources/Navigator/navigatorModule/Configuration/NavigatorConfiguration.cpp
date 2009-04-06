/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "Prerequisites.h"
#include "NavigatorConfiguration.h"
#include "tinyxml.h"
#include <CTBase64.h>

using namespace CommonTools;

typedef std::list<IParameterListener *>::iterator configurationListenerList_Iterator;

std::string  ConfigurationParameter::writeXml() const   
{    
    std::stringstream s;
    s << "\t<parameter ";
    s << "name=\"" << mName << "\" ";
    s << "value=\"" << mValue << "\"";
    s << "/>\n";
    return s.str();
}


void ConfigurationParameter::valueChanged()
{
    // send param changed to listener
    for (configurationListenerList_Iterator it = mListeners.begin(); it != mListeners.end(); it++)
    {
        IParameterListener * pt = *it;
        pt->parameterChanged(this);
    }
}

String ConfigurationParameter::getProtectedValueString()const   
{  
    String str = CTBase64::decrypt(mValue);
    return str;   
} 

void  ConfigurationParameter::setProtectedValueString(const String & strValue)   
{  
    mValue = CTBase64::encrypt(strValue);
    valueChanged();
}

void ConfigurationParameter::addListener(IParameterListener * pListener)
{
    mListeners.push_back(pListener);
};

void ConfigurationParameter::removeListener(IParameterListener * pListener)
{
    for (configurationListenerList_Iterator it = mListeners.begin(); it != mListeners.end(); it++)
    {
        IParameterListener * pt = *it;
        if (pt == pListener)
        {
            mListeners.erase(it);
            break;
        }
    }
}


Configuration::Configuration()
{

}

bool Configuration::loadConfig(const String & filename)
{
    mFilename = filename;
    TiXmlDocument doc(filename.c_str());
    TiXmlHandle hdl(&doc);

    if(!doc.LoadFile())
    {
        return false;
    }

    TiXmlElement* elem = hdl.FirstChildElement().FirstChildElement().Element();

    if(!elem)
    {
        return false;
    }

   
    while (elem)
    {
        const char *name = elem->Attribute("name"); 
        const char *value = elem->Attribute("value"); 
        if (name)
            findParam(name, value);

        elem = elem->NextSiblingElement(); // iteration 
    }

    return true;
}


typedef std::map<String, ConfigurationParameter>::iterator configurationParameterMap_Iterator;


bool Configuration::saveConfig()
{
    std::ofstream out(mFilename.c_str(), std::ios::out |std::ios::trunc);
    if (!out)
        return false;

    out << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n";
    out << "<SolipsisConfiguration>\n";

    for (configurationParameterMap_Iterator it = mParametersMap.begin(); it != mParametersMap.end(); it++)
    {
        ConfigurationParameter & pt = it->second;
        out << pt.writeXml();        
    }

    out << "</SolipsisConfiguration>\n";
    return true;  
}

ConfigurationParameter * Configuration::findParam(const String & paramName, const String & defaultValue)
{
    configurationParameterMap_Iterator it = mParametersMap.find(paramName);
    if (it != mParametersMap.end())
    {
        return &it->second;
    }

    ConfigurationParameter & param = mParametersMap[paramName];
    param.initValues(paramName, defaultValue);
    return &param;
}
