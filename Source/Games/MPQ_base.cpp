/*

	Basic, Non-Game-Specific MPQ functions

*/

#include "MPQ_base.h"
#include "../Engine/classes.h"

// Temp for the first function
#include "WorldofWarcraft/enums_WoW.h"
#include "WorldofWarcraft/lists_WoW.h"

using namespace WMVEngine;
using namespace WMVEngine::MPQ;

/*
Most of the MPQ functions are directly converted from WMV 0.7.0.1, as their functions
haven't needed to be changed since they were originally written. These SHOULD work
with all versions of WoW, but have yet to be fully tested.

NOTE: I've decided NOT to include the capability of using local files. If we want to
restore that functionality, we can simply copy the old source code for it, and update
it for QT and the new variable names.
*/

// --== MPQ Archive ==--

// This is still very WoW-centeric, and will remain so until we can figure out a way to generalize it.
MPQArchive::MPQArchive(QString filename, QStringList ArchList) : OK(false), ErrorCode(MPQERROR_OKAY)
{
	QLOG_INFO() << "Initializing MPQ Archive:" << filename;

	QMessageBox err;
	err.setIcon(QMessageBox::Critical);
	FileInfo = QFileInfo(filename);
	isPatchFile = false;


	// Check if the file exists
	if (QFile(FileInfo.absoluteFilePath()).exists() == false){
		QLOG_ERROR() << "Unable to find Archive on disk. May not exist!";
		err.setWindowTitle(QObject::tr("Error: Unable to open MPQ file!"));
		err.setText(QObject::tr("Could not an open MPQ file, because we could not find it!\n\nPlease make sure any downloading/updating is complete before trying again."));
		err.exec();

		ErrorCode = MPQERROR_FILENOTFOUND;
		return;
	}

	// Check for .Lock file.
	// Actually, we should move this to ABOVE this function. We shouldn't call MPQArchive() if .lock exists.
	QLOG_INFO() << "MPQ Archive file found! Checking for Lock File...";
	if (QFile(QString(FileInfo.absoluteFilePath()).append(".lock")).exists()){
		QLOG_ERROR() << "Discovered a lock file! We will be unable to open this archive.";
		err.setWindowTitle(QObject::tr("Error: Unable to open MPQ file!"));
		err.setText(QObject::tr("Could not open MPQ file, because it is in use.\nPlease close World of Warcraft, then try again."));
		err.exec();

		ErrorCode = MPQERROR_LOCKFILEFOUND;
		return;
	}

	// Attempt to open the MPQ file
	QLOG_INFO() << "Lock file not found! Attempting to open MPQ Archive...";
//	g_WMV->updateStatusBar(QObject::tr("Attempting to open %1...").arg(FileInfo.absoluteFilePath()));
	if (!SFileOpenArchive(FileInfo.absoluteFilePath().toUtf8(), 0, MPQ_OPEN_FORCE_MPQ_V1|MPQ_OPEN_READ_ONLY, &MPQArch )) {
		size_t ec = GetLastError();
		QLOG_ERROR() << "Opening Failed. Error Code:" << ec;
		ErrorCode = MPQERROR_COULDNOTOPEN;
		ErrorData = ec;
		return;
	}
	QLOG_INFO() << "Successfully opened MPQ Archive.";

	// Do Patch files, But Skip the Cache Directory
	if (!(FileInfo.absolutePath().toLower().contains("cache")) && FileInfo.completeBaseName().toLower().startsWith("patch") && !isPartialMPQ(filename,"wow-update-")) { // skip the Patch files archives
		QLOG_INFO() <<  "Finding and applying Patch files...";
		// Process Patch Files
		for(ssize_t j=ArchList.count()-1; j>=0; j--) {
			QFileInfo arch(ArchList.value((int)j));

			// Find Locale, if added.
			size_t loc = LOCALE_ENUS;		// Default to enUS
			for (size_t l=0;l<LocaleList.count();l++){
				if (arch.completeBaseName().contains(LocaleList.value((int)l))){
					loc = l;
					break;
				}
			}
			QString sloc = LocaleList.value((int)loc);

			QLOG_TRACE() << "Processing MPQ patch" << arch.absoluteFilePath();

			if (!arch.completeBaseName().toLower().startsWith("wow-update-"))
				continue;
			if (arch.completeBaseName().length() == strlen("wow-update-xxxxx.mpq")) {
				QLOG_TRACE() << "Applying Patch file" << FileInfo.fileName() << "to" << arch.fileName() << "base & locale directory...";
				SFileOpenPatchArchive(MPQArch, ArchList.value(j).toUtf8(), "base", 0);
				SFileOpenPatchArchive(MPQArch, ArchList.value(j).toUtf8(), sloc.toUtf8(), 0);
			} else if (arch.absolutePath() == FileInfo.absolutePath()) {
				QLOG_TRACE() << "Applying Patch file" << arch.fileName() << "to" << arch.absoluteDir().path() << "directory...";
				SFileOpenPatchArchive(MPQArch,  ArchList.value(j).toUtf8(), "", 0);
			}
		}
		QLOG_INFO() << "Successfully patched MPQ Archive.";
	}

	OK = true;
	LoadedArchives.push_back(cArchiveHandle(FileInfo, &MPQArch, isPatchFile));
	
	QLOG_INFO() << "Successfully added" << FileInfo.fileName() << "to our loaded archives.";
//	g_WMV->updateStatusBar(QObject::tr("Successfully added %1!").arg(FileInfo.absoluteFilePath()));
}

MPQArchive::MPQArchive(QString filename) : OK(false), ErrorCode(MPQERROR_OKAY)
{
	QStringList a;
	for (QList<cArchiveHandle>::Iterator i=LoadedArchives.begin();i!=LoadedArchives.end(); ++i){
		a.push_back(i->Info.filePath());
	}
	MPQArchive::MPQArchive(filename, a);
}

