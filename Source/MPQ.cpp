#include "MPQ.h"

typedef QMap<QString, HANDLE*> ArchiveSet;
static ArchiveSet LoadedArchives;

void MPQArchive::Close()
{
	// If the file wasn't properly loaded, then there's nothing to close.
	if (OK == false)
		return;

	// Close the Current Archive
	SFileCloseArchive(MPQArch);

	// Remove from the list of loaded Archive Files
	for (ArchiveSet::Iterator iter=LoadedArchives.begin(); iter!=LoadedArchives.end(); ++iter) {
		HANDLE thisMPQ = iter.value();
		if (thisMPQ == MPQArch){
			LoadedArchives.erase(iter);
			return;
		}
	}
}