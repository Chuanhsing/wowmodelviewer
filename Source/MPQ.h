#ifndef MPQ_H
#define MPQ_H

#include <QString>
#include <QList>
#include <QMap>
#include "enums.h"
#include "lists.h"
#include "Settings_Main.h"
#include "StormLib/src/StormLib.h"

class MPQArchive
{
private:
	bool OK;			// Was there any problems loading the Archive?
	HANDLE MPQArch;		// Identifies the current MPQ file.

public:
	// Constructors
	MPQArchive(QString ArchiveName);
	~MPQArchive() {};

	// Functions
	bool isPartialMPQ(QString ArchiveName);
	void Close();
};

// Retrieves a list of MPQ files, based on the specified WoWDir's version.
QList<QString> MPQList_Get(stWoWDir WoWDir);

#endif