/*
	This file is for common and global-level classes and structures used by WMV.
*/
#ifndef COMMON_CLASSES_H
#define COMMON_CLASSES_H

#include <QString>
#include <QList>
#include <qdir.h>
#include "constants.h"

namespace WMVEngine {

// Archive Type
class cArchiveHandle {
public:
	QFileInfo Info;
	HANDLE Handle;
	bool isPatched;

	cArchiveHandle(QFileInfo i = QFileInfo(), HANDLE h = NULL, bool p = false){
		Info = i;
		Handle = h;
		isPatched = p;
	}
};
typedef QList<cArchiveHandle> t_ArchiveSet;

} // Namespace

#endif