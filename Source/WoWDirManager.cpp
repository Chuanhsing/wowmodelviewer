#include <QMenu>
#include "wowdirmanager.h"
#include "globals.h"
#include "lists.h"
#include "Settings_Main.h"
#include "MPQ.h"

// WoW Directory Manager
WoWDirManager::WoWDirManager(QWidget *parent) : QDialog(parent), ui_WoWDirManager(new Ui::WoWDirManager)
{
	// -= Setup Window =-
	ui_WoWDirManager->setupUi(this);

	// -= WoW Type Selector Menu =-
	QMenu *menuWoWTypeSelector = new QMenu(this);
	
	// Icons
	QIcon iconVanilla;
    iconVanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::Off);
    iconVanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon1-Vanilla"), QSize(), QIcon::Normal, QIcon::On);
	QIcon iconTBC;
    iconTBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon2-TBC"), QSize(), QIcon::Normal, QIcon::Off);
    iconTBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon2-TBC"), QSize(), QIcon::Normal, QIcon::On);
	QIcon iconWotLK;
    iconWotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon3-WotLK"), QSize(), QIcon::Normal, QIcon::Off);
	iconWotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon3-WotLK"), QSize(), QIcon::Normal, QIcon::On);
	QIcon iconCata;
    iconCata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon4-Cataclysm"), QSize(), QIcon::Normal, QIcon::Off);
	iconCata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon4-Cataclysm"), QSize(), QIcon::Normal, QIcon::On);
	QIcon iconPTR;
    iconPTR.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::Off);
	iconPTR.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::On);

	// Actions
	QAction *actionWTS_Vanilla = new QAction(parent);
	actionWTS_Vanilla->setText(ExpansionNameList.value(EXPANSIONNAME_VANILLA));
    actionWTS_Vanilla->setObjectName(QString::fromUtf8("actionWTS_Vanilla"));
	actionWTS_Vanilla->setIcon(iconVanilla);
	//actionWTS_Vanilla->setCheckable(true);
	QAction *actionWTS_BurningCrusade = new QAction(parent);
	actionWTS_BurningCrusade->setText(ExpansionNameList.value(EXPANSIONNAME_BURNINGCRUSADE));
    actionWTS_BurningCrusade->setObjectName(QString::fromUtf8("actionWTS_BurningCrusade"));
	actionWTS_BurningCrusade->setIcon(iconTBC);
	//actionWTS_BurningCrusade->setCheckable(true);
	QAction *actionWTS_WotLK = new QAction(parent);
	actionWTS_WotLK->setText(ExpansionNameList.value(EXPANSIONNAME_WOTLK));
    actionWTS_WotLK->setObjectName(QString::fromUtf8("actionWTS_WotLK"));
	actionWTS_WotLK->setIcon(iconWotLK);
	//actionWTS_WotLK->setCheckable(true);
	QAction *actionWTS_Cataclysm = new QAction(parent);
	actionWTS_Cataclysm->setText(ExpansionNameList.value(EXPANSIONNAME_CATACLYSM));
    actionWTS_Cataclysm->setObjectName(QString::fromUtf8("actionWTS_Cataclysm"));
	actionWTS_Cataclysm->setIcon(iconCata);
	//actionWTS_Cataclysm->setCheckable(true);
	QAction *actionWTS_PTR = new QAction(parent);
	actionWTS_PTR->setText(ExpansionNameList.value(EXPANSIONNAME_PTR));
    actionWTS_PTR->setObjectName(QString::fromUtf8("actionWTS_PTR"));
	actionWTS_PTR->setIcon(iconPTR);
	//actionWTS_PTR->setCheckable(true);

	// Append Actions
	menuWoWTypeSelector->addAction(actionWTS_Vanilla);
	menuWoWTypeSelector->addAction(actionWTS_BurningCrusade);
	menuWoWTypeSelector->addAction(actionWTS_WotLK);
	menuWoWTypeSelector->addAction(actionWTS_Cataclysm);
	menuWoWTypeSelector->addAction(actionWTS_PTR);
	
	// Set Menu
	ui_WoWDirManager->WoWTypeButton->setMenu(menuWoWTypeSelector);

	// -= Populate Window =-
}
WoWDirManager::~WoWDirManager()
{
	delete ui_WoWDirManager;
}

