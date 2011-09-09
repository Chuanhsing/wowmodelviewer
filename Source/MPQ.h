#ifndef MPQ_H
#define MPQ_H

#include <QString>
#include <QList>
#include "enums.h"
#include "lists.h"
#include "classes.h"
#include "StormLib/src/StormLib.h"

class MPQArchive
{
private:
	bool OK;			// Was there any problems loading the Archive?
	HANDLE MPQArch;		// Identifies the current MPQ file.

public:
	// Data
	size_t ErrorCode;
	size_t ErrorData;

	// Constructors
	MPQArchive(QString);
	~MPQArchive() {	Close(); };

	// Functions
	bool isPartialMPQ(QString);
	void Close();
};

class MPQFile
{
	bool EoF;
	unsigned char *buffer;
	size_t pointer,size;

	// Disables Copying
	//MPQFile(const MPQFile &f){}
	//void operator=(const MPQFile &f) {}

public:
	MPQFile():EoF(false),buffer(0),pointer(0),size(0) {}
	MPQFile(QString);	// Filenames are NOT case sensitive
	void openFile(QString);
	~MPQFile();

	void close();
	
	bool isPartialMPQ(QString filename);
};

// Retrieves a list of MPQ files, based on the specified WoWDir's version.
QList<QString> MPQList_Get(st_WoWDir &WoWDir);

#endif