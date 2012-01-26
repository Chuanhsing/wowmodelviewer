/*
	3D Model Exporter Header

	By stating the exporter's capabilities, as layed out in this file,
	we can easily build a chart, showing users what each exporter can do.
*/

#ifndef MODEL_EXPORTER_HEADER_H
#define MODEL_EXPORTER_HEADER_H

#include <qstring.h>
#include <qlist.h>
#include "ModelViewerModel.h"

namespace WMVEngine {

// This increments every time we change the ExporterInfo or ExporterCanDo class, once exporter plug-in development has started.
#define MODELEXPORTERHEADERVERSION 1

// This class sets the variables and options of the exporter.
class ExporterInfo {
private:
	// Both of these numbers must match the current version numbers for the exporter to be accepted!
	size_t HeaderVersion;					// The version of the Exporter Header we're using.
	size_t ModelVersion;					// The Model Viewer Model version number.

public:
	QString Exporter_Name;					// The name of the Exporter. This is used in the pulldown menu. (Examples: Lightwave 3D, FBX, Wavefront OBJ)
	QString Exporter_Notes;					// A string to write any notes about the exporter in.
	QList<QString> Exporter_Extensions;		// A list of model extensions used by this format. Most common goes first. (Examples: lwo, fbx, obj)
	bool hasOptionsPage;					// The exporter has an options page/dialog available before exporting.
	
	/*
	Operating System Support
	
		-1 = Definitely Unsupported. If there is no chance the exporter will work, this is the setting. This will disable the exporter on these systems.
		0 = Possibly supported. This means there's no code support, but it might work. This will issue a warning to people using it.
		1 = Thoeretically supported. On this setting, there's some support, and it SHOULD work, but it has not been confirmed.
		2 = Confirmed supported. It's been thoroughly tested on this OS, and it definitely works.
	*/
	int OS_Windows;							// Status of Windows support.
	int OS_MacOSX;							// Status of Mac OSX support.
	int OS_Linux;							// Status of Linux support.
	
	// Constructor
	// n = Name, l = QList of Extentions, hop = hasOptionsPage, osw = Windows support, osm = Mac Support, osl = Linux support, note = Any notes for this exporter.
	ExporterInfo(QString n, QList<QString> l, bool hop = false, int osw = -1, int osm = -1, int osl = -1, QString note = QString()) {
		HeaderVersion = MODELEXPORTERHEADERVERSION;
		ModelVersion = MODELVIEWERMODELVERSION;
		
		Exporter_Name = n;
		Exporter_Extensions = l;
		Exporter_Notes = note;
		hasOptionsPage = hop;
		
		OS_Windows = osw;
		OS_MacOSX = osm;
		OS_Linux = osl;
	}

	size_t getHeaderVersion(){
		return HeaderVersion;
	}
	size_t getModelVersion(){
		return ModelVersion;
	}
};

// This class defines the capabilities of exporters.
// Variables that are not set by plug-ins are considered to be set to false.
class ExporterCanDo {
private:
	size_t HeaderVersion;					// The version of the Exporter Header we're using.

public:
	// M2 Capabilities
	bool M2_Export;							// This plug-in can export M2 models.
	bool M2_Options;						// Has an options page/dialog for M2-Specific settings.
	bool M2_PreserveDirectory;				// Can export the model using the in-game directory structure. This applys to ALL items exported by this exporter! (Models, Images, etc...)
	bool M2_InitialPose;					// Can export the initial, unanimated pose of animated models.
	bool M2_Attached;						// Can export attached items, such as helmets, weapons and shoulders.
	bool M2_LODs;							// Can export the various Levels Of Detail versions of the model.
	bool M2_AsVerts;						// Can export the model using Verticies, rather than indices.
	bool M2_Geosets;						// Can export Geosets as separate groups or sub-meshes.
	bool M2_CanBillboardPolys;				// Can export Billboard Polygons. (Polygons that always face the camera.)
	bool M2_Images;							// Can export the images for a model.
	bool M2_BlendMode;						// Can export a model's Blend Mode. (Allows for things like Ghosts, projections and light shafts.)
	bool M2_Surface;						// Can export a surface/material for the model with all the exported textures properly applied to it.
	bool M2_SurfaceLum;						// Can export a luminous surface.
	bool M2_SurfaceSpec;					// Can export a surface with Specularity.
	bool M2_SurfaceTrans;					// Can export a surface with Transparency.
	bool M2_SurfaceDoubleSided;				// Can export Double-Sided surfaces.
	bool M2_UVData;							// Can export UV Maps.
	bool M2_UVAnim;							// Can export UV Animation.
	bool M2_Normals;						// Can export the Normal Maps.
	bool M2_VertexColors;					// Can export Vertex Coloring data.
	bool M2_VertexColorAnim;				// Can export Animation for Vertex Colors.
	bool M2_Bones;							// Can export Bones.
	bool M2_BoneLimits;						// Can export the influence maps of Bones.
	bool M2_BoneAnimation;					// Can export a Bone's animation.
	bool M2_Cameras;						// Can export Cameras.
	bool M2_CameraAnim;						// Can export Camera Animation.
	bool M2_CameraFOV;						// Can export the Field Of View of a camera.
	bool M2_CameraClipNear;					// Can export the near clipping plane of a camera.
	bool M2_CameraClipFar;					// Can export the far clipping plane of a camera.
	bool M2_CameraTarget;					// Can export Camera Targets.
	bool M2_CameraTargetAnim;				// Can export Camera Target animation.
	bool M2_Lights;							// Can export Lights from M2 files.
	bool M2_LightTypes;						// Can export the different types of Lights.
	bool M2_LightsAnim;						// Can export a Light's animation.
	bool M2_LightsColor;					// Can export Light color information.
	bool M2_LightsColorAnim;				// Can export Light color animation.
	bool M2_LightsAttenuationStart;			// Can export the Start range of a Light's Attenuation. (Falloff)
	bool M2_LightsAttenuationEnd;			// Can export the End range of a Light's Attenuation.
	bool M2_LightsAttenuationStartAnim;		// Can export the animation of the Start Attenuation.
	bool M2_LightsAttenuationEndAnim;		// Can export the animation of the End Attenuation.
	bool M2_Particles;						// Can export Particles from M2 files.
	bool M2_ParticlesAnim;					// Can export Particle animation from M2 files.
	bool M2_Ribbons;						// Can export Ribbon/Contrail effects from M2 files.

