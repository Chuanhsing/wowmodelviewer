// our headers
#include "util.h"
#include "charcontrol.h"
#include "modelviewer.h"
#include "itemselection.h"

#include "globalvars.h"


int slotOrder[] = {	
	CS_SHIRT,
	CS_HEAD,
	CS_NECK,
	CS_SHOULDER,
	CS_PANTS,
	CS_BOOTS,
	CS_CHEST,
	CS_TABARD,
	CS_BELT,
	CS_BRACERS,
	CS_GLOVES,
	CS_HAND_RIGHT,
	CS_HAND_LEFT,
	CS_CAPE,
	CS_QUIVER
};

int slotOrderWithRobe[] = {
	CS_SHIRT,
	CS_HEAD,
	CS_NECK,
	CS_SHOULDER,
	CS_BOOTS,
	CS_PANTS,
	CS_BRACERS,
	CS_CHEST,
	CS_GLOVES,
	CS_TABARD,
	CS_BELT,
	CS_HAND_RIGHT,
	CS_HAND_LEFT,
	CS_CAPE,
	CS_QUIVER
};

IMPLEMENT_CLASS(CharControl, wxWindow)

BEGIN_EVENT_TABLE(CharControl, wxWindow)
	EVT_SPIN(ID_SKIN_COLOR, CharControl::OnSpin)
	EVT_SPIN(ID_FACE_TYPE, CharControl::OnSpin)
	EVT_SPIN(ID_HAIR_COLOR, CharControl::OnSpin)
	EVT_SPIN(ID_HAIR_STYLE, CharControl::OnSpin)
	EVT_SPIN(ID_FACIAL_HAIR, CharControl::OnSpin)
	EVT_SPIN(ID_FACIAL_COLOR, CharControl::OnSpin)

	EVT_SPIN(ID_TABARD_ICON, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_ICONCOLOR, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_BORDER, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_BORDERCOLOR, CharControl::OnTabardSpin)
	EVT_SPIN(ID_TABARD_BACKGROUND, CharControl::OnTabardSpin)

	EVT_BUTTON(ID_MOUNT, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_HEAD, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_NECK, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_SHOULDER, CharControl::OnButton)
	
	EVT_BUTTON(ID_EQUIPMENT + CS_SHIRT, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_CHEST, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_BELT, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_PANTS, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_BOOTS, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_BRACERS, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_GLOVES, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_CAPE, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_HAND_RIGHT, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_HAND_LEFT, CharControl::OnButton)

	EVT_BUTTON(ID_EQUIPMENT + CS_QUIVER, CharControl::OnButton)
	EVT_BUTTON(ID_EQUIPMENT + CS_TABARD, CharControl::OnButton)
END_EVENT_TABLE()

CharControl::CharControl(wxWindow* parent, wxWindowID id)
{
	wxLogMessage(_T("Creating Char Control..."));

	// If they are using the Test Patch and a test item list exist,  use it instead.
	char filename[20];
	if (langID == 5)
		strncpy(filename, "zhTW/items.csv", sizeof(filename));
	else if (langID == 4)
		strncpy(filename, "zhCN/items.csv", sizeof(filename));
	else
		strncpy(filename, "enUS/items.csv", sizeof(filename));
	if (wxFile::Exists(filename))
		items.open(filename);
	else
		wxLogMessage(_T("Error: Could not find items.csv to load an item list from."));

	if(Create(parent, id, wxDefaultPosition, wxSize(100,700), 0, _T("CharControl")) == false) {
		wxLogMessage("GUI Error: Failed to create a window frame for the Character Control!");
		return;
	}

	wxFlexGridSizer *top = new wxFlexGridSizer(1);
	top->AddGrowableCol(0);

	wxFlexGridSizer *gs = new wxFlexGridSizer(3, 5, 4);

	#define ADD_CONTROLS(type, id, caption) \
	gs->Add(new wxStaticText(this, wxID_ANY, caption), wxSizerFlags().Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL)); \
	gs->Add(spins[type] = new wxSpinButton(this, id, wxDefaultPosition, wxSize(30,16), wxSP_HORIZONTAL|wxSP_WRAP), wxSizerFlags(1).Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL)); \
	gs->Add(spinLabels[type] = new wxStaticText(this, wxID_ANY, _T("0")), wxSizerFlags(2).Align(wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL));

	ADD_CONTROLS(SPIN_SKIN_COLOR, ID_SKIN_COLOR, _("Skin color"))
	ADD_CONTROLS(SPIN_FACE_TYPE, ID_FACE_TYPE, _("Face type"))
	ADD_CONTROLS(SPIN_HAIR_COLOR, ID_HAIR_COLOR, _("Hair color"))
	ADD_CONTROLS(SPIN_HAIR_STYLE, ID_HAIR_STYLE, _("Hair style"))
	ADD_CONTROLS(SPIN_FACIAL_HAIR, ID_FACIAL_HAIR, _("Facial feature"))
	ADD_CONTROLS(SPIN_FACIAL_COLOR, ID_FACIAL_COLOR, _("Facial color"))
	#undef ADD_CONTROLS
	
	//gs->Add(new wxButton(this, ID_CHAR_RANDOMISE, _("Randomise"), wxDefaultPosition, wxDefaultSize), wxSizerFlags().Proportion(0).Expand());

	top->Add(gs,wxSizerFlags().Proportion(1).Expand().Border(wxALL, 10));

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		buttons[i] = NULL;
		labels[i] = NULL;
	}
	
	top->Add(new wxStaticText(this, -1, _("Equipment"), wxDefaultPosition, wxSize(200,20), wxALIGN_CENTRE), wxSizerFlags().Border(wxTOP, 10));
	wxFlexGridSizer *gs2 = new wxFlexGridSizer(2, 5, 5);
	gs2->AddGrowableCol(1);

	#define ADD_CONTROLS(type, caption) \
	gs2->Add(buttons[type]=new wxButton(this, ID_EQUIPMENT + type, caption), wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL)); \
	gs2->Add(labels[type]=new wxStaticText(this, -1, _("---- None ----")), wxSizerFlags().Proportion(1).Expand().Align(wxALIGN_CENTER_VERTICAL).Border(wxRIGHT, 10));
	
	ADD_CONTROLS(CS_HEAD, _("Head"))
	//ADD_CONTROLS(CS_NECK, _T("Neck"))
	ADD_CONTROLS(CS_SHOULDER, _("Shoulder"))

	ADD_CONTROLS(CS_SHIRT, _("Shirt"))
	ADD_CONTROLS(CS_CHEST, _("Chest"))
	ADD_CONTROLS(CS_BELT, _("Belt"))
	ADD_CONTROLS(CS_PANTS, _("Legs"))
	ADD_CONTROLS(CS_BOOTS, _("Boots"))

	ADD_CONTROLS(CS_BRACERS, _("Bracers"))
	ADD_CONTROLS(CS_GLOVES, _("Gloves"))
	ADD_CONTROLS(CS_CAPE, _("Cape"))

	ADD_CONTROLS(CS_HAND_RIGHT, _("Right hand"))
	ADD_CONTROLS(CS_HAND_LEFT, _("Left hand"))

	ADD_CONTROLS(CS_QUIVER, _("Quiver"))
	ADD_CONTROLS(CS_TABARD, _("Tabard"))
	#undef ADD_CONTROLS
	
	top->Add(gs2, wxEXPAND);

	// Create our tabard customisation spin buttons
	wxGridSizer *gs3 = new wxGridSizer(2, 5, 5);
	#define ADD_CONTROLS(type, id, caption) \
	gs3->Add(new wxStaticText(this, -1, caption), wxSizerFlags().Align(wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL)); \
	gs3->Add(tabardSpins[type]=new wxSpinButton(this, id, wxDefaultPosition, wxDefaultSize, wxSP_HORIZONTAL|wxSP_WRAP), wxSizerFlags(1).Align(wxALIGN_CENTER_VERTICAL));

	ADD_CONTROLS(SPIN_TABARD_ICON, ID_TABARD_ICON, _("Icon"))
	ADD_CONTROLS(SPIN_TABARD_ICONCOLOR, ID_TABARD_ICONCOLOR, _("Icon Color"))
	ADD_CONTROLS(SPIN_TABARD_BORDER, ID_TABARD_BORDER, _("Border"))
	ADD_CONTROLS(SPIN_TABARD_BORDERCOLOR, ID_TABARD_BORDERCOLOR, _("Border Color"))
	ADD_CONTROLS(SPIN_TABARD_BACKGROUND, ID_TABARD_BACKGROUND, _("Background Color"))

	#undef ADD_CONTROLS

	top->Add(new wxStaticText(this, -1, _("Tabard details")), wxSizerFlags().Align(wxALIGN_CENTRE).Border(wxALL, 1));
	top->Add(gs3, wxEXPAND);
	top->Add(new wxButton(this, ID_MOUNT, _("Choose mount")), wxSizerFlags().Align(wxALIGN_CENTRE).Border(wxTOP, 15));

	//p->SetSizer(top);
	
	top->SetSizeHints(this);
	Show(true);
	SetAutoLayout(true);
	SetSizer(top);
	Layout();
	
	choosingSlot = 0;
	itemDialog = 0;
}

CharControl::~CharControl()
{
	
}


bool CharControl::Init()
{
	charTex = 0;
	hairTex = 0;
	furTex = 0;
	capeTex = 0;
	gobTex = 0;

	td.showCustom = false;
	bSheathe = false;

	cd.useNPC = 0;
	cd.showEars = true;
	cd.showHair = true;
	cd.showFacialHair = true;
	cd.showUnderwear = true;

	// set max values for custom tabard
	td.maxBackground = 50;
	td.maxBorder = 9;
	td.maxBorderColor = 16;
	td.maxIcon = 169;
	td.maxIconColor = 16;

	return true;
}

