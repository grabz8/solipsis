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

#ifndef __Panel2DOverlayElement_h__
#define __Panel2DOverlayElement_h__

#include <OgreBorderPanelOverlayElement.h>
#include <OgreOverlayElementFactory.h>

using namespace Ogre;

namespace Solipsis {

class Panel2DOverlayElement : public BorderPanelOverlayElement
{
public:
    /** Constructor */
    Panel2DOverlayElement(const String& name);
    virtual ~Panel2DOverlayElement();

	const String& getTypeName(void) const;

    /** Sets the size of the title border.
    @remarks
        This method sets the size of border used to resize the panel.
        Remember that the dimensions specified here are in relation to the size of
        the screen, so 0.1 is 1/10th of the screen width or height. Also note that because
        most screen resolutions are 1.333:1 width:height ratio that using the same
        border size will look slightly bigger across than up.
    @param size The size of the border as a factor of the screen dimensions ie 0.2 is one-fifth
        of the screen size.
    */
    void setTitleBorderSize(Real size);

    /** Gets the size of the border used to resize the panel. */
    Real getTitleBorderSize(void) const;

    /** Sets the minimal width of the panel.
    @param width The minimal width of the panel.
    */
    void setMinWidth(Real width);

    /** Gets the minimal width of the panel. */
    Real getMinWidth(void) const;

    /** Sets the minimal height of the panel.
    @param height The minimal height of the panel.
    */
    void setMinHeight(Real height);

    /** Gets the minimal height of the panel. */
    Real getMinHeight(void) const;


    /** Command object for specifying size of the title border (see ParamCommand).*/
    class _OgrePrivate CmdTitleBorderSize : public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };
    /** Command object for specifying minimal width of the panel (see ParamCommand).*/
    class _OgrePrivate CmdMinWidth : public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };
    /** Command object for specifying minimal height of the panel (see ParamCommand).*/
    class _OgrePrivate CmdMinHeight : public ParamCommand
    {
    public:
        String doGet(const void* target) const;
        void doSet(void* target, const String& val);
    };

protected:
    Real mTitleBorderSize;
    Real mMinWidth;
    Real mMinHeight;

    static String msTypeName;

    /// Internal method for setting up parameters
    void addBaseParameters(void);

    // Command objects
    static CmdTitleBorderSize msCmdTitleBorderSize;
    static CmdMinWidth msCmdMinWidth;
    static CmdMinHeight msCmdMinHeight;
};

/** Factory for creating Panel2DOverlayElement instances. */
class Panel2DOverlayElementFactory: public OverlayElementFactory
{
public:
    /** See OverlayElementFactory */
    OverlayElement* createOverlayElement(const String& instanceName)
    {
        return OGRE_NEW Panel2DOverlayElement(instanceName);
    }
    /** See OverlayElementFactory */
    const String& getTypeName(void) const
    {
        static String name = "SolipsisPanel2D";
        return name;
    }
};

} // namespace Solipsis

#endif // #ifndef __Panel2DOverlayElement_h__