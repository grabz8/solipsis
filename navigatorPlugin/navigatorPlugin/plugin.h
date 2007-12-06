/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "pluginbase.h"

#include "NavigatorModule.h"
#include "IApplication.h"
#include "IInstance.h"
#include "IWindow.h"
#include "Event.h"

class nsPluginInstance :
    public nsPluginInstanceBase,
    public Solipsis::IWindow
{
public:
  nsPluginInstance(nsPluginCreateData * aCreateDataStruct);
  ~nsPluginInstance();

  NPBool init(NPWindow* aWindow);
  void shut();
  NPBool isInitialized();

    // overridden from nsPluginInstanceBase
    NPError NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
    NPError DestroyStream (NPStream *stream, NPError reason);
    // we need to provide implementation of this method as it will be
    // used by Mozilla to retrive the scriptable peer
    virtual NPError GetValue(NPPVariable variable, void *value);
    // end overridden

    // overridden from Solipsis::IWindow
    virtual void * getHandle() { return mhWnd; }
    virtual unsigned int getWidth() { return mWidth; }
    virtual unsigned int getHeight() { return mHeight; }
    // end overridden

  // locals
  const char * getVersion();

    // get and set
    NPP& getInstance() { return mInstance; }
    NPObject* getScriptableObject();
    void setWidth(unsigned int v) { mWidth = v; }
    void setHeight(unsigned int v) { mHeight = v; }
    Solipsis::IInstance* getIInstance() { return mNavigatorInstance; }

    // javascript tests only
    const char* getPaintString();
    void showVersion();
    void clear();

protected:
    // create the navigator instance
    void _createInstance();

    LRESULT OnMouseLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseRButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
/*	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);*/

    // keyboard and window messages hanlders
    static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK fnHookKeyboard(int code, WPARAM wParam, LPARAM lParam);

private:
  NPP mInstance;
  NPBool mInitialized;
  HWND mhWnd;

    // old window proc for window messages
    WNDPROC mOldProc;
    // keyboard hook
    HHOOK mKeyboardHook;
    bool mFirstPersonMode;

    // last mouse evt
    Solipsis::MouseEvt lastMouseEvt;

    // navigator instance
    Solipsis::IInstance* mNavigatorInstance;

    // window attributes
    unsigned int mWidth;
    unsigned int mHeight;

    // scriptable object for javascript calls
    NPObject* mScriptableObject;

    // javascript calls for tests only
    char mPaintString[256];
};

#endif // __PLUGIN_H__
