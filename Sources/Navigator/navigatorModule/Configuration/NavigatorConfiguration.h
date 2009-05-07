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

#ifndef __Configuration_h__
#define __Configuration_h__

#include <OgrePrerequisites.h>
#include <OgreStringConverter.h>

using namespace Ogre;

class ConfigurationParameter;
class IParameterListener
{
// the only function to set
public :
    virtual void parameterChanged(ConfigurationParameter* pParameter) = 0;
};

typedef std::list<IParameterListener *> configurationListenerList;

class ConfigurationParameter
{
protected:
    String mName;
    String mValue;

    configurationListenerList mListeners;
 
public:
    ConfigurationParameter()
    {

    };
    ConfigurationParameter(const String & name, String value)
    {
        initValues(name,value);
    }

    void initValues(const String & name, String value)
    {
        mName = name;
        mValue = value;
   }

    // adds a parameter listener
    void addListener(IParameterListener * pListener);
    void removeListener(IParameterListener * pListener);
   
    bool getValueBool() const  {  return StringConverter::parseBool(mValue);   }
    void setValueBool(bool bValue)   {  mValue = StringConverter::toString(bValue);valueChanged();}

    float getValueFloat() const  {  return StringConverter::parseBool(mValue);   }
    void  setValueFloat(float fValue)   {  mValue = StringConverter::toString(fValue);valueChanged();}

    int getValueInt() const  {  return StringConverter::parseInt(mValue);   }
    void  setValueInt(int iValue)   {  mValue = StringConverter::toString(iValue);valueChanged();}

    const String & getValueString()const   {  return mValue;   }
    void  setValueString(const String & strValue)   {  mValue = strValue;valueChanged();}

    String getProtectedValueString()const;
    void  setProtectedValueString(const String & strValue);

    std::string  writeXml() const;      

protected:
    // send value to LUA 
    void valueChanged();
};

typedef std::map<String, ConfigurationParameter> configurationParameterMap;

class Configuration
{
protected:
    String mFilename;

    configurationParameterMap mParametersMap;
public:
    Configuration();

    // load the configuration file
    bool loadConfig(const String & filename);

    // save the configuration file
    // get The LUA values before
    bool saveConfig();

    // find an existing parameter or create one if not exists
    ConfigurationParameter * findParam(const String & paramName, const String & defaultValue = "");
};

#endif // #ifndef __Configuration_h__