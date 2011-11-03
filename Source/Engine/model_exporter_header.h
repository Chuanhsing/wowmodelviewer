/*
	3D Model Exporter Header

	By stating the exporter's capabilities, as layed out in this file,
	we can easily build a chart, showing users what each exporter can do.
*/

#ifndef MODEL_EXPORTER_HEADER_H
#define MODEL_EXPORTER_HEADER_H

#include <qstring.h>

namespace WMVEngine {

// This increments every time we change the ExporterCanDo class
#define MODELEXPORTERHEADERVERSION 1;

// This class defines the capabilities of exporters.
// Variables that are not set by plugins are considered to be set to false.
class ExporterCanDo {
public:
	QString ExporterNotes;					// A string to write any notes about the exporter in.
	int HeaderVersion;						// The version of the Exporter Header we're using.

	// M2 Capabilities
	bool M2_Export;							// This Plugin can export M2 models
	bool M2_InitialPose;					// Can export the initial, unanimated pose
	bool M2_Attached;						// Can export attached items, such as helmets, weapons and shoulders
	bool M2_AsVerts;						// Can export the model using Verticies, rather than indices
	bool M2_Images;							// Can export the images for a model
	bool M2_UVData;							// Can export UV Maps
	bool M2_Normals;						// Can export the Normal Maps
	bool M2_VertexColors;					// Can export Vertex Coloring data
	bool M2_Bones;							// Can export Bones
	bool M2_BoneLimits;						// Can export the influence maps of Bones
	bool M2_Animation;						// Can export M2 animation
	bool M2_Cameras;						// Can export Cameras
	bool M2_CameraAnim;						// Can export Camera Animation
	bool M2_Lights;							// Can export Lights from M2 files

	// WMO Capabilities
	bool WMO_Export;						// This Plugin can export WMO models
	bool WMO_AsVerts;						// Can export the model using Verticies, rather than indices
	bool WMO_Images;						// Can export the images for a model
	bool WMO_UVData;						// Can export UV Maps
	bool WMO_Normals;						// Can export the Normal Maps
	bool WMO_VertexColors;					// Can export Vertex Coloring data
	bool WMO_Lights;						// Can export Lights from WMO files
	bool WMO_Doodads;						// Can export Doodads
	bool WMO_DoodadSets;					// Can export sets/collections of doodads
	bool WMO_DoodadsAsNulls;				// Can export doodads as reference nulls
	bool WMO_DoodadsAsModels;				// Can export doodads as the individual models
	bool WMO_DoodadsAsSingleModel;			// Can export doodads as a single model
	bool WMO_DoodadsMergedWithParent;		// Can export doodads as part of the parent model
	bool WMO_DoodadsAsSubobject;			// Can export doodads as a new subobject/layer of the parent model

