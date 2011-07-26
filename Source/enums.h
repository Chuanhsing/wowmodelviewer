#ifndef ENUMS_H
#define ENUMS_H

enum InterfaceType
{
	INTERFACETYPE_NONE,			// No File
	INTERFACETYPE_CHARACTER,	// Character M2 File
	INTERFACETYPE_CREATURE,		// Creatures & NPC M2 Files
	INTERFACETYPE_ITEM,			// M2 Models that aren't a Character, Creature or NPC.
	INTERFACETYPE_SET,			// WMO Files
	INTERFACETYPE_LANDSCAPE,	// ADT Files
	INTERFACETYPE_IMAGE,		// Image Files
	INTERFACETYPE_SOUND		// Sound Files
};

enum FileType
{
	FILETYPE_MODEL,		// M2 Files
	FILETYPE_SET,		// WMO Files
	FILETYPE_LANDSCAPE,	// ADT Files
	FILETYPE_IMAGE,		// BLP Files
	FILETYPE_SOUND		// WAV, OGG & MP3 Files
};

enum WoWVersion
{
	WOW_VANILLA = 10000,
	WOW_TBC = 20000,
	WOW_WOTLK = 30000,
	WOW_CATACLYSM = 40000
};

#endif
