/*
	3D Model Exporter Header

	By stating the exporter's capabilities, as layed out in this file,
	we can easily build a chart, showing users what each exporter can do.
*/

#ifndef MODEL_EXPORTER_HEADER_H
#define MODEL_EXPORTER_HEADER_H

#include <qstring.h>

namespace WMVEngine {

// This increments every time we change the ExporterCanDo class, once exporter plugin development has started.
#define MODELEXPORTERHEADERVERSION 1;

// This class defines the capabilities of exporters.
// Variables that are not set by plugins are considered to be set to false.
class ExporterCanDo {
public:
	QString ExporterNotes;					// A string to write any notes about the exporter in.
	size_t HeaderVersion;					// The version of the Exporter Header we're using.

	// M2 Capabilities
	bool M2_Export;							// This Plugin can export M2 models.
	bool M2_InitialPose;					// Can export the initial, unanimated pose.
	bool M2_Attached;						// Can export attached items, such as helmets, weapons and shoulders.
	bool M2_LODs;							// Can export the various Levels Of Detail versions of the model.
	bool M2_AsVerts;						// Can export the model using Verticies, rather than indices.
	bool M2_Images;							// Can export the images for a model.
	bool M2_Surface;						// Can export a surface/material for the model.
	bool M2_SurfaceAnim;					// Can export an animated surface.
	bool M2_SurfaceLum;						// Can export a luminous surface.
	bool M2_SurfaceSpec;					// Can export a surface with Specularity.
	bool M2_SurfaceTrans;					// Can export a surface with Transparancy.
	bool M2_SurfaceDoubleSided;				// Can export Double-Sided surfaces.
	bool M2_UVData;							// Can export UV Maps.
	bool M2_UVAnim;							// Can export UV Animation.
	bool M2_Normals;						// Can export the Normal Maps.
	bool M2_VertexColors;					// Can export Vertex Coloring data.
	bool M2_Bones;							// Can export Bones.
	bool M2_BoneLimits;						// Can export the influence maps of Bones.
	bool M2_Animation;						// Can export M2 animation.
	bool M2_Cameras;						// Can export Cameras.
	bool M2_CameraAnim;						// Can export Camera Animation.
	bool M2_CameraFOV;						// Can export the Field Of View of a camera.
	bool M2_CameraClipFar;					// Can export the far clipping plane of a camera.
	bool M2_CameraTarget;					// Can export Camera Targets.
	bool M2_CameraTargetAnim;				// Can export Camera Target animation.
	bool M2_Lights;							// Can export Lights from M2 files.
	bool M2_LightTypes;						// Can export the different types of Lights.
	bool M2_LightsAnim;						// Can export a Light's animation.
	bool M2_LightsColor;					// Can export Light color information.
	bool M2_LightsColorAnim;				// Can export Light color animation.
	bool M2_LightsFalloff;					// Can export Light falloff settings.
	bool M2_LightsFalloffAnim;				// Can export Light falloff animation.
	bool M2_Particals;						// Can export Particals from M2 files.
	bool M2_ParticalsAnim;					// Can export Partical animation from M2 files.
	bool M2_Ribbons;						// Can export Ribbon-like effects from M2 files.