	// ADT Capabilities
	bool ADT_Export;						// This Plugin can export ADT models
	bool ADT_AsVerts;						// Can export the model using Verticies, rather than indices
	bool ADT_Quality_Choice;				// User has a choice of ADT Quality when exporting. If = true, one of the following Quality choices MUST be true!
	bool ADT_Quality_High;					// Can export High quality ADT Files. (4 Triangles per Chunk. Highest quality, every point is used.)
	bool ADT_Quality_Medium;				// Can export Medium quality. (2 Triangles per Chunk. Uses only 4 points, with 1 point left over.)
	bool ADT_Quality_Low;					// Can export Low quality. (Only makes 2 Triangles per Block. Medium makes 512 Triangles per Block.)
	bool ADT_Images;						// Can export the images/Tilesets for a model
	bool ADT_Normals;						// Can export the Normal Maps
	bool ADT_VertexColors;					// Can export Vertex Coloring data
	bool ADT_Doodads;						// Can export Doodads of this ADT file
	bool ADT_DoodadsAsNulls;				// Can export doodads as reference nulls
	bool ADT_DoodadsAsModels;				// Can export doodads as the individual models
	bool ADT_DoodadsAsSingleModel;			// Can export doodads as a single model
	bool ADT_DoodadsMergedWithParent;		// Can export doodads as part of the parent model
	bool ADT_DoodadsAsSubobject;			// Can export doodads as a new subobject/layer of the parent model
	bool ADT_WMOs;							// Can export WMOs of this ADT file
	bool ADT_WMOsAsNulls;					// Can export WMOs as reference nulls
	bool ADT_WMOsAsModels;					// Can export WMOs as the individual models
	bool ADT_WMOsAsSingleModel;				// Can export WMOs as a single model
	bool ADT_WMOsMergedWithParent;			// Can export WMOs as part of the parent model
	bool ADT_WMOsAsSubobject;				// Can export WMOs as a new subobject/layer of the parent model
	bool ADT_WMO_Doodads;					// Can export WMOs with their doodads
	bool ADT_WMO_DoodadSets;				// Can export WMOs with their doodad sets
	bool ADT_WMO_DoodadsAsNulls;			// Can export WMO doodads as reference nulls
	bool ADT_WMO_DoodadsAsModels;			// Can export WMO doodads as the individual models
	bool ADT_WMO_DoodadsAsSingleModel;		// Can export WMO doodads as a single model
	bool ADT_WMO_DoodadsMergedWithParent;	// Can export WMO doodads as part of the parent model
	bool ADT_WMO_DoodadsAsSubobject;		// Can export WMO doodads as a new subobject/layer of the parent model

	ExporterCanDo(){
		// Primary Options
		ExporterNotes = QString();
		HeaderVersion = MODELEXPORTERHEADERVERSION;
		M2_Export = false;
		WMO_Export = false;
		ADT_Export = false;

		// M2 Options
		M2_InitialPose = false;
		M2_Attached = false;
		M2_AsVerts = false;
		M2_Images = false;
		M2_UVData = false;
		M2_Normals = false;
		M2_VertexColors = false;
		M2_Bones = false;
		M2_BoneLimits = false;
		M2_Animation = false;
		M2_Cameras = false;
		M2_CameraAnim = false;
		M2_Lights = false;

		// WMO Options
		WMO_AsVerts = false;
		ADT_Quality_Choice = false;
		ADT_Quality_High = false;
		ADT_Quality_Medium = false;
		ADT_Quality_Low = false;
		WMO_Images = false;
		WMO_UVData = false;
		WMO_Normals = false;
		WMO_VertexColors = false;
		WMO_Lights = false;
		WMO_Doodads = false;
		WMO_DoodadSets = false;
		WMO_DoodadsAsNulls = false;
		WMO_DoodadsAsModels = false;
		WMO_DoodadsAsSingleModel = false;
		WMO_DoodadsMergedWithParent = false;
		WMO_DoodadsAsSubobject = false;

		// ADT Options
		ADT_AsVerts = false;
		ADT_Images = false;
		ADT_Normals = false;
		ADT_VertexColors = false;
		ADT_Doodads = false;
		ADT_DoodadsAsNulls = false;
		ADT_DoodadsAsModels = false;
		ADT_DoodadsAsSingleModel = false;
		ADT_DoodadsMergedWithParent = false;
		ADT_DoodadsAsSubobject = false;
		ADT_WMOs = false;
		ADT_WMOsAsNulls = false;
		ADT_WMOsAsModels = false;
		ADT_WMOsAsSingleModel = false;
		ADT_WMOsMergedWithParent = false;
		ADT_WMOsAsSubobject = false;
		ADT_WMO_Doodads = false;
		ADT_WMO_DoodadSets = false;
		ADT_WMO_DoodadsAsNulls = false;
		ADT_WMO_DoodadsAsModels = false;
		ADT_WMO_DoodadsAsSingleModel = false;
		ADT_WMO_DoodadsMergedWithParent = false;
		ADT_WMO_DoodadsAsSubobject = false;
	}

};	// End ExporterCanDo class

}	// End Namespace

#endif