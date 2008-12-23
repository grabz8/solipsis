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

#ifndef __SWFTextureSource_h__
#define __SWFTextureSource_h__

#include <ExternalTextureSourceEx.h>
#include <Event.h>

namespace Solipsis {

class SWFPlugin;

/**
 *  Provides an external SWF texture source for Ogre materials.
 *  Example usage of SWF textures from materials:<br>
 *  \code
    texture_unit
    {
        texture_source swf
        {
            mrl video.mpg
            width 256
            height 256
            frames_per_second 25
            swf_params <additional SWF parameters>
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
class SWFTextureSource : public ExternalTextureSourceEx
{
public:
    /** Constructor
    @param[in] plugin The SWF plugin that owns the texture source
    */
    SWFTextureSource(SWFPlugin* plugin);

    /** Reports that a SWF instance has been destroyed
    @param[in] id Instance id
    */
    void instanceDestroyed(int id);

    /** Clear all SWF instances
    */
    void clearInstances();

	//------------------------------------------------------------------------------//
	/* Command objects for specifying some base features							*/
    class _OgrePrivate CmdUrl : public Ogre::ParamCommand
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
    class _OgrePrivate CmdSwfParams : public Ogre::ParamCommand
    {
    public:
		Ogre::String doGet(const void* target) const;
        void doSet(void* target, const Ogre::String& val);
    };

    // Get/set wrappers for SWF textures source commands.
    // Can be used directly before creating a SWF texture if the material
    // doesn't have required SWF parameters set
	//! Sets the media resource link
    void setUrl(const Ogre::String& iUrl) { mUrl = iUrl; }
	//! Gets currently set media resource link
    const Ogre::String& getUrl() const { return mUrl; }
	//! Sets the video width
	void setWidth(int iWidth) { mWidth = iWidth; }
	//! Gets currently set video width
	const int getWidth() const { return mWidth; }
	//! Sets the video height
	void setHeight(int iHeight) { mHeight = iHeight; }
	//! Gets currently set video height
	const int getHeight() const { return mHeight; }
	//! Sets the additional SWF parameters
    void setSwfParams(const Ogre::String& iSwfParams) { mSwfParams = iSwfParams; }
	//! Gets currently set additional SWF parameters
    const Ogre::String& getSwfParams() const { return mSwfParams; }

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
    Ogre::String handleEvt(const Ogre::String& material, const Ogre::String& evt);//  { return ""; }
    /// @copydoc Solipsis::ExternalTextureSourceEx::handleEvt
    void handleEvt(const Ogre::String& material, const Event& evt);// {}

protected:

	static CmdUrl msCmdUrl;             //! Command for setting media resource link
	static CmdWidth msCmdWidth;         //! Command for setting video width
	static CmdHeight msCmdHeight;       //! Command for setting video height
	static CmdSwfParams msCmdSwfParams; //! Command for setting additional SWF parameters

    /// SWF plugin
    SWFPlugin* mPlugin;

    /// Media resource link
    Ogre::String mUrl;
    /// Video width
    int mWidth;
    /// Video height
    int mHeight;
    /// Additional SWF parameters
    Ogre::String mSwfParams;

    typedef std::vector<Ogre::MaterialPtr> MaterialList;
    typedef std::map<int, MaterialList> MaterialListMap;
    /// list of materials using each SWF texture instance identified by its ID
    MaterialListMap mMaterials;
};

} // namespace Solipsis

#endif // #ifndef __SWFTextureSource_h__
