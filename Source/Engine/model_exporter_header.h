/*
	3D Model Exporter Header

	By stating the exporter's capabilities, as layed out in this file,
	we can easily build a chart, showing users what each exporter can do.
	
	The exporters will only ever export data from the Model Viewer Model format. At no point should any exporter ever use
	WoW-specific information for input. All WoW-specific options will be handled by the model viewer prior to the exporter
	getting the information.
	
	This will help us make quick and easy changes and upgrades that will effect every exporter, even the ones we didn't program.
	
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
public:
	bool HasPreOptions;					// Can/will display an options page prior to exporting.
	bool PreserveDirectory;				// Can export the model using the supplied directory structure. This applys to ALL items exported by this exporter! (Models, Images, etc...)
	bool Images;						// Can export the images for a model.
	bool CanBillboardPolys;				// Can export Billboard Polygons. (Polygons that always face the camera.)
	bool ModelGroups;					// Can export model groups/Geosets as groups or submeshes.
	bool Surface;						// Can export a surface/material for the model with all the exported textures properly applied to it.
	bool SurfaceAnim;					// Can export the animation of a surface texture.
	bool SurfaceLum;					// Can export a surface's luminosity texture/property. (An image that is luminous or a surface's luminosity.)
	bool SurfaceSpec;					// Can export a surface's Specularity texture.
	bool SurfaceTrans;					// Can export a surface's Transparency texture.
	bool SurfaceDoubleSided;			// Can export Double-Sided surfaces.
	bool SurfaceReflection;				// Can export specified textures that will reflect the sky.
	bool BlendMode;						// Can export a model's Blend Mode. (Allows for things like Ghosts, projections and light shafts.)
	bool UVData;						// Can export UV Maps.
	bool UVAnim;						// Can export UV Animation.
	bool Normals;						// Can export a polygon's Normal Vector.
	bool VertexColors;					// Can export Vertex Coloring data.
	bool VertexColorAnim;				// Can export Animation for Vertex Colors.
	bool Bones;							// Can export Bones/Joints.
	bool BoneLimits;					// Can export the influence maps of Bones.
	bool BoneAnimation;					// Can export a Bone's animation.
	bool Fog;							// Can export/setup Fog.
	bool FogColor;						// Can export Fog color.
	bool FogZones;						// Can export Fog as localized zones. If = false, and Fog = true, then Fog is exported as global Fog.
	bool Cameras;						// Can export Cameras.
	bool CameraAnim;					// Can export Camera Animation.
	bool CameraFOV;						// Can export the Field Of View of a camera.
	bool CameraClipNear;				// Can export the near clipping plane of a camera.
	bool CameraClipFar;					// Can export the far clipping plane of a camera.
	bool CameraTarget;					// Can export Camera Targets.
	bool CameraTargetAnim;				// Can export Camera Target animation.
	bool Lights;						// Can export Lights.
	bool LightTypes;					// Can export the different types of Lights.
	bool LightsAnim;					// Can export a Light's animation.
	bool LightsColor;					// Can export Light color information.
	bool LightsColorAnim;				// Can export Light color animation.
	bool LightsAttenuationStart;		// Can export the Start range of a Light's Attenuation. (Falloff)
	bool LightsAttenuationEnd;			// Can export the End range of a Light's Attenuation.
	bool LightsAttenuationStartAnim;	// Can export the animation of the Start Attenuation.
	bool LightsAttenuationEndAnim;		// Can export the animation of the End Attenuation.
	bool Particles;						// Can export Particles with textures.
	bool ParticlesAnim;					// Can export Particle animation.
	bool Ribbons;						// Can export Ribbon/Contrail effects.
	
	ExporterCanDo() {
		HasPreOptions = false;
		PreserveDirectory = false;
		Images = false;
		CanBillboardPolys = false;
		ModelGroups = false;
		Surface = false;
		SurfaceAnim = false;
		SurfaceLum = false;
		SurfaceSpec = false;
		SurfaceTrans = false;
		SurfaceDoubleSided = false;
		SurfaceReflection = false;
		BlendMode = false;
		UVData = false;
		UVAnim = false;
		Normals = false;
		VertexColors = false;
		VertexColorAnim = false;
		Bones = false;
		BoneLimits = false;
		BoneAnimation = false;
		Fog = false;
		FogColor = false;
		FogZones = false;
		Cameras = false;
		CameraAnim = false;
		CameraFOV = false;
		CameraClipNear = false;
		CameraClipFar = false;
		CameraTarget = false;
		CameraTargetAnim = false;
		Lights = false;
		LightTypes = false;
		LightsAnim = false;
		LightsColor = false;
		LightsColorAnim = false;
		LightsAttenuationStart = false;
		LightsAttenuationEnd = false;
		LightsAttenuationStartAnim = false;
		LightsAttenuationEndAnim = false;
		Particles = false;
		ParticlesAnim = false;
		Ribbons = false;
	}
};	// End ExporterCanDo class

}	// End Namespace

#endif