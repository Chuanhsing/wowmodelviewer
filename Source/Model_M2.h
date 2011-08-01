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

template <class M2Version>
class Model_M2;

typedef Model_M2<Model_M2_v00>	Model_M2_00;		// Vanilla M2 Model
typedef Model_M2<Model_M2_v04>	Model_M2_04;		// Burning Crusade M2 Model
typedef Model_M2<Model_M2_v08>	Model_M2_08;		// Wrath M2 Model
typedef Model_M2<Model_M2_v09>	Model_M2_09;		// First Cataclysm M2 Model
typedef Model_M2<Model_M2_v10>	Model_M2_10;		// Second Cataclysm M2 Model

// Primary M2 Class
// Store Data, and Add Functions.
template <class M2Version>
class Model_M2
{


};

#endif