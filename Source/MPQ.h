#ifndef MPQ_H
#define MPQ_H

#include <QList>
#include <QMap>
#include <QString>
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

#endif