	// WMO Capabilities
	bool WMO_Export;						// This plug-in can export WMO models.
	bool WMO_Options;						// Has an options page/dialog for WMO-Specific settings.
	bool WMO_PreserveDirectory;				// Can export the model using the in-game directory structure. This applys to ALL items exported by this exporter! (Models, Images, etc...)
	bool WMO_AsVerts;						// Can export the model using Verticies, rather than indices.
	bool WMO_Collisions;					// Can export Collision Polys.
	bool WMO_Groups;						// Can export sub-meshes as separate groups or Sub-meshes.
	bool WMO_CanBillboardPolys;				// Can export Billboard Polygons. (Polygons that always face the camera.)
	bool WMO_Images;						// Can export the images for a model.
	bool WMO_Surface;						// Can export a surface/material for the model with all the exported textures properly applied to it.
	bool WMO_SurfaceLum;					// Can export a luminous surface.
	bool WMO_SurfaceSpec;					// Can export a surface with Specularity.
	bool WMO_SurfaceDoubleSided;			// Can export Double-Sided surfaces.
	bool WMO_UVData;						// Can export UV Maps.
	bool WMO_Normals;						// Can export the Normal Maps.
	bool WMO_VertexColors;					// Can export Vertex Coloring data.
	bool WMO_Lights;						// Can export Lights from WMO files.
	bool WMO_LightTypes;					// Can export the different types of Lights.
	bool WMO_LightsColor;					// Can export Light color information.
	bool WMO_LightsAttenuationStart;		// Can export the Start range of a Light's Attenuation.
	bool WMO_LightsAttenuationEnd;			// Can export the End range of a Light's Attenuation.
	bool WMO_Water;							// Can export Water planes from WMO files.
	bool WMO_WaterTexture;					// Can export the texture/surface-settings of WMO Water.
	bool WMO_Fog;							// Can export/setup Fog.
	bool WMO_FogColor;						// Can export Fog color.
	bool WMO_FogZones;						// Can export Fog as localized zones. If = false, and Fog = true, then Fog is exported as global Fog.
	bool WMO_Doodads;						// Can export Doodads. Doodads contain all the properties of M2 files.
	bool WMO_DoodadSets;					// Can export sets/collections of doodads.
	bool WMO_DoodadsAsNulls;				// Can export doodads as reference nulls.
	bool WMO_DoodadsAsModels;				// Can export doodads as the individual models.
	bool WMO_DoodadsAsSingleModel;			// Can export all the doodads as a single model.
	bool WMO_DoodadSetsAsSingleModel;		// Can export each doodad set as a single model.
	bool WMO_DoodadsMergedWithParent;		// Can export doodads as part of the parent model.
	bool WMO_DoodadSetsMergedWithParent;	// Can export doodad sets as part of the parent model.
	bool WMO_DoodadsAsSubobject;			// Can export doodads as a single sub-object/layer of the parent model.
	bool WMO_DoodadsAsSubobjects;			// Can export doodads as separate sub-objects/layers of the parent model.
	bool WMO_DoodadSetsAsSubobject;			// Can export doodad sets as a single sub-object/layer of the parent model.
	bool WMO_DoodadSetsAsSubobjects;		// Can export doodad sets as separate sub-objects/layers of the parent model.
	bool WMO_DoodadLights;					// Can export a doodad's lights. Has all the Light properies of M2 files.

