/*
	Global WMO Header File
	Call this when you need access to a WMO file.
*/
#ifndef MODEL_WMO_H
#define MODEL_WMO_H

#include "MPQ.h"

// File Formats
#include "Models\Model_WMO_v17.h"

// Primary WMO Class
// Store Data, and Add Functions.
template <class WMOVersion>
class Model_WMO : public cModelBase
{
public:
	// Constructor
	Model_WMO<WMOVersion>() {};
};


#endif