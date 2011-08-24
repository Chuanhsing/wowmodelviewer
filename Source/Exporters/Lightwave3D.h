#ifndef LIGHTWAVE3D_H
#define LIGHTWAVE3D_H

#include "Settings_Exporters.h"

// Initialize Settings & Variables
void Lightwave3D_Init(){
	QString g = "Lightwave3D/";		// GroupPrefix

	SettingsExList.insert(g+"BuildContentDirs", true);
}

#endif