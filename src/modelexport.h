
#ifndef MODELEXPORT_H
#define MODELEXPORT_H

#include "model.h"
#include "modelcanvas.h"

void ExportM2to3DS(Model *m, const char *fn);

void ExportM2toMS3D(Attachment *att, const char *fn);

void ExportM2toLWO(Model *m, const char *fn);

void ExportM2toOBJ(Model *m, const char *fn);


#endif
