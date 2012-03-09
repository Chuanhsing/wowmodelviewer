#ifndef WMV_LISTS_H
#define WMV_LISTS_H

// This is used for various lists and other static variables.

#include <QString>
#include <QList>
#include "classes.h"

namespace WMVEngine {

// --== Static Global Variables ==--

// Our master list of currently loaded archives
static t_ArchiveSet LoadedArchives;

// A list to temporaily store an archive list.
static t_ArchiveSet TempArchiveList;

} // Namespace

#endif