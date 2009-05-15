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

#include "Prerequisites.h"

#include "Panel2DButtonOverlayelement.h"

namespace Solipsis {

String Panel2DButtonOverlayElement::msTypeName = "SolipsisPanel2DButton";
Panel2DButtonOverlayElement::CmdAction Panel2DButtonOverlayElement::msCmdAction;
Panel2DButtonOverlayElement::CmdActionEvent Panel2DButtonOverlayElement::msCmdActionEvent;
Panel2DButtonOverlayElement::CmdUVCoordsDefault Panel2DButtonOverlayElement::msCmdUVCoordsDefault;
Panel2DButtonOverlayElement::CmdUVCoordsEvent Panel2DButtonOverlayElement::msCmdUVCoordsEvent;
Panel2DButtonOverlayElement::CmdUVCoordsIncHover Panel2DButtonOverlayElement::msCmdUVCoordsIncHover;

//-------------------------------------------------------------------------------------
Panel2DButtonOverlayElement::Panel2DButtonOverlayElement(const String& name)
    : PanelOverlayElement(name), 
    mAction(""),
    mActionEvent(""),
    mDefaultU1(0.0),
    mDefaultV1(0.0),
    mDefaultU2(1.0),
    mDefaultV2(1.0),
    mEventU1(0.0),
    mEventV1(0.0),
    mEventU2(1.0),
    mEventV2(1.0),
    mIncHoverU(0.0),
    mIncHoverV(0.0),
    mEventOccurred(false),
    mHover(false)
{
    if (createParamDictionary("SolipsisPanel2DButtonOverlayElement"))
    {
        addBaseParameters();
    }
}

//-------------------------------------------------------------------------------------
Panel2DButtonOverlayElement::~Panel2DButtonOverlayElement()
{
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::addBaseParameters(void)
{
    PanelOverlayElement::addBaseParameters();
    ParamDictionary* dict = getParamDictionary();

    dict->addParameter(ParameterDef("action", 
        "The action of the button"
        , PT_STRING),
        &msCmdAction);
    dict->addParameter(ParameterDef("action_event", 
        "The action of the button when event occurred"
        , PT_STRING),
        &msCmdActionEvent);
    dict->addParameter(ParameterDef("uv_coords_default", 
        "The default texture coordinates for the texture. 1 set of uv values."
        , PT_STRING),
        &msCmdUVCoordsDefault);
    dict->addParameter(ParameterDef("uv_coords_event", 
        "The texture coordinates for the texture when event occurred. 1 set of uv values."
        , PT_STRING),
        &msCmdUVCoordsEvent);
    dict->addParameter(ParameterDef("uv_coords_inc_hover", 
        "The increment texture coordinates for the texture when mouse is over. uv values."
        , PT_STRING),
        &msCmdUVCoordsIncHover);
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setAction(String action)
{
    mAction = action;
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::getAction(void) const
{
    return mAction;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setActionEvent(String actionEvent)
{
    mActionEvent = actionEvent;
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::getActionEvent(void) const
{
    return mActionEvent;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setUVDefault(Real u1, Real v1, Real u2, Real v2)
{
	mDefaultU1 = u1;
	mDefaultU2 = u2;
	mDefaultV1 = v1;
	mDefaultV2 = v2;
	setUV(u1, v1, u2, v2);
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::getUVDefault(Real& u1, Real& v1, Real& u2, Real& v2) const
{
	u1 = mDefaultU1;
	u2 = mDefaultU2;
	v1 = mDefaultV1;
	v2 = mDefaultV2;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setUVEvent(Real u1, Real v1, Real u2, Real v2)
{
	mEventU1 = u1;
	mEventU2 = u2;
	mEventV1 = v1;
	mEventV2 = v2;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::getUVEvent(Real& u1, Real& v1, Real& u2, Real& v2) const
{
	u1 = mEventU1;
	u2 = mEventU2;
	v1 = mEventV1;
	v2 = mEventV2;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setUVIncHover(Real u, Real v)
{
	mIncHoverU = u;
	mIncHoverV = v;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::getUVIncHover(Real& u, Real& v) const
{
	u = mIncHoverU;
	v = mIncHoverV;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::updateUV()
{
    if (!mEventOccurred)
    {
        if (mHover)
            setUV(mDefaultU1 + mIncHoverU, mDefaultV1 + mIncHoverV, mDefaultU2 + mIncHoverU, mDefaultV2 + mIncHoverV);
        else
            setUV(mDefaultU1, mDefaultV1, mDefaultU2, mDefaultV2);
    }
    else
    {
        if (mHover)
            setUV(mEventU1 + mIncHoverU, mEventV1 + mIncHoverV, mEventU2 + mIncHoverU, mEventV2 + mIncHoverV);
        else
            setUV(mEventU1, mEventV1, mEventU2, mEventV2);
    }
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setEventState(bool eventOccurred)
{
    if (eventOccurred == mEventOccurred)
        return;
    mEventOccurred = eventOccurred;
    updateUV();
}

//-------------------------------------------------------------------------------------
bool Panel2DButtonOverlayElement::getEventState()
{
    return mEventOccurred;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::setHoverState(bool hover)
{
    if (hover == mHover)
        return;
    mHover = hover;
    updateUV();
}

//-------------------------------------------------------------------------------------
bool Panel2DButtonOverlayElement::getHoverState()
{
    return mHover;
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::getCurrentAction()
{
    if (mEventOccurred)
        return mActionEvent;
    return mAction;
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::CmdAction::doGet(const void* target) const
{
	const Panel2DButtonOverlayElement* t = static_cast<const Panel2DButtonOverlayElement*>(target);
    return t->getAction();
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::CmdAction::doSet(void* target, const String& val)
{
    static_cast<Panel2DButtonOverlayElement*>(target)->setAction(val);
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::CmdActionEvent::doGet(const void* target) const
{
	const Panel2DButtonOverlayElement* t = static_cast<const Panel2DButtonOverlayElement*>(target);
    return t->getActionEvent();
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::CmdActionEvent::doSet(void* target, const String& val)
{
    static_cast<Panel2DButtonOverlayElement*>(target)->setActionEvent(val);
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::CmdUVCoordsDefault::doGet(const void* target) const
{
    Real u1, v1, u2, v2;

    static_cast<const Panel2DButtonOverlayElement*>(target)->getUVDefault(u1, v1, u2, v2);
    String ret = " " + StringConverter::toString(u1) + " "
         + StringConverter::toString(v1) + " " + StringConverter::toString(u2) + " "
         + StringConverter::toString(v2);

    return ret;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::CmdUVCoordsDefault::doSet(void* target, const String& val)
{
    std::vector<String> vec = StringUtil::split(val);

    static_cast<Panel2DButtonOverlayElement*>(target)->setUVDefault(
        StringConverter::parseReal(vec[0]),
        StringConverter::parseReal(vec[1]),
        StringConverter::parseReal(vec[2]),
        StringConverter::parseReal(vec[3])
        );
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::CmdUVCoordsEvent::doGet(const void* target) const
{
    Real u1, v1, u2, v2;

    static_cast<const Panel2DButtonOverlayElement*>(target)->getUVEvent(u1, v1, u2, v2);
    String ret = " " + StringConverter::toString(u1) + " "
         + StringConverter::toString(v1) + " " + StringConverter::toString(u2) + " "
         + StringConverter::toString(v2);

    return ret;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::CmdUVCoordsEvent::doSet(void* target, const String& val)
{
    std::vector<String> vec = StringUtil::split(val);

    static_cast<Panel2DButtonOverlayElement*>(target)->setUVEvent(
        StringConverter::parseReal(vec[0]),
        StringConverter::parseReal(vec[1]),
        StringConverter::parseReal(vec[2]),
        StringConverter::parseReal(vec[3])
        );
}

//-------------------------------------------------------------------------------------
String Panel2DButtonOverlayElement::CmdUVCoordsIncHover::doGet(const void* target) const
{
    Real u, v;

    static_cast<const Panel2DButtonOverlayElement*>(target)->getUVIncHover(u, v);
    String ret = " " + StringConverter::toString(u) + " "
         + StringConverter::toString(v);

    return ret;
}

//-------------------------------------------------------------------------------------
void Panel2DButtonOverlayElement::CmdUVCoordsIncHover::doSet(void* target, const String& val)
{
    std::vector<String> vec = StringUtil::split(val);

    static_cast<Panel2DButtonOverlayElement*>(target)->setUVIncHover(
        StringConverter::parseReal(vec[0]),
        StringConverter::parseReal(vec[1])
        );
}

//-------------------------------------------------------------------------------------
const String& Panel2DButtonOverlayElement::getTypeName(void) const
{
    return msTypeName;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
