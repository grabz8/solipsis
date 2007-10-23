#ifndef __SCRIPTABLEPLUGINOBJECT_H__
#define __SCRIPTABLEPLUGINOBJECT_H__

#include "pluginbase.h"

// forward definition of the plugin
class nsPluginInstance;

class ScriptablePluginObject :  public NPObject
{
public:
    ScriptablePluginObject(NPP inst) : myInstance(inst) { }

    virtual void Invalidate() { }
    virtual bool HasMethod(NPIdentifier name);
    virtual bool Invoke(NPIdentifier name, const NPVariant *args,
        uint32_t argCount, NPVariant *result);
    virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
        NPVariant *result);
    virtual bool HasProperty(NPIdentifier name);
    virtual bool GetProperty(NPIdentifier name, NPVariant *result);
    virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
    virtual bool RemoveProperty(NPIdentifier name) { return false; }

    static void _Deallocate(NPObject *npobj);
    static void _Invalidate(NPObject *npobj);
    static bool _HasMethod(NPObject *npobj, NPIdentifier name);
    static bool _Invoke(NPObject *npobj, NPIdentifier name,
        const NPVariant *args, uint32_t argCount,
        NPVariant *result);
    static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
        uint32_t argCount, NPVariant *result);
    static bool _HasProperty(NPObject * npobj, NPIdentifier name);
    static bool _GetProperty(NPObject *npobj, NPIdentifier name,
        NPVariant *result);
    static bool _SetProperty(NPObject *npobj, NPIdentifier name,
        const NPVariant *value);
    static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);

    void setPlugin(nsPluginInstance* plugin) { myPlugin = plugin; }

    // class definition structure
    static NPClass sScriptablePluginObject_NPClass;

private:
    NPP myInstance;
    nsPluginInstance* myPlugin;
};

#endif // __SCRIPTABLEPLUGINOBJECT_H__
