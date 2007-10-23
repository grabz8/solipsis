/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsINavigatorPlugin.idl
 */

#ifndef __gen_nsINavigatorPlugin_h__
#define __gen_nsINavigatorPlugin_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsINavigatorPlugin */
#define NS_INAVIGATORPLUGIN_IID_STR "7a157632-c0dd-413f-ab97-f8411073b6a1"

#define NS_INAVIGATORPLUGIN_IID \
  {0x7a157632, 0xc0dd, 0x413f, \
    { 0xab, 0x97, 0xf8, 0x41, 0x10, 0x73, 0xb6, 0xa1 }}

class NS_NO_VTABLE nsINavigatorPlugin : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_INAVIGATORPLUGIN_IID)

  /* void showVersion (); */
  NS_IMETHOD ShowVersion(void) = 0;

  /* void clear (); */
  NS_IMETHOD Clear(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSINAVIGATORPLUGIN \
  NS_IMETHOD ShowVersion(void); \
  NS_IMETHOD Clear(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSINAVIGATORPLUGIN(_to) \
  NS_IMETHOD ShowVersion(void) { return _to ShowVersion(); } \
  NS_IMETHOD Clear(void) { return _to Clear(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSINAVIGATORPLUGIN(_to) \
  NS_IMETHOD ShowVersion(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ShowVersion(); } \
  NS_IMETHOD Clear(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Clear(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsNavigatorPlugin : public nsINavigatorPlugin
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSINAVIGATORPLUGIN

  nsNavigatorPlugin();

private:
  ~nsNavigatorPlugin();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsNavigatorPlugin, nsINavigatorPlugin)

nsNavigatorPlugin::nsNavigatorPlugin()
{
  /* member initializers and constructor code */
}

nsNavigatorPlugin::~nsNavigatorPlugin()
{
  /* destructor code */
}

/* void showVersion (); */
NS_IMETHODIMP nsNavigatorPlugin::ShowVersion()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void clear (); */
NS_IMETHODIMP nsNavigatorPlugin::Clear()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsINavigatorPlugin_h__ */
