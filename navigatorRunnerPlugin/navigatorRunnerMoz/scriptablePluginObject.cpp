#include "scriptablePluginObject.h"
#include "plugin.h"

void ScriptablePluginObject::_Deallocate(NPObject *npobj)
{
    // Call the virtual destructor.
    delete (ScriptablePluginObject *)npobj;
}

void ScriptablePluginObject::_Invalidate(NPObject *npobj)
{
    ((ScriptablePluginObject *)npobj)->Invalidate();
}

bool ScriptablePluginObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
    return ((ScriptablePluginObject *)npobj)->HasMethod(name);
}

bool ScriptablePluginObject::_Invoke(NPObject *npobj, NPIdentifier name,
                                    const NPVariant *args, uint32_t argCount,
                                    NPVariant *result)
{
    return ((ScriptablePluginObject *)npobj)->Invoke(name, args, argCount, result);
}

bool ScriptablePluginObject::_InvokeDefault(NPObject *npobj,
                                           const NPVariant *args,
                                           uint32_t argCount,
                                           NPVariant *result)
{
    return ((ScriptablePluginObject *)npobj)->InvokeDefault(args, argCount, result);
}

bool ScriptablePluginObject::_HasProperty(NPObject * npobj, NPIdentifier name)
{
    return ((ScriptablePluginObject *)npobj)->HasProperty(name);
}

bool ScriptablePluginObject::_GetProperty(NPObject *npobj, NPIdentifier name,
                                         NPVariant *result)
{
    return ((ScriptablePluginObject *)npobj)->GetProperty(name, result);
}

bool ScriptablePluginObject::_SetProperty(NPObject *npobj, NPIdentifier name,
                                         const NPVariant *value)
{
    return ((ScriptablePluginObject *)npobj)->SetProperty(name, value);
}

bool ScriptablePluginObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
    return ((ScriptablePluginObject *)npobj)->RemoveProperty(name);
}

bool ScriptablePluginObject::HasMethod(NPIdentifier name)
{
    NPUTF8* sname = NPN_UTF8FromIdentifier(name);
    if (strcmp(sname, "showVersion") == 0) return true;
    else if (strcmp(sname, "clear") == 0) return true;
    return false;
}

bool ScriptablePluginObject::HasProperty(NPIdentifier name)
{
    return false;
}

bool ScriptablePluginObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    return false;
}

bool ScriptablePluginObject:: SetProperty(NPIdentifier name, const NPVariant *value)
{
    return false;
}

bool ScriptablePluginObject::Invoke(NPIdentifier name, const NPVariant *args,
                               uint32_t argCount, NPVariant *result)
{
	return true;
}

bool ScriptablePluginObject::InvokeDefault(const NPVariant *args, uint32_t argCount,
                                      NPVariant *result)
{
    return false;
}

static NPObject* AllocateScriptablePluginObject(NPP npp, NPClass *aClass)
{
    return new ScriptablePluginObject(npp);
}

NPClass ScriptablePluginObject::sScriptablePluginObject_NPClass = {
	NP_CLASS_STRUCT_VERSION,
	AllocateScriptablePluginObject,
	ScriptablePluginObject::_Deallocate,
	ScriptablePluginObject::_Invalidate,
	ScriptablePluginObject::_HasMethod,
	ScriptablePluginObject::_Invoke,
	ScriptablePluginObject::_InvokeDefault,
	ScriptablePluginObject::_HasProperty,
	ScriptablePluginObject::_GetProperty,
	ScriptablePluginObject::_SetProperty,
	ScriptablePluginObject::_RemoveProperty
};