void WoWDirManager::on_WDM_bDirAdd_clicked(){
	QString dir = QFileDialog::getExistingDirectory(this,QObject::tr("Choose the WoW Directory you wish to add."),LastDir.path());
	QDir dDir(dir);
	LastDir = dDir;
	g_WMV->updateStatusBar(QObject::tr("Checking Directory %1...").arg(dir));
	if (!QDir(dir).exists()){
		g_WMV->updateStatusBar(QObject::tr("Specified Directory doesn't exist."));
		return;
	}
	if ((!QFile(dir+"/WoW.exe").exists())&&(!QFile(dir+"/World of Warcraft.app").exists())){
		g_WMV->updateStatusBar(QObject::tr("Directory found, but couldn't find World of Warcraft Application."));
		QMessageBox err;
		err.setIcon(QMessageBox::Critical);
		err.setWindowTitle(QObject::tr("Unable to find World of Warcraft"));
		err.setText(QObject::tr("Unable to find the World of Warcraft application.\nPlease make sure to select the proper directory!\n\nIt should have the following folders:\nData, Interface, Logs, and WTF"));
		err.exec();
		return;
	}
	g_WMV->updateStatusBar(QObject::tr("Directory is valid! Gathering Localization Data..."));

	// Get Locales
	int loc = LOCALE_ENUS;			// Default to enUS locale
	bool isVanilla = false;
	QStringList items;
	QDir idir(dDir.path().append("/Data"));
	if (QFile(idir.path().append("/interface.MPQ")).exists()){
		// WoW is Vanilla. Add all locales to our list
		isVanilla = true;
		for (size_t i=0;i<LocaleList.count();i++){
			items << LocaleList.value((int)i);
		}
	}else{
		// Scan for Locale dirs, then add them to the list
		for (size_t i=0;i<LocaleList.count();i++){
			QString llv = LocaleList.value((int)i);
			if (QDir(idir.path().append("/"+llv)).exists()){
				items << llv;
			}
		}
	}

	if (items.count() > 1){
		// Display list and get selection
		QInputDialog loc_d;
		loc_d.setComboBoxItems(items);
		loc_d.setWindowTitle(QObject::tr("World of Warcraft Locale"));
		if (isVanilla == true){
			loc_d.setLabelText(QObject::tr("Which locale is this installation?"));
		}else{
			loc_d.setLabelText(QObject::tr("Which of the installed locales would you like to use?"));
		}
		loc_d.exec();

		QString val = loc_d.textValue();

		for (size_t j=0;j<LocaleList.count();j++){
			if (val == LocaleList.value((int)j)){
				loc = (int)j;
				break;
			}
		}
	}else{
		for (size_t j=0;j<LocaleList.count();j++){
			if (items.value(0) == LocaleList.value((int)j)){
				loc = (int)j;
				break;
			}
		}
		/*
		Use for Debugging, or if we want to notify people that there's only 1 locale.

		QMessageBox msg;
		msg.setIcon(QMessageBox::Information);
		msg.setWindowTitle(QObject::tr("World of Warcraft Locale"));
		msg.setText(QObject::tr("Could only find one installed Locale: %1").arg(LocaleList.value(loc)));
		msg.exec();
		*/
	}

	st_WoWDir t_dir = ScanWoWDir(dDir,loc,WoWDirList.count());
	if (t_dir.Locale == LOCALE_NONE){
		g_WMV->updateStatusBar(QObject::tr("Error scanning WoW directory."));
		QMessageBox err;
		err.setIcon(QMessageBox::Critical);
		err.setWindowTitle(QObject::tr("Error Scanning WoW Directory"));
		err.setText(QObject::tr("An error occured while trying to scan your WoW directory for files.\n\nPlease double-check the directory, and try again."));
		err.exec();
		return;
	}

	QInputDialog n;
	n.setWindowTitle(QObject::tr("Set Directory Name"));
	n.setLabelText(QObject::tr("Please choose a name for this Directory."));
	QString name = t_dir.Name;
	if (name == "WoW Folder"){
		name = dDir.dirName();
	}
	if (items.count() > 1)
		name.append("-"+LocaleList.value(t_dir.Locale));
	n.setTextValue(name);
	n.exec();

	t_dir.Name = n.textValue();

	// Add to saved Directory List
	// Save the Settings
	// Refresh the Menu's Dir List

	// Debug Message
	QMessageBox msg;
	msg.setText(QObject::tr("Found the World of Warcraft Application!\nInstall Name: %3\nDir Result: %1\nChosen Locale: %2").arg(t_dir.Directory.absolutePath()).arg(LocaleList.value(t_dir.Locale)).arg(t_dir.Name));
	msg.exec();
}

