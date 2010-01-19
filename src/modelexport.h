
#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

#define RADIAN 57.295779513082320876798154814114

// If your exported doesn't do WMO or m2 files, include a faux function anyways. Then add a non-working
// function at the bottom of modelexport.cpp so it has something to look for. The non-working exporter
// can be disabled in filecontrol.cpp, so it won't ever run.

//3D Studio Max
void ExportM2to3DS(Model *m, const char *fn, bool init);
void ExportWMOto3DS(WMO *m, const char *fn);

// Milkshape
void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoMS3D(WMO *m, const char *fn);

// Wavefront Object
void ExportM2toOBJ(Model *m, const char *fn, bool init);
void ExportWMOtoOBJ(WMO *m, const char *fn);

// X3D/XHTML
void ExportM2toX3D(Model *m, const char *fn, bool init);
void ExportM2toXHTML(Model *m, const char *fn, bool init);
void ExportWMOtoX3D(WMO *m, const char *fn);
void ExportWMOtoXHTML(WMO *m, const char *fn);

// Lightwave
void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoLWO(WMO *m, const char *fn);
void ExportWMOObjectstoLWO(WMO *m, const char *fn);
#ifdef _DEBUG
	void ExportWMOtoLWO2(WMO *m, const char *fn);
#endif

// Collada
void ExportM2toCOLLADA(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoCOLLADA(WMO *m, const char *fn);

// Common functions
void SaveTexture(wxString fn);
void SaveTexture2(wxString infn, wxString outfn);
Vec3D QuaternionToXYZ(Vec3D Dir, float W);



// BLP format
struct BLPHeader {
	char		Type[4];
	uint32		Version;
	uint8		Compression;
	uint8		AlphaDepth;
	uint8		AlphaCompression;
	uint8		MipMap;
	uint32		sizeX;
	uint32		sizeY;
	uint32 		offsetMipMap[16];
	uint32		sizeMipMap[16];
};



#endif
