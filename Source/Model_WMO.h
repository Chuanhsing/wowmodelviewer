/*
	Global WMO Header File
	Call this when you need access to a WMO file.
*/
#ifndef MODEL_WMO_H
#define MODEL_WMO_H

// File Formats
#include "Models\Model_WMO_v17.h"

// Primary WMO Class
// Store Data, and Add Functions.
template <class WMOVersion>
class Model_WMO
{
public:
	// Constructor
	
	Model_WMO<WMOVersion>() {};

	// Data structures
	QString ModelName;		// Complete File Path & Name for this model.
	QString FileName;		// Just the File Name of the model.
	QString PathName;		// Just the Path for the model.
};


#endif