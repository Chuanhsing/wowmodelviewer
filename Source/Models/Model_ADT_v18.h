/*
	ADT Model, Version 18
	For Any version of WoW
*/
#ifndef MODEL_ADT_V18_H
#define MODEL_ADT_V18_H

#include "..\enums.h"

class Model_ADT_v18
{
public:
	// Function List
	size_t ReadFile();		// Common function, should be in every model version.
};


// --== ADT Functions ==--

// -= ReadFile =-
// Since every model version is different, every model version should have it's own ReadFile function.
size_t Model_ADT_v18::ReadFile(){
	// Read a normal ADT file.

	// Read the split-up ADT files use in Cataclysm.

	return MODELREADERROR_OKAY;
}


#endif