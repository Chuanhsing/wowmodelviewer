/*
	Global ADT Header File
	Call this when you need access to an ADT file.
*/
#ifndef MODEL_ADT_H
#define MODEL_ADT_H

// File Formats
#include "Models\Model_ADT_v18.h"

template <class ADTVersion>
class Model_ADT;

typedef Model_ADT<Model_ADT_v18>	Model_ADT_18;		// All known ADT files

// --== Primary ADT Class ==--
// Store Data, and Add Functions.
template <class ADTVersion>
class Model_ADT
{

	// Functions
	void ReadFile();
};

// --== ADT Functions ==--
template <class ADTVersion>
void ReadFile(){
	// Read a normal ADT file.

	// Read the split-up ADT files.
}

#endif