/*
	This file is for common and global-level classes and structures used by WMV.
*/
#ifndef COMMON_CLASSES_H
#define COMMON_CLASSES_H

#include <QString>
#include <QDir>
#include <QList>
#include "enums.h"

// WoW Directory Structure
// Used to store WoW Directory information
struct st_WoWDir {
	QString Name;			// Name of this Directory
	QDir Directory;			// Location of this Dir
	int Locale;				// This Directory's chosen Locale.
	int Version;			// Version of this Dir. Should match a WoWVersion.
	int Position;			// Position in the WoW directory list. No two should ever have the same number!

	QList<QString> MPQList;	// List of MPQ files associated with this Directory.

	st_WoWDir(){
		Directory = QDir();
		Name = "WoW Folder";
		Locale = LOCALE_NONE;
		Version = WOW_NOTLOADED;
		MPQList = QList<QString>();
	};
	st_WoWDir(QDir d, int l = LOCALE_NONE, int v = WOW_NOTLOADED, QString n = "WoW Folder", QList<QString> m = QList<QString>()){
		Directory = d;
		Name = n;
		Locale = l;
		Version = v;
		MPQList = m;
	};
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