// Close the MPQ Archive
void MPQArchive::Close()
{
	// If the file wasn't properly loaded, then there's nothing to close.
	if (OK == false)
		return;

	// Close the Current Archive
	SFileCloseArchive(MPQArch);

	// Remove from the list of loaded Archive Files
	for (size_t i=0;i<LoadedArchives.count(); i++) {
		HANDLE thisMPQ = LoadedArchives.value((int)i).Handle;
		if (thisMPQ == MPQArch){
			LoadedArchives.removeAt((int)i);
			return;
		}
	}
}

// Determine if the MPQ is a Partical MPQ
bool checkIsPartialMPQ(QString filename, QString namecheck)
{
	QFileInfo f(filename);
	if (f.completeBaseName().toLower().startsWith(namecheck))
		return true;
	return false;
}
bool MPQArchive::isPartialMPQ(QString filename, QString namecheck){		// MPQArchive's isPartialMPQ
	return checkIsPartialMPQ(filename, namecheck);
}
bool MPQFile::isPartialMPQ(QString filename, QString namecheck){		// MPQFile's isPartialMPQ
	return checkIsPartialMPQ(filename, namecheck);
}


// --== MPQ File ==--

MPQFile::MPQFile(QString filename):EoF(false),buffer(0),pointer(0),size(0){
	openFile(filename,LoadedArchives);
}
MPQFile::MPQFile(QString filename, t_ArchiveSet arch):EoF(false),buffer(0),pointer(0),size(0){
	openFile(filename,arch);
}

MPQFile::~MPQFile()
{
	close();
}

// Open file. If Archives are not specified, use the LoadedArchs.
// This mirror's the original function
void MPQFile::openFile(QString filename){
	openFile(filename,LoadedArchives);
}

// Open the file from an MPQ
void MPQFile::openFile(QString filename, t_ArchiveSet arch){
	QLOG_INFO() << "Attempting to open" << filename << "from MPQ files...";
	EoF = false;
	buffer = 0;
	pointer = 0;
	size = 0;

	// Set File Name
	FileName = filename;

	// Set this MPQ's list.
	fileList = arch;

	/*
	Not sure if we need this yet... This is from the original code, and needs to be converted.

	// zhCN alternate file mode
	if (bAlternate && !filename.Lower().StartsWith(wxT("alternate"))) {
		wxString alterName = wxT("alternate")+SLASH+filename;

		for(ArchiveSet::iterator i=gOpenArchives.begin(); i!=gOpenArchives.end(); ++i)
		{
			HANDLE &mpq_a = *i->second;

			HANDLE fh;

			if( !SFileOpenFileEx( mpq_a, alterName.fn_str(), SFILE_OPEN_PATCHED_FILE, &fh ) )
				continue;

			// Found!
			DWORD filesize = SFileGetFileSize( fh );
			size = filesize;

			// HACK: in patch.mpq some files don't want to open and give 1 for filesize
			if (size<=1) {
				eof = true;
				buffer = 0;
				return;
			}

			buffer = new unsigned char[size];
			SFileReadFile( fh, buffer, (DWORD)size );
			SFileCloseFile( fh );

			return;
		}
	}
	*/

	for(QList<cArchiveHandle>::Iterator i=fileList.begin();i!=fileList.end(); ++i)
	{
		cArchiveHandle &tarch = *i;
		QLOG_TRACE() << "Checking Archive:" << tarch.Info.absoluteFilePath();
		HANDLE MPQArch = tarch.Handle;
		HANDLE fh;

		int flags = (tarch.isPatched==true ? SFILE_OPEN_PATCHED_FILE : SFILE_OPEN_FROM_MPQ);

		if( !SFileOpenFileEx(MPQArch, filename.toUtf8(), flags, &fh ) )
			continue;

		// Found!
		QLOG_TRACE() << "Found the file!";
		DWORD filesize = SFileGetFileSize( fh );
		size = filesize;
		QLOG_TRACE() << "Reported filesize: " << size;

		/* Need to see if this is still true &/or still needed.

		// HACK: in patch.mpq some files don't want to open and give 1 for filesize
		if (size<=1) {
			EoF = true;
			buffer = 0;
			return;
		}
		*/

		QLOG_TRACE() << "Copying the data into the buffer...";
		buffer = new unsigned char[size];
		SFileReadFile( fh, buffer, (DWORD)size );
		SFileCloseFile( fh );
		
		QLOG_INFO() << "Sucessfully opened the file from the MPQs!";

		return;
	}

	QLOG_ERROR() << "File not found inside the MPQs!";

	EoF = true;
	buffer = 0;
}


bool MPQFile::exists()
{
	for(size_t i=0;i<fileList.count(); i++)
	{
		cArchiveHandle file = fileList.value((int)i);
		HANDLE &mpq_a = file.Handle;

		if( SFileHasFile( mpq_a, FileName.toUtf8() ) )
			return true;
	}
	return false;
}

size_t MPQFile::read(void* dest, size_t bytes)
{
	if (EoF) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos > size) {
		bytes = size - pointer;
		EoF = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = rpos;

	return bytes;
}

void MPQFile::close()
{
	buffer = 0;
	EoF = true;
}
bool MPQFile::isEof()
{
    return EoF;
}
void MPQFile::seek(ssize_t offset) {
	pointer = offset;
	EoF = (pointer >= size);
}

void MPQFile::seekRelative(ssize_t offset)
{
	pointer += offset;
	EoF = (pointer >= size);
}
size_t MPQFile::getSize()
{
	return size;
}
