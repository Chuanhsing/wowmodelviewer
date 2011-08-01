/*
	Global WMO Header File
	Call this when you need access to a WMO file.
*/
#ifndef MODEL_WMO_H
#define MODEL_WMO_H

// File Formats
#include "Models\Model_WMO_v17.h"

template <class WMOVersion>
class Model_WMO;

typedef Model_WMO<Model_WMO_v17> Model_WMO_17;		// All known WMO files

// Primary WMO Class
// Store Data, and Add Functions.
template <class WMOVersion>
class Model_WMO
{

};

#endif