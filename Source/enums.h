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
*/
enum WoWVersion
{
	// Vanilla WoW
	WOW_VANILLA_FIRST = 3975,		// Vanilla's Original, unpatched TOC. Assumed this is used for all versions before 1.3.
	WOW_VANILLA_103 = 1300,			// Vanilla 1.3 & 1.4 - First recorded TOC number
	WOW_VANILLA_105 = 1500,			// Vanilla 1.5
	WOW_VANILLA_106 = 1600,			// Vanilla 1.6
	WOW_VANILLA_107 = 1700,			// Vanilla 1.7
	WOW_VANILLA_108 = 1800,			// Vanilla 1.8
	WOW_VANILLA_109 = 10900,		// Vanilla 1.9
	WOW_VANILLA_110 = 11000,		// Vanilla 1.10
	WOW_VANILLA_111 = 11100,		// Vanilla 1.11
	WOW_VANILLA = 11200,			// Final Vanilla, 1.12

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
	WOW_WOTLK = 30300,				// Final Wrath of the Lich King, 3.3.5a

	// Cataclysm
	WOW_CATACLYSM_FIRST = 40000,	// First Cataclysm (4.0.x)
	WOW_CATACLYSM_401 = 40100,		// Cataclysm 4.1.x
	WOW_CATACLYSM_402 = 40200,		// Cataclysm 4.2.x (Current Version)
	WOW_CATACLYSM = 49999,			// Max Cataclysm. Used this way until Cata has ended.

	// Others
	WOW_BETA = 59999,				// Next Expansion Beta ID
	WOW_PTR = 100000,				// Public Test Realm
	WOW_NOTLOADED = 999999			// Unloaded WoW
};

// Number matches location in LocaleList
enum WoWLocale
{
	LOCALE_ENUS = 1,
	LOCALE_ENGB,
	LOCALE_FRFR,
	LOCALE_DEDE,
	LOCALE_KOKR,
	LOCALE_ZHCN,
	LOCALE_ZHTW,
	LOCALE_ESES,
	LOCALE_ESMX,
	LOCALE_RURU,
	LOCALE_JAJP,
	LOCALE_PTBR
};

// Error Codes for reading and loading models.
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

#endif
