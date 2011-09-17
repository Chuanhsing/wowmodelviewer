/*
	This file is for common and global-level classes and structures used by WMV.
*/
#ifndef COMMON_CLASSES_H
#define COMMON_CLASSES_H

#include <QString>
#include <QDir>
#include <QList>
#include "enums.h"
#include "constants.h"

// WoW Directory Structure
// Used to store WoW Directory information
struct st_WoWDir {
	QString Name;			// Name of this Directory
	QDir Directory;			// Location of this Dir
	int Locale;				// This Directory's chosen Locale.
	int Version;			// Version of this Dir. Should match a WoWVersion.
	int Position;			// Position in the WoW directory list. No two should ever have the same number!

	QList<QString> MPQList;	// List of MPQ files associated with this Directory.

	st_WoWDir(QDir d = QDir(), int l = LOCALE_NONE, int v = WOW_NOTLOADED, QString n = "WoW Folder", QList<QString> m = QList<QString>()){
		Directory = d;
		Name = n;
		Locale = l;
		Version = v;
		MPQList = m;
	};

	bool operator== (st_WoWDir a){
		if ((a.Directory == Directory) && (a.Name == Name) && (a.Locale == Locale) && (a.Version == Version) && (a.Position == Position) && (a.MPQList == MPQList))
			return true;
		return false;
	}
};

// Base Model class. All model types (M2, WMO, ADT) inherit this class.
class cModelBase {
public:
	// Data structures
	QString ModelName;		// Complete File Path & Name for this model.
	QString FileName;		// Just the File Name of the model.
	QString PathName;		// Just the Path for the model.
};

// Archive Type
struct cMPQArchHandle{
public:
	QFileInfo Info;
	HANDLE Handle;
	bool isPatched;

	cMPQArchHandle(QFileInfo i = QFileInfo(), HANDLE h = NULL, bool p = false){
		Info = i;
		Handle = h;
		isPatched = p;
	}
};
typedef QList<cMPQArchHandle> t_ArchiveSet;

#endif