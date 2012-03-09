#ifndef MODELVIEWERMODELFORMAT_H
#define MODELVIEWERMODELFORMAT_H

class Quatd{};	// Temp until we can get it to compile with the math_Quat file.

#include <qdir.h>
#include "enums.h"
#include "../Foundation/Math/math_vector.h"
//#include "../Foundation/Math/math_quaternion.h"
#include "../Games/ModelSource_enums.h"

/*
	Model Viewer Model Format.

	This is our internal model format. Whatever models we load, they are placed into this model format.
	As such, it needs to be able to contain the capabilities of all the models we can read. This is
	the model format that the exporters read and export data from.
*/

#define MODELVIEWERMODELVERSION 1	// Updated whenever we change one of our internal model formats.

namespace WMVEngine {

// Animation class. Use this to give any class animation properties.
template <class T>
class Animation {
public:
	QList<T> Block;
	QList<size_t> KeyFrames;

	// Get the value
	size_t getValue(size_t n){
		return Block.value(n);
	}
	// Get the value of a keyframe
	T getFrameValue(size_t n){
		for (int i=0;i<KeyFrames.count();i++){
			if (KeyFrames.value(i) == n){
				return Block.value(i);
			}
		}
	}
};

// Animation Sequence Data
class cAnimSequence {};

// Texture Images
class cTexture {
public:
	size_t type;			// Indicates the type of the texture, as defined in the enums.h file
	QString filename;		// The name of the image file, with extension.
	QString path;			// The path to the image, relative to the model.
	size_t layer;			// The layer of the texture. Higher numbers go on top of lower ones.
};

// Our internal model format
class ModelViewerModel {
private:
	size_t ModelVersion;

public:
	QList<ModelViewerModel>Submodels;	// Used for sub-objects, such as attached models or doodads.
	QString ModelName;					// Name of the Model. IE: The Big Bad Wolf
	QString FileName;					// The filename of the model. IE: BigBadWolf.m2
	QDir FilePath;						// Path to the file. IE: Creatures/Wolves/BigBadWolf
	size_t ModelSource;					// The source of the model, as defined in Games\ModelSource_enums.h

	Vec3d iPosition;					// Initial Position of the model
	Quatd iRotation;					// Initial Rotation of the model
	Vec3d iScale;						// Initial Scale of the model
	size_t ParentBone;					// The bone number this model is attached to. -1 for none.
	QList<cTexture> TextureList;		// List of textures used by the model.
	QList<cAnimSequence> AnimationList;	// List of animations for this model.

	// Constructors
private:
	// Privatize the default, so people can't build empty models.
	ModelViewerModel() {
		ModelVersion = MODELVIEWERMODELVERSION;
		ParentBone = -1;
		iPosition = Vec3d(0.0);
		iRotation = Quatd();		// 0,0,0,1
		iScale = Vec3d(1.0);
		Submodels = QList<ModelViewerModel>();
		AnimationList = QList<cAnimSequence>();
		TextureList = QList<cTexture>();
		ModelSource = NOMODELSOURCE;
	}
public:
	ModelViewerModel(QString file){
		ModelViewerModel();		// Start with an empty model.

		// Detect the game Type

		// Grab the model data

		// Convert the data

		// Fill out our model format's data

		// Finish up
	}
	
	// Functions for getting Private Data
	size_t getModelVersion(){
		return ModelVersion;
	}
};

}

#endif