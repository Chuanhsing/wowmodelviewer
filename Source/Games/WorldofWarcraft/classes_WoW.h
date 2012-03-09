#ifndef WORLDOFWARCRAFT_CLASSES_H
#define WORLDOFWARCRAFT_CLASSES_H

/*
	This file is for World of Warcraft-specific classes and structures.
*/

#include <QString>
#include <QList>
#include <qdir.h>
#include "../Engine/constants.h"
#include "enums_WoW.h"

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

#endif