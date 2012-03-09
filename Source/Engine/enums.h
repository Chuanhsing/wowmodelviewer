#ifndef ENUMS_H
#define ENUMS_H

// Viewer Interface Mode
enum InterfaceMode
{
	INTERFACEMODE_VIEWER = 0,	// Default/Classic Viewer mode
	INTERFACEMODE_CINEMA		// Cinema Mode, for Machinima & the like.
};

// Type of the loaded File
enum FileType
{
	FILETYPE_NONE = 0,			// No File
	FILETYPE_MODEL,				// Animatable Models
	FILETYPE_MODEL_STATIC,		// Static Models
	FILETYPE_SET,				// Large Static models, usually used for buildings.
	FILETYPE_LANDSCAPE,			// Landscape/ground planes
	FILETYPE_IMAGE,				// Image Files
	FILETYPE_SOUND				// Sound Files
};

// Image Texture Types
enum ImageTextureType
{
	TEXTURETYPE_COLOR = 0,		// A color map indicating the color of the model.
	TEXTURETYPE_DIFFUSE,		// A grayscale map that controls how much light should affect the model. Often used for Ambient Occlusion.
	TEXTURETYPE_EMISSIVE,		// A grayscale map for modifying the areas of a model that are lit up.
	TEXTURETYPE_SPECULAR,		// A grayscale map indicating the shinyness of a model
	TEXTURETYPE_GLOSS,			// A grayscale map used for manipulating the sharpness of the specularity
	TEXTURETYPE_REFLECTIVE,		// A grayscale map depicting reflective areas
	TEXTURETYPE_TRANSPARENT,	// A grayscale transparancy map
	TEXTURETYPE_NORMAL,			// A color Normal map
	TEXTURETYPE_DISPLACEMENT	// A grayscale map depicting the displacement or "bump" of a model.
};


// --== Error Codes ==--

// Reading and loading models.
enum ModelReadErrors
{
	MODELREADERROR_OKAY = 0,
	MODELREADERROR_FILENOTFOUND,
	MODELREADERROR_UNKNOWNFORMAT,
	MODELREADERROR_INVALIDFORMAT,
	MODELREADERROR_CANTOPENFILE,
	MODELREADERROR_CANTREADFILE,
	MODELREADERROR_CANTOPENARCHIVE,
	MODELREADERROR_CANTREADARCHIVE
};

#endif
