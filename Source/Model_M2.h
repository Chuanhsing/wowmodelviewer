/*
	Global M2 Header File
	Call this when you need access to a M2 file.
*/
#ifndef MODEL_M2_H
#define MODEL_M2_H

// File Formats
#include "Models\Model_M2_v00.h"
#include "Models\Model_M2_v04.h"
#include "Models\Model_M2_v08.h"
#include "Models\Model_M2_v09.h"
#include "Models\Model_M2_v10.h"

// Primary M2 Class
// Store Data, and Add Functions.
template <class M2Version>
class Model_M2
{
public:
	// Constructor
	Model_M2<M2Version>() {};

	// Data structures
	QString ModelName;		// Complete File Path & Name for this model.
	QString FileName;		// Just the File Name of the model.
	QString PathName;		// Just the Path for the model.
};

#endif