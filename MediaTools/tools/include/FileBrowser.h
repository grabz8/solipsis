#ifndef __FileBrowser_h__
#define __FileBrowser_h__

#include <String>

namespace FileBrowser
{

	///brief Method which display a window in which the user can browse the files and folders on his disks and choose an place to save a file.
	///breif	param pFilter : filtre files with a specicfic extension (ex : pFilter = "Solipsis Files (*.saf)\0*.saf\0" )
	///breif	param pExtension : extension of the file saving (ex : pExtension = "*.saf")
	///return The path of the file choosed by the user, can be an invalid path ! If the user click on Cancel, then NULL is returned.
	char* displayWindowForSaving(const char * pFilter, std::string pExtension );
		
	///brief Method which display a window in which the user can browse the files and folders on his disks and choose an place to load a file.
	///breif	param pFilter : filtre files with a specicfic extension (ex : pFilter = "Solipsis Files (*.saf)\0*.saf\0" )
	///breif	param pExtension : extension of the file saving (ex : pExtension = "*.saf")
	///return The path of the file choosed by the user, can be an invalid path ! If the user click on Cancel, then NULL is returned.
	char* displayWindowForLoading(const char * pFilter , std::string pExtension);
		
	///brief Method which display a window in which the user can browse the files and folders on his disks and choose an place to save an image (jpg, bmp...).
	///return The path of the image choosed by the user, should return a valid path. If the user click on Cancel, then NULL is returned.
	char* browseImageToSave();

	///brief Method which display a window with a message and title passed in parameter.
	///param title Title of the window.
	///param message Message to display in the window.
	void displayMessageWindow(const char* title, const char* message);
}


#endif
