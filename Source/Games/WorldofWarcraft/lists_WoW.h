#ifndef WORLDOFWARCRAFT_LISTS_H
#define WORLDOFWARCRAFT_LISTS_H

// This is used for various WoW-Specific lists and other static variables.

#include <QString>
#include <QList>

// Locale List
// Placement matches WoWLocale number
static QList<QString> LocaleList = QList<QString>()
	<< "enUS"		// English, US
	<< "enGB"		// English, Great Britian
	<< "frFR"		// French
	<< "deDE"		// German
	<< "koKR"		// Korean
	<< "zhCN"		// Chinese, Simplified
	<< "zhTW"		// Chinese, Traditional
	<< "esES"		// Spanish, Spain
	<< "esMX"		// Spanish, Mexico
	<< "ruRU"		// Russian
	<< "jaJP"		// Japanese
	<< "ptBR"		// Portuguese, Brazil
	<< "itIT"		// Italian
	<< "plPL"		// Polish

	<< "None";		// Used for Error Detection. Should ALWAYS be last!

// Expansion Name List
static QList<QString> ExpansionNameList = QList<QString>()
	<< QObject::tr("Vanilla")
	<< QObject::tr("Burning Crusade")
	<< QObject::tr("Wrath of the Lich King")
	<< QObject::tr("Cataclysm")
	<< QObject::tr("Mists of Pandaria")
	
	<< QObject::tr("Public Test Realm")
	<< QObject::tr("Beta Server");

#endif