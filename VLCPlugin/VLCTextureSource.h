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

#ifndef __VLCTextureSource_h__
#define __VLCTextureSource_h__

#include <ExternalTextureSourceEx.h>
#include <Event.h>

namespace Solipsis {

class VLCPlugin;

/**
 *  Provides an external VLC texture source for Ogre materials.
 *  Example usage of VLC textures from materials:<br>
 *  \code
    texture_unit
    {
        texture_source vlc
        {
            mrl video.mpg
            width 256
            height 256
            frames_per_second 25
            vlc_params <additional VLC parameters>
        }
    }
 *  \endcode
 *  Refer to VLC manual for mrl and params
 *   local media file : mrl <filename>
 *   udp stream : mrl udp://@<ip>:<port>
 *   http stream : mrl http://<ip>:<port>
 *   directShow webcam : dshow:// :dshow-vdev="" :dshow-adev="" :dshow-size=""
 *  To stream the video you can use the VLC standard parameters, take care to define ALWAYS the destination display vmem in your duplicate list,
 *  for eg. to stream the video on HTTP (here video is transcoded into MPEG2 for HTTP, ...so... ! CPU consuming !):
 *   vlc_params --sout #transcode{vcodec=mp2v,vb=1024,scale=1,acodec=mpga,ab=192,channels=2}:duplicate{dst=display{vmem},dst=std{access=http,mux=ts,dst=127.0.0.1:8080}}
 *  same eg. but with mpeg video (no transcodage)
 *   vlc_params --sout #duplicate{dst=display{vmem},dst=std{access=http,mux=ts,dst=127.0.0.1:8080}}
 */
class VLCTextureSource : public ExternalTextureSourceEx
{
public:
    /** Constructor
    @param[in] plugin The VLC plugin that owns the texture source
    */
    VLCTextureSource(VLCPlugin* plugin);

    /** Reports that a VLC instance has been destroyed
    @param[in] id Instance id
    */
    void instanceDestroyed(int id);

    /** Clear all VLC instances
    */
    void clearInstances();

	//------------------------------------------------------------------------------//
	/* Command objects for specifying some base features							*/
    class _OgrePrivate CmdMrl : public Ogre::ParamCommand
    {
    public:
		Ogre::String doGet(const void* target) const;
        void doSet(void* target, const Ogre::String& val);
    };
    class _OgrePrivate CmdWidth : public Ogre::ParamCommand
    {
    public:
		Ogre::String doGet(const void* target) const;
        void doSet(void* target, const Ogre::String& val);
    };
    class _OgrePrivate CmdHeight : public Ogre::ParamCommand
    {
    public:
		Ogre::String doGet(const void* target) const;
        void doSet(void* target, const Ogre::String& val);
    };
    class _OgrePrivate CmdVlcParams : public Ogre::ParamCommand
    {
    public:
		Ogre::String doGet(const void* target) const;
        void doSet(void* target, const Ogre::String& val);
    };

    // Get/set wrappers for VLC textures source commands.
    // Can be used directly before creating a VLC texture if the material
    // doesn't have required VLC parameters set
	//! Sets the media resource link
    void setMrl(const Ogre::String& iMrl) { mMrl = iMrl; }
	//! Gets currently set media resource link
    const Ogre::String& getMrl() const { return mMrl; }
	//! Sets the video width
	void setWidth(int iWidth) { mWidth = iWidth; }
	//! Gets currently set video width
	const int getWidth() const { return mWidth; }
	//! Sets the video height
	void setHeight(int iHeight) { mHeight = iHeight; }
	//! Gets currently set video height
	const int getHeight() const { return mHeight; }
	//! Sets the additional VLC parameters
    void setVlcParams(const Ogre::String& iVlcParams) { mVlcParams = iVlcParams; }
	//! Gets currently set additional VLC parameters
    const Ogre::String& getVlcParams() const { return mVlcParams; }

protected:
    /// @copydoc Ogre::ExternalTextureSource::initialise
    bool initialise();
    /// @copydoc Ogre::ExternalTextureSource::shutDown
    void shutDown();

    /// @copydoc Ogre::ExternalTextureSource::createDefinedTexture
    void createDefinedTexture(const Ogre::String& material, const Ogre::String& group);
    /// @copydoc Ogre::ExternalTextureSource::destroyAdvancedTexture
    void destroyAdvancedTexture(const Ogre::String& material, const Ogre::String& group);
    /// @copydoc Solipsis::ExternalTextureSourceEx::handleEvt
    Ogre::String handleEvt(const Ogre::String& material, const Ogre::String& evt);
    /// @copydoc Solipsis::ExternalTextureSourceEx::handleEvt
    void handleEvt(const Ogre::String& material, const Event& evt) {}

protected:
	static CmdMrl msCmdMrl;             //! Command for setting media resource link
	static CmdWidth msCmdWidth;         //! Command for setting video width
	static CmdHeight msCmdHeight;       //! Command for setting video height
	static CmdVlcParams msCmdVlcParams; //! Command for setting additional VLC parameters

    /// VLC plugin
    VLCPlugin* mPlugin;

    /// Media resource link
    Ogre::String mMrl;
    /// Video width
    int mWidth;
    /// Video height
    int mHeight;
    /// Additional VLC parameters
    Ogre::String mVlcParams;

    typedef std::vector<Ogre::MaterialPtr> MaterialList;
    typedef std::map<int, MaterialList> MaterialListMap;
    /// list of materials using each VLC texture instance identified by its ID
    MaterialListMap mMaterials;
};

} // namespace Solipsis

#endif // #ifndef __VLCTextureSource_h__
