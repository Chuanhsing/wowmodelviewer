#ifndef MODELVIEWERMODELFORMAT_H
#define MODELVIEWERMODELFORMAT_H

class Quatd{};

#include <qdir.h>
#include "enums.h"
#include "../Foundation/Math/math_vector.h"
//#include "../Foundation/Math/math_quaternion.h"

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
class AnimSequence {};

// Our internal model format
class ModelViewerModel {
private:
	size_t ModelVersion;

public:
	QList<ModelViewerModel>Submodels;	// Used for sub-objects, such as attached models or doodads.
	QString ModelName;					// Name of the Model. IE: The Big Bad Wolf
	QString FileName;					// The filename of the model. IE: BigBadWolf.m2
	QDir FilePath;						// Path to the file. IE: Creatures/Wolves/BigBadWolf

	Vec3d Position;						// Position of the model
	Quatd Rotation;						// Rotation of the model
	Vec3d Scale;						// Scale of the model
	size_t ParentBone;					// The bone number this model is attached to. -1 for none.
	QList<AnimSequence> AnimationList;	// List of animations for this model.
	size_t SourceID;					// Used for identifying the current model's source type.

	// Constructors
private:
	// Privatize the default, so people can't build empty models.
	ModelViewerModel() {
		ModelVersion = MODELVIEWERMODELVERSION;
		ParentBone = -1;
		Position = Vec3d(0.0);
		Rotation = Quatd();		// 0,0,0,1
		Scale = Vec3d(1.0);
		SourceID = NOMODELSOURCE;
		AnimationList = QList<AnimSequence>();
	}
public:
	ModelViewerModel(QString file){
		ModelViewerModel();
		// Fill out the data.
	}
	
	// Functions for getting Private Data
	size_t getModelVersion(){
		return ModelVersion;
	}
};

}

#endif