	// WMO Capabilities
	bool WMO_Export;						// This Plugin can export WMO models.
	bool WMO_AsVerts;						// Can export the model using Verticies, rather than indices.
	bool WMO_Collisions;					// Can export Collision Polys.
	bool WMO_Images;						// Can export the images for a model.
	bool WMO_Surface;						// Can export a fully functioning surface for the model.
	bool WMO_SurfaceLum;					// Can export a luminous surface.
	bool WMO_SurfaceSpec;					// Can export a surface with Specularity.
	bool WMO_SurfaceDoubleSided;			// Can export Double-Sided surfaces.
	bool WMO_UVData;						// Can export UV Maps.
	bool WMO_Normals;						// Can export the Normal Maps.
	bool WMO_VertexColors;					// Can export Vertex Coloring data.
	bool WMO_Lights;						// Can export Lights from WMO files.
	bool WMO_LightTypes;					// Can export the different types of Lights.
	bool WMO_LightsColor;					// Can export Light color information.
	bool WMO_LightsFalloff;					// Can export Light falloff settings.
	bool WMO_Water;							// Can export Water planes from WMO files.
	bool WMO_WaterTexture;					// Can export the texture/surface-settings of WMO Water.
	bool WMO_Fog;							// Can export/setup Fog.
	bool WMO_FogColor;						// Can export Fog color.
	bool WMO_FogZones;						// Can export Fog as localized zones. If = false, and Fog = true, then Fog is exported as global Fog.
	bool WMO_Doodads;						// Can export Doodads. Doodads contain all the properties of M2 files.
	bool WMO_DoodadSets;					// Can export sets/collections of doodads.
	bool WMO_DoodadsAsNulls;				// Can export doodads as reference nulls.
	bool WMO_DoodadsAsModels;				// Can export doodads as the individual models.
	bool WMO_DoodadsAsSingleModel;			// Can export doodads as a single model.
	bool WMO_DoodadSetsAsSingleModel;		// Can export each doodad set as a single model.
	bool WMO_DoodadsMergedWithParent;		// Can export doodads as part of the parent model.
	bool WMO_DoodadsAsSubobject;			// Can export doodads as a new subobject(s)/layer(s) of the parent model.
	bool WMO_DoodadLights;					// Can export Doodad Lights.

	// ADT Capabilities
	// All ADT maps are divided into 64x64 Blocks for a total of 4096 Blocks. Each Block is divided into 16x16 Chunks.
	// Each Chunk can contain up to 4 Triangles (Polys), using 5 points.
	bool ADT_Export;						// This Plugin can export ADT models.
	bool ADT_Quality_Choice;				// User has a choice of ADT Quality when exporting. If = true, one of the following Quality choices MUST be true!
	bool ADT_Quality_Level_0;				// Can export Level 0 quality ADT Files. 4 Polys per Chunk. Every point is used to make Polys.
	bool ADT_Quality_Level_1;				// Can export Level 1 quality. 2 Polys per Chunk. Uses only 4 points, with 1 point left over.
	bool ADT_Quality_Level_2;				// Can export Level 2 quality. 2 Polys per 4 Chunks, 4 Chunks per Block.
	bool ADT_Quality_Level_3;				// Can export Level 3 quality. 2 Polys per Block, 4096 Blocks. Level 1 makes 512 Polys per Block.
	bool ADT_Quality_Level_4;				// Can export Level 4 quality. 2 Polys per 4 Blocks, 256 Blocks.
	bool ADT_Quality_Level_5;				// Can export Level 5 quality. 2 Polys per 8 Blocks, 64 Blocks.
	bool ADT_Quality_Level_6;				// Can export Level 6 quality. 2 Polys per 16 Blocks, 16 Blocks.
	bool ADT_Quality_Level_7;				// Can export Level 7 quality. 2 Polys per ADT file.
	bool ADT_Quality_RemoveExcess;			// Can remove excess/unused points when not using Level 0 Quality.
	bool ADT_AsVerts;						// Can export the model using Verticies, rather than indices.
	bool ADT_BoundingBox;					// Can export the Bounding Box used for flying as Polys.
	bool ADT_Images;						// Can export the images/Tilesets for a model.
	bool ADT_ExportAlphas;					// Can export alpha maps used for texturing.
	bool ADT_ExportShadows;					// Can export shadow maps used for texturing.
	bool ADT_Surface;						// Can create a properly layered surface/texture using the alpha maps.
	bool ADT_SurfaceAnim;					// Can export the texture animation of an ADT texture.
	bool ADT_SurfaceReflection;				// Can export specified layers that will reflect the sky.
	bool ADT_VertexColors;					// Can export Vertex Coloring data.
	bool ADT_Normals;						// Can export the Normal Maps.
	bool ADT_Water;							// Can export Water planes from ADT files.
	bool ADT_WaterTexture;					// Can export the texture/surface-settings of ADT Water.
	bool ADT_Doodads;						// Can export Doodads of this ADT file. Doodads contain all the properties of M2 files.
	bool ADT_DoodadsAsNulls;				// Can export doodads as reference nulls.
	bool ADT_DoodadsAsModels;				// Can export doodads as the individual models.
	bool ADT_DoodadsAsSingleModel;			// Can export doodads as a single model.
	bool ADT_DoodadsMergedWithParent;		// Can export doodads as part of the parent model.
	bool ADT_DoodadsAsSubobject;			// Can export doodads as a new subobject/layer of the parent model.
	bool ADT_WMOs;							// Can export WMOs of this ADT file. Contains all the properties of WMO files.
	bool ADT_WMOsAsNulls;					// Can export WMOs as reference nulls.
	bool ADT_WMOsAsModels;					// Can export WMOs as the individual models.
	bool ADT_WMOsAsSingleModel;				// Can export WMOs as a single model.
	bool ADT_WMOsMergedWithParent;			// Can export WMOs as part of the parent model.
	bool ADT_WMOsAsSubobject;				// Can export WMOs as a new subobject(s)/layer(s) of the parent model.

