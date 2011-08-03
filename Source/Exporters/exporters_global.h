#ifndef EXPORTERS_GLOBAL_H
#define EXPORTERS_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef EXPORTERS_LIB
# define EXPORTERS_EXPORT Q_DECL_EXPORT
#else
# define EXPORTERS_EXPORT Q_DECL_IMPORT
#endif

#endif // EXPORTERS_GLOBAL_H
