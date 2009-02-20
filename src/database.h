#ifndef DATABASE_H
#define DATABASE_H

// Combined the previous 5 various "db" files into one.
// trying to cut down on excess files.
// Also instead of declaring the db objects inside various classes
// may aswell declare them as globals since pretty much most the
// different objects need to access them at one point or another.

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// WX
#include <wx/string.h>

// STL
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>


// OUR HEADERS
#include "dbcfile.h"
#include "enums.h"
#include "util.h"

// 
class AnimDB;
class CharClassesDB;
class CharFacialHairDB;
class CharHairGeosetsDB;
class CharRacesDB;
class CharSectionsDB;
class CreatureModelDB;
class CreatureSkinDB;
class HelmGeosetDB;
class ItemDatabase;
class ItemDisplayDB;
class ItemSetDB;
class ItemSubClassDB;
class ItemVisualDB;
class ItemVisualEffectDB;
class NPCDatabase;
class NPCDB;
class StartOutfitDB;
class CreatureTypeDB;

// dbs
extern ItemDatabase	items;
extern AnimDB animdb;
extern HelmGeosetDB	helmetdb;
extern ItemVisualEffectDB effectdb;
extern ItemDisplayDB itemdb;
extern CreatureModelDB modeldb;
extern NPCDatabase npcs;
extern ItemSetDB setsdb;
extern CreatureSkinDB skindb;
extern StartOutfitDB startdb;
extern ItemSubClassDB subclassdb;
extern ItemVisualDB visualdb;
extern CharHairGeosetsDB hairdb;
extern CharSectionsDB chardb;
extern CharClassesDB classdb;
extern CharFacialHairDB	facialhairdb;
extern CharRacesDB racedb;
extern NPCDB npcdb;
extern CreatureTypeDB npctypedb;


class AnimDB: public DBCFile
{
public:
	AnimDB(): DBCFile("DBFilesClient\\AnimationData.dbc") {}
	~AnimDB() {}

	/// Fields
	static const size_t AnimID = 0;		// uint
	static const size_t Name = 1;		// string

	Record getByAnimID(unsigned int id);
};

// ============

class CharHairGeosetsDB: public DBCFile
{
public:
	CharHairGeosetsDB(): DBCFile("DBFilesClient\\CharHairGeosets.dbc") {}
	~CharHairGeosetsDB() {}

	/// Fields, WotLK ready
	static const size_t CharHairGeosetID = 0;	// uint
	static const size_t Race = 1;				// uint
	static const size_t Gender = 2;				// uint
	static const size_t Section = 3;			// uint
	static const size_t Geoset = 4;				// uint
	static const size_t Flags = 5;				// uint

	Record getByParams(unsigned int race, unsigned int gender, unsigned int section);
	int getGeosetsFor(unsigned int race, unsigned int gender);
};

class CharSectionsDB: public DBCFile
{
public:
	CharSectionsDB(): DBCFile("DBFilesClient\\CharSections.dbc") {}
	~CharSectionsDB() {}

	/// Fields
	static const size_t SectonID = 0;	// uint
	static const size_t Race = 1;		// uint
	static const size_t Gender = 2;		// uint
	static const size_t Type = 3;		// uint
	#ifndef WotLK
	static const size_t Section = 4;	// uint
	static const size_t Color = 5;		// uint
	static const size_t Tex1 = 6;		// string
	static const size_t Tex2 = 7;		// string
	static const size_t Tex3 = 8;		// string
	static const size_t IsNPC = 9;		// uint | 1 for npc
	#else
	static const size_t Tex1 = 4;		// string
	static const size_t Tex2 = 5;		// string
	static const size_t Tex3 = 6;		// string
//	static const size_t IsNPC = 7;		// uint | 1 for npc
	static const size_t Section = 8;	// uint
	static const size_t Color = 9;		// uint
	#endif

	/// Types
	static const size_t SkinType = 0;
	static const size_t FaceType = 1;
	static const size_t FacialHairType = 2;
	static const size_t HairType = 3;
	static const size_t UnderwearType = 4;

	Record getByParams(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int color, unsigned int npc);
	int getColorsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int section, unsigned int npc);
	int getSectionsFor(unsigned int race, unsigned int gender, unsigned int type, unsigned int color, unsigned int npc);
};


// HACK: Work around to make this compatible with both Burning Crusade and older (1.12 and earlier) WoW files
// TODO: Clean this code up at a later date when the future format is more concrete.
class CharRacesDB: public DBCFile
{
public:
	CharRacesDB(): DBCFile("DBFilesClient\\ChrRaces.dbc") 	{}

	~CharRacesDB() {}

	// Burning Crusade
	/// Fields
	static const size_t RaceID = 0;		// uint
	static const size_t ShortName = 6;	// string
	static const size_t Name = 12;		// string
	static const size_t FullName = 14;	// string
	static const size_t GeoType1 = 65;	// string
	static const size_t GeoType2 = 66;	// string
	static const size_t GeoType3 = 67;	// string

	Record getByName(wxString name);
	Record getById(unsigned int id);
};


class CharFacialHairDB: public DBCFile
{
public:
	CharFacialHairDB(): DBCFile("DBFilesClient\\CharacterFacialHairStyles.dbc") {}
	~CharFacialHairDB() {}

	/// Fields
	static const size_t Race = 0;				// uint
	static const size_t Gender = 1;				// uint
	static const size_t Style = 2;				// uint
	#ifndef WotLK
	static const size_t Geoset100 = 6;			// uint
	static const size_t Geoset300 = 7;			// uint
	static const size_t Geoset200 = 8;			// uint
	#else
	static const size_t Geoset100 = 3;			// uint
	static const size_t Geoset300 = 4;			// uint
	static const size_t Geoset200 = 5;			// uint
	#endif
	
	Record getByParams(unsigned int race, unsigned int gender, unsigned int style);
	int getStylesFor(unsigned int race, unsigned int gender);

private:

};


class CharClassesDB: public DBCFile
{
public:
	CharClassesDB(): DBCFile("DBFilesClient\\ChrClasses.dbc") {}
	~CharClassesDB() {}

	/// Fields
	static const size_t ClassID = 0;	// uint
	static const size_t Name = 4;		// string - english name
	static const size_t RawName = 14;	// string

	Record getById(unsigned int id);
};


class HelmGeosetDB: public DBCFile
{
public:
	HelmGeosetDB(): DBCFile("DBFilesClient\\HelmetGeosetVisData.dbc") {}
	~HelmGeosetDB() {}

	/// Fields
	static const size_t TypeID = 0;		// uint
	static const size_t Field1 = 1;		// uint
	static const size_t Field2 = 2;		// uint
	static const size_t Field3 = 3;		// uint
	static const size_t Field4 = 4;		// uint
	static const size_t Field5 = 5;		// uint

	Record getById(unsigned int id);
};

// ==============================================

// -----------------------------------
// Item Stuff
// -----------------------------------

class ItemDatabase;

extern char* ItemTypeNames[NUM_ITEM_TYPES];

class ItemDisplayDB: public DBCFile
{
public:
	ItemDisplayDB(): DBCFile("DBFilesClient\\ItemDisplayInfo.dbc") {}
	~ItemDisplayDB() {}

	/// Fields
	static const size_t ItemDisplayID = 0;	// uint
	static const size_t Model = 1;			// string
	static const size_t Model2 = 2;			// string
	static const size_t Skin = 3;			// string
	static const size_t Skin2 = 4;			// string
	static const size_t Icon = 5;			// string
	static const size_t Texture = 6;			// string
	static const size_t GeosetA = 7;		// uint
	static const size_t GeosetB = 8;		// uint
	static const size_t GeosetC = 9;		// uint
	static const size_t GeosetD = 10;		// uint
	static const size_t Unknown = 11;		// uint
	static const size_t ItemGroupSounds = 12;			// uint
	static const size_t GeosetVisID1 = 13;	// uint
	static const size_t GeosetVisID2 = 14;	// uint
	static const size_t TexArmUpper = 15;	// string
	static const size_t TexArmLower = 16;	// string
	static const size_t TexHands = 17;		// string
	static const size_t TexChestUpper = 18;	// string
	static const size_t TexChestLower = 19;	// string
	static const size_t TexLegUpper = 20;	// string
	static const size_t TexLegLower = 21;	// string
	static const size_t TexFeet = 22;		// string
	static const size_t Visuals = 23;		// uint

	Record getById(unsigned int id);
	bool hasId(unsigned int id);

private:

};


class ItemVisualDB: public DBCFile
{
public:
	ItemVisualDB(): DBCFile("DBFilesClient\\ItemVisuals.dbc") {}
	~ItemVisualDB() {}

	/// Fields
	static const size_t VisualID = 0;	// uint
	static const size_t Effect1 = 1;	// uint
	static const size_t Effect2 = 2;	// uint
	static const size_t Effect3 = 3;	// uint
	static const size_t Effect4 = 4;	// uint
	static const size_t Effect5 = 5;	// uint

	Record getById(unsigned int id);
};

class ItemVisualEffectDB: public DBCFile
{
public:
	ItemVisualEffectDB(): DBCFile("DBFilesClient\\ItemVisualEffects.dbc") {}
	~ItemVisualEffectDB() {}

	/// Fields
	static const size_t EffectID = 0;	// uint
	static const size_t Model = 1;		// string

	Record getById(unsigned int id);
};


class ItemSetDB: public DBCFile
{
	std::set<int> avail;

public:
	ItemSetDB(): DBCFile("DBFilesClient\\ItemSet.dbc") {}
	~ItemSetDB() {}

	static const size_t NumItems = 10;

	/// Fields
	static const size_t SetID = 0;	// uint
	static const size_t Name = 1;	// string
	static const size_t ItemIDBase = 18; // 10 * uint

	Record getById(unsigned int id);
	void cleanup(ItemDatabase &itemdb);
	bool available(unsigned int id);
};

class StartOutfitDB: public DBCFile
{
public:
	StartOutfitDB(): DBCFile("DBFilesClient\\CharStartOutfit.dbc") {}
	~StartOutfitDB() {}

	static const size_t NumItems = 12;

	/// Fields
	static const size_t StartOutfitID = 0;	// uint
	static const size_t Race = 4;	// byte offset
	static const size_t Class = 5;	// byte offset
	static const size_t Gender = 6;	// byte offset
	static const size_t ItemIDBase = 2; // 12 * uint
//	static const size_t ItemDisplayIDBase = 14; // 12 * uint
//	static const size_t ItemTypeBase = 26; // 12 * uint

	Record getById(unsigned int id);
};

struct ItemRecord {
	wxString name;
	int id, itemclass, subclass, type, model, sheath, quality;

	ItemRecord(const char* line);
	ItemRecord(wxString name, int type): id(0), name(name), type(type), itemclass(-1), subclass(-1), model(0), sheath(0), quality(0)
	{}
	ItemRecord(const ItemRecord &r): id(r.id), name(r.name), itemclass(r.itemclass), subclass(r.subclass), type(r.type), model(r.model), sheath(r.sheath), quality(r.quality)
	{}

	const bool operator< (const ItemRecord &r) const
	{
		if (type == r.type) 
			return name < r.name;
		else 
			return type < r.type;
	}
};

class ItemDatabase {
public:
//	ItemDatabase(const char* filename);
	ItemDatabase();

	std::vector<ItemRecord> items;
	std::map<int, int> itemLookup;

	void cleanup(ItemDisplayDB &itemdb);
	void open(const char* filename);

	const ItemRecord& get(int id);
	int getItemNum(int id);
};

/*
class ItemClassDB: public DBCFile
{
public:
	ItemClassDB(): DBCFile("DBFilesClient\\ItemClass.dbc") {}
	~ItemClassDB() {}

	/// Fields
	static const size_t ClassID = 0;	// uint
	static const size_t Name = 3;		// string

	//Record getById(unsigned int id);
};
*/

class ItemSubClassDB: public DBCFile
{
public:
	ItemSubClassDB(): DBCFile("DBFilesClient\\ItemSubClass.dbc") {}
	~ItemSubClassDB() {}

	/// Fields
	static const size_t ClassID = 0;	// int
	static const size_t SubClassID = 1;	// int
	static const size_t Flags = 4;		// uint
	// ...
	static const size_t Hands = 9;		// int
	static const size_t Name = 10;		// string


	Record getById(int id, int subid);
};

// ============/////////////////=================/////////////////


// ------------------------------
// NPC Stuff
// -------------------------------
struct NPCRecord {
	wxString name;
	int id, model, type;

	NPCRecord(const char* line);
	//NPCRecord(const char* name): id(0), name(name), model(0) {}
	NPCRecord(const NPCRecord &r): id(r.id), name(r.name), model(r.model), type(r.type) {}

	const bool operator< (const NPCRecord &r) const
	{ 
		return name < r.name;
	}
};

class NPCDatabase {
public:
	NPCDatabase(const char* filename);
	NPCDatabase() { }

	std::vector<NPCRecord> npcs;
	std::map<int, int> npcLookup;

	void open(const char* filename);

	const NPCRecord& get(int id);
	const NPCRecord& getByID(int id);
};

// =========================================

class SpellEffectsDB: public DBCFile
{
public:
	SpellEffectsDB(): DBCFile("DBFilesClient\\SpellVisualEffectName.dbc") {}
	~SpellEffectsDB() {}

	/// Fields
	static const size_t ID = 0;				// uint
	static const size_t EffectName = 1;		// string
	static const size_t ModelName = 2;		// string
	static const size_t SpellType = 3;		// uint
	static const size_t UnknownValue2 = 4;	// uint

	Record getById(unsigned int id);
	Record getByName(wxString name);
};


// ===============================================

class CreatureModelDB: public DBCFile
{
public:
	CreatureModelDB(): DBCFile("DBFilesClient\\CreatureModelData.dbc") {}
	~CreatureModelDB() {}

	/// Fields
	static const size_t ModelID = 0;		// uint
	static const size_t Type = 1;			// uint
	static const size_t Filename = 2;		// string

	// filenames need to end in mdx though ;(
	Record getByFilename(std::string fn);
	Record getByID(unsigned int id);
private:

};

class CreatureSkinDB: public DBCFile
{
public:
	CreatureSkinDB(): DBCFile("DBFilesClient\\CreatureDisplayInfo.dbc") {}
	~CreatureSkinDB() {}

	/// Fields
	static const size_t SkinID = 0;			// uint
	static const size_t ModelID = 1;		// uint
	static const size_t NPCID = 3;			// uint
	static const size_t Scale = 4;			// float
	static const size_t Opacity = 5;			// uint, 0-255, 255 is solid
	static const size_t Skin = 6;			// string
	static const size_t Skin2 = 7;			// string
	static const size_t Skin3 = 8;			// string

	Record getByModelID(unsigned int id);
	Record getBySkinID(unsigned int id);
};

class CreatureTypeDB: public DBCFile
{
public:
	CreatureTypeDB(): DBCFile("DBFilesClient\\CreatureType.dbc") {}
	~CreatureTypeDB() {}
	
	// Fields
	static const size_t ID = 0;			// uint
	static const size_t Name = 1;		// string

	Record getByID(unsigned int id);
};

class NPCDB: public DBCFile
{
public:
	NPCDB(): DBCFile("DBFilesClient\\CreatureDisplayInfoExtra.dbc") {}
	~NPCDB() {}

	/// Fields
	static const size_t NPCID = 0;			// uint
	static const size_t RaceID = 1;			// uint
	static const size_t Gender = 2;			// bool
	static const size_t SkinColor = 3;		// uint
	static const size_t Face = 4;		// uint
	static const size_t HairStyle = 5;		// uint
	static const size_t HairColor = 6;		// uint
	static const size_t FacialHair = 7;		// uint
	static const size_t HelmID = 8;			// uint
	static const size_t ShoulderID = 9;		// uint
	static const size_t ShirtID = 10;		// uint
	static const size_t ChestID = 11;		// uint
	static const size_t BeltID = 12;		// uint
	static const size_t PantsID = 13;		// uint
	static const size_t BootsID = 14;		// uint
	static const size_t BracersID = 15;		// uint
	static const size_t GlovesID = 16;		// uint
	static const size_t TabardID = 17;		// uint
	static const size_t CapeID = 18;		// uint

	// Changed in WoW 2.0.1, the 20th column is now an index offset to the filename.
	static const size_t Filename = 20;		// string.

	Record getByFilename(std::string fn);
	Record getByNPCID(unsigned int id);

};


#endif