// Scan the passed directory and return a WoWDir.
st_WoWDir ScanWoWDir(QDir dir, int locale, int position = 0){
	if (dir.exists() == false)
		return st_WoWDir();

	st_WoWDir wdir(dir,locale);
	wdir.Position = position;
	QString sloc = LocaleList.value(locale);

	QMessageBox a;

	// Check for realmlist.wtf file. Used to detect PTR.
	// Only needed for latest version that's hidden in the Locale folder.
	QDir ldir(dir);
	ldir.cd("Data/"+sloc);
	if (ldir.exists() == true){
		QFile r(ldir.absolutePath().append("/realmlist.wtf"));
		if (r.exists() == true){
			if (r.open(QFile::ReadOnly)){
				r.seek(0);		// Make sure we're at the start of the file.
				QString rtxt = r.readLine();
				if (rtxt.contains("public-test") == true) {
					wdir.Version = WOW_PTR;
					wdir.Name = QObject::tr("Public Test Realm","PTR Folder Default Name");
					MPQList_Get(wdir);
					return wdir;
				}
			}
		}
	}
	
	// If we've gotten here, it's either not PTR, or we can't detect that it is.
	// So, now we should figure out what version of the game we're running.

	// Scan for the inteface's MPQ file, and any patch files for it.
	QList<QString> iface = QList<QString>();
	QDir idir(dir.path().append("/Data"));
	if (QFile(idir.path().append("/interface.MPQ")).exists()){
		// Found Vanilla WoW
		iface.push_back(idir.path().append("/interface.MPQ"));
		QDir patch(idir);
		patch.setFilter(QDir::Files);
		QStringList f;
		f << "patch.MPQ" << "patch-?.MPQ";
		patch.setNameFilters(f);
		patch.setSorting(QDir::Name);
		for (size_t i=0;i<patch.count();i++){
			iface.push_back(patch.entryList().value((int)i));
		}
	}else if (QFile(ldir.path().append("/locale-"+sloc+".MPQ")).exists()){
		// Found a different WoW
		iface.push_back(ldir.path().append("/locale-"+sloc+".MPQ"));
		QDir patch(ldir);
		patch.setFilter(QDir::Files);
		QStringList f;
		f << "*-locale-????.MPQ" << "patch-????*.MPQ";						// Burning Crusade & WotLK
		f << "expansion?-locale-????.MPQ" << "wow-update-????-?????.MPQ";	// Cataclysm
		patch.setNameFilters(f);
		patch.setSorting(QDir::Name);
		for (size_t i=0;i<patch.count();i++){
			iface.push_back(patch.entryList().value((int)i));
		}
	}

	// Fail if there's not any interface files.
	if (iface.count() <= 0){
		return st_WoWDir();
	}

	// Initialize our limited MPQ list, so we can extract the TOC file

	/*
	// Extract file from MPQ archives
	MPQFile mf("Interface\\FrameXML\\FrameXML.toc");
	
	// Get the TOC Version Number
	goto 2nd line
	skip to interface number
	size_t inum = read to \r;		// This will get us the interface number, no matter how many digits it is. NOTE: This should work weither it's 1.x or 99.x
	wdir.Version = inum;
	if (inum <= WOW_VANILLA ){
		wdir.Name = QObject::tr("Vanilla Directory","Vanilla Folder Default Name");
	}else if (inum <= WOW_TBC ){
		wdir.Name = QObject::tr("Burning Crusade Directory","Burning Crusade Folder Default Name");
	}else if (inum <= WOW_WOTLK ){
		wdir.Name = QObject::tr("WotLK Directory","WotLK Folder Default Name");
	}else if (inum <= WOW_CATACLYSM ){
		wdir.Name = QObject::tr("Cataclysm Directory","Burning Crusade Folder Default Name");
	}else if (inum <= WOW_BETA ){
		wdir.Name = QObject::tr("Beta Directory","Beta Folder Default Name");
	}

	MPQList_Get(wdir);		// Don't bother getting the list until we have a version number!
	*/

	return wdir;
}