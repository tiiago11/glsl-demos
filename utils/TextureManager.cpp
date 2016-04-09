//**********************************************
//Singleton Texture Manager class
//Written by Ben English
//benjamin.english@oit.edu
//
//For use with OpenGL and the FreeImage library
//**********************************************

#include "TextureManager.h"

#include <iostream>

using namespace std;

TextureManager* TextureManager::m_inst(0);

TextureManager* TextureManager::Inst()
{
	if(!m_inst)
		m_inst = new TextureManager();

	return m_inst;
}

TextureManager::TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();
	#endif
}

//these should never be called
//TextureManager::TextureManager(const TextureManager& tm){}
//TextureManager& TextureManager::operator=(const TextureManager& tm){}
	
TextureManager::~TextureManager()
{
	// call this ONLY when linking with FreeImage as a static library
	#ifdef FREEIMAGE_LIB
		FreeImage_DeInitialise();
	#endif

	UnloadAllTextures();
	m_inst = 0;
}

bool TextureManager::LoadTexture(const char* filename, const unsigned int texID, GLenum image_format, GLint internal_format, GLint level, GLint border)
{
	// Determine the format of the image.
	// Note: The second paramter ('size') is currently unused, and we should use 0 for it.
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	cout << format << endl;

	// Image not found? Abort! Without this section we get a 0 by 0 image with 0 bits-per-pixel but we don't abort, which
	// you might find preferable to dumping the user back to the desktop.
	if (format == -1)
	{
		cout << "Could not find image: " << filename << " - Aborting." << endl;
		return false;
	}

	// Found image, but couldn't determine the file format? Try again...
	if (format == FIF_UNKNOWN)
	{
		cout << "Couldn't determine file format - attempting to get from file extension..." << endl;

		// ...by getting the filetype from the filename extension (i.e. .PNG, .GIF etc.)
		// Note: This is slower and more error-prone that getting it from the file itself,
		// also, we can't use the 'U' (unicode) variant of this method as that's Windows only.
		format = FreeImage_GetFIFFromFilename(filename);

		// Check that the plugin has reading capabilities for this format (if it's FIF_UNKNOWN,
		// for example, then it won't have) - if we can't read the file, then we bail out =(
		if (!FreeImage_FIFSupportsReading(format))
		{
			cout << "Detected image format cannot be read!" << endl;
			return false;
		}
	}

	// If we're here we have a known image format, so load the image into a bitap
	FIBITMAP* bitmap = FreeImage_Load(format, filename);

	// How many bits-per-pixel is the source image?
	int bitsPerPixel = FreeImage_GetBPP(bitmap);


	cout << "Color type: ";

	switch (FreeImage_GetColorType(bitmap))
	{
	case FIC_MINISWHITE:
		cout << "FIC_MINISWHITE";
		break;
	case FIC_MINISBLACK:
		cout << "FIC_MINISBLACK";
		break;
	case FIC_RGB:
		cout << "FIC_RGB";
		break;
	case FIC_PALETTE:
		cout << "FIC_PALETTE";
		break;
	case FIC_RGBALPHA:
		cout << "FIC_RGBALPHA";
		break;
	case FIC_CMYK:
		cout << "FIC_CMYK";
		break;
	default:
		break;
	}

	cout << endl;

	cout << "Image type: ";

	switch (FreeImage_GetImageType(bitmap))
	{
	case FIT_UNKNOWN:
		cout << "FIT_UNKNOWN";
		break;
	case FIT_BITMAP:
		cout << "FIT_BITMAP";
		break;
	case FIT_UINT16:
		cout << "FIT_UINT16";
		break;
	case FIT_INT16:
		cout << "FIT_INT16";
		break;
	case FIT_UINT32:
		cout << "FIT_UINT32";
		break;
	case FIT_INT32:
		cout << "FIT_INT32";
		break;
	case FIT_FLOAT:
		cout << "FIT_FLOAT";
		break;
	case FIT_DOUBLE:
		cout << "FIT_DOUBLE";
		break;
	case FIT_COMPLEX:
		cout << "FIT_COMPLEX";
		break;
	case FIT_RGB16:
		cout << "FIT_RGB16";
		break;
	case FIT_RGBA16:
		cout << "FIT_RGBA16";
		break;
	case FIT_RGBF:
		cout << "FIT_RGBF";
		break;
	case FIT_RGBAF:
		cout << "FIT_RGBAF";
		break;
	default:
		break;
	}

	cout << endl;

	FIBITMAP* convertedbitmap = FreeImage_ConvertToStandardType(bitmap, true);

	FreeImage_Unload(bitmap);

	bitmap = convertedbitmap;


	cout << "Converted image type: ";

	switch (FreeImage_GetImageType(bitmap))
	{
	case FIT_UNKNOWN:
		cout << "FIT_UNKNOWN";
		break;
	case FIT_BITMAP:
		cout << "FIT_BITMAP";
		break;
	case FIT_UINT16:
		cout << "FIT_UINT16";
		break;
	case FIT_INT16:
		cout << "FIT_INT16";
		break;
	case FIT_UINT32:
		cout << "FIT_UINT32";
		break;
	case FIT_INT32:
		cout << "FIT_INT32";
		break;
	case FIT_FLOAT:
		cout << "FIT_FLOAT";
		break;
	case FIT_DOUBLE:
		cout << "FIT_DOUBLE";
		break;
	case FIT_COMPLEX:
		cout << "FIT_COMPLEX";
		break;
	case FIT_RGB16:
		cout << "FIT_RGB16";
		break;
	case FIT_RGBA16:
		cout << "FIT_RGBA16";
		break;
	case FIT_RGBF:
		cout << "FIT_RGBF";
		break;
	case FIT_RGBAF:
		cout << "FIT_RGBAF";
		break;
	default:
		break;
	}

	cout << endl;

	// Convert our image up to 32 bits (8 bits per channel, Red/Green/Blue/Alpha) -
	// but only if the image is not already 32 bits (i.e. 8 bits per channel).
	// Note: ConvertTo32Bits returns a CLONE of the image data - so if we
	// allocate this back to itself without using our bitmap32 intermediate
	// we will LEAK the original bitmap data, and valgrind will show things like this:
	//
	// LEAK SUMMARY:
	//  definitely lost: 24 bytes in 2 blocks
	//  indirectly lost: 1,024,874 bytes in 14 blocks    <--- Ouch.
	//
	// Using our intermediate and cleaning up the initial bitmap data we get:
	//
	// LEAK SUMMARY:
	//  definitely lost: 16 bytes in 1 blocks
	//  indirectly lost: 176 bytes in 4 blocks
	//
	// All above leaks (192 bytes) are caused by XGetDefault (in /usr/lib/libX11.so.6.3.0) - we have no control over this.
	//
	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32)
	{
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Skipping conversion." << endl;
		bitmap32 = bitmap;
	}
	else
	{
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Converting to 32-bit colour." << endl;
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
		if (bitmap32 == NULL)
		{
			cout << "Unable to convert image to 32bits." << endl;
			bitmap32 = FreeImage_ConvertTo24Bits(bitmap);
			if (bitmap32 == NULL)
			{
				cout << "Unable to convert image to 24bits." << endl;
				return false;
			}
		}
	}

	// Some basic image info - strip it out if you don't care
	int imageWidth = FreeImage_GetWidth(bitmap32);
	int imageHeight = FreeImage_GetHeight(bitmap32);
	cout << "Image: " << filename << " is size: " << imageWidth << "x" << imageHeight << "." << endl;


	size.x = imageWidth;
	size.y = imageHeight;

	// Get a pointer to the texture data as an array of unsigned bytes.
	// Note: At this point bitmap32 ALWAYS holds a 32-bit colour version of our image - so we get our data from that.
	// Also, we don't need to delete or delete[] this textureData because it's not on the heap (so attempting to do
	// so will cause a crash) - just let it go out of scope and the memory will be returned to the stack.
	GLubyte* textureData = FreeImage_GetBits(bitmap32);

	// Generate a texture ID and bind to it
	//GLuint tempTextureID;
	//glGenTextures(1, &tempTextureID);
	//glBindTexture(GL_TEXTURE_2D, tempTextureID);

	// Construct the texture.
	// Note: The 'Data format' is the format of the image data as provided by the image library. FreeImage decodes images into
	// BGR/BGRA format, but we want to work with it in the more common RGBA format, so we specify the 'Internal format' as such.
	//glTexImage2D(GL_TEXTURE_2D,    // Type of texture
	//	0,                // Mipmap level (0 being the top level i.e. full size)
	//	GL_RGBA,          // Internal format
	//	imageWidth,       // Width of the texture
	//	imageHeight,      // Height of the texture,
	//	0,                // Border in pixels
	//	GL_BGRA,      // Data format
	//	GL_UNSIGNED_BYTE, // Type of texture data
	//	textureData);     // The image data to use for this texture


	//image format
	/*FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	//OpenGL's image ID to map to
	GLuint gl_texID;
	
	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if(fif == FIF_UNKNOWN) 
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if(fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if(!dib)
		return false;

	//dib = FreeImage_ConvertTo24Bits(dib);
	//image_format = GL_RGB;
	//internal_format = GL_RGB;

	//dib = FreeImage_ConvertToGreyscale(dib);
	//dib = FreeImage_ConvertToRGB16(dib);
	//image_format = GL_RGB16;
	//internal_format = GL_RGB16;

	//dib = FreeImage_ConvertTo32Bits(dib);
	//image_format = GL_RGBA;
	//internal_format = GL_RGBA;

	

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if((bits == 0) || (width == 0) || (height == 0))
		return false;

	cout << "Width: " << width << endl << "Height: " << height << endl;

	*/

	//OpenGL's image ID to map to
	GLuint gl_texID;

	//if this texture ID is in use, unload the current texture
	if(m_texID.find(texID) != m_texID.end())
		glDeleteTextures(1, &(m_texID[texID]));

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//store the texture ID mapping
	m_texID[texID] = gl_texID;
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use
	//glTexImage2D(GL_TEXTURE_2D, level, internal_format, imageWidth, imageHeight,
	//	border, image_format, GL_UNSIGNED_BYTE, textureData);

	glTexImage2D(GL_TEXTURE_2D,    // Type of texture
		0,                // Mipmap level (0 being the top level i.e. full size)
		GL_RGBA,          // Internal format
		imageWidth,       // Width of the texture
		imageHeight,      // Height of the texture,
		0,                // Border in pixels
		GL_BGRA,		  // Data format
		GL_UNSIGNED_BYTE, // Type of texture data
		textureData);     // The image data to use for this texture

	//Free FreeImage's copy of the data
	//FreeImage_Unload(dib);

	// Specify our minification and magnification filters
	GLint minificationFilter = GL_NEAREST;
	GLint magnificationFilter = GL_NEAREST;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnificationFilter);

	// If we're using MipMaps, then we'll generate them here.
	// Note: The glGenerateMipmap call requires OpenGL 3.0 as a minimum.
	if (minificationFilter == GL_LINEAR_MIPMAP_LINEAR ||
		minificationFilter == GL_LINEAR_MIPMAP_NEAREST ||
		minificationFilter == GL_NEAREST_MIPMAP_LINEAR ||
		minificationFilter == GL_NEAREST_MIPMAP_NEAREST)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// Check for OpenGL texture creation errors
	GLenum glError = glGetError();
	if (glError)
	{
		cout << "There was an error loading the texture: " << filename << endl;

		switch (glError)
		{
		case GL_INVALID_ENUM:
			cout << "Invalid enum." << endl;
			break;

		case GL_INVALID_VALUE:
			cout << "Invalid value." << endl;
			break;

		case GL_INVALID_OPERATION:
			cout << "Invalid operation." << endl;

		default:
			cout << "Unrecognised GLenum." << endl;
			break;
		}

		cout << "See https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml for further details." << endl;
	}

	// Unload the 32-bit colour bitmap
	FreeImage_Unload(bitmap32);

	// If we had to do a conversion to 32-bit colour, then unload the original
	// non-32-bit-colour version of the image data too. Otherwise, bitmap32 and
	// bitmap point at the same data, and that data's already been free'd, so
	// don't attempt to free it again! (or we'll crash).
	if (bitsPerPixel != 32)
	{
		FreeImage_Unload(bitmap);
	}

	//return success
	return true;
}

bool TextureManager::UnloadTexture(const unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, unload it's texture, and remove it from the map
	if(m_texID.find(texID) != m_texID.end())
	{
		glDeleteTextures(1, &(m_texID[texID]));
		m_texID.erase(texID);
	}
	//otherwise, unload failed
	else
	{
		result = false;
	}

	return result;
}

bool TextureManager::BindTexture(const unsigned int texID)
{
	bool result(true);
	//if this texture ID mapped, bind it's texture as current
	if(m_texID.find(texID) != m_texID.end())
		glBindTexture(GL_TEXTURE_2D, m_texID[texID]);
	//otherwise, binding failed
	else
		result = false;

	return result;
}

void TextureManager::UnloadAllTextures()
{
	//start at the begginning of the texture map
	std::map<unsigned int, GLuint>::iterator i = m_texID.begin();

	//Unload the textures untill the end of the texture map is found
	while(i != m_texID.end())
		UnloadTexture(i->first);

	//clear the texture map
	m_texID.clear();
}