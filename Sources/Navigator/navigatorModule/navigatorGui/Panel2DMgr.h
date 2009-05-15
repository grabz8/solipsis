/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#ifndef __Panel2DMgr_h__
#define __Panel2DMgr_h__

#include <map>
#include <Ogre.h>
#include <CTSingleton.h>
#include "Panel2D.h"

namespace Solipsis {

/** This class manages 2D panels for WWW/VLC/VNC materials.
*/
class Panel2DMgr : public CommonTools::Singleton<Panel2DMgr>, public KeyboardEventListener, public MouseEventListener
{
    friend class Panel2D;

protected:
    typedef std::map<Ogre::String, Panel2DFactory*> PanelFactoryMap;
    PanelFactoryMap mPanelFactoryMap;
    typedef std::map<Ogre::String, Panel2D*> PanelMap;
    PanelMap mPanelMap;
    Panel2D *mFocusedPanel;
    static const Ogre::ushort ms_DefaultMinZOrder, ms_DefaultMaxZOrder;
    Ogre::ushort mMinZOrder, mMaxZOrder;
    Ogre::ushort mZOrderCounter;
    static const int ms_DefaultCreationShift;
    int mNextCreationPosX, mNextCreationPosY;
    bool mMoving;
    Panel2D::Part mSizingPart;
    MouseState mMovingSizingMouseState;
    int mSizingX, mSizingY, mSizingW, mSizingH;

public:
    /// Constructor
    Panel2DMgr();
    /// Destructor
    ~Panel2DMgr();

    /// Initialize mouse cursors (resizing, ...)
    void initializeMouseCursors();
	void setZOrderMinMax(Ogre::ushort minZOrder, Ogre::ushort maxZOrder);

    void registerPanel2DFactory(Panel2DFactory* factory);
    void unregisterPanel2DFactory(Panel2DFactory* factory);

    Panel2D* createPanel(const Ogre::String& type, const Ogre::String& name);
    void destroyPanel(Panel2D* panel);
    void destroyPanel(const Ogre::String& name);
    /// Destroy all panels
    void destroyAllPanels();
    Panel2D* getPanel(const String& name);
    Panel2D* focusPanel(int x, int y, Panel2D* focusedPanel = 0);
    Panel2D* getFocusedPanel();
    /// Defocus current panel
    void defocus();

    /** See KeyboardEventListener. */
    virtual bool keyPressed(const KeyboardEvt& evt);
    /** See KeyboardEventListener. */
    virtual bool keyReleased(const KeyboardEvt& evt);

    /** See MouseEventListener. */
    virtual bool mouseMoved(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mousePressed(const MouseEvt& evt);
    /** See MouseEventListener. */
    virtual bool mouseReleased(const MouseEvt& evt);

protected:
    Panel2D* getPanelOverMouse(int x, int y);
    /// Recompute Z order of all panels
	void recomputeZOrder();
};

} // namespace Solipsis

#endif // #ifndef __Panel2DMgr_h__
