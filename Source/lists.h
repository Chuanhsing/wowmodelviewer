#ifndef WMV_LISTS_H
#define WMV_LISTS_H

#include <qstring.h>
#include <qlist.h>
#include "version.h"

// Placement matches WoWLocale number
static QList<QString> LocaleList;

inline void PopulateLists(){
	// Locale List
	LocaleList.push_back("enUS");		// American English
	LocaleList.push_back("enGB");		// British English (Identical to enUS)
	LocaleList.push_back("frFR");		// French
	LocaleList.push_back("deDE");		// German
	LocaleList.push_back("koKR");		// Korean
	LocaleList.push_back("zhCN");		// Chinese Simplified
	LocaleList.push_back("zhTW");		// Chinese Traditional
	LocaleList.push_back("esES");		// Spanish Spanish
	LocaleList.push_back("esMX");		// Mexican Spanish
	LocaleList.push_back("ruRU");		// Russian
	LocaleList.push_back("jaJP");		// Japanese
	LocaleList.push_back("ptBR");		// Brazilian Portuguese
};

#endif