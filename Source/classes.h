/*
	This file is for common and global-level classes and structures used by WMV.
*/
#ifndef COMMON_CLASSES_H
#define COMMON_CLASSES_H

// WoW Directory Structure
// Used to store WoW Directory information
struct stWoWDir {
	QString Name;			// Name of this Directory
	QDir Directory;			// Location of this Dir
	int Version;			// Version of this Dir. Should match a WoWVersion.
	int Position;			// Position in the WoW directory list. No two should ever have the same number!
};

// Base Model class. All model types (M2, WMO, ADT) inherit this class.
class cModelBase {
public:
	// Data structures
	QString ModelName;		// Complete File Path & Name for this model.
	QString FileName;		// Just the File Name of the model.
	QString PathName;		// Just the Path for the model.
};

#endif