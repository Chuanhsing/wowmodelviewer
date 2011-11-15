#ifndef WMV_ENUMS_H
#define WMV_ENUMS_H

enum InterfaceMode
{
	INTERFACEMODE_VIEWER = 0,	// Default/Classic Viewer mode
	INTERFACEMODE_CINEMA		// Cinema Mode, for Machinima & the like.
};

enum Viewer_InterfaceType
{
	VIEWER_INTERFACETYPE_NONE = 0,		// No File
	VIEWER_INTERFACETYPE_CHARACTER,		// Character M2 File
	VIEWER_INTERFACETYPE_CREATURE,		// Creatures & NPC M2 Files
	VIEWER_INTERFACETYPE_ITEM,			// M2 Models that aren't a Character, Creature or NPC.
	VIEWER_INTERFACETYPE_SET,			// WMO Files
	VIEWER_INTERFACETYPE_LANDSCAPE,		// ADT Files
	VIEWER_INTERFACETYPE_IMAGE,			// Image Files
	VIEWER_INTERFACETYPE_SOUND			// Sound Files
};

enum FileType
{
	FILETYPE_MODEL = 0,	// M2 Files
	FILETYPE_SET,		// WMO Files
	FILETYPE_LANDSCAPE,	// ADT Files
	FILETYPE_IMAGE,		// BLP Files
	FILETYPE_SOUND		// WAV, OGG & MP3 Files
};

/*
-= World of Warcraft Version Numbers =-

These Match the TOC Numbers of the game. There's a good chance we may never
need all of these, but it's probably a good thing to have, just in case.
Note: Version is used for detecting file-type formats, not content! You can
have only Vanilla content, but the files are in Cata formats.
*/
enum WoWVersion
{
	// Vanilla WoW, Pre 1.3 
	// TOCs matching between WOW_VANILLA_PRE13_FIRST and WOW_VANILLA_PRE13_LAST are pre-1.3 Vanillas.
	// This is used, rather than simply listing and matching like the rest, because before 1.3, the TOC
	// number matched WoW's Build number.
	WOW_VANILLA_PRE13_FIRST = 3975,		// Vanilla 1.0.0 TOC number. 1.0.0's Build number is actually 3980...
	WOW_VANILLA_PRE13_LAST = 4222,		// Vanilla 1.2.4 Build/TOC number
	
	// Vanilla WoW
	WOW_VANILLA_FIRST = 1300,		// Vanilla 1.3.x & 1.4.x - First recorded modern TOC number
	WOW_VANILLA_105 = 1500,			// Vanilla 1.5.x
	WOW_VANILLA_106 = 1600,			// Vanilla 1.6.x
	WOW_VANILLA_107 = 1700,			// Vanilla 1.7.x
	WOW_VANILLA_108 = 1800,			// Vanilla 1.8.x
	WOW_VANILLA_109 = 10900,		// Vanilla 1.9.x
	WOW_VANILLA_110 = 11000,		// Vanilla 1.10.x
	WOW_VANILLA_111 = 11100,		// Vanilla 1.11.x
	WOW_VANILLA = 11200,			// Final Vanilla, 1.12.x

	// The Burning Cursade
	WOW_TBC_FIRST = 20000,			// First Burning Crusade (2.0.1)
	WOW_TBC_203 = 20003,			// Burning Crusade 2.0.x
	WOW_TBC_210 = 20100,			// Burning Crusade 2.1.x
	WOW_TBC_220 = 20200,			// Burning Crusade 2.2.x
	WOW_TBC_230 = 20300,			// Burning Crusade 2.3.x
	WOW_TBC = 20400,				// Final Burning Crusade, 2.4.x

	// Wrath of the Lich King
	WOW_WOTLK_FIRST = 30000,		// First Wrath of the Lich King (3.0.x)
	WOW_WOTLK_310 = 30100,			// Wrath of the Lich King 3.1.x
	WOW_WOTLK_320 = 30200,			// Wrath of the Lich King 3.2.x
	WOW_WOTLK = 30300,				// Final Wrath of the Lich King, 3.3.x

	// Cataclysm
	WOW_CATACLYSM_FIRST = 40000,	// First Cataclysm (4.0.x)
	WOW_CATACLYSM_401 = 40100,		// Cataclysm 4.1.x
	WOW_CATACLYSM_402 = 40200,		// Cataclysm 4.2.x
	WOW_CATACLYSM_403 = 40300,		// Cataclysm 4.3.x (Current Version)
	WOW_CATACLYSM = 49999,			// Max Cataclysm. Used this way until Cata has ended.

	// Mists of Pandaria
	WOW_MOP_FIRST = 50000,			// First Mists of Pandaria (5.0.x)
	WOW_MOP = 59990,				// Max Mists of Pandaria

	// Others
	WOW_BETA = 59999,				// Beta for Mists of Pandaria
	WOW_PTR = 100000,				// Public Test Realm
	WOW_NOTLOADED = 999999			// Unloaded WoW
};

// Number matches location in LocaleList
enum WoWLocale
{
	LOCALE_ENUS = 0,		// English, US
	LOCALE_ENGB,			// English, Great Britian (England)
	LOCALE_FRFR,			// French
	LOCALE_DEDE,			// German
	LOCALE_KOKR,			// Korean
	LOCALE_ZHCN,			// Simplified Chinese
	LOCALE_ZHTW,			// Traditional Chinese
	LOCALE_ESES,			// Spanish, Spain
	LOCALE_ESMX,			// Spanish, Mexico
	LOCALE_RURU,			// Russian
	LOCALE_JAJP,			// Japanese
	LOCALE_PTBR,			// Portuguese, Brazil
	LOCALE_ITIT,			// Italian
	LOCALE_PLPL,			// Polish

	LOCALE_NONE		// if Locale = LOCALE_NONE, then there was an error.
};

// Expansion Name Identifier
enum ExpansionName
{
	EXPANSIONNAME_VANILLA = 0,
	EXPANSIONNAME_BURNINGCRUSADE,
	EXPANSIONNAME_WOTLK,
	EXPANSIONNAME_CATACLYSM,
	EXPANSIONNAME_MOP,

	EXPANSIONNAME_PTR,
	EXPANSIONNAME_BETA
};

// These are based on official WoW numbers
enum CharRaces
{
	RACE_HUMAN = 1,
	RACE_ORC,
	RACE_DWARF,
	RACE_NIGHTELF,
	RACE_UNDEAD,
	RACE_TAUREN,
	RACE_GNOME,
	RACE_TROLL,
	RACE_GOBLIN,
	RACE_BLOODELF,
	RACE_DRAENEI,	// 11

	RACE_WORGEN = 22
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
	MODELREADERROR_CANTOPENMPQ,
	MODELREADERROR_CANTREADMPQ
};

enum MPQErrorCodes
{
	MPQERROR_OKAY = 0,
	MPQERROR_FILENOTFOUND,
	MPQERROR_LOCKFILEFOUND,
	MPQERROR_COULDNOTOPEN,
};


#endif
