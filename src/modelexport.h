#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

#define RADIAN 57.295779513082320876798154814114

// Structures
struct Vertex3f {
	float x;
	float y;
	float z;
};

struct ModelData {
	Vertex3f vertex;
	float tu;
	float tv;
	Vertex3f normal;
	unsigned short groupIndex;
	char boneid;
	char boneidEx[3];
	char weight[4];
};

struct GroupData {
	ModelRenderPass p;
	Model *m;
};

static wxString Attach_Names[] = { // enum POSITION_SLOTS
	_T("Left wrist (shield) / Mount"), // 0
	_T("Right Palm"), 
	_T("Left Palm"), 
	_T("Right Elbow"), 
	_T("Left Elbow"), 
	_T("Right Shoulder"), // 5
	_T("Left Shoulder"), 
	_T("Right Knee"), 
	_T("Left Knee"), 
	_T("Right Hip"),
	_T("Left Hip"), // 10
	_T("Helmet"), 
	_T("Back"), 
	_T("Right Shoulder Horizontal"), 
	_T("Left Shoulder Horizontal"), 
	_T("Front Hit Region"), // 15
	_T("Rear Hit Region"), 
	_T("Mouth"), 
	_T("Head Region"), 
	_T("Base"),
	_T("Above"), // 20
	_T("Pre-Cast 1 L"), 
	_T("Pre-Cast 1 R"), 
	_T("Pre-Cast 2 L"), 
	_T("Pre-Cast 2 R"),
	_T("Pre-Cast 3"), //25 
	_T("Upper Back Sheath R"), 
	_T("Upper Back Sheath L"), 
	_T("Middle Back Sheath"), 
	_T("Belly"), 
	_T("Reverse Back, Up Back L"), //30
	_T("Right Back"), 
	_T("Left Hip Sheath"), 
	_T("Right Hip Sheath"), 
	_T("Spell Impact"),
	_T("Right Palm (Unk1)"), //35
	_T("Right Palm (Unk2)"),
	_T("Demolishervehicle"),
	_T("Demolishervehicle2"),
	_T("Vehicle Seat1"),
	_T("Vehicle Seat2"), // 40
	_T("Vehicle Seat3"),
	_T("Vehicle Seat4"),
};

static wxString Bone_Names[] = { // enum KeyBoneTable
	_T("ArmL"), // 0
	_T("ArmR"),
	_T("ShoulderL"),
	_T("ShoulderR"),
	_T("SpineLow"),
	_T("Waist"), // 5
	_T("Head"),
	_T("Jaw"),
	_T("RFingerIndex"),
	_T("RFingerMiddle"),
	_T("RFingerPinky"), // 10
	_T("RFingerRing"),
	_T("RThumb"),
	_T("LFingerIndex"),
	_T("LFingerMiddle"),
	_T("LFingerPinky"), // 15
	_T("LFingerRing"),
	_T("LThumb"),
	_T("BTH"),
	_T("CSR"),
	_T("CSL"), // 20
	_T("Breath"),
	_T("Name"),
	_T("NameMount"),
	_T("CHD"),
	_T("CCH"), // 25
	_T("Root"),
	_T("Wheel1"),
	_T("Wheel2"),
	_T("Wheel3"),
	_T("Wheel4"), // 30
	_T("Wheel5"),
	_T("Wheel6"),
	_T("Wheel7"),
	_T("Wheel8"),
	_T("")
};


// Common functions
void LogExportData(wxString FileExtension, wxString Directory, wxString ExportType);
void SaveTexture(wxString fn);
void SaveTexture2(wxString file, wxString outdir, wxString ExportID, wxString suffix);
Vec3D QuaternionToXYZ(Vec3D Dir, float W);
void InitCommon(Attachment *att, bool init, ModelData *&verts, GroupData *&groups, unsigned short &numVerts, unsigned short &numGroups, unsigned short &numFaces);
wxString GetM2TextureName(Model *m, const char *fn, ModelRenderPass p, int PassNumber);
void MakeModelFaceForwards(Vec3D &vect, bool flipX);

// --== Exporter Functions ==--
// If your exporter doesn't do WMO or M2 files, include a faux function anyways. Then add a non-working
// function at the bottom of modelexport.cpp so it has something to look for. The non-working exporter
// can be disabled in filecontrol.cpp, so it won't ever run.

// Raw Model File
void SaveBaseFile();

// Lightwave
void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoLWO(WMO *m, const char *fn);
void ExportADTtoLWO(MapTile *m, const char *fn);

// Wavefront Object
void ExportM2toOBJ(Attachment *att, Model *m, wxString fn, bool init);
void ExportWMOtoOBJ(WMO *m, wxString fn);

// Milkshape
void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoMS3D(WMO *m, const char *fn);

// Collada
void ExportM2toCOLLADA(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoCOLLADA(WMO *m, const char *fn);

//3D Studio Max
void ExportM2to3DS(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOto3DS(WMO *m, const char *fn);

// X3D/XHTML
void ExportM2toX3D(Model *m, const char *fn, bool init);
void ExportM2toXHTML(Model *m, const char *fn, bool init);
void ExportWMOtoX3D(WMO *m, const char *fn);
void ExportWMOtoXHTML(WMO *m, const char *fn);

// Ogre XML
void ExportM2toOgreXml(Model *m, const char *fn, bool init);
void ExportWMOtoOgreXml(WMO *m, const char *fn);

// FBX
void ExportM2toFBX(Model* m, const char* fn, bool init);
void ExportWMOtoFBX(WMO* m, const char* fn);

// M3
void ExportM2toM3(Attachment *att, Model* m, const char* fn, bool init);

#endif
