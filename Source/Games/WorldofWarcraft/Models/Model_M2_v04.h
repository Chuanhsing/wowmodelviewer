/*
	M2 Model, Version 4
	For The Burning Crusade
*/
#ifndef MODEL_M2_V04_H
#define MODEL_M2_V04_H

#include "..\enums.h"

class Model_M2_v04
{
public:
	// Function List
	size_t ReadFile();		// Common function, should be in every model version.
};


// --== M2 Functions ==--

// -= ReadFile =-
// Since every model version is different, every model version should have it's own ReadFile function.
size_t Model_M2_v04::ReadFile(){
	return MODELREADERROR_OKAY;
}

#endif