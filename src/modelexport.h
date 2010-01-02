
#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

void ExportM2to3DS(Model *m, const char *fn, bool init);

void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init);

void ExportM2toLWO(Attachment *att, Model *m, const char *fn, bool init);

void ExportM2toOBJ(Model *m, const char *fn, bool init);

void ExportWMOtoLWO(WMO *m, const char *fn);

void ExportWMOObjectstoLWO(WMO *m, const char *fn);

void ExportWMOtoOBJ(WMO *m, const char *fn);

void SaveTexture(wxString fn);

#endif
