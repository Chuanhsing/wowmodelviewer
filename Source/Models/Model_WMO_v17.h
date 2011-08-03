/*
	WMO Model, Version 17
	For Any version of WoW
*/
#ifndef MODEL_WMO_V17_H
#define MODEL_WMO_V17_H

#include "..\enums.h"

class Model_WMO_v17
{
public:
	// Function List
	size_t ReadFile();		// Common function, should be in every model version.
};


// --== WMO Functions ==--

// -= ReadFile =-
// Since every model version is different, every model version should have it's own ReadFile function.
size_t Model_WMO_v17::ReadFile(){
	return MODELREADERROR_OKAY;
}

#endif