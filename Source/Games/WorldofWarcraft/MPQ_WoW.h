#ifndef WORLDOFWARCRAFT_MPQ_H
#define WORLDOFWARCRAFT_MPQ_H

#include <QString>
#include <QList>
#include "../MPQ_base.h"
#include "classes_WoW.h"

namespace WMVEngine {

// Retrieves a list of MPQ files, based on the specified WoWDir's version.
QList<QString> MPQList_Get(st_WoWDir &WoWDir);

} // Namespace

#endif