//void CharControl::UpdateModel(Model *m)
void CharControl::UpdateModel(Attachment *a)
{
	if (!a)
		return;

	charAtt = a;
	model = (Model*)charAtt->model;

	// The following isn't actually needed, 
	// pretty sure all this gets taken care of by TextureManager and CharTexture
	charTex = 0;
	if (charTex==0) 
		glGenTextures(1, &charTex);

	cd.reset();
	td.showCustom = false;

	// hide most geosets
	for (size_t i=0; i<model->geosets.size(); i++) {
		int id = model->geosets[i].id;
		model->showGeosets[i] = (id==0);
	}

	size_t p1 = model->name.find_first_of('\\', 0);
	size_t p2 = model->name.find_first_of('\\', p1+1);
	size_t p3 = model->name.find_first_of('\\', p2+1);

	std::string raceName = model->name.substr(p1+1,p2-p1-1);
	std::string genderName = model->name.substr(p2+1,p3-p2-1);

	unsigned int race, gender;

	try {
		// Okay for some reason Blizzard have removed the full racial names
		// out of the ChrRaces.dbc.  Going to have to hardcode the values.
		CharRacesDB::Record raceRec = racedb.getByName(raceName.c_str());
		race = raceRec.getUInt(CharRacesDB::RaceID);
		
		/*
		if (raceName == "Human")
			race = 1;
		else if (raceName == "Orc")
			race = 2;
		else if (raceName == "Dwarf")
			race = 3;
		else if (raceName == "Nightelf")
			race = 4;
		else if (raceName == "Scourge")
			race = 5;
		else if (raceName == "Tauren")
			race = 6;
		else if (raceName == "Gnome")
			race = 7;
		else if (raceName == "Troll")
			race = 8;
		else if (raceName == "Goblin")
			race = 9;
		else if (raceName == "Bloodelf")
			race = 10;
		else if (raceName == "Draenei")
			race = 11;
		else if (raceName == "Felorc")
			race = 12;
		else if (raceName == "Naga_")
			race = 13;
		else if (raceName == "Broken")
			race = 14;
		else if (raceName == "Skeleton")
			race = 15;
		else
			race = 0;
		*/

		gender = (genderName == "female" || genderName == "Female" || genderName == "FEMALE") ? 1 : 0;
	
	} catch (CharRacesDB::NotFound) {
		// wtf
		race = 0;
		gender = 0;
	}

	// Enable the use of NPC skins if its  a goblin.
	if (race==9)
		cd.useNPC=1;
	else
		cd.useNPC=0;

	if (race==6 || race==8 || race==11 || race==13 || race==14) // If its a troll/tauren/dranei/naga/broken, show the feet (dont wear boots)
		cd.showFeet = true;
	else
		cd.showFeet = false;

	// get max values
	cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
	if (cd.maxSkinColor==0 && cd.useNPC==1) {
		wxMessageBox("The selected character does not have any NPC skins!\nSwitching back to normal character skins.");
		cd.useNPC = 0;
		cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
	}
	cd.maxFaceType  = chardb.getSectionsFor(race, gender, CharSectionsDB::FaceType, 0, cd.useNPC);
	cd.maxHairColor = chardb.getColorsFor(race, gender, CharSectionsDB::HairType, 0, 0);
	cd.maxFacialHair = facialhairdb.getStylesFor(race, gender);
	cd.maxFacialColor = cd.maxHairColor;

	// Re-set the menu
	if (cd.useNPC)
		g_modelViewer->optMenu->Check(ID_USE_NPCSKINS, 1);
	else
		g_modelViewer->optMenu->Check(ID_USE_NPCSKINS, 0);

	g_modelViewer->charMenu->Check(ID_SHOW_FEET, 0);
	// ----

	cd.race = race;
	cd.gender = gender;

	std::set<int> styles;
	for (CharHairGeosetsDB::Iterator it = hairdb.begin(); it != hairdb.end(); ++it) {
		if (it->getUInt(CharHairGeosetsDB::Race)==race && it->getUInt(CharHairGeosetsDB::Gender)==gender) {
			styles.insert(it->getUInt(CharHairGeosetsDB::Section));
		}
	}
	cd.maxHairStyle = (int)styles.size();

	if (cd.maxFaceType==0) cd.maxFaceType = 1;
	if (cd.maxSkinColor==0) cd.maxSkinColor = 1;
	if (cd.maxHairColor==0) cd.maxHairColor = 1;
	if (cd.maxHairStyle==0) cd.maxHairStyle = 1;
	if (cd.maxFacialHair==0) cd.maxFacialHair = 1;
	spins[0]->SetRange(0, cd.maxSkinColor-1);
	spins[1]->SetRange(0, cd.maxFaceType-1);
	spins[2]->SetRange(0, cd.maxHairColor-1);
	spins[3]->SetRange(0, cd.maxHairStyle-1);
	spins[4]->SetRange(0, cd.maxFacialHair-1);
	spins[5]->SetRange(0, cd.maxHairColor-1);

	td.Icon = randint(0, td.maxIcon);
	td.IconColor = randint(0, td.maxIconColor);
	td.Border = randint(0, td.maxBorder);
	td.BorderColor = 0;
	td.Background = randint(0, td.maxBackground);

	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);

	tabardSpins[SPIN_TABARD_ICON]->SetValue(td.Icon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetValue(td.IconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetValue(td.Border);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetValue(td.BorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetValue(td.Background);

	tabardSpins[SPIN_TABARD_ICON]->SetRange(0, td.maxIcon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetRange(0, td.maxIconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetRange(0, td.maxBorder);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, td.maxBorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetRange(0, td.maxBackground);

	for (int i=0; i<NUM_SPIN_BTNS; i++) 
		spins[i]->Refresh(false);
	for (int i=0; i<NUM_TABARD_BTNS; i++) 
		tabardSpins[i]->Refresh(false);
	for (int i=0; i<NUM_SPIN_BTNS; i++)
		spinLabels[i]->SetLabel(wxString::Format("%i", spins[i]->GetValue()));

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		if (labels[i]) 
			labels[i]->SetLabel(_T("---- None ----"));
	}

	if (useRandomLooks)
		RandomiseChar();

	RefreshModel();
}

void CharControl::UpdateNPCModel(Attachment *a, unsigned int id)
{
	if (!a)
		return;

	charAtt = a;
	model = (Model*)a->model;

	charTex = 0;
	if (charTex==0) 
		glGenTextures(1, &charTex);

	// Open the first record, just so we can declare the var.
	NPCDB::Record npcrec = npcdb.getRecord(0);

	// Get the NPC Record to construct the NPC character model from.
	try {
		npcrec = npcdb.getByNPCID(id);
	} catch (...) {
		wxLogMessage(_T("Exception Error: %s : line #%i : %s\n\tUnable to load NPC #%i"), __FILE__, __LINE__, __FUNCTION__, id);
		return;
	}

	cd.reset();
	td.showCustom = false;

	// hide most geosets
	for (size_t i=0; i<model->geosets.size(); i++) {
		int id = model->geosets[i].id;
		model->showGeosets[i] = (id==0);
	}

	size_t p1 = model->name.find_first_of('\\', 0);
	size_t p2 = model->name.find_first_of('\\', p1+1);
	size_t p3 = model->name.find_first_of('\\', p2+1);

	std::string raceName = model->name.substr(p1+1,p2-p1-1);
	std::string genderName = model->name.substr(p2+1,p3-p2-1);

	int race = 0, gender = 0;

	try {
		CharRacesDB::Record raceRec = racedb.getByName(raceName.c_str());
		race = raceRec.getUInt(CharRacesDB::RaceID);

		gender = (genderName == "Female") ? 1 : 0;
	} catch (CharRacesDB::NotFound) {
		// wtf
		race = 0;
		gender = 0;
	}

	cd.race = race;
	cd.gender = gender;

	// Enable the use of NPC skins if its a goblin.
	if (race==9)
		cd.useNPC=1;
	else
		cd.useNPC=0;

	// Model Characteristics
	try {
		cd.skinColor = npcrec.getUInt(NPCDB::SkinColor);
		cd.faceType = npcrec.getUInt(NPCDB::Face);
		cd.hairColor = npcrec.getUInt(NPCDB::HairColor);
		cd.hairStyle = npcrec.getUInt(NPCDB::HairStyle);
		cd.facialHair = npcrec.getUInt(NPCDB::FacialHair);
		cd.facialColor = cd.hairColor;
	} catch (...) {
		wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}
	
	cd.maxFaceType = 0;
	cd.maxSkinColor = 0;
	cd.maxHairColor = 0;
	cd.maxHairStyle = 0;
	cd.maxFacialHair = 0;
	spins[0]->SetRange(0, cd.maxSkinColor-1);
	spins[1]->SetRange(0, cd.maxFaceType-1);
	spins[2]->SetRange(0, cd.maxHairColor-1);
	spins[3]->SetRange(0, cd.maxHairStyle-1);
	spins[4]->SetRange(0, cd.maxFacialHair-1);
	spins[5]->SetRange(0, cd.maxHairColor-1);

	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);

	tabardSpins[SPIN_TABARD_ICON]->SetValue(td.Icon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetValue(td.IconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetValue(td.Border);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetValue(td.BorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetValue(td.Background);

	tabardSpins[SPIN_TABARD_ICON]->SetRange(0, td.maxIcon);
	tabardSpins[SPIN_TABARD_ICONCOLOR]->SetRange(0, td.maxIconColor);
	tabardSpins[SPIN_TABARD_BORDER]->SetRange(0, td.maxBorder);
	tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, td.maxBorderColor);
	tabardSpins[SPIN_TABARD_BACKGROUND]->SetRange(0, td.maxBackground);

	for (int i=0; i<NUM_SPIN_BTNS; i++) 
		spins[i]->Refresh(false);
	for (int i=0; i<NUM_TABARD_BTNS; i++) 
		tabardSpins[i]->Refresh(false);

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		if (labels[i]) 
			labels[i]->SetLabel(_T("---- None ----"));
	}

	// Equip our npc
	try {
		cd.equipment[CS_HEAD] = npcrec.getUInt(NPCDB::HelmID);
		cd.equipment[CS_SHOULDER] = npcrec.getUInt(NPCDB::ShoulderID);
		cd.equipment[CS_SHIRT] = npcrec.getUInt(NPCDB::ShirtID);
		cd.equipment[CS_CHEST] = npcrec.getUInt(NPCDB::ChestID);
		cd.equipment[CS_BELT] = npcrec.getUInt(NPCDB::BeltID);
		cd.equipment[CS_PANTS] = npcrec.getUInt(NPCDB::PantsID);
		cd.equipment[CS_BOOTS] = npcrec.getUInt(NPCDB::BootsID);
		cd.equipment[CS_BRACERS] = npcrec.getUInt(NPCDB::BracersID);
		cd.equipment[CS_GLOVES] = npcrec.getUInt(NPCDB::GlovesID);
		cd.equipment[CS_TABARD] = npcrec.getUInt(NPCDB::TabardID);
		cd.equipment[CS_CAPE] = npcrec.getUInt(NPCDB::CapeID);
		if (cd.equipment[CS_TABARD] != 0) 
			cd.geosets[12] = 2;
	} catch (...) {
		wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	RefreshNPCModel();
}

void CharControl::OnSpin(wxSpinEvent &event)
{
	if (!g_canvas)
		return;

	if (event.GetId()==ID_SKIN_COLOR) 
		cd.skinColor = event.GetPosition();

	if(g_canvas->model->modelType == MT_NPC)
		return;

	if (event.GetId()==ID_FACE_TYPE) 
		cd.faceType = event.GetPosition();
	else if (event.GetId()==ID_HAIR_COLOR) {
		cd.hairColor = event.GetPosition();
		cd.facialColor = event.GetPosition();
	} else if (event.GetId()==ID_HAIR_STYLE) 
		cd.hairStyle = event.GetPosition();
	else if (event.GetId()==ID_FACIAL_HAIR) 
		cd.facialHair = event.GetPosition();
	else if (event.GetId()==ID_FACIAL_COLOR) 
		cd.facialColor = event.GetPosition();

	for (int i=0; i<6; i++)
		spinLabels[i]->SetLabel(wxString::Format("%i", spins[i]->GetValue()));

	RefreshModel();
}

void CharControl::OnCheck(wxCommandEvent &event)
{
	if (event.GetId()==ID_SHOW_UNDERWEAR) 
		cd.showUnderwear = event.IsChecked();
	else if (event.GetId()==ID_SHOW_HAIR) 
		cd.showHair = event.IsChecked();
	else if (event.GetId()==ID_SHOW_FACIALHAIR) 
		cd.showFacialHair = event.IsChecked();
	else if (event.GetId()==ID_SHOW_EARS) 
		cd.showEars = event.IsChecked();
	else if (event.GetId()==ID_SHEATHE) 
		bSheathe = event.IsChecked();
	else if (event.GetId()==ID_SHOW_FEET) 
		cd.showFeet = event.IsChecked();
	else if (event.GetId()==ID_USE_NPCSKINS) {		
		// All this extra checking is to modify the the 'bounds' of the max skins on the spin button.
		size_t p1 = model->name.find_first_of('\\', 0);
		size_t p2 = model->name.find_first_of('\\', p1+1);
		size_t p3 = model->name.find_first_of('\\', p2+1);

		std::string raceName = model->name.substr(p1+1,p2-p1-1);
		std::string genderName = model->name.substr(p2+1,p3-p2-1);

		unsigned int race, gender;

		try {
			CharRacesDB::Record raceRec = racedb.getByName(raceName.c_str());
			race = raceRec.getUInt(CharRacesDB::RaceID);
			gender = (genderName == "female" || genderName == "Female" || genderName == "FEMALE") ? 1 : 0;
		} catch (CharRacesDB::NotFound) {
			// wtf
			race = 0;
			gender = 0;
		}

		// If the race is a goblin, then ignore this
		if (race == 9) {
			g_modelViewer->optMenu->Check(ID_USE_NPCSKINS, true);
			return;
		}

		//  set our flag
		cd.useNPC = event.IsChecked();

		cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
		if (cd.maxSkinColor==0 && cd.useNPC==1) {
			wxMessageBox("The selected character does not have any NPC skins!\nSwitching back to normal character skins.");
			cd.useNPC = 0;
			cd.maxSkinColor = chardb.getColorsFor(race, gender, CharSectionsDB::SkinType, 0, cd.useNPC);
		} else {
			cd.skinColor = 0;
			spins[0]->SetValue(0);
			spins[0]->SetRange(0, cd.maxSkinColor-1);
		}
	}

	//  Update controls associated
	RefreshEquipment();
	g_modelViewer->UpdateControls();	
	// ----
}

bool slotHasModel(int i)
{
	return (i==CS_HEAD || i==CS_SHOULDER || i==CS_HAND_LEFT || i==CS_HAND_RIGHT || i==CS_QUIVER);
}

inline void CharControl::RandomiseChar()
{
	// Choose random values for the looks! ^_^
	cd.skinColor = randint(0, cd.maxSkinColor-1);
	cd.faceType = randint(0, cd.maxFaceType-1);
	cd.hairColor = randint(0, cd.maxHairColor-1);
	cd.hairStyle = randint(0, cd.maxHairStyle-1);
	cd.facialHair = randint(0, cd.maxFacialHair-1);
	cd.facialColor = cd.hairColor;

	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);
}

void CharControl::RefreshEquipment()
{
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		if (slotHasModel(i)) 
			RefreshItem(i);

		if (g_canvas->model->modelType != MT_NPC) {
			if (labels[i])
				labels[i]->SetLabel(CSConv(items.get(cd.equipment[i]).name));
		}
	}
}

void CharControl::OnButton(wxCommandEvent &event)
{
	// This stores are equipment directory path in session
	static wxString dir = cfgPath.BeforeLast('\\'); 

	//if (dir.Last() != '\\')
	//	dir.Append('\\');

	if (event.GetId()==ID_CHAR_RANDOMISE) {
		RandomiseChar();
		
	} else if (event.GetId()==ID_SAVE_EQUIPMENT) {
		wxFileDialog dialog(this, _("Save equipment"), dir, wxEmptyString, _T("Equipment files (*.eq)|*.eq"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT, wxDefaultPosition);
		if (dialog.ShowModal()==wxID_OK) {
			wxString s(dialog.GetPath());
			cd.save(s, &td);

			// Save directory path
			dir = dialog.GetDirectory();
		}

	} else if (event.GetId()==ID_LOAD_EQUIPMENT) {
		wxFileDialog dialog(this, _("Load equipment"), dir, wxEmptyString, _T("Equipment files (*.eq)|*.eq"), wxFD_OPEN|wxFD_FILE_MUST_EXIST, wxDefaultPosition);
		if (dialog.ShowModal()==wxID_OK) {
			wxString s(dialog.GetPath());
			if (cd.load(s, &td)) {
				spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
				spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
				spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
				spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
				spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
				spins[SPIN_FACIAL_COLOR]->SetValue(cd.hairColor);
				for (int i=0; i<NUM_SPIN_BTNS; i++) 
					spins[i]->Refresh(false);
			}
			RefreshEquipment();

			// Save directory path
			dir = dialog.GetDirectory();
		}

	} else if (event.GetId()==ID_CLEAR_EQUIPMENT) {
		for (int i=0; i<NUM_CHAR_SLOTS; i++) 
			cd.equipment[i] = 0;
		RefreshEquipment();

	} else if (event.GetId()==ID_LOAD_SET) {
		selectSet();

	} else if (event.GetId()==ID_LOAD_START) {
		selectStart();

	} else if (event.GetId()==ID_MOUNT) {
		selectMount();

	} else {
		for (int i=0; i<NUM_CHAR_SLOTS; i++) {
			if (buttons[i] && (wxButton*)event.GetEventObject()==buttons[i]) {
				selectItem(i, cd.equipment[i], buttons[i]->GetLabel().GetData());
				break;
			}
		}
	}

	if (g_canvas->model->modelType == MT_NPC)
		RefreshNPCModel();
	else
		RefreshModel();
}


const char* regionPaths[NUM_REGIONS] =
{
	"",
	"Item\\TextureComponents\\ArmUpperTexture\\",
	"Item\\TextureComponents\\ArmLowerTexture\\",
	"Item\\TextureComponents\\HandTexture\\",
	"",
	"",
	"Item\\TextureComponents\\TorsoUpperTexture\\",
	"Item\\TextureComponents\\TorsoLowerTexture\\",
	"Item\\TextureComponents\\LegUpperTexture\\",
	"Item\\TextureComponents\\LegLowerTexture\\",
	"Item\\TextureComponents\\FootTexture\\"
};

void CharControl::RefreshModel()
{
	hairTex = 0;
	furTex = 0;
	gobTex = 0;
	capeTex = 0;

	// Reset geosets
	for (int i=0; i<16; i++) 
		cd.geosets[i] = 1;
	cd.geosets[1] = cd.geosets[2] = cd.geosets[3] = 0;

	// show ears, if toggled
	if (cd.showEars) 
		cd.geosets[7] = 2;

	CharTexture tex;

	CharSectionsDB::Record rec = chardb.getRecord(0);

	// HACK: for Vrykul, Alfred 2008/10/11, TODO
	if (cd.race==16)
		cd.skinColor = 5;

	// base character layer/texture
	try {
		rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::SkinType, 0, cd.skinColor, cd.useNPC);
		tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_BASE, 0);
		
		// Tauren fur
		const char *furTexName = rec.getString(CharSectionsDB::Tex2);
		if (strlen(furTexName))
			furTex = texturemanager.add(furTexName);

	} catch (CharSectionsDB::NotFound) {
		wxLogMessage(_T("Assertion base character Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	// HACK: for goblin males, explicitly load a hair texture
	if (cd.race==9 && cd.gender==0 && gobTex==0) {
        gobTex = texturemanager.add("Creature\\Goblin\\Goblin.blp");		
	}

	// Hair related boolean flags
	bool bald = false;
	bool showHair = cd.showHair;
	bool showFacialHair = cd.showFacialHair;

	if (cd.race != 9) { // Goblin chars base texture already contains all this stuff.

		// Display underwear on the model?
		if (cd.showUnderwear) {
			try {
				rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::UnderwearType, 0, cd.skinColor, cd.useNPC);
				tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_PELVIS_UPPER, 1); // pants
				tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_TORSO_UPPER, 1); // top
			} catch (CharSectionsDB::NotFound) {
				wxLogMessage(_T("DBC underwear Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
			}
		}

		// face
		try {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FaceType, cd.faceType, cd.skinColor, cd.useNPC);
			tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_FACE_LOWER, 1);
			tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_UPPER, 1);
		} catch (CharSectionsDB::NotFound) {
			wxLogMessage(_T("DBC face Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}

		// facial feature geosets
		try {
			CharFacialHairDB::Record frec = facialhairdb.getByParams(cd.race, cd.gender, cd.facialHair);
			cd.geosets[1] = frec.getUInt(CharFacialHairDB::Geoset100);
			cd.geosets[2] = frec.getUInt(CharFacialHairDB::Geoset200);
			cd.geosets[3] = frec.getUInt(CharFacialHairDB::Geoset300);
		} catch (CharFacialHairDB::NotFound) {
			wxLogMessage(_T("DBC facial feature geosets Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}

		// facial feature, gone ?
		try {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FacialHairType, cd.facialHair, cd.facialColor, 0);
			tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_FACE_LOWER, 2);
			tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_UPPER, 2);
		} catch (CharSectionsDB::NotFound) {
			wxLogMessage(_T("DBC facial feature Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	
	}

	// select hairstyle geoset(s)
	for (CharHairGeosetsDB::Iterator it = hairdb.begin(); it != hairdb.end(); ++it) {
		if (it->getUInt(CharHairGeosetsDB::Race)==cd.race && it->getUInt(CharHairGeosetsDB::Gender)==cd.gender) {
			unsigned int id = it->getUInt(CharHairGeosetsDB::Geoset);
			unsigned int section = it->getUInt(CharHairGeosetsDB::Section);

			if (id!=0) {
				for (size_t j=0; j<model->geosets.size(); j++) {
					if (model->geosets[j].id == id) 
						model->showGeosets[j] = (cd.hairStyle == section) && showHair;
				}

			} else if (cd.hairStyle==section) {
				bald = true;
			}
		}
	}

	// hair
	try {
		rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, cd.hairStyle, cd.hairColor, 0);
		const char* hairTexfn = rec.getString(CharSectionsDB::Tex1);
		if (strlen(hairTexfn)) 
			hairTex = texturemanager.add(hairTexfn);
		else {
			// oops, looks like we're missing a hair texture. Let's try with hair style #0.
			// (only a problem for orcs with no hair but some beard
			try {
				rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, 0, cd.hairColor, 0);
				hairTexfn = rec.getString(CharSectionsDB::Tex1);
				if (strlen(hairTexfn)) 
					hairTex = texturemanager.add(hairTexfn);
				else 
					hairTex = 0;
			} catch (CharSectionsDB::NotFound) {
				// oh well, give up.
				hairTex = 0; // or chartex?
			}
		}
		if (!bald) {
			tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_LOWER, 3);
			tex.addLayer(rec.getString(CharSectionsDB::Tex3), CR_FACE_UPPER, 3);
		}

	} catch (CharSectionsDB::NotFound) {
		wxLogMessage(_T("DBC hair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		hairTex = 0;
	}

	// If they have no hair, toggle the 'bald' flag.
	if (!showHair)
		bald = true;
	
	// Hide facial hair if it isn't toggled and they don't have tusks, horns, etc.
	if (!showFacialHair) {		
		try {
			CharRacesDB::Record race = racedb.getById(cd.race);
			wxString tmp = race.getString(CharRacesDB::GeoType1);
			if (tmp == "NORMAL") {
				cd.geosets[1] = 1;
				cd.geosets[2] = 1;
				cd.geosets[3] = 1;
			}
		} catch (CharRacesDB::NotFound) {
			wxLogMessage(_T("Assertion FacialHair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
    }

	/*
	// Temporary work-around - need to do more research.
	// Check to see if we are wearing a helmet - if so, we need to hide our hair
	if (cd.equipment[CS_HEAD] != 0) {
		try {
			const ItemRecord &item = items.get(cd.equipment[CS_HEAD]);
			int type = item.type;
			if (type==IT_HEAD) {
				ItemDisplayDB::Record r = itemdb.getById(item.model);
				
				int geoID;
				if(cd.gender == 0)
					geoID = r.getUInt(ItemDisplayDB::GeosetF);
				else
					geoID = r.getUInt(ItemDisplayDB::GeosetG);

				HelmGeosetDB::Record rec = helmetdb.getById(geoID);
				char c1 = rec.getByte(HelmGeosetDB::Field1);
				char c2 = rec.getByte(HelmGeosetDB::Field2);
				char c3 = rec.getByte(HelmGeosetDB::Field3);
				unsigned char c4 = rec.getByte(HelmGeosetDB::Field4);
				unsigned char c5 = rec.getByte(HelmGeosetDB::Field5);
				
				// TODO: Work out what exactly these geosets mean and act accordingly.
				// These values point to records in HelmetGeosetVisData.dbc
				// Still not sure if the 2 columns are for male / female or
				// for facial hair / normal hair
				std::cout << "----------\n" << r.getUInt(ItemDisplayDB::GeosetF) << "\t" << r.getUInt(ItemDisplayDB::GeosetG) << "\n";
				
				std::cout << (unsigned int)rec.getByte(HelmGeosetDB::Field1) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field2) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field3) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field4) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field5) << "\n";
				rec = helmetdb.getById(r.getUInt(ItemDisplayDB::GeosetG));
				std::cout << (unsigned int)rec.getByte(HelmGeosetDB::Field1) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field2) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field3) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field4) << "\t" << (unsigned int)rec.getByte(HelmGeosetDB::Field5) << "\n";
				
				
				//std::cout << (int)c1 << " " << (int)c2 << " " << (int)c3 << " " << (unsigned int)c4 << " " << (unsigned int)c5 << "\n";

				if(c1 == 1)
					showHair = false;
					
				if(c5 == 1)
					showFacialHair = false;

				//if(r.getUInt(ItemDisplayDB::GeosetG) > 265)
				//	showFacialHair = false;
			}
		} catch (...) {}
	}
	*/
	

	// check if we have a robe on
	bool hadRobe = false;
	if (cd.equipment[CS_CHEST] != 0) {
		try {
			const ItemRecord &item = items.get(cd.equipment[CS_CHEST]);
			if (item.type==IT_ROBE) {
				ItemDisplayDB::Record r = itemdb.getById(item.model);
				if (r.getUInt(ItemDisplayDB::GeosetC)==1) 
					hadRobe = true;
			}
		} catch (ItemDisplayDB::NotFound) {
			wxLogMessage(_T("Assertion robe Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// check if we have a kilt on, just like our robes
	if (cd.equipment[CS_PANTS] != 0) {
		try {
			const ItemRecord &item = items.get(cd.equipment[CS_PANTS]);
			int type = item.type;
			if (type==IT_PANTS) {
				ItemDisplayDB::Record r = itemdb.getById(item.model);
				if (r.getUInt(ItemDisplayDB::GeosetC)==1) 
					hadRobe = true;
			}
		} catch (ItemDisplayDB::NotFound) {
			wxLogMessage(_T("Assertion kilt Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// Default
	slotOrderWithRobe[7] = CS_CHEST;
	slotOrderWithRobe[8] = CS_GLOVES;

	// check the order of robe/gloves
	if (cd.equipment[CS_CHEST] && cd.equipment[CS_GLOVES]) {
		try {
			//const ItemRecord &item = items.get(cd.equipment[CS_CHEST]);
			//if (item.type==IT_ROBE) {
			//	ItemDisplayDB::Record r = itemdb.getById(item.model);
				//if (r.getUInt(ItemDisplayDB::GeosetA)>0) {
					const ItemRecord &item2 = items.get(cd.equipment[CS_GLOVES]);
					ItemDisplayDB::Record r2 = itemdb.getById(item2.model);
					if (r2.getUInt(ItemDisplayDB::GeosetA)==0) {
						slotOrderWithRobe[7] = CS_GLOVES;
						slotOrderWithRobe[8] = CS_CHEST;
					}
				//}
			//}
		} catch (ItemDisplayDB::NotFound) {
			wxLogMessage(_T("Assertion robe/gloves Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}
	
	// dressup
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		int sn = hadRobe ? slotOrderWithRobe[i] : slotOrder[i];
		if (cd.equipment[sn] != 0) 
			AddEquipment(sn, cd.equipment[sn], 10+i, tex);
	}

	// reset geosets
	for (size_t j=0; j<model->geosets.size(); j++) {
		int id = model->geosets[j].id;

		// hide top-of-head if we have hair.
		if (id == 1)
			model->showGeosets[j] = bald;

		for (int i=1; i<16; i++) {
			int a = i*100, b = (i+1) * 100;
			if (id>a && id<b) 
				model->showGeosets[j] = (id == (a + cd.geosets[i]));
		}
	}

	// finalize character texture
	tex.compose(charTex);
	
	// set replacable textures
	model->replaceTextures[1] = charTex;
	model->replaceTextures[2] = capeTex;
	model->replaceTextures[6] = hairTex;
	model->replaceTextures[8] = furTex;
	model->replaceTextures[11] = gobTex;

	/*
	for (int i=0; i<40; i++) {
		model->atts[i].dr = (model->atts[i].id==cd.hairStyle);
	}
	*/

	for (int i=0; i<NUM_SPIN_BTNS; i++)
		spinLabels[i]->SetLabel(wxString::Format("%i", spins[i]->GetValue()));


	spins[SPIN_SKIN_COLOR]->SetValue(cd.skinColor);
	spins[SPIN_FACE_TYPE]->SetValue(cd.faceType);
	spins[SPIN_HAIR_COLOR]->SetValue(cd.hairColor);
	spins[SPIN_HAIR_STYLE]->SetValue(cd.hairStyle);
	spins[SPIN_FACIAL_HAIR]->SetValue(cd.facialHair);
	spins[SPIN_FACIAL_COLOR]->SetValue(cd.facialColor);
}

void CharControl::RefreshNPCModel()
{
	hairTex = 0;
	furTex = 0;
	gobTex = 0;
	capeTex = 0;

	// Reset geosets
	for (int i=0; i<16; i++) 
		cd.geosets[i] = 1;
	cd.geosets[1] = cd.geosets[2] = cd.geosets[3] = 0;

	// show ears, if toggled
	if (cd.showEars) 
		cd.geosets[7] = 2;

	CharTexture tex;

	// Open first record to declare var
	CharSectionsDB::Record rec = chardb.getRecord(0);
	// It is VITAL that this record can be retrieved to display the NPC
	try {
		rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::SkinType, 0, cd.skinColor, cd.useNPC);
	} catch (...) {
		wxLogMessage(_T("DBC Error: %s : line #%i : %s\n\tAttempting to use character base colour."), __FILE__, __LINE__, __FUNCTION__);
		try {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::SkinType, 0, 0, cd.useNPC);
		} catch (...) {
			wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
			return;
		}
	}

	// base layer texture
	try {
		if (!customSkin.IsEmpty()) {
			tex.addLayer(customSkin, CR_BASE, 0);
		} else { 
			tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_BASE, 0);

			if (cd.showUnderwear) {
				rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::UnderwearType, 0, cd.skinColor, cd.useNPC);
				tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_PELVIS_UPPER, 1); // panties
				tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_TORSO_UPPER, 1); // bra
			}

			// face
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FaceType, cd.faceType, cd.skinColor, cd.useNPC);
			tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_FACE_LOWER, 1);
			tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_UPPER, 1);

			// facial hair
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::FacialHairType, cd.facialHair, cd.facialColor, 0);
			tex.addLayer(rec.getString(CharSectionsDB::Tex1), CR_FACE_LOWER, 2);
			tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_UPPER, 2);
		} 

		// Tauren fur
		const char *furTexName = rec.getString(CharSectionsDB::Tex2);
		if (strlen(furTexName))
			furTex = texturemanager.add(furTexName);

	} catch (...) {
		wxLogMessage(_T("Exception base layer Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	// hair
	try {
		CharSectionsDB::Record rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, cd.hairStyle, cd.hairColor, cd.useNPC);
		const char* hairTexfn = rec.getString(CharSectionsDB::Tex1);
		if (strlen(hairTexfn)) {
			hairTex = texturemanager.add(hairTexfn);
		} else {
			rec = chardb.getByParams(cd.race, cd.gender, CharSectionsDB::HairType, 1, cd.hairColor, cd.useNPC);
			hairTexfn = rec.getString(CharSectionsDB::Tex1);
			if (strlen(hairTexfn)) 
				hairTex = texturemanager.add(hairTexfn);
			else 
				hairTex = 0;
		}

		//tex.addLayer(rec.getString(CharSectionsDB::Tex2), CR_FACE_LOWER, 3);
		//tex.addLayer(rec.getString(CharSectionsDB::Tex3), CR_FACE_UPPER, 3);

	} catch (CharSectionsDB::NotFound) {
		wxLogMessage(_T("Assertion hair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		hairTex = 0;
	}
	
	bool bald = false;
	bool showHair = cd.showHair;
	bool showFacialHair = cd.showFacialHair;

	// facial hair geosets
	try {
		CharFacialHairDB::Record frec = facialhairdb.getByParams(cd.race, cd.gender, cd.facialHair);
		cd.geosets[1] = frec.getUInt(CharFacialHairDB::Geoset100);
		cd.geosets[2] = frec.getUInt(CharFacialHairDB::Geoset200);
		cd.geosets[3] = frec.getUInt(CharFacialHairDB::Geoset300);

		// Hide facial fair if it isn't toggled and they don't have tusks, horns, etc.
		if (showFacialHair == false) {		
			CharRacesDB::Record race = racedb.getById(cd.race);
			std::string tmp = race.getString(CharRacesDB::GeoType1);
			if (tmp == "NORMAL") {
				cd.geosets[1] = 1;
				cd.geosets[2] = 1;
				cd.geosets[3] = 1;
			}
		}
	} catch (CharFacialHairDB::NotFound) {
		wxLogMessage(_T("Assertion facial hair Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
	}

	// select hairstyle geoset(s)
	for (CharHairGeosetsDB::Iterator it = hairdb.begin(); it != hairdb.end(); ++it) {
		if (it->getUInt(CharHairGeosetsDB::Race)==cd.race && it->getUInt(CharHairGeosetsDB::Gender)==cd.gender) {
			unsigned int id = it->getUInt(CharHairGeosetsDB::Geoset);
			unsigned int section = it->getUInt(CharHairGeosetsDB::Section);
			if (id!=0) {
				for (size_t j=0; j<model->geosets.size(); j++) {
					if (model->geosets[j].id == id)
						model->showGeosets[j] = (cd.hairStyle==section) && showHair;
				}
			} else if (cd.hairStyle==section) 
				bald = true;
		}
	}

	// If they have no hair, toggle the 'bald' flag.
	if (!showHair)
		bald = true;
	
	// check if we have a robe on
	bool hadRobe = false;
	if (cd.equipment[CS_CHEST] != 0) {
		try {
			const ItemRecord &item = items.get(cd.equipment[CS_CHEST]);
			int type = item.type;
			if (type==IT_ROBE) {
				ItemDisplayDB::Record r = itemdb.getById(item.model);
				if (r.getUInt(ItemDisplayDB::GeosetC)==1) 
					hadRobe = true;
			}
		} catch (...) {
			wxLogMessage(_T("Exception robe Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}
	

	// check if we have a kilt on, just like our robes
	if (cd.equipment[CS_PANTS] != 0) {
		try {
			const ItemRecord &item = items.get(cd.equipment[CS_PANTS]);
			int type = item.type;
			if (type==IT_PANTS) {
				ItemDisplayDB::Record r = itemdb.getById(item.model);
				if (r.getUInt(ItemDisplayDB::GeosetC)==1) 
					hadRobe = true;
			}
		} catch (...) {
			wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// Default
	slotOrderWithRobe[7] = CS_CHEST;
	slotOrderWithRobe[8] = CS_GLOVES;

	// check the order of robe/gloves
	if (cd.equipment[CS_CHEST] && cd.equipment[CS_GLOVES]) {
		try {
			//const ItemRecord &item = items.get(cd.equipment[CS_CHEST]);
			//if (item.type==IT_ROBE) {
			//	ItemDisplayDB::Record r = itemdb.getById(item.model);
				//if (r.getUInt(ItemDisplayDB::GeosetA)>0) {
					ItemDisplayDB::Record r = itemdb.getById(cd.equipment[CS_GLOVES]);
					if (r.getUInt(ItemDisplayDB::GeosetA)==0) {
						slotOrderWithRobe[7] = CS_GLOVES;
						slotOrderWithRobe[8] = CS_CHEST;
					}
				//}
			//}
		} catch (...) {
			wxLogMessage(_T("Exception Error: %s : line #%i : %s"), __FILE__, __LINE__, __FUNCTION__);
		}
	}

	// dressup
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		int sn = hadRobe ? slotOrderWithRobe[i] : slotOrder[i];
		if (cd.equipment[sn] != 0) 
			AddEquipment(sn, cd.equipment[sn], 10+i, tex, false);
	}
	

	// reset geosets
	for (size_t j=0; j<model->geosets.size(); j++) {
		int id = model->geosets[j].id;
		
		// hide top-of-head if we have hair.
		if (id == 1) 
			model->showGeosets[j] = bald;

		for (int i=1; i<16; i++) {
			int a = i*100, b = (i+1) * 100;
			if (id>a && id<b) 
				model->showGeosets[j] = (id == (a + cd.geosets[i]));
		}
	}

	// finalize texture
	tex.compose(charTex);
	
	// set replacable textures
	model->replaceTextures[1] = charTex;
	model->replaceTextures[2] = capeTex;
	model->replaceTextures[6] = hairTex;
	model->replaceTextures[8] = furTex;
	
	model->replaceTextures[11] = gobTex;
}

void CharControl::AddEquipment(int slot, int itemnum, int layer, CharTexture &tex, bool lookup)
{
	if (slot==CS_PANTS && cd.geosets[13]==2) 
		return; // if we are wearing a robe, no pants for us! ^_^

	try {
		const ItemRecord &item = items.get(itemnum);
		int type = item.type;
		int itemID = 0;

		if (lookup)
			itemID = item.model;
		else
			itemID = itemnum;

		ItemDisplayDB::Record r = itemdb.getById(itemID);
		
		// Just a rough check to make sure textures are only being added to where they're suppose to.
		if (slot == CS_CHEST || slot == CS_SHIRT) {
			cd.geosets[8] = 1 + r.getUInt(ItemDisplayDB::GeosetA);

			tex.addLayer(makeItemTexture(CR_ARM_UPPER, r.getString(ItemDisplayDB::TexArmUpper)), CR_ARM_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_ARM_LOWER, r.getString(ItemDisplayDB::TexArmLower)), CR_ARM_LOWER, layer);

			tex.addLayer(makeItemTexture(CR_TORSO_UPPER, r.getString(ItemDisplayDB::TexChestUpper)), CR_TORSO_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_TORSO_LOWER, r.getString(ItemDisplayDB::TexChestLower)), CR_TORSO_LOWER, layer);

			if (type == IT_ROBE) {
				tex.addLayer(makeItemTexture(CR_LEG_UPPER, r.getString(ItemDisplayDB::TexLegUpper)), CR_LEG_UPPER, layer);
				tex.addLayer(makeItemTexture(CR_LEG_LOWER, r.getString(ItemDisplayDB::TexLegLower)), CR_LEG_LOWER, layer);
			}
		} /*else if (slot == CS_SHOULDER)
			tex.addLayer(makeItemTexture(CR_ARM_UPPER, r.getString(ItemDisplayDB::TexArmUpper)), CR_ARM_UPPER, layer);
		*/else if (slot == CS_BELT)
			tex.addLayer(makeItemTexture(CR_LEG_UPPER, r.getString(ItemDisplayDB::TexLegUpper)), CR_LEG_UPPER, layer);
		else if (slot == CS_BRACERS)
			tex.addLayer(makeItemTexture(CR_ARM_LOWER, r.getString(ItemDisplayDB::TexArmLower)), CR_ARM_LOWER, layer);
		else if (slot == CS_PANTS) {
			cd.geosets[9] = 1 + r.getUInt(ItemDisplayDB::GeosetB);

			tex.addLayer(makeItemTexture(CR_LEG_UPPER, r.getString(ItemDisplayDB::TexLegUpper)), CR_LEG_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_LEG_LOWER, r.getString(ItemDisplayDB::TexLegLower)), CR_LEG_LOWER, layer);
		} else if (slot == CS_GLOVES) {
			cd.geosets[4] = 1 + r.getUInt(ItemDisplayDB::GeosetA);

			tex.addLayer(makeItemTexture(CR_HAND, r.getString(ItemDisplayDB::TexHands)), CR_HAND, layer);
			tex.addLayer(makeItemTexture(CR_ARM_LOWER, r.getString(ItemDisplayDB::TexArmLower)), CR_ARM_LOWER, layer);
		} else if (slot == CS_BOOTS) { // && cd.showFeet==false) {
			cd.geosets[5] = 1 + r.getUInt(ItemDisplayDB::GeosetA);

			tex.addLayer(makeItemTexture(CR_LEG_LOWER, r.getString(ItemDisplayDB::TexLegLower)), CR_LEG_LOWER, layer);
			if (!cd.showFeet)
				tex.addLayer(makeItemTexture(CR_FOOT, r.getString(ItemDisplayDB::TexFeet)), CR_FOOT, layer);
		} else if (slot==CS_TABARD && td.showCustom) { // Display our customised tabard
			cd.geosets[12] = 2;
			tex.addLayer(td.GetBackgroundTex(CR_TORSO_UPPER).c_str(), CR_TORSO_UPPER, layer);
			tex.addLayer(td.GetBackgroundTex(CR_TORSO_LOWER).c_str(), CR_TORSO_LOWER, layer);
			tex.addLayer(td.GetIconTex(CR_TORSO_UPPER).c_str(), CR_TORSO_UPPER, layer);
			tex.addLayer(td.GetIconTex(CR_TORSO_LOWER).c_str(), CR_TORSO_LOWER, layer);
			tex.addLayer(td.GetBorderTex(CR_TORSO_UPPER).c_str(), CR_TORSO_UPPER, layer);
			tex.addLayer(td.GetBorderTex(CR_TORSO_LOWER).c_str(), CR_TORSO_LOWER, layer);

		} else if (slot==CS_TABARD) { // if its just a normal tabard then do the usual
			cd.geosets[12] = 2;
			tex.addLayer(makeItemTexture(CR_TORSO_UPPER, r.getString(ItemDisplayDB::TexChestUpper)), CR_TORSO_UPPER, layer);
			tex.addLayer(makeItemTexture(CR_TORSO_LOWER, r.getString(ItemDisplayDB::TexChestLower)), CR_TORSO_LOWER, layer);
		
		} else if (slot==CS_CAPE) { // capes
			cd.geosets[15] = 1 + r.getUInt(ItemDisplayDB::GeosetA);

			// load the cape texture
			const char *tex = r.getString(ItemDisplayDB::Skin);
			if (tex && strlen(tex)) 
				capeTex = texturemanager.add(AnimControl::makeSkinTexture("Item\\ObjectComponents\\Cape\\",tex));
		}

		// robe
		if (cd.geosets[13]==1) 
			cd.geosets[13] = 1 + r.getUInt(ItemDisplayDB::GeosetC);
		if (cd.geosets[13]==2) {
			cd.geosets[5] = 0;		// hide the boots
			//cd.geosets[9] = 0;		// hide the pants
			cd.geosets[12] = 0;		// also hide the tabard.
		}

		// gloves - this is so gloves have preference over shirt sleeves.
		if (cd.geosets[4] > 1) 
			cd.geosets[8] = 0;

	} catch (ItemDisplayDB::NotFound) {}
}

void CharControl::RefreshItem(int slot)
{	
	if (!charAtt)
		return;

	// delete all attachments in that slot
	charAtt->delSlot(slot);

	int itemnum = cd.equipment[slot];
	if (itemnum!=0) {
		// load new model(s)
		int id1=-1, id2=-1;
		wxString path;
		//float sc = 1.0f;

		if (slot==CS_HEAD) {
			id1 = 11;
			path = "Item\\ObjectComponents\\Head\\";
		} else if (slot==CS_SHOULDER) {
			id1 = 6;
			id2 = 5;
			path = "Item\\ObjectComponents\\Shoulder\\";
		} else if (slot == CS_HAND_LEFT) {
			id1 = 2;
			model->charModelDetails.closeLHand = true;
		} else if (slot == CS_HAND_RIGHT) {
			id1 = 1;
			model->charModelDetails.closeRHand = true;
		} else if (slot == CS_QUIVER) {
			id1 = 26;
			path = "Item\\ObjectComponents\\Quiver\\";
		} else 
			return;

		if (slot==CS_HAND_LEFT || slot==CS_HAND_RIGHT) {
			if (items.get(itemnum).type == IT_SHIELD) {
				path = "Item\\ObjectComponents\\Shield\\";
				id1 = 0;
			} else {
				path = "Item\\ObjectComponents\\Weapon\\";
			}

			// If we're sheathing our weapons, relocate the items to
			// their correct positions
			if (bSheathe && items.get(itemnum).sheath>0) {	
				id1 = items.get(itemnum).sheath;
				if (id1==32 && slot==CS_HAND_LEFT)
					id1 = 33;
				
				if (id1==26 && items.get(itemnum).subclass==7 && slot==CS_HAND_LEFT)
					id1 = 27;

				if (slot==CS_HAND_LEFT)
					model->charModelDetails.closeLHand = false;
				if (slot==CS_HAND_RIGHT)
					model->charModelDetails.closeRHand = false;

				/*
				26 = upper right back
				27 = upper left back
				28 = center back
				30 = upside down, upper left back -- staves, spears
				32 = left hip
				33 = right hip
				*/
			}
		}

		try {

			// This corrects the problem with trying to automatically load equipment on NPC's
			int ItemID = 0;
			if (g_canvas->model->modelType == MT_NPC)
				ItemID = itemnum;
			else {
				const ItemRecord &item = items.get(itemnum);
				ItemID = item.model;
			}
			
			ItemDisplayDB::Record r = itemdb.getById(ItemID);

			GLuint tex;
			std::string mp;
			bool succ = false;
			Attachment *att = NULL;
			Model *m = NULL;

			if (id1>=0) {
				mp = path + r.getString(ItemDisplayDB::Model);

				if (slot==CS_HEAD) {
					// sigh, head items have more crap to sort out
					mp = mp.substr(0, mp.length()-4); // delete .mdx
					mp.append("_");
					try {
						CharRacesDB::Record race = racedb.getById(cd.race);
						mp.append(race.getString(CharRacesDB::ShortName));
						mp.append(cd.gender?"F":"M");
						mp.append(".m2");
					} catch (CharRacesDB::NotFound) {
						mp = "";
					}
				}

				if (mp.length()) {
					att = charAtt->addChild(mp.c_str(), id1, slot);
					if (att) {
						m = static_cast<Model*>(att->model);
						if (m->ok) {
							mp = path + r.getString(ItemDisplayDB::Skin);
							mp.append(".blp");
							tex = texturemanager.add(mp);
							m->replaceTextures[2] = tex;
							
							succ = true;
						}
					}
				}
			}
			if (id2>=0) {
				mp = path + r.getString(ItemDisplayDB::Model2);
				if (mp.length()) {
					att = charAtt->addChild(mp.c_str(), id2, slot);
					if (att) {
						m = static_cast<Model*>(att->model);
						if (m->ok) {
							mp = path + r.getString(ItemDisplayDB::Skin2);
							mp.append(".blp");
							tex = texturemanager.add(mp);
							m->replaceTextures[2] = tex;

							succ = true;
						}
					}
				}
			}

			if (succ) {
				// Manual position correction of items equipped on the back, staves, 2h weapons, quivers, etc.
				//if (id1 > 25 && id1 < 32)
				//	att->pos = Vec3D(0.0f, 0.0f, 0.06f);

				// okay, see if we have any glowy effects
				int visualid = r.getInt(ItemDisplayDB::Visuals);
				
				if (visualid == 0) {
					if ((g_modelViewer->enchants->RHandEnchant > -1) && (slot == CS_HAND_RIGHT)) {
						visualid = g_modelViewer->enchants->RHandEnchant;
					} else if ((g_modelViewer->enchants->LHandEnchant > -1) && (slot == CS_HAND_LEFT)) {
						visualid = g_modelViewer->enchants->LHandEnchant;
					}
				}

				if (m == NULL)
					m = static_cast<Model*>(att->model);

				if (visualid > 0) {
					try {
						ItemVisualDB::Record vis = visualdb.getById(visualid);
						for (int i=0; i<5; i++) {
							// try all five visual slots
							int effectid = vis.getInt(ItemVisualDB::Effect1 + i);
							if (effectid==0 || m->attLookup[i]<0) continue;

							try {
								ItemVisualEffectDB::Record eff = effectdb.getById(effectid);
								const char *filename = eff.getString(ItemVisualEffectDB::Model);

								att->addChild(filename, i, -1);

							} catch (ItemVisualEffectDB::NotFound) {}
						}
					} catch (ItemVisualDB::NotFound) {}
				}

			} else {
				cd.equipment[slot] = 0; // no such model? :(
			}

		} catch (ItemDisplayDB::NotFound) {}
	}
}

void CharControl::RefreshCreatureItem(int slot)
{
	// delete all attachments in that slot
	g_canvas->root->delSlot(slot);

	int itemnum = cd.equipment[slot];
	if (itemnum!=0) {
		// load new model(s)
		int id1=-1;
		wxString path;
		//float sc = 1.0f;

		if (slot == CS_HAND_LEFT) 
			id1 = 2;
		else if (slot == CS_HAND_RIGHT) 
			id1 = 1;
		else 
			return;

		if (slot==CS_HAND_LEFT || slot==CS_HAND_RIGHT) {
			if (items.get(itemnum).type == IT_SHIELD) {
				path = "Item\\ObjectComponents\\Shield\\";
				id1 = 0;
			} else {
				path = "Item\\ObjectComponents\\Weapon\\";
			}
		}

		try {
			const ItemRecord &item = items.get(itemnum);
			ItemDisplayDB::Record r = itemdb.getById(item.model);

			GLuint tex;
			std::string mp;
			bool succ = false;
			Attachment *att = NULL;
			Model *m = NULL;

			if (id1>=0) {
				mp = path + r.getString(ItemDisplayDB::Model);

				if (mp.length()) {
					att = g_canvas->root->addChild(mp.c_str(), id1, slot);
					if (att) {
						m = static_cast<Model*>(att->model);
						if (m->ok) {
							mp = path + r.getString(ItemDisplayDB::Skin);
							mp.append(".blp");
							tex = texturemanager.add(mp);
							m->replaceTextures[2] = tex;
							succ = true;
						}
					}
				}
			}

			if (succ) {
				// okay, see if we have any glowy effects
				int visualid = r.getInt(ItemDisplayDB::Visuals);
				
				if (visualid == 0) {
					if ((g_modelViewer->enchants->RHandEnchant > -1) && (slot == CS_HAND_RIGHT)) {
						visualid = g_modelViewer->enchants->RHandEnchant;
					} else if ((g_modelViewer->enchants->LHandEnchant > -1) && (slot == CS_HAND_LEFT)) {
						visualid = g_modelViewer->enchants->LHandEnchant;
					}
				}

				if (m == NULL)
					m = static_cast<Model*>(att->model);

				if (visualid > 0) {
					try {
						ItemVisualDB::Record vis = visualdb.getById(visualid);
						for (int i=0; i<5; i++) {
							// try all five visual slots
							int effectid = vis.getInt(ItemVisualDB::Effect1 + i);
							if (effectid==0 || m->attLookup[i]<0) continue;

							try {
								ItemVisualEffectDB::Record eff = effectdb.getById(effectid);
								const char *filename = eff.getString(ItemVisualEffectDB::Model);

								att->addChild(filename, i, -1);

							} catch (ItemVisualEffectDB::NotFound) {}
						}
					} catch (ItemVisualDB::NotFound) {}
				}

			} else {
				cd.equipment[slot] = 0; // no such model? :(
			}

		} catch (ItemDisplayDB::NotFound) {}
	}
}

wxString CharControl::makeItemTexture(int region, const wxString name)
{
	// just return an empty filename
	if (name.Length() < 3) 
		return "";

	wxChar leggings = name[name.Length() - 2];
	
	// try prefered version first
	wxString fn = regionPaths[region];
	fn += name;
	fn += "_";

	if (leggings == 'l' || leggings == 'L')
		fn += "U";
	else
		fn += cd.gender ? "F" : "M";
		
	fn += ".blp";
	if (MPQFile::getSize(fn.fn_str()) > 0)  //MPQFile::exists(fn.c_str()) && 
		return fn;

	if (fn.Length() < 5)
		return "";

	// if that failed try alternate texture version
	if (leggings == 'l' || leggings == 'L')
		fn[fn.Length()-5] = cd.gender ? 'F' : 'M';
	else
		fn[fn.Length()-5] = 'U';

	if (MPQFile::getSize(fn.fn_str()) > 0) //MPQFile::exists(fn.c_str()) && 
		return fn;

	fn = regionPaths[region];
	fn += name;	
	fn += ".blp";

	// return the default name, nothing else could be found.
	return fn;
}

void CharTexture::compose(TextureID texID)
{
	// if we only have one texture then don't bother with compositing
	if (components.size()==1) {
		Texture temp(components[0].name);
		texturemanager.LoadBLP(texID, &temp);
		return;
	}

	std::sort(components.begin(), components.end());
	unsigned char destbuf[256*256*4], tempbuf[256*256*4];
	memset(destbuf, 0, 256*256*4);

	for (std::vector<CharTextureComponent>::iterator it = components.begin(); it != components.end(); ++it) {
		CharTextureComponent &comp = *it;
		const CharRegionCoords &coords = regions[comp.region];
		TextureID temptex = texturemanager.add(comp.name);
		Texture &tex = *((Texture*)texturemanager.items[temptex]);

		tex.getPixels(tempbuf);

		// blit the texture region over the original

		//assert(tex.w==coords.xsize && tex.h==coords.ysize);
		if (tex.w==coords.xsize && tex.h==coords.ysize) {
			for (int y=0, dy=coords.ypos; y<tex.h; y++,dy++) {
				for (int x=0, dx=coords.xpos; x<tex.w; x++,dx++) {
					unsigned char *src = tempbuf + y*tex.w*4 + x*4;
					unsigned char *dest = destbuf + dy*256*4 + dx*4;

					// this is slow and ugly but I don't care
					float r = src[3] / 255.0f;
					float ir = 1.0f - r;
					// zomg RGBA?
					dest[0] = (unsigned char)(dest[0]*ir + src[0]*r);
					dest[1] = (unsigned char)(dest[1]*ir + src[1]*r);
					dest[2] = (unsigned char)(dest[2]*ir + src[2]*r);
					dest[3] = 255;
				}
			}
		}

		texturemanager.del(temptex);
	}

	// good, upload this to video
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, destbuf);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void CharDetails::reset()
{
	skinColor = 0;
	faceType = 0;
	hairColor = 0;
	hairStyle = 0;
	facialHair = 0;

	showUnderwear = true;
	showHair = true;
	showFacialHair = true;
	showEars = true;
	showFeet = false;

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		equipment[i] = 0;
	}
}


bool correctType(int type, int slot)
{
	if (type == IT_ALL) 
		return true;

	switch (slot) {
	case CS_HEAD:		return (type == IT_HEAD);
	case CS_NECK:		return (type == IT_NECK);
	case CS_SHOULDER:	return (type == IT_SHOULDER);
	case CS_SHIRT:		return (type == IT_SHIRT);
	case CS_CHEST:		return (type == IT_CHEST || type == IT_ROBE);
	case CS_BELT:		return (type == IT_BELT);
	case CS_PANTS:		return (type == IT_PANTS);
	case CS_BOOTS:		return (type == IT_BOOTS);
	case CS_BRACERS:	return (type == IT_BRACERS);
	case CS_GLOVES:		return (type == IT_GLOVES);

	// Slight correction.  Type 21 = Lefthand weapon, Type 22 = Righthand weapon
	//case CS_HAND_RIGHT:	return (type == IT_1HANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER);
	//case CS_HAND_LEFT:	return (type == IT_1HANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED || type == IT_CLAW || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_HAND_RIGHT:	return (type == IT_LEFTHANDED || type == IT_GUN || type == IT_THROWN || type == IT_2HANDED || type == IT_DAGGER);
	case CS_HAND_LEFT:	return (type == IT_RIGHTHANDED || type == IT_BOW || type == IT_SHIELD || type == IT_2HANDED || type == IT_DAGGER || type == IT_OFFHAND);
	case CS_CAPE:		return (type == IT_CAPE);
	case CS_TABARD:		return (type == IT_TABARD);
	case CS_QUIVER:		return (type == IT_QUIVER);
	}
	return false;
}


void CharControl::ClearItemDialog()
{
	if (itemDialog) {
		itemDialog->Show(FALSE);
		itemDialog->Destroy();
		wxDELETE(itemDialog);
	}
}

void CharControl::selectItem(int slot, int current, const wxChar *caption)
{
	ClearItemDialog();

	numbers.clear();
	choices.Clear();

	std::vector<int> quality;

	// collect all items for this slot, making note of the occurring subclasses
	std::set<std::pair<int,int> > subclassesFound;
	
	int sel=0, ord=0;
	for (std::vector<ItemRecord>::iterator it = items.items.begin(); it != items.items.end(); ++it) {
		if (correctType(it->type, slot)) {
			choices.Add(CSConv(it->name));
			numbers.push_back(it->id);
			quality.push_back(it->quality);

			if (it->id == current) 
				sel = ord;

			ord++;

			if (it->itemclass > 0) 
				subclassesFound.insert(std::pair<int,int>(it->itemclass,it->subclass));
		}
	}

	// make category list
	cats.clear();
	catnames.clear();

	std::map<std::pair<int,int>, int> subclasslookup;
	for (ItemSubClassDB::Iterator it=subclassdb.begin(); it != subclassdb.end(); ++it) {
		int cl = it->getInt(ItemSubClassDB::ClassID);
		int scl = it->getInt(ItemSubClassDB::SubClassID);
		// only add the subclass if it was found in the itemlist
		if (cl>0 && subclassesFound.find(std::pair<int,int>(cl,scl)) != subclassesFound.end()) {
			
			// Used to go through the 'string fields' looking for the one with data.
			wxString str;
			/*
			for (int i = ItemSubClassDB::Name; i<18; i++)
			{
				str = CSConv(it->getString(i));
				if (!str.IsEmpty()) {
					//p.name = str.fn_str();
					break;
				}
			}
			*/
			str = CSConv(it->getString(ItemSubClassDB::Name + langID));
			//string str = it->getString(ItemSubClassDB::Name);

			int hands = it->getInt(ItemSubClassDB::Hands);
			if (hands > 0) {
				str.append(wxString::Format(" (%d-handed)", hands));
			}
			catnames.Add(str);
			subclasslookup[std::pair<int,int>(cl,scl)] = (int)catnames.size()-1;
		}
	}

	if (subclassesFound.size() > 1) {
		// build category list
		for (size_t i=0; i<numbers.size(); i++) {
			ItemRecord r = items.get(numbers[i]);
			cats.push_back(subclasslookup[std::pair<int,int>(r.itemclass, r.subclass)]);
		}

		itemDialog = new CategoryChoiceDialog(this, UPDATE_ITEM, g_modelViewer, _T("Choose an item"), caption, choices, cats, catnames, &quality);
	} else {
		itemDialog = new FilteredChoiceDialog(this, UPDATE_ITEM, g_modelViewer, _T("Choose an item"), caption, choices, &quality);
	}

	itemDialog->SetSelection(sel);

	wxSize s = itemDialog->GetSize();
	const int w = 250;
	if (s.GetWidth() > w) {
		itemDialog->SetSizeHints(w,-1,-1,-1,-1,-1);
		itemDialog->SetSize(w, -1);
	}

	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
	choosingSlot = slot;
}

/*
struct NumStringPair {
	int id;
	string name;

	const bool operator< (const NumStringPair &p) const {
		return name < p.name;
	}
};
*/

void CharControl::selectSet()
{
	ClearItemDialog();

	std::vector<NumStringPair> items;

	// Adds "none" to select
	NumStringPair n; n.id = -1; n.name = _T("---- None ----");
	items.push_back(n);

	for (ItemSetDB::Iterator it = setsdb.begin(); it != setsdb.end(); ++it) {
		int id = it->getUInt(ItemSetDB::SetID);
		if (setsdb.available(id)) {
			NumStringPair p;
			p.id = id;
/*
			p.name = "null";
			
			// Used to go through the 'string fields' looking for the one with data.
			for (int i=0; i<NUM_LOCALES; i++)
			{
				//wxString str = wxString(it->getString(i), *wxConvCurrent);
				wxString str(wxCSConv("iso-8859-1").cWC2WX(wxConvUTF8.cMB2WC(it->getString(ItemSetDB::Name + i))));
				if (!str.IsNull() && !str.IsEmpty()) {
					p.name = str.fn_str();
					break;
				}
			}
			//p.name = it->getString(ItemSetDB::Name);
*/
			p.name = CSConv(it->getString(ItemSetDB::Name + langID));
			items.push_back(p);
		}
	}

	std::sort(items.begin(), items.end());
	numbers.clear();
	choices.Clear();
	for (std::vector<NumStringPair>::iterator it = items.begin(); it != items.end(); ++it) {
		//choices.Add(wxString(it->name.c_str(), *wxConvCurrent));
		choices.Add(wxString(it->name.c_str(), *wxConvCurrent));
		numbers.push_back(it->id);
	}

	itemDialog = new FilteredChoiceDialog(this, UPDATE_SET, g_modelViewer, _("Choose an item set"), _("Item sets"), choices, NULL);
	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
}

void CharControl::selectStart()
{
	ClearItemDialog();

	numbers.clear();
	choices.Clear();

	for (StartOutfitDB::Iterator it = startdb.begin(); it != startdb.end(); ++it) {
		if ((it->getByte(StartOutfitDB::Race) == cd.race) && (it->getByte(StartOutfitDB::Gender) == cd.gender)) {
			try {
				CharClassesDB::Record r = classdb.getById(it->getByte(StartOutfitDB::Class));
/*
				for (int i=0; i<NUM_LOCALES; i++)
				{
					//wxString str = wxString(r.getString(i), *wxConvCurrent);
					wxString str(wxCSConv("iso-8859-1").cWC2WX(wxConvUTF8.cMB2WC(r.getString(CharClassesDB::Name + i))));
					if (!str.IsNull() && !str.IsEmpty()) {
						choices.Add(str);
						numbers.push_back(it->getUInt(StartOutfitDB::StartOutfitID));
						break;
					}
				}
*/
				choices.Add(CSConv(r.getString(CharClassesDB::Name + langID)));
				numbers.push_back(it->getUInt(StartOutfitDB::StartOutfitID));
			} catch (CharClassesDB::NotFound) {}
		}
	}

	itemDialog = new ChoiceDialog(this, UPDATE_START, g_modelViewer, _T("Choose a class"), _T("Classes"), choices);
	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
}

bool filterCreatures(string fn)
{
	wxString tmp(fn);
	tmp.LowerCase();
	return (tmp.Mid(0,4)=="crea" && tmp.EndsWith("m2"));
}

std::vector<wxString> creaturemodels;
std::vector<bool> ridablelist;

// TODO: Add an equivilant working version of this function for Linux / Mac OS X
void CharControl::selectMount()
{
#ifdef _WIN32
	ClearItemDialog();

	numbers.clear();
	choices.Clear();
	cats.clear();
	catnames.Clear();
	catnames.Add("Known ridable models");
	catnames.Add("Other models");

	static bool filelistInitialized = false;

	if (!filelistInitialized) {
		std::set<FileTreeItem> filelist;

		//#ifdef _WIN32
		stdext::hash_set<std::string> knownRidable;
		//#else
		//	__gnu_cxx::hash_set<std::string> knownRidable;
		//#endif

		getFileLists(filelist, filterCreatures);

		FILE *f = fopen("ridable.csv","r");
		if (f) {
			wxString tmp;
			while (!feof(f)) {
				char buf[1024];
				fgets(buf, 1024, f);
				size_t l = strlen(buf);
				if (buf[l-1]=='\n') 
					buf[l-1]=0;
				tmp = buf;
				tmp.LowerCase();
				knownRidable.insert(tmp.c_str());
				//creaturemodels.push_back(string(buf));
			}
		}
		
		for (std::set<FileTreeItem>::iterator it = filelist.begin(); it != filelist.end(); ++it) {
			wxString str((*it).fn);
			str.MakeLower();
			creaturemodels.push_back(str);
			ridablelist.push_back(knownRidable.find(str)!=knownRidable.end());
		}
		filelistInitialized = true;
	}

	choices.Add("---- None ----");
	cats.push_back(0);
	
	for (size_t i=0; i<creaturemodels.size(); i++) {
		choices.Add(wxString(creaturemodels[i].c_str() + 9, *wxConvCurrent));
		cats.push_back(ridablelist[i]?0:1);
	}

	// TODO: obtain a list of "known ridable" models, and filter the list into two categories
	itemDialog = new FilteredChoiceDialog(this, UPDATE_MOUNT, g_modelViewer, _T("Choose a mount"), _T("Creatures"), choices, 0);
	CategoryChoiceDialog *itemDialog = new CategoryChoiceDialog(this, UPDATE_MOUNT, g_modelViewer, _T("Choose a mount"), _T("Creatures"), choices, cats, catnames, 0);
	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Check(1, false);
	itemDialog->DoFilter();
	itemDialog->Show();

	const int w = 250;
	itemDialog->SetSizeHints(w,-1,-1,-1,-1,-1);
	itemDialog->SetSize(w, -1); 
	this->itemDialog = itemDialog;
#endif
}

void CharControl::selectNPC()
{
	ClearItemDialog();

	numbers.clear();
	choices.Clear();

	std::vector<int> quality;
	
	//choices.Add("---- None ----");
	//numbers.push_back(-1);
	//quality.push_back(0);

	// collect all items for this type, making note of the occurring subclasses
	std::vector<int> typesFound;
	int sel=0, ord=0;

	for (std::vector<NPCRecord>::iterator it=npcs.npcs.begin();  it!=npcs.npcs.end(); ++it) {
		if (it->model > 0) {
			choices.Add(CSConv(it->name));
			numbers.push_back(it->id);
			quality.push_back(0);

			/*
			if (it->id == current) 
				sel = ord;
			*/
			ord++;
			
			if ((*it).type > 0) 
				typesFound.push_back(it->type);
		}
	}
	
	// make category list
	cats.clear();
	catnames.clear();

	std::map<int, int> typeLookup;
	for (CreatureTypeDB::Iterator it=npctypedb.begin();  it!=npctypedb.end(); ++it) {
		int type = it->getUInt(CreatureTypeDB::ID);

		// Used to go through the 'string fields' looking for the one with data.
		// This is a problem when the DBC files are the non-english ones.
		wxString str;
		/*
		for (int i = CreatureTypeDB::Name; i<6; i++)
		{
			str = CSConv(it->getString(i));
			if (!str.IsEmpty()) {
				break;
			}
		}*/
		str = CSConv(it->getString(CreatureTypeDB::Name + langID));

		catnames.Add(str);
		typeLookup[type] = (int)catnames.size()-1;
	}

	if (typesFound.size() > 1) {
		// build category list
		for (size_t i=0; i<numbers.size(); i++) {
			NPCRecord r = npcs.getByID(numbers[i]);
			cats.push_back(typeLookup[r.type]);
		}

		itemDialog = new CategoryChoiceDialog(this, UPDATE_NPC, g_modelViewer, _T("Select an NPC"), _T("NPC Models"), choices, cats, catnames, &quality, false, true);
	} else {
		itemDialog = new FilteredChoiceDialog(this, UPDATE_NPC, g_modelViewer, _T("Select an NPC"), _T("NPC Models"), choices, &quality, false);
	}
	
	itemDialog->SetSelection(sel);
	
	wxSize s = itemDialog->GetSize();
	const int w = 250;
	if (s.GetWidth() > w) {
		itemDialog->SetSizeHints(w,-1,-1,-1,-1,-1);
		itemDialog->SetSize(w, -1);
	}

	itemDialog->Move(itemDialog->GetParent()->GetPosition() + wxPoint(4,64));
	itemDialog->Show();
}

void CharControl::OnUpdateItem(int type, int id)
{
	switch (type) {
	case UPDATE_ITEM:
		if (choosingSlot == CS_HAND_LEFT)
			model->charModelDetails.closeLHand = false;
		else if (choosingSlot == CS_HAND_RIGHT)
			model->charModelDetails.closeRHand = false;

		cd.equipment[choosingSlot] = numbers[id];
		if (slotHasModel(choosingSlot))
			RefreshItem(choosingSlot);
		labels[choosingSlot]->SetLabel(CSConv(items.get(cd.equipment[choosingSlot]).name));

		// Check if its a 'guild tabard'
		if (choosingSlot == CS_TABARD) 
			td.showCustom = (labels[choosingSlot]->GetLabel() == "Guild Tabard");

		break;

	case UPDATE_SET:
		id = numbers[id];

		if (id) {
			for (int i=0; i<NUM_CHAR_SLOTS; i++) {
				//if (i!=CS_HAND_LEFT && i!=CS_HAND_RIGHT) 
				cd.equipment[i] = 0;
			}
			cd.loadSet(setsdb, items, id);
			RefreshEquipment();
			RefreshModel();
		}
		break;

	case UPDATE_START:
		id = numbers[id];

		if (id) {
			for (int i=0; i<NUM_CHAR_SLOTS; i++) cd.equipment[i] = 0;
			cd.loadStart(startdb, items, id);
			RefreshEquipment();
		}
		break;

	case UPDATE_MOUNT:
		//canvas->timer.Stop();

		if (g_canvas->root->model) {
			delete g_canvas->root->model;
			g_canvas->root->model = 0;
			g_canvas->model = 0;
		}

		if (id == 0) {
			// clearing the mount
			model->charModelDetails.isMounted = false;
			g_canvas->model = model;
			g_canvas->ResetView();
			charAtt->scale = g_canvas->root->scale;
			charAtt->id = 0;
			g_animControl->UpdateModel(model);
		} else {
			//wxString fn(creaturemodels[id-1].c_str());
			Model *m = new Model(creaturemodels[id-1], false);
			m->isMount = true;

			// TODO: check if model is ridable
			g_canvas->root->model = m;
			g_canvas->model = m;
			g_animControl->UpdateModel(m);
			
			// find the "mount" animation
			for (size_t i=0; i<model->header.nAnimations; i++) {
				if (model->anims[i].animID == 91) {
					model->animManager->Stop();
					model->currentAnim = (int)i;
					model->animManager->Set(0,(short)i,0);
					break;
				}
			}
			
			g_canvas->curAtt = g_canvas->root;
			charAtt->parent = g_canvas->root;
			//charAtt->id = 42;
			model->charModelDetails.isMounted = true;
			

			// Need to set this - but from what
			// Model data doesn't contain sizes for different race/gender
			// Character data doesn't contain sizes for different mounts
			// possibly some formula that from both models that needs to be calculated.
			// For "Taxi" mounts scale should be 1.0f I think, for now I'll ignore them
			// I really have no idea!  
			if(creaturemodels[id-1].Mid(9,9).IsSameAs("Kodobeast", false))
				charAtt->scale = 2.25f;
			else
				charAtt->scale = 1.0f;
			
			//Model *mChar = static_cast<Model*>(charAtt->model);
			//charAtt->scale = m->rad / mChar->rad;

			// Human Male = 2.0346599
			// NE Male = 2.5721216
			// NE Female = 2.2764397

			// RidingFrostSaber = 2.4360743
			// 1.00000

			//canvas->root->scale = 0.5f;
			//Attachment *att = charAtt->addChild("World\\ArtTest\\Boxtest\\xyz.m2", 24, -1);
			//m-> = att->scale;
			//delete att;

			g_canvas->ResetView();
			g_canvas->model->rot.x = 0; // mounted characters look better from the side
		}
		//canvas->timer.Start();
		break;

	case UPDATE_CREATURE_ITEM:
		cd.equipment[choosingSlot] = numbers[id];
		RefreshCreatureItem(choosingSlot);
		return;

	case UPDATE_NPC:
		g_modelViewer->LoadNPC(npcs.get(id).model);

		break;
	}

	//  Update controls associated
	g_modelViewer->UpdateControls();
}

void CharControl::OnTabardSpin(wxSpinEvent &event)
{
	if (!g_canvas || !g_canvas->model || g_canvas->model->modelType == MT_NPC)
		return;

	switch (event.GetId()) {
	case ID_TABARD_ICON:
		td.Icon = event.GetPosition();
		break;
	case ID_TABARD_ICONCOLOR:
		td.IconColor = event.GetPosition();
		break;
	case ID_TABARD_BORDER:
        td.Border = event.GetPosition();
		if (td.Border > 5)
		{
			td.BorderColor = 0;
			tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetValue(0);
			tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, 3);
		}
		else
			tabardSpins[SPIN_TABARD_BORDERCOLOR]->SetRange(0, 16);
		break;
	case ID_TABARD_BORDERCOLOR:
		td.BorderColor = event.GetPosition();
		break;
	case ID_TABARD_BACKGROUND:
		td.Background = event.GetPosition();
		break;
	}

	RefreshModel();
}

string TabardDetails::GetBackgroundTex(int slot)
{
	ostringstream tmpStream;

	string tmpU = "textures\\GuildEmblems\\Background_";
	if (Background < 10)
		tmpU += "0";
	tmpStream << Background;
	tmpU += tmpStream.str();
	tmpU += "_TU_U.blp";
	
	string tmpL = tmpU;
	tmpL[37] = 'L';

	if (slot == CR_TORSO_UPPER)
		return tmpU;
	else
		return tmpL;
}

string TabardDetails::GetBorderTex(int slot)
{
	ostringstream tmpStream;

	string tmpU = "textures\\GuildEmblems\\Border_";

	if (Border < 10)
		tmpU += "0";
	tmpStream << Border << "_";
	tmpU += tmpStream.str();
	tmpStream.flush();
	tmpStream.str("");

	if (BorderColor < 10)
		tmpU += "0";
	tmpStream << BorderColor;
	tmpU += tmpStream.str();

	tmpU += "_TU_U.blp";
	
	string tmpL = tmpU;
	tmpL[36] = 'L';

	if (slot == CR_TORSO_UPPER)
		return tmpU;
	else
		return tmpL;
}

string TabardDetails::GetIconTex(int slot)
{
	ostringstream tmpStream;

	string tmpU = "textures\\GuildEmblems\\Emblem_";

	if(Icon < 10)
		tmpU += "0";
	tmpStream << Icon << "_";
	tmpU += tmpStream.str();
	tmpStream.flush();
	tmpStream.str("");

	if(IconColor < 10)
		tmpU += "0";
	tmpStream << IconColor;
	tmpU += tmpStream.str();

	tmpU += "_TU_U.blp";
	
	string tmpL = tmpU;
	tmpL[tmpU.length() - 7] = 'L';

	if(slot == CR_TORSO_UPPER)
		return tmpU;
	else
		return tmpL;
}

void CharDetails::save(wxString fn, TabardDetails *td)
{
	// TODO: save/load as xml?
	// wx/xml/xml.h says the api will change, do not use etc etc.

	ofstream f(fn.fn_str(), ios_base::out|ios_base::trunc);
	f << race << " " << gender << endl;
	f << skinColor << " " << faceType << " " << hairColor << " " << hairStyle << " " << facialHair << " " << facialColor << endl;
	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		f << equipment[i] << endl;
	}

	// 5976 is the ID value for "Guild Tabard"
	if (equipment[CS_TABARD] == 5976) {
		f << td->Background << " " << td->Border << " " << td->BorderColor << " " << td->Icon << " " << td->IconColor << endl;
	}
	f.close();
}

bool CharDetails::load(wxString fn, TabardDetails *td)
{
	unsigned int r, g;
	int tmp;
	bool same = false;

	// for (int i=0; i<NUM_CHAR_SLOTS; i++) 
			// equipment[i] = 0;

	ifstream f(fn.fn_str());
	f >> r >> g;

	if (r==race && g==gender) {
		f >> skinColor >> faceType >> hairColor >> hairStyle >> facialHair >> facialColor;
		same = true;
	} else {
		int dummy;
		for (int i=0; i<6; i++) f >> dummy;
	}

	for (int i=0; i<NUM_CHAR_SLOTS; i++) {
		f >> tmp;

		// 
		if (tmp > 0)
			equipment[i] = tmp;
	}

	// 5976 is the ID value for "Guild Tabard"
	if (equipment[CS_TABARD] == 5976 && !f.eof()) {
		f >> td->Background >> td->Border >> td->BorderColor >> td->Icon >> td->IconColor;
		td->showCustom = true;
	}

	f.close();
	return same;
}

void CharDetails::loadSet(ItemSetDB &sets, ItemDatabase &items, int setid)
{
	try {
		ItemSetDB::Record rec = sets.getById(setid);
		for (size_t i=0; i<ItemSetDB::NumItems; i++) {
			int id = rec.getInt(ItemSetDB::ItemIDBase + i);

			//if (id==0)
			//	continue;

			const ItemRecord &r = items.get(id);
			if (r.type > 0) {
				// find a slot for it
				for (int s=0; s<NUM_CHAR_SLOTS; s++) {
					if (correctType(r.type, s)) {
						equipment[s] = id;
						break;
					}
				}
			}
		}
	} catch (ItemSetDB::NotFound) {}
}

void CharDetails::loadStart(StartOutfitDB &start, ItemDatabase &items, int setid)
{
	try {
		StartOutfitDB::Record rec = start.getById(setid);
		for (size_t i=0; i<StartOutfitDB::NumItems; i++) {
			int id = rec.getInt(StartOutfitDB::ItemIDBase + i);
			if (id==0) continue;
			const ItemRecord &r = items.get(id);
			if (r.type > 0) {
				// find a slot for it
				for (int s=0; s<NUM_CHAR_SLOTS; s++) {
					if (correctType(r.type, s)) {
						equipment[s] = id;
						break;
					}
				}
			}
		}
	} catch (ItemSetDB::NotFound) {}
}

const std::string CharControl::selectCharModel()
{
	wxArrayString arr;
	std::vector<int> ids;
	for (CharRacesDB::Iterator it = racedb.begin(); it != racedb.end(); ++it) {
		char buf[64];
		sprintf(buf,"%s Male", it->getString(CharRacesDB::FullName));
		arr.Add(buf);
		sprintf(buf,"%s Female", it->getString(CharRacesDB::FullName));
		arr.Add(buf);
		ids.push_back(it->getInt(CharRacesDB::RaceID));
	}
	wxSingleChoiceDialog dialog(this, _T("Choose a character model"), _T("Races"), arr);
	if (dialog.ShowModal() == wxID_OK) {
		int sel = dialog.GetSelection();
		int raceid = ids[sel >> 1];
		int gender = sel & 1;
		string genderStr = gender ? "Female" : "Male";
		try {
			CharRacesDB::Record r = racedb.getById(raceid);
			std::string path = "Character\\";
			path += r.getString(CharRacesDB::Name);
			path += "\\" + genderStr + "\\";
			path += r.getString(CharRacesDB::Name);
			path += genderStr + ".m2";
			return path;
		} catch (CharRacesDB::NotFound) {
			return ""; // wtf
		}
	}
	return "";
}


