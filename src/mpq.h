// libmpq based implementation by :wumpus:
#if defined (_MAC)
#include "mpq_libmpq.h"
#elif defined (_LINUX)
#include "mpq_libmpq.h"
#endif

// ShadowF mpq library
//#include "mpq_sfmpq.h"

// StormLib library
#if defined (_WINDOWS)
#include "mpq_stormlib.h"
#endif
