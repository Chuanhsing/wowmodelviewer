
#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

void ExportM2to3DS(Model *m, const char *fn);

void ExportM2toMS3D(Attachment *att, Model *m, const char *fn, bool init);

void ExportM2toLWO2(Attachment *att, Model *m, const char *fn, bool init);

void ExportM2toLWO(Model *m, const char *fn);

void ExportM2toOBJ(Model *m, const char *fn);

void ExportWMOto3DS(WMO *m, const char *fn);
 
void ExportWMOtoMS3D(WMO *m, const char *fn);

void ExportWMOtoLWO(WMO *m, const char *fn);

void ExportWMOtoOBJ(WMO *m, const char *fn);


#endif
