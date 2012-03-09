/*

	Basic, Non-Game-Specific MPQ functions

*/
#ifndef MPQ_BASE_H
#define MPQ_BASE_H

#include <QFileInfo>
#include <QMessageBox>
#include "MPQ_enums.h"
#include "../Engine/wmv_engine.h"
#include "StormLib/src/StormLib.h"
#include "../Engine/lists.h"

namespace WMVEngine {
namespace MPQ {

// Directory Slash type for MPQ files
#define SLASH_MPQ "\\"

class MPQArchive
{
private:
	HANDLE MPQArch;		// Identifies the current MPQ file.

public:
	// Data
	bool OK;			// Was there any problems loading the Archive?
	QFileInfo FileInfo;
	bool isPatchFile;
	size_t ErrorCode;
	size_t ErrorData;

	// Constructors
	MPQArchive(QString);
	MPQArchive(QString,QStringList);
	~MPQArchive() {	Close(); };

	// Functions
	bool isPartialMPQ(QString, QString);
	HANDLE getHANDLE() { return MPQArch; };
	void Close();
};

class MPQFile
{
	// Private Variables
	bool EoF;
	unsigned char *buffer;
	size_t pointer,size;
	t_ArchiveSet fileList;

	// Disables Copying
	MPQFile(const MPQFile &){}
	void operator=(const MPQFile &) {}

public:
	// Constructors
	MPQFile():EoF(false),buffer(0),pointer(0),size(0) {fileList = LoadedArchives;}
	MPQFile(QString);
	MPQFile(QString,t_ArchiveSet);
	void openFile(QString);					// Filenames are NOT case sensitive
	void openFile(QString,t_ArchiveSet);	// The Actual function. With t_ArchiveSet, we can access un-loaded MPQ lists.
	~MPQFile();

	// Variables
	QString FileName;

	// Functions
	bool isEof();
	void close();
	bool isPartialMPQ(QString, QString);
	void seek(ssize_t offset);
	void seekRelative(ssize_t);
	size_t read(void* dest, size_t bytes);
	size_t getSize();
	size_t getPos();
	bool exists();
};

}	// MPQ Namespace
}	// WMVEngine Namespace

#endif