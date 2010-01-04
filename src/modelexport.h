
#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

//3D Studio Max
void ExportM2to3DS(Model *m, const char *fn, bool init);
void ExportWMOto3DS(WMO *m, const char *fn);

// Milkshape
void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init);
void ExportWMOtoMS3D(WMO *m, const char *fn);

// Wavefront Object
void ExportM2toOBJ(Model *m, const char *fn, bool init);
void ExportWMOtoOBJ(WMO *m, const char *fn);

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

#endif
