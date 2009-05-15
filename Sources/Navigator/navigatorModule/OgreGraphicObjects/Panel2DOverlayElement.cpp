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

#include "Panel2DOverlayelement.h"

namespace Solipsis {

String Panel2DOverlayElement::msTypeName = "SolipsisPanel2D";
Panel2DOverlayElement::CmdTitleBorderSize Panel2DOverlayElement::msCmdTitleBorderSize;
Panel2DOverlayElement::CmdMinWidth Panel2DOverlayElement::msCmdMinWidth;
Panel2DOverlayElement::CmdMinHeight Panel2DOverlayElement::msCmdMinHeight;

//-------------------------------------------------------------------------------------
Panel2DOverlayElement::Panel2DOverlayElement(const String& name)
    : BorderPanelOverlayElement(name), 
    mTitleBorderSize(0),
    mMinWidth(100),
    mMinHeight(100)
{
    if (createParamDictionary("SolipsisPanel2DOverlayElement"))
    {
        addBaseParameters();
    }
}

//-------------------------------------------------------------------------------------
Panel2DOverlayElement::~Panel2DOverlayElement()
{
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::addBaseParameters(void)
{
    BorderPanelOverlayElement::addBaseParameters();
    ParamDictionary* dict = getParamDictionary();

    dict->addParameter(ParameterDef("title_border_size", 
        "The size of the title border"
        , PT_STRING),
        &msCmdTitleBorderSize);
    dict->addParameter(ParameterDef("min_width", 
        "The minimal width of the panel"
        , PT_STRING),
        &msCmdMinWidth);
    dict->addParameter(ParameterDef("min_height", 
        "The minimal height of the panel"
        , PT_STRING),
        &msCmdMinHeight);
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::setTitleBorderSize(Real size)
{
    if (mMetricsMode != GMM_RELATIVE)
    {
        mTitleBorderSize = static_cast<unsigned>(size);
    }
    else
    {
        mTitleBorderSize = size;
    }
}

//-------------------------------------------------------------------------------------
Real Panel2DOverlayElement::getTitleBorderSize(void) const
{
    if (mMetricsMode == GMM_PIXELS)
    {
		return mTitleBorderSize;
	}
	else
	{
		return mTitleBorderSize;
	}
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::setMinWidth(Real width)
{
    mMinWidth = width;
}

//-------------------------------------------------------------------------------------
Real Panel2DOverlayElement::getMinWidth(void) const
{
	return mMinWidth;
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::setMinHeight(Real height)
{
    mMinHeight = height;
}

//-------------------------------------------------------------------------------------
Real Panel2DOverlayElement::getMinHeight(void) const
{
	return mMinHeight;
}

//-------------------------------------------------------------------------------------
String Panel2DOverlayElement::CmdTitleBorderSize::doGet(const void* target) const
{
	const Panel2DOverlayElement* t = static_cast<const Panel2DOverlayElement*>(target);
    return String(
		StringConverter::toString(t->getTitleBorderSize())	);
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::CmdTitleBorderSize::doSet(void* target, const String& val)
{
    static_cast<Panel2DOverlayElement*>(target)->setTitleBorderSize(
        StringConverter::parseReal(val)
        );
}

//-------------------------------------------------------------------------------------
String Panel2DOverlayElement::CmdMinWidth::doGet(const void* target) const
{
	const Panel2DOverlayElement* t = static_cast<const Panel2DOverlayElement*>(target);
    return String(
		StringConverter::toString(t->getMinWidth())	);
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::CmdMinWidth::doSet(void* target, const String& val)
{
    static_cast<Panel2DOverlayElement*>(target)->setMinWidth(
        StringConverter::parseReal(val)
        );
}

//-------------------------------------------------------------------------------------
String Panel2DOverlayElement::CmdMinHeight::doGet(const void* target) const
{
	const Panel2DOverlayElement* t = static_cast<const Panel2DOverlayElement*>(target);
    return String(
		StringConverter::toString(t->getMinHeight())	);
}

//-------------------------------------------------------------------------------------
void Panel2DOverlayElement::CmdMinHeight::doSet(void* target, const String& val)
{
    static_cast<Panel2DOverlayElement*>(target)->setMinHeight(
        StringConverter::parseReal(val)
        );
}

//-------------------------------------------------------------------------------------
const String& Panel2DOverlayElement::getTypeName(void) const
{
    return msTypeName;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
