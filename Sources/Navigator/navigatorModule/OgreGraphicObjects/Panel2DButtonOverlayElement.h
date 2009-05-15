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

#ifndef __Panel2DButtonOverlayElement_h__
#define __Panel2DButtonOverlayElement_h__

#include <OgrePanelOverlayElement.h>
#include <OgreOverlayElementFactory.h>

using namespace Ogre;

namespace Solipsis {

class Panel2DButtonOverlayElement : public PanelOverlayElement
{
public:
    /** Constructor */
    Panel2DButtonOverlayElement(const String& name);
    virtual ~Panel2DButtonOverlayElement();

	const String& getTypeName(void) const;

    /** Sets the action of the button.
    @param evt The action string of the button.
    */
    void setAction(String action);

    /** Gets the action of the button. */
    String getAction(void) const;

    /** Sets the action of the button when event occurred.
    @param evt The action string of the button.
    */
    void setActionEvent(String actionEvent);

    /** Gets the action of the button when event occurred. */
    String getActionEvent(void) const;

    /** Sets the default texture coordinates. */
    void setUVDefault(Real u1, Real v1, Real u2, Real v2);

    /** Get the default uv coordinates*/
    void getUVDefault(Real& u1, Real& v1, Real& u2, Real& v2) const;

    /** Sets the texture coordinates when event occurred. */
    void setUVEvent(Real u1, Real v1, Real u2, Real v2);

    /** Get the uv coordinates when event occurred*/
    void getUVEvent(Real& u1, Real& v1, Real& u2, Real& v2) const;

    /** Sets the texture coordinates when mouse is over. */
    void setUVIncHover(Real u, Real v);

    /** Get the uv coordinates when mouse is over*/
    void getUVIncHover(Real& u, Real& v) const;


    /** set event and apply UV if event occurred or not. */
    void setEventState(bool eventOccurred);
    /** Get the event state. */
    bool getEventState();
    /** Set hover and apply UV if mouse is over. */
    void setHoverState(bool hover);
    /** Get the hover state. */
    bool getHoverState();
    /** Get the current action according to event state. */
    String getCurrentAction();


    /** Command object for specifying action of the button (see ParamCommand).*/
    class _OgrePrivate CmdAction: public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };
    /** Command object for specifying action of the button when event occurred (see ParamCommand).*/
    class _OgrePrivate CmdActionEvent: public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };
    /** Command object for specifying default UV coordinates (see ParamCommand).*/
    class _OgrePrivate CmdUVCoordsDefault: public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };
    /** Command object for specifying UV coordinates when event occurred (see ParamCommand).*/
    class _OgrePrivate CmdUVCoordsEvent: public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };
    /** Command object for specifying UV coordinates when mouse is over (see ParamCommand).*/
    class _OgrePrivate CmdUVCoordsIncHover: public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };

protected:
    String mAction;
    String mActionEvent;
    Real mDefaultU1, mDefaultV1, mDefaultU2, mDefaultV2;
    Real mEventU1, mEventV1, mEventU2, mEventV2;
    Real mIncHoverU, mIncHoverV;

    static String msTypeName;

    bool mEventOccurred, mHover;

    /// Internal method for setting up parameters
    void addBaseParameters(void);

    void updateUV();

    // Command objects
    static CmdAction msCmdAction;
    static CmdActionEvent msCmdActionEvent;
    static CmdUVCoordsDefault msCmdUVCoordsDefault;
    static CmdUVCoordsEvent msCmdUVCoordsEvent;
    static CmdUVCoordsIncHover msCmdUVCoordsIncHover;
};

/** Factory for creating Panel2DButtonOverlayElement instances. */
class Panel2DButtonOverlayElementFactory: public OverlayElementFactory
{
public:
    /** See OverlayElementFactory */
    OverlayElement* createOverlayElement(const String& instanceName)
    {
        return OGRE_NEW Panel2DButtonOverlayElement(instanceName);
    }
    /** See OverlayElementFactory */
    const String& getTypeName(void) const
    {
        static String name = "SolipsisPanel2DButton";
        return name;
    }
};

} // namespace Solipsis

#endif // #ifndef __Panel2DButtonOverlayElement_h__