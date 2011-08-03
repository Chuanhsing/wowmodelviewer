/*
	M2 Model, Version 9
	For Cataclysm, before Patch 12319
*/
#ifndef MODEL_M2_V09_H
#define MODEL_M2_V09_H

#include "..\enums.h"

class Model_M2_v09
{
public:
	// Function List
	size_t ReadFile();		// Common function, should be in every model version.
};


// --== M2 Functions ==--

// -= ReadFile =-
// Since every model version is different, every model version should have it's own ReadFile function.
size_t Model_M2_v09::ReadFile(){
	return MODELREADERROR_OKAY;
}

#endif