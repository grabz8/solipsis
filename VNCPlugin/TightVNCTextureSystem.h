/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

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

#ifndef _TIGHTVNCTEXTURESYSTEM_H_
#define _TIGHTVNCTEXTURESYSTEM_H_

// GREG BEGIN
//#include <OgreExternalTextureSource.h>
#include <ExternalTextureSourceEx.h>
#include <Event.h>
// GREG END

namespace Solipsis
{
class VNCPlugin;
}

/**
 *  Provides an external VNC texture source for Ogre materials.
 *  Example usage of VNC textures from materials:<br>
 *  \code
    texture_unit
    {
        texture_source vnc
        {
            address vnc://192.168.0.1:5900
            password secret
        }
    }
 *  \endcode
 */
// GREG BEGIN
//class TightVNCTextureSystem : public Ogre::ExternalTextureSource
class TightVNCTextureSystem : public Solipsis::ExternalTextureSourceEx
// GREG END
{
public:
    /**
     *  @param plugin The TightVNC plugin that owns the texture system
     */
    TightVNCTextureSystem(Solipsis::VNCPlugin* plugin);

    /**
     *	@param id Connection id
     *  @param textureName VNC texture name
     */
    void connectionCreated(int id, const Ogre::String& textureName);

    /**
     *  Reports that a VNC connection has been closed (or canceled)
     *  @param id Connection id
     */
    void connectionClosed(int id);

    void clearConnections();

    // Get/set wrappers for VNC textures source commands.
    // Can be used directly before creating a VNC texture if the material
    // doesn't have required VNC parameters set
    void setAddress(const Ogre::String& addr);
    const Ogre::String& getAddress() const { return mAddress; }

// GREG BEGIN
    void setPwd(const Ogre::String& pwd);
    const Ogre::String& getPwd() const { return mPwd; }
// GREG END

protected:
    bool initialise();
    void shutDown();

    void createDefinedTexture(const Ogre::String& material, const Ogre::String& group);
    void destroyAdvancedTexture(const Ogre::String& material, const Ogre::String& group);

// GREG BEGIN
    /// @copydoc Solipsis::ExternalTextureSourceEx::handleEvt
    Ogre::String handleEvt(const Ogre::String& material, const Ogre::String& evt) { return ""; }
    /// @copydoc Solipsis::ExternalTextureSourceEx::handleEvt
    void handleEvt(const Ogre::String& material, const Solipsis::Event& evt);
// GREG END

    bool requiresAuthorization() const;

private:
    // ------------------------------------------------------------------------
    // Material commands to set VNC connection parameters

    struct _OgrePrivate CmdAddress : public Ogre::ParamCommand
    {
        Ogre::String doGet(const void* target) const
        {
            return ((TightVNCTextureSystem*)target)->getAddress();
        }

        void doSet(void* target, const Ogre::String& val)
        {
            ((TightVNCTextureSystem*)target)->setAddress(val);
        }
    };
// GREG BEGIN
    struct _OgrePrivate CmdPwd : public Ogre::ParamCommand
    {
        Ogre::String doGet(const void* target) const
        {
            return ((TightVNCTextureSystem*)target)->getPwd();
        }

        void doSet(void* target, const Ogre::String& val)
        {
            ((TightVNCTextureSystem*)target)->setPwd(val);
        }
    };
// GREG END

    // ------------------------------------------------------------------------

    Solipsis::VNCPlugin* mPlugin;

    CmdAddress mCmdAddr;
    Ogre::String mAddress;
// GREG BEGIN
    CmdPwd mCmdPwd;
    Ogre::String mPwd;
// GREG END

    typedef std::vector<Ogre::MaterialPtr> MaterialList;
    typedef std::map<int, MaterialList> IDMaterialMap;
    IDMaterialMap mMaterials;

};  //  class TightVNCTextureSystem

#endif  //  _TIGHTVNCTEXTURESYSTEM_H_
