/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsINavigatorRunnerMoz.idl
 */

#ifndef __gen_nsINavigatorRunnerMoz_h__
#define __gen_nsINavigatorRunnerMoz_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsINavigatorRunnerMoz */
#define NS_INAVIGATORRUNNERMOZ_IID_STR "0c0f1398-9379-11dc-8314-0800200c9a66"

#define NS_INAVIGATORRUNNERMOZ_IID \
  {0x0c0f1398, 0x9379, 0x11dc, \
    { 0x83, 0x14, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 }}

class NS_NO_VTABLE nsINavigatorRunnerMoz : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_INAVIGATORRUNNERMOZ_IID)

  /* void showVersion (); */
  NS_IMETHOD ShowVersion(void) = 0;

  /* void clear (); */
  NS_IMETHOD Clear(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSINAVIGATORRUNNERMOZ \
  NS_IMETHOD ShowVersion(void); \
  NS_IMETHOD Clear(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSINAVIGATORRUNNERMOZ(_to) \
  NS_IMETHOD ShowVersion(void) { return _to ShowVersion(); } \
  NS_IMETHOD Clear(void) { return _to Clear(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSINAVIGATORRUNNERMOZ(_to) \
  NS_IMETHOD ShowVersion(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ShowVersion(); } \
  NS_IMETHOD Clear(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Clear(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsNavigatorRunnerMoz : public nsINavigatorRunnerMoz
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSINAVIGATORRUNNERMOZ

  nsNavigatorRunnerMoz();

private:
  ~nsNavigatorRunnerMoz();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsNavigatorRunnerMoz, nsINavigatorRunnerMoz)

nsNavigatorRunnerMoz::nsNavigatorRunnerMoz()
{
  /* member initializers and constructor code */
}

nsNavigatorRunnerMoz::~nsNavigatorRunnerMoz()
{
  /* destructor code */
}

/* void showVersion (); */
NS_IMETHODIMP nsNavigatorRunnerMoz::ShowVersion()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void clear (); */
NS_IMETHODIMP nsNavigatorRunnerMoz::Clear()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsINavigatorRunnerMoz_h__ */
