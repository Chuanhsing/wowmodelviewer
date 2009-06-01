#ifndef CHARCONTROL_H
#define CHARCONTROL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// wx
#include "wx/spinbutt.h"

// stl
#include <string>
#include <vector>

// our headers
#include "database.h"
#include "model.h"
#include "modelcanvas.h"

#include "enums.h"

// forward class declarations
class ChoiceDialog;
class ModelViewer;

bool slotHasModel(int i);
bool correctType(int type, int slot);

struct CharRegionCoords {
	int xpos, ypos, xsize, ysize;
};

const CharRegionCoords regions[NUM_REGIONS] =
{
	{0, 0, 256, 256},	// base
	{0, 0, 128, 64},	// arm upper
	{0, 64, 128, 64},	// arm lower
	{0, 128, 128, 32},	// hand
	{0, 160, 128, 32},	// face upper
	{0, 192, 128, 64},	// face lower
	{128, 0, 128, 64},	// torso upper
	{128, 64, 128, 32},	// torso lower
	{128, 96, 128, 64}, // pelvis upper
	{128, 160, 128, 64},// pelvis lower
	{128, 224, 128, 32}	// foot
};

struct CharTextureComponent
{
	wxString name;
	int region;
	int layer;

	const bool operator<(const CharTextureComponent& c) const
	{
		return layer < c.layer;
	}
};

struct CharTexture
{
	std::vector<CharTextureComponent> components;
	void addLayer(wxString fn, int region, int layer)
	{
		if (!fn || fn.length()==0)
			return;

		CharTextureComponent ct;
		ct.name = fn;
		ct.region = region;
		ct.layer = layer;
		components.push_back(ct);
	}
	void compose(TextureID texID);
};

struct TabardDetails
{
	int Icon;
	int IconColor;
	int Border;
	int BorderColor;
	int Background;

	int maxIcon;
	int maxIconColor;
	int maxBorder;
	int maxBorderColor;
	int maxBackground;

	bool showCustom;

	std::string GetIconTex(int slot);
	std::string GetBorderTex(int slot);
	std::string GetBackgroundTex(int slot);
};

struct CharDetails
{
	unsigned int skinColor;
	unsigned int faceType;
	unsigned int hairColor;
	unsigned int hairStyle;
	unsigned int facialHair;

	unsigned int facialColor;
	unsigned int maxFacialColor;

	unsigned int maxHairStyle, maxHairColor, maxSkinColor, maxFaceType, maxFacialHair;

	unsigned int race, gender;

	unsigned int useNPC;

	bool showUnderwear, showEars, showHair, showFacialHair, showFeet;

	int equipment[NUM_CHAR_SLOTS];
	int geosets[16];

	// save + load equipment
	void save(wxString fn, TabardDetails *td);
	bool load(wxString fn, TabardDetails *td);

	void loadSet(ItemSetDB &sets, ItemDatabase &items, int setid);
	void loadStart(StartOutfitDB &start, ItemDatabase &items, int cls);

	void reset();
};


class CharControl: public wxWindow
{
	DECLARE_CLASS(CharControl)
    DECLARE_EVENT_TABLE()

	wxSpinButton *spins[6];
	wxStaticText *spinLabels[6];
	wxSpinButton *tabardSpins[5];
	wxButton *buttons[NUM_CHAR_SLOTS];
	wxStaticText *labels[NUM_CHAR_SLOTS];
	

	void AddEquipment(int slot, int itemnum, int layer, CharTexture &tex, bool lookup = true);

public:
	// Item selection stuff
	ChoiceDialog *itemDialog;
	int choosingSlot;
	std::vector<int> numbers, cats;
	wxArrayString choices, catnames;

	CharControl(wxWindow* parent, wxWindowID id);
	~CharControl();

	bool Init();
	//void UpdateModel(Model *m);
	void UpdateModel(Attachment *a);
	void UpdateNPCModel(Attachment *a, unsigned int id);
	
	void RefreshModel();
	void RefreshNPCModel();
	void RefreshItem(int slot);
	void RefreshCreatureItem(int slot);
	void RefreshEquipment();
	inline void RandomiseChar();

	TextureID charTex, hairTex, furTex, capeTex, gobTex;

	bool bSheathe;

	void OnSpin(wxSpinEvent &event);
	void OnTabardSpin(wxSpinEvent &event);
	void OnCheck(wxCommandEvent &event);
	void OnButton(wxCommandEvent &event);

	void OnUpdateItem(int type, int id);

	CharDetails cd;
	TabardDetails td;

	Attachment *charAtt;
	Model *model;

	wxString makeItemTexture(int region, const wxString name);
	wxString customSkin;

	void ClearItemDialog();

	void selectItem(int type, int current, const wxChar *caption=_T("Item"));
	void selectSet();
	void selectStart();
	void selectMount();
	void selectNPC();

	const std::string selectCharModel();
};


#endif