	/* --= ADT Capabilities =--
	Basics:
		All ADT maps are divided into 16x16 Blocks for a total of 256 Blocks. Each Block is divided into 8x8 (for a total of 64) Chunks.
		Each Chunk can contain up to 4 Triangles (Polys), using 5 points. (Four on the outside, one in the middle.)

	Height Maps:
		Since, at Level 00, there are 3 points going in any direction, if we convert that to Pixels, we get a minimum resolution of
		384x384, using the formula Points times Chunks times Blocks. This gives us 3(Points)*8(Chunks)*16(Blocks), which equals 384.
		However, 384 will require averaging 3 points/pixels to get the 4 "missing" pixel's data. (Top Center, Bottom Center, Left
		Center and Right Center.) Using Level 01, we can do 256x256, and ignore the middle point, making the job much easier, but
		this will result in a lower-resolution image with significantly less data. Now, when you allow users to choose a resolution,
		you will have to be able to average any 2/3 points over a series of pixels. Using 1024x1024 for example, each point will
		need to provide data for 8 pixels. For a smooth transition, those 8 pixels will need to be averaged with the ajoining 8
		pixels. Hopefully we'll find an easy method for exporters to do this, so you don't have to program all that math yourself.
		Ideally, the exporter will simply pass the ADT height data to a function and get back an image to save.

	*/
	bool ADT_Export;						// This plug-in can export ADT models.
	bool ADT_Options;						// Has an options page/dialog for ADT-Specific settings.
	bool ADT_PreserveDirectory;				// Can export the model using the in-game directory structure. This applys to ALL items exported by this exporter! (Models, Images, etc...)
	bool ADT_Quality_Choice;				// User has a choice of ADT Quality when exporting. If = true, one of the following Quality choices MUST be true!
	bool ADT_Quality_Level_00;				// Can export Level 00 quality ADT Files. 4 Polys per Chunk. Every point is used to make Polys. 65536 polys total.
	bool ADT_Quality_Level_01;				// Can export Level 01 quality. 2 Polys per Chunk. Uses only 4 points, with 1 point left over. 32768 polys.
	bool ADT_Quality_Level_02;				// Can export Level 02 quality. 2 Polys per 4 Chunks, 16 Chunks per Block. 8192 polys.
	bool ADT_Quality_Level_03;				// Can export Level 03 quality. 2 Polys per 8 Chunks, 4 Chunks per Block. 2048 polys.
	bool ADT_Quality_Level_04;				// Can export Level 04 quality. 2 Polys per Block, 256 Blocks. 512 polys.
	bool ADT_Quality_Level_05;				// Can export Level 05 quality. 2 Polys per 4 Blocks, 64 Blocks. 128 polys.
	bool ADT_Quality_Level_06;				// Can export Level 06 quality. 2 Polys per 8 Blocks, 16 Blocks. 32 polys.
	bool ADT_Quality_Level_07;				// Can export Level 07 quality. 2 Polys per 16 Blocks, 4 Blocks. 8 polys.
	bool ADT_Quality_Level_08;				// Can export Level 08 quality. 2 Polys per ADT file.
	bool ADT_Quality_RemoveExcess;			// Can remove excess/unused points when not using Level 00 Quality.
	bool ADT_BoundingBox;					// Can export the Bounding Box used for flying as Polys.
	bool ADT_Images;						// Can export the images/Tilesets for a model.
	bool ADT_ExportHeightMap;				// Can export a grayscale heightmap image to represent height data. Used in mapping the High-res height data to a low-poly object.
	bool ADT_ExportAlphas;					// Can export alpha maps used for texturing.
	bool ADT_ExportShadowMap;				// Can export shadow maps used for low-quality shadows texturing.
	bool ADT_ChooseRes_HeightMap;			// Can choose a Resolution (X pixels by Y pixels) for the heightmap.
	bool ADT_ChooseRes_Alphas;				// Can choose a Resolution for the alphas.
	bool ADT_ChooseRes_ShadowMap;			// Can choose a Resolution for the shadow maps.
	bool ADT_Surface;						// Can export a surface/material for the model with all the exported textures properly applied to it.
	bool ADT_SurfaceAnim;					// Can export the animation of an ADT texture.
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
	bool ADT_DoodadsAsSubobject;			// Can export doodads as a single sub-object/layer of the parent model.
	bool ADT_DoodadsAsSubobjects;			// Can export doodads as separate sub-objects/layers of the parent model.
	bool ADT_WMOs;							// Can export WMOs of this ADT file. Contains all the properties of WMO files.
	bool ADT_WMOsAsNulls;					// Can export WMOs as reference nulls.
	bool ADT_WMOsAsModels;					// Can export WMOs as the individual models.
	bool ADT_WMOsAsSingleModel;				// Can export WMOs as a single model.
	bool ADT_WMOsMergedWithParent;			// Can export WMOs as part of the parent model.
	bool ADT_WMOsAsSubobject;				// Can export the WMOs as a single sub-object/layer of the parent model.
	bool ADT_WMOsAsSubobjects;				// Can export the WMOs as separate sub-objects/layers of the parent model.

