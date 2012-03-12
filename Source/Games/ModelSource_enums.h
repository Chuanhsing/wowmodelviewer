#ifndef MODELSOURCE_ENUMS_H
#define MODELSOURCE_ENUMS_H

/* --== Model Source Codes ==--

	This is a list used to identify the origin of a model when used in the ModelViewerModel format.

*/

enum GameSourceID{
	GAME_WORLDOFWARCRAFT = 0
	
	/* Possible future games, as examples ONLY!
	
	GAME_STARCRAFTII,
	GAME_DIABLO3,
	GAME_STARWARSTHEOLDREPUBLIC
	
	*/
};

enum ModelSource{
	NOMODELSOURCE = 0,				// No Model source will declare the model invalid.

	// World of Warcraft model sources
	WORLDOFWARCRAFT_M2,				// M2 Model file
	WORLDOFWARCRAFT_WMO,			// WMO Set file
	WORLDOFWARCRAFT_ADT,			// ADT Landscape file
};

#endif