	ExporterCanDo() {
		// Primary Options
		ExporterNotes = QString();
		HeaderVersion = MODELEXPORTERHEADERVERSION;
		M2_Export = false;
		WMO_Export = false;
		ADT_Export = false;

		// M2 Options
		M2_InitialPose = false;
		M2_Attached = false;
		M2_LODs = false;
		M2_AsVerts = false;
		M2_Images = false;
		M2_Surface = false;
		M2_SurfaceAnim = false;
		M2_SurfaceLum = false;
		M2_SurfaceSpec = false;
		M2_SurfaceTrans = false;
		M2_SurfaceDoubleSided = false;
		M2_UVData = false;
		M2_UVAnim = false;
		M2_Normals = false;
		M2_VertexColors = false;
		M2_Bones = false;
		M2_BoneLimits = false;
		M2_Animation = false;
		M2_Cameras = false;
		M2_CameraAnim = false;
		M2_CameraFOV = false;
		M2_CameraClipFar = false;
		M2_CameraTarget = false;
		M2_CameraTargetAnim = false;
		M2_Lights = false;
		M2_LightTypes = false;
		M2_LightsAnim = false;
		M2_LightsColor = false;
		M2_LightsColorAnim = false;
		M2_LightsFalloff = false;
		M2_LightsFalloffAnim = false;
		M2_Particals = false;
		M2_ParticalsAnim = false;
		M2_Ribbons = false;

		// WMO Options
		WMO_AsVerts = false;
		WMO_Collisions = false;
		WMO_Images = false;
		WMO_Surface = false;
		WMO_SurfaceLum = false;
		WMO_SurfaceSpec = false;
		WMO_SurfaceDoubleSided = false;
		WMO_UVData = false;
		WMO_Normals = false;
		WMO_VertexColors = false;
		WMO_Lights = false;
		WMO_LightTypes = false;
		WMO_LightsColor = false;
		WMO_LightsFalloff = false;
		WMO_Water = false;
		WMO_WaterTexture = false;
		WMO_Fog = false;
		WMO_FogColor = false;
		WMO_FogZones = false;
		WMO_Doodads = false;
		WMO_DoodadSets = false;
		WMO_DoodadsAsNulls = false;
		WMO_DoodadsAsModels = false;
		WMO_DoodadsAsSingleModel = false;
		WMO_DoodadSetsAsSingleModel = false;
		WMO_DoodadsMergedWithParent = false;
		WMO_DoodadsAsSubobject = false;
		WMO_DoodadLights = false;

		// ADT Options
		ADT_Quality_Choice = false;
		ADT_Quality_Level_0 = false;
		ADT_Quality_Level_1 = false;
		ADT_Quality_Level_2 = false;
		ADT_Quality_Level_3 = false;
		ADT_Quality_Level_4 = false;
		ADT_Quality_Level_5 = false;
		ADT_Quality_Level_6 = false;
		ADT_Quality_Level_7 = false;
		ADT_Quality_RemoveExcess = false;
		ADT_AsVerts = false;
		ADT_BoundingBox = false;
		ADT_Images = false;
		ADT_ExportAlphas = false;
		ADT_ExportShadows = false;
		ADT_Surface = false;
		ADT_SurfaceAnim = false;
		ADT_SurfaceReflection = false;
		ADT_VertexColors = false;
		ADT_Normals = false;
		ADT_Water = false;
		ADT_WaterTexture = false;
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
	}

};	// End ExporterCanDo class

}	// End Namespace

#endif