	ExporterCanDo() {
		// Primary Options
		HeaderVersion = MODELEXPORTERHEADERVERSION;
		M2_Export = false;
		WMO_Export = false;
		ADT_Export = false;
		M2_Options = false;
		WMO_Options = false;
		ADT_Options = false;

		// M2 Options
		M2_PreserveDirectory = false;
		M2_InitialPose = false;
		M2_Attached = false;
		M2_LODs = false;
		M2_AsVerts = false;
		M2_Geosets = false;
		M2_CanBillboardPolys = false;
		M2_Images = false;
		M2_BlendMode = false;
		M2_Surface = false;
		M2_SurfaceLum = false;
		M2_SurfaceSpec = false;
		M2_SurfaceTrans = false;
		M2_SurfaceDoubleSided = false;
		M2_UVData = false;
		M2_UVAnim = false;
		M2_Normals = false;
		M2_VertexColors = false;
		M2_VertexColorAnim = false;
		M2_Bones = false;
		M2_BoneLimits = false;
		M2_BoneAnimation = false;
		M2_Cameras = false;
		M2_CameraAnim = false;
		M2_CameraFOV = false;
		M2_CameraClipNear = false;
		M2_CameraClipFar = false;
		M2_CameraTarget = false;
		M2_CameraTargetAnim = false;
		M2_Lights = false;
		M2_LightTypes = false;
		M2_LightsAnim = false;
		M2_LightsColor = false;
		M2_LightsColorAnim = false;
		M2_LightsAttenuationStart = false;
		M2_LightsAttenuationEnd = false;
		M2_LightsAttenuationStartAnim = false;
		M2_LightsAttenuationEndAnim = false;
		M2_Particles = false;
		M2_ParticlesAnim = false;
		M2_Ribbons = false;

		// WMO Options
		WMO_PreserveDirectory = false;
		WMO_AsVerts = false;
		WMO_Collisions = false;
		WMO_Groups = false;
		WMO_CanBillboardPolys = false;
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
		WMO_LightsAttenuationStart = false;
		WMO_LightsAttenuationEnd = false;
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
		WMO_DoodadSetsMergedWithParent = false;
		WMO_DoodadsAsSubobject = false;
		WMO_DoodadsAsSubobjects = false;
		WMO_DoodadSetsAsSubobject = false;
		WMO_DoodadSetsAsSubobjects = false;
		WMO_DoodadLights = false;

		// ADT Options
		ADT_PreserveDirectory = false;
		ADT_Quality_Choice = false;
		ADT_Quality_Level_00 = false;
		ADT_Quality_Level_01 = false;
		ADT_Quality_Level_02 = false;
		ADT_Quality_Level_03 = false;
		ADT_Quality_Level_04 = false;
		ADT_Quality_Level_05 = false;
		ADT_Quality_Level_06 = false;
		ADT_Quality_Level_07 = false;
		ADT_Quality_Level_08 = false;
		ADT_Quality_RemoveExcess = false;
		ADT_BoundingBox = false;
		ADT_Images = false;
		ADT_ExportHeightMap = false;
		ADT_ExportAlphas = false;
		ADT_ExportShadowMap = false;
		ADT_ChooseRes_HeightMap = false;
		ADT_ChooseRes_Alphas = false;
		ADT_ChooseRes_ShadowMap = false;
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
		ADT_DoodadsAsSubobjects = false;
		ADT_WMOs = false;
		ADT_WMOsAsNulls = false;
		ADT_WMOsAsModels = false;
		ADT_WMOsAsSingleModel = false;
		ADT_WMOsMergedWithParent = false;
		ADT_WMOsAsSubobject = false;
		ADT_WMOsAsSubobjects = false;
	}

};	// End ExporterCanDo class

}	// End Namespace

#endif