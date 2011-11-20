#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QFile>
#include "../Engine/WoWDirectoryTools.h"
#include "WoWDirManager.h"
#include "../Engine/lists.h"
//#include "../Application/Settings_Main.h"
//#include "../Engine/MPQ.h"

using namespace WMVEngine;

// WoW Directory Manager
WoWDirManager::WoWDirManager(QWidget *parent) : QDialog(parent), ui_WoWDirManager(new Ui::WoWDirManager)
{
	// -= Setup Window =-
	ui_WoWDirManager->setupUi(this);
	/*
	// Icons
    iconVanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon01-Vanilla"), QSize(), QIcon::Normal, QIcon::Off);
    iconVanilla.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon01-Vanilla"), QSize(), QIcon::Normal, QIcon::On);
    iconTBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon02-TBC"), QSize(), QIcon::Normal, QIcon::Off);
    iconTBC.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon02-TBC"), QSize(), QIcon::Normal, QIcon::On);
    iconWotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon03-WotLK"), QSize(), QIcon::Normal, QIcon::Off);
	iconWotLK.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon03-WotLK"), QSize(), QIcon::Normal, QIcon::On);
    iconCata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon04-Cataclysm"), QSize(), QIcon::Normal, QIcon::Off);
	iconCata.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon04-Cataclysm"), QSize(), QIcon::Normal, QIcon::On);
    iconMoP.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon05-MoP"), QSize(), QIcon::Normal, QIcon::Off);
	iconMoP.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon05-MoP"), QSize(), QIcon::Normal, QIcon::On);
    iconPTR.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::Off);
	iconPTR.addFile(QString::fromUtf8(":/WoW Versions/WoWIcon-PTR"), QSize(), QIcon::Normal, QIcon::On);

	// Set Up List
	SetupList();
	*/
}

WoWDirManager::~WoWDirManager()
{
	delete ui_WoWDirManager;
}
void WoWDirManager::init(){
	// Update List
	UpdateList();
}


// Save the Directory to the WoWDirs.ini file
void WoWDirManager::saveDir(st_WoWDir dir, bool noUpdate = false)
{
	/*
	// Get the Group Name
	QString g = WoWDirGroupName(dir);

	QLOG_INFO() << "Saving new WoWDir:" << g;

	// Set the Dir's Variables
	sWoWDirs.setValue(g+"/Name",dir.Name);
	sWoWDirs.setValue(g+"/Directory",dir.Directory.absolutePath());
	sWoWDirs.setValue(g+"/Version",dir.Version);
	sWoWDirs.setValue(g+"/Position",dir.Position);
	sWoWDirs.setValue(g+"/Locale",dir.Locale);
	// Build our MPQ List.
	QString mpql = "";
	for (size_t i=0;i<dir.MPQList.count();i++){
		if (i > 0)
			mpql.append(";");		// MPQ List Seperator!
		mpql.append(dir.MPQList.value((int)i));
	}
	sWoWDirs.setValue(g+"/MPQList",mpql);

	// Update INI file
	sWoWDirs.sync();
	QLOG_INFO() << "Finished saving new WoWDir.";

	// Update List
	if (noUpdate == false)
		UpdateList();
	*/
}

// Called when someone clicks the "Add" button
void WoWDirManager::on_WDM_bDirAdd_clicked(){
	/*
	QLOG_INFO() << "Attempting to add a new WoWDir...";
	// Directory chooser
	QString dir = QFileDialog::getExistingDirectory(this,tr("Choose the WoW Directory you wish to add."),sWMVSettings.value("WDMLastDir").toString());
	if (dir == QString()){
		// User probably hit cancel, so silently fail.
		QLOG_WARN() << "User Cancelled adding a new WoWDir.";
		return;
	}
	QLOG_INFO() << "New WoWDir Path:" << dir;
	QDir dDir(dir);
	g_WMV->updateStatusBar(tr("Checking Directory %1...").arg(dir));
	if (!QDir(dir).exists()){
		QLOG_ERROR() << "Specified WoWDir Directory doesn't exist.";
		g_WMV->updateStatusBar(tr("Specified Directory doesn't exist."));
		return;
	}
	
	QMessageBox err;
	err.setIcon(QMessageBox::Critical);

	if ((!QFile(dir+"/WoW.exe").exists())&&(!QFile(dir+"/World of Warcraft.app").exists())){
		QLOG_ERROR() << "Directory found, but it doesn't appear to contain the World of Warcraft Application.";
		g_WMV->updateStatusBar(tr("Directory found, but couldn't find World of Warcraft Application."));
		err.setWindowTitle(tr("Unable to find World of Warcraft"));
		err.setText(tr("Unable to find the World of Warcraft application.\nPlease make sure to select the proper directory!\n\nYou should see the following folders:\nData, Interface, Logs, and WTF"));
		err.exec();
		return;
	}
	
	// Save Last WoW Directory
	sWMVSettings.setValue("WDMLastDir", dDir.absolutePath());
	sWMVSettings.sync();

	g_WMV->updateStatusBar(tr("Directory is valid! Gathering Localization Data..."));
	QLOG_INFO() << "Successfully found World of Warcraft! Scanning for Locale information...";

	// Get Locales
	int loc = LOCALE_ENUS;			// Default to enUS locale
	bool isVanilla = false;
	QStringList items;
	QDir idir(dDir.path().append("/Data"));
	if (QFile(idir.path().append("/interface.MPQ")).exists()){
		// WoW is Vanilla. Add all locales to our list
		QLOG_INFO() << "WoWDir appears to be Vanilla WoW. We will ask which Locale it is.";
		isVanilla = true;
		for (size_t i=0;i<LocaleList.count();i++){
			items << LocaleList.value((int)i);
		}
	}else{
		// Scan for Locale dirs, then add them to the list
		QLOG_INFO() << "WoWDir is NOT Vanilla. Scanning for Locale directories...";
		for (size_t i=0;i<LocaleList.count();i++){
			QString llv = LocaleList.value((int)i);
			if (QDir(idir.path().append("/"+llv)).exists()){
				items << llv;
			}
		}
		QLOG_INFO() << "Scan Complete. Found" << items.count() << (items.count()>1?"locales.":"locale.");
	}

	if (items.count() == 0){
		g_WMV->updateStatusBar(tr("Error gathering Localization Data. No Locales found."));
		QLOG_ERROR() << "No locales in list. Aborting add WoWDir...";
		err.setWindowTitle(tr("No Locales Found"));
		if (isVanilla == true){
			err.setText(tr("There was an error gathering the list of WoW Locales. Please try again."));
		}else{
			err.setText(tr("Could not find any Locales in your World of Warcraft Directory.\nCheck your \"WoW/Data\" folder for locale directories."));
		}
		err.exec();
		return;
	}
	
	if (items.count() > 1){
		// Display list and get selection
		QInputDialog loc_d;
		loc_d.setComboBoxItems(items);
		loc_d.setWindowTitle(tr("World of Warcraft Locale"));
		if (isVanilla == true){
			loc_d.setLabelText(tr("Which locale is this installation?"));
		}else{
			loc_d.setLabelText(tr("Which of the installed locales would you like to use?"));
		}
		loc_d.exec();

		QString val = loc_d.textValue();

		for (size_t j=0;j<LocaleList.count();j++){
			if (val == LocaleList.value((int)j)){
				loc = (int)j;
				break;
			}
		}
		QLOG_INFO() << "User has chosen to use" << LocaleList.value(loc) << "as our locale.";
	}else{
		for (size_t j=0;j<LocaleList.count();j++){
			if (items.value(0) == LocaleList.value((int)j)){
				loc = (int)j;
				break;
			}
		}

		QLOG_INFO() << "Only 1 local found. Using" << LocaleList.value(loc) << "as our locale.";
		*/
		/*
		Use for Debugging, or if we want to notify people that there's only 1 locale.

		QMessageBox msg;
		msg.setIcon(QMessageBox::Information);
		msg.setWindowTitle(QObject::tr("World of Warcraft Locale"));
		msg.setText(QObject::tr("Could only find one installed Locale: %1").arg(LocaleList.value(loc)));
		msg.exec();
		*/
		/*
	}
	g_WMV->updateStatusBar(tr("Locale data gathered. Preparing to scan Directory..."));


	st_WoWDir t_dir = ScanWoWDir(dDir,loc,WoWDirList.count());
	if (t_dir.Locale == LOCALE_NONE){
		QLOG_ERROR() << "An error occured while scanning the WoW Directory.";
		g_WMV->updateStatusBar(tr("Error scanning WoW directory."));
		err.setWindowTitle(tr("Error Scanning WoW Directory"));
		err.setText(tr("An error occured while trying to scan your WoW directory for files.\n\nPlease double-check the directory, and try again."));
		err.exec();
		return;
	}
	QLOG_INFO() << "Sucessfully scanned the WoW Directory. Asking for the Directory's name...";
	g_WMV->updateStatusBar(tr("WoW Directory Scanned. Choosing Directory Name..."));

	QInputDialog n;
	g_WMV->updateStatusBar(tr("Setting the Directory's name..."));
	n.setWindowTitle(tr("Set Directory Name"));
	n.setLabelText(tr("Please choose a name for this Directory."));
	QString name = t_dir.Name;
	if (name == "WoW Folder"){
		name = dDir.dirName();
	}
	n.setTextValue(name);
	n.exec();

	t_dir.Name = n.textValue();

	// Save the Directory
	g_WMV->updateStatusBar(tr("Saving the Directory's information..."));
	saveDir(t_dir);

	// Debug Message
	*/
	/*
	QMessageBox msg;
	msg.setText(tr("Found the World of Warcraft Application!\nInstall Name: %3\nDir Result: %1\nChosen Locale: %2").arg(t_dir.Directory.absolutePath(),LocaleList.value(t_dir.Locale),t_dir.Name));
	msg.exec();
	*/
	/*
	g_WMV->updateStatusBar(tr("Successfully gathered Directory data!"));
	*/
}

// Edit the currently selected directory.
void WoWDirManager::on_WDM_bDirEdit_clicked(){
	Window_WDMEdit.init();
	Window_WDMEdit.show();
}

// Make the Currently selected directory the Current Directory.
void WoWDirManager::on_WDM_bDirMakeCurrent_clicked(){
	/*
	QLOG_INFO() << "Attempting to set Current WoW Dir...";
	QString dirname = "None";
	QString olddir = sWMVSettings.value("CurrentWoWDir").toString();

	for (size_t i=0;i<List->count();i++){
		QListWidgetItem *a = List->item((int)i);
		if (a->isSelected() == true){
			dirname = a->data(Qt::UserRole).toString();
			break;
		}
	}
	st_WoWDir b(WoWDirList.value(dirname));

	QLOG_INFO() << "Setting Current Directory...";
	g_WMV->CurrentDir = WoWDirList.value(dirname);
	sWMVSettings.setValue("CurrentWoWDir",dirname);
	sWMVSettings.sync();

	g_WMV->WoWTypeNext = b.Version;
	g_WMV->canReloadWoW = true;
	
	QLOG_INFO() << "Updating WoWDir List...";
	UpdateList();
*/
}
void WoWDirManager::ListOffset(int offset){
/*
	int num = -1;
	for (size_t i=0;i<List->count();i++){
		QListWidgetItem *a = List->item((int)i);
		if (a->isSelected() == true){
			num = WoWDirList.value(a->data(Qt::UserRole).toString()).Position;
			break;
		}
	}
	if ((num == -1)||(num+offset<0)||(num+offset>List->count()-1))
		return;

	ReOrderList<unsigned int,int>(num, offset);
	List->item(num+offset)->setSelected(true);
	UpdateList();
	*/
}

// Move directory up the list
void WoWDirManager::on_WDM_bDirUp_clicked(){
	QLOG_INFO() << "Moving Directory Up 1 Slot...";
	ListOffset(-1);
}

// Move directory down the list
void WoWDirManager::on_WDM_bDirDown_clicked(){
	QLOG_INFO() << "Moving Directory Down 1 Slot...";
	ListOffset(1);
}

// Called when someone clicks the "Delete All" button
void WoWDirManager::on_WDM_bDirDeleteAll_clicked(){
	/*
	QLOG_INFO() << "User clicked Delete All button. Asking for confirmation...";
	QMessageBox daConfirm;
	daConfirm.setModal(true);
	daConfirm.setWindowTitle(tr("Delete All Confirmation"));
	daConfirm.setText(tr("Are you sure you wish to delete all of your WoW Directories? If you do, you will need to add a new directory for WMV to work.\n\nNOTE: This will not delete the directories from your Hard Drive."));
	daConfirm.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
	daConfirm.setDefaultButton(QMessageBox::No);

	int choice = daConfirm.exec();;
	QLOG_INFO() << "Confirmation result:" << (choice==QMessageBox::Yes?"Yes":"No");

	if (choice == QMessageBox::Yes){
		QLOG_INFO() << "User has chosen to Delete All their WoW Directories. Deleting...";
		WoWDirList.clear();
		sWoWDirs.clear();
		g_WMV->CurrentDir = st_WoWDir();
		sWMVSettings.setValue("CurrentWoWDir","None");
		sWMVSettings.sync();
		g_WMV->UnLoadWoW();
		UpdateList();
		SettingsList.insert("WDMLastDir","None");
		QLOG_INFO() << "Finished deleting all WoW Directories.";
		return;
	}
	QLOG_INFO() << "User has chosen to cancel the Delete All function.";
	*/
}

// Prepare the List
void WoWDirManager::SetupList()
{
	// Copy settings to our local list
	List = ui_WoWDirManager->WDM_List;
	
	// Change various settings to fit our needs
	
	// Re-Apply the settings to the visible list.
	ui_WoWDirManager->WDM_List = List;
}

// Update the List with our items!
void WoWDirManager::UpdateList()
{
	/*
	ReadWoWDirList();
	QLOG_INFO() << "Updating WDM List...";
	// Grab selected item
	int selection = 0;	// default to the first item
	for (size_t i=0;i<List->count();i++){
		QListWidgetItem *selected = List->item((int)i);
		if (selected->isSelected() == true){
			selection = (int)i;
			break;
		}
	}
	List->clear();	// Clear the list of all old data, as we'll be regenerating it all...
	QStringList groups = sWoWDirs.childGroups();
	QMap<int,st_WoWDir> sortlist;

	for (size_t i=0;i<groups.count();i++){
		st_WoWDir a = WoWDirList.value(groups.value((int)i),st_WoWDir());
		//QLOG_TRACE() << "Discovered" << a.Name << "locale:" << LocaleList.value(a.Locale) << "of" << groups.value((int)i);
		if (a == st_WoWDir())
			continue;
		sortlist.insertMulti(a.Position,a);	// Used Multi just in case. We don't want to overwrite directories with the same position number.
	}

	QLOG_INFO() << "Sorting WDM List...";
	for (QMap<int,st_WoWDir>::Iterator i=sortlist.begin();i!=sortlist.end();i++){
		st_WoWDir a = i.value();
		a.Position = i.key();
		QListWidgetItem *newItem = new QListWidgetItem;
		QString loc = LocaleList.value(a.Locale);

		// Catch-all/Default Icon. Should always be the latest release's icon.
		newItem->setIcon(iconCata);

		if (a.Version <= WOW_VANILLA){
			newItem->setIcon(iconVanilla);
		}else if (a.Version <= WOW_TBC){
			newItem->setIcon(iconTBC);
		}else if (a.Version <= WOW_WOTLK){
			newItem->setIcon(iconWotLK);
		}else if (a.Version <= WOW_CATACLYSM){
			newItem->setIcon(iconCata);
		}else if ((a.Version <= WOW_BETA) || (a.Version == WOW_PTR)){
			newItem->setIcon(iconPTR);
		// Unreleased Expanions come AFTER the Beta and PTR.
		}else if (a.Version <= WOW_MOP){
			newItem->setIcon(iconMoP);
		}
		newItem->setData(Qt::UserRole,WoWDirGroupName(a));
		QString ver = QString::number(a.Version);
		if (a.Version == WOW_PTR){
			ver = tr("PTR");
		}else if (a.Version == WOW_BETA){
			ver = tr("Beta");
		}

		//QLOG_TRACE() << "Setting text for Directory...";
		newItem->setText(QString("%1, %2, %3").arg(a.Name,ver,loc));
		if (a == g_WMV->CurrentDir){
			newItem->setText(QString("%1, %2, %3, %4").arg(a.Name,ver,loc,tr("(Current)")));
		}
		List->insertItem(a.Position, newItem);
	}

	List->item(selection)->setSelected(true);

	// Apply the new data
	ui_WoWDirManager->WDM_List = List;
	QLOG_INFO() << "Finished Updating WDM List!";
	g_WMV->UpdateViewerMenu();
	*/
}

// Scan the passed directory and return a WoWDir.
st_WoWDir ScanWoWDir(QDir dir, int locale, int position = 0){
	if (dir.exists() == false){
		QLOG_ERROR() << "Specified directory does not exist. Stopping ScanWoWDir...";
		return st_WoWDir();
	}
	
	QLOG_INFO() << "Scanning WoW Directory:" << dir.absolutePath();
	//g_WMV->updateStatusBar(QObject::tr("Scanning WoW Directory..."));
	st_WoWDir wdir(dir,locale);
	/*
	wdir.Position = position;
	QString sloc = LocaleList.value(locale);

	QMessageBox err;
	err.setIcon(QMessageBox::Critical);

	// Check for realmlist.wtf file. Used to detect PTR.
	// Only needed for latest version that's hidden in the Locale folder.
	QLOG_INFO() << "Checking for PTR RealmList file...";
	QDir ldir(dir);
	ldir.cd("Data/"+sloc);
	if (ldir.exists() == true){
		QFile r(ldir.absolutePath().append("/realmlist.wtf"));
		if (r.exists() == true){
			if (r.open(QFile::ReadOnly)){
				r.seek(0);		// Make sure we're at the start of the file.
				QString rtxt = r.readLine();
				if (rtxt.contains("public-test") == true) {
					QLOG_INFO() << "Found RealmList file. Contents indicate that this Directory is for a PTR version of WoW.";
					wdir.Version = WOW_PTR;
					wdir.Name = QObject::tr("Public Test Realm","PTR Folder Default Name");
					MPQList_Get(wdir);
					g_WMV->updateStatusBar(QObject::tr("Scanning Complete. PTR Version Found!"));
					return wdir;
				}
			}
		}
	}
	QLOG_INFO() << "This Directory appears to not belong to a PTR version of WoW.";
	
	// If we've gotten here, it's either not PTR, or we can't detect that it is.
	// So, now we should figure out what version of the game we're running.

	// Scan for the inteface's MPQ file, and any patch files for it.
	QStringList iface = QStringList();
	QDir idir(dir);
	idir.cd("Data");
	if (QFile(idir.path().append("/interface.MPQ")).exists()){
		// Found Vanilla WoW
		QLOG_INFO() << "Checking Vanilla Directory for Interface MPQ files...";
		iface.push_back("interface.MPQ");
		if (QFile(idir.path().append("/patch.MPQ")).exists())
			iface.push_back("patch.MPQ");
		if (QFile(idir.path().append("/patch-2.MPQ")).exists())
			iface.push_back("patch-2.MPQ");
	}else if (QFile(ldir.path().append("/locale-"+sloc+".MPQ")).exists()){
		// Found a different WoW
		QLOG_INFO() << "Checking Directory for Interface MPQ files...";
		iface.push_back(QString(sloc+"/locale-"+sloc+".MPQ"));
		QDir patch(ldir);
		patch.setFilter(QDir::Files);
		patch.setSorting(QDir::Name | QDir::IgnoreCase);
		QStringList f;
		f << "*-locale-????.MPQ";					// Burning Crusade & WotLK Locale files
		f << "expansion?-locale-????.MPQ";			// Cataclysm Locale files
		patch.setNameFilters(f);
		for (size_t i=0;i<patch.count();i++){
			iface.push_back(sloc+"/"+patch.entryList().value((int)i));
		}
		// patch-locale.MPQ comes AFTER patch-locale-2.MPQ alphabetically in QT, so manually add it first.
		if (QFile(ldir.path().append("/patch-"+sloc+".MPQ")).exists())
			iface.push_back(sloc+"/patch-"+sloc+".MPQ");
		f.clear();
		f << "patch-????-?.MPQ";					// Burning Crusade & WotLK Patch files
		f << "wow-update-????-?????.MPQ";			// Cataclysm Patch files
		patch.setNameFilters(f);
		for (size_t i=0;i<patch.count();i++){
			iface.push_back(sloc+"/"+patch.entryList().value((int)i));
		}
		// Cache Locale files
		patch = idir;
		patch.cd("Cache/"+sloc);
		f.clear();
		f << "patch-????-?????.MPQ";				// Cataclysm Cache Patch files
		patch.setNameFilters(f);
		for (size_t i=0;i<patch.count();i++){
			iface.push_back("Cache/"+sloc+"/"+patch.entryList().value((int)i));
		}
	}

	// Fail if there's not any interface files.
	if (iface.count() <= 0){
		QLOG_ERROR() << "No interface MPQ files found. Returning blank WoWDir...";
		err.setWindowTitle(QObject::tr("No interface MPQs found."));
		err.setText(QObject::tr("No interface MPQ files found.\n\nPlease check your WoW Directory and try again."));
		err.exec();
		return st_WoWDir();
	}

	// Log the Interface MPQ List.
	QLOG_TRACE() << "Interface MPQ List:";
	for (size_t i=0;i<iface.count();i++){
		QLOG_TRACE() << "   " << static_cast<const char*>(iface.value((int)i).toUtf8());
	}

	// Initialize our limited MPQ list, so we can extract the TOC file
	t_ArchiveSet tocarch;
	TempArchiveList.clear();	// Clear the Temp Archives, as we only needed them for patching.
	QLOG_INFO() << "Initializing the Interface MPQ files...";
	for (ssize_t i=iface.count()-1;i>=0;i--){
		QString a = dir.absolutePath().append("/Data/").append(iface.value((int)i));
		QLOG_TRACE() << "Processing" << a << "...";
		if (QFile(a).exists()){
			if (QFile(a+".lock").exists()){
				QLOG_ERROR() << "MPQ file is Locked. Returning empty WoWDir...";
				g_WMV->updateStatusBar(QObject::tr("Error: MPQs files are locked and cannot be accessed. Please close World of Warcraft, then try again."));
				err.setWindowTitle(QObject::tr("Error: Unable to open MPQ file!"));
				err.setText(QObject::tr("Could not access MPQ file, because it is in use.\n\nPlease close World of Warcraft, then try again."));
				err.exec();
				return st_WoWDir();
			}else{
				QLOG_TRACE() << "Opening the MPQ archive...";
				g_WMV->updateStatusBar(QObject::tr("Adding %1 to the Inteface MPQ List...").arg(a));
				MPQArchive *mpq = new MPQArchive(a,iface);
				if (!mpq->OK)
					continue;
				tocarch.push_back(cMPQArchHandle(mpq->FileInfo,mpq->getHANDLE(),mpq->isPatchFile));
			}
		}
	}
	QLOG_INFO() << "Sucessfully Initialized the Interface MPQ files! Attemping to open TOC file...";

	if (tocarch.count()<=0){
		QLOG_ERROR() << "No Archives loaded.";
		g_WMV->updateStatusBar(QObject::tr("Error: No MPQs files loaded."));
		err.setWindowTitle(QObject::tr("Error: No MPQs files have been loaded!"));
		err.setText(QObject::tr("An error occured that resulted in no MPQ files being loaded.\n\nPlease close World of Warcraft, then try again."));
		err.exec();
		return st_WoWDir();
	}

	QLOG_TRACE() << "TOCArch list";
	for (size_t i=0;i<tocarch.count();i++){
		QLOG_TRACE() << "   " << tocarch.value((int)i).Info.absoluteFilePath() << "=" << (unsigned int)tocarch.value((int)i).Handle;
	}

	// Extract file from MPQ archives
	g_WMV->updateStatusBar(QObject::tr("Attempting to find TOC file..."));
	MPQFile mf(QString("Interface%1FrameXML%1FrameXML.toc").arg(SLASH_MPQ),tocarch);

	// If TOC file isn't found in the MPQ...
	QLOG_INFO() << "Checking if TOC file exists...";
	bool ex = mf.exists();
	QLOG_TRACE() << "File Exists: "<< (ex?"true":"false");
	if (ex==false){
		QLOG_ERROR() << "TOC File not found in MPQs.";
		mf.close();
		g_WMV->updateStatusBar(QObject::tr("Error trying to find the TOC file in the interface MPQs."));
		err.setText(QObject::tr("Error finding the TOC file in the MPQ Archives.\n\nPlease make sure all downloads/updates have finished, then try again."));
		err.setWindowTitle(QObject::tr("Error Finding TOC File."));
		err.exec();
		return st_WoWDir();
	}

	// Check validity of TOC File.
	QLOG_INFO() << "Checking Validity of TOC file...";
	if (mf.isEof()){
		QLOG_ERROR() << "Error reading TOC File.";
		mf.close();
		g_WMV->updateStatusBar(QObject::tr("Error reading the TOC file from the interface MPQs."));
		err.setText(QObject::tr("Error opening the TOC file from the MPQ Archives.\n\nIf this problem persists, try repairing or reinstalling WoW."));
		err.setWindowTitle(QObject::tr("Error Opening TOC File."));
		err.exec();
		return st_WoWDir();
	}
	
	QLOG_INFO() << "Found TOC file! Getting TOC Number...";
	g_WMV->updateStatusBar(QObject::tr("TOC file Found! Getting TOC number..."));

	// Get the TOC Version Number from the File
	mf.seek(51); // offset to "## Interface: "
	unsigned char toc[6];
	memset(toc,'\0', 6);
	mf.read(toc,5);
	QString tocnum = QString((char *)toc);
	size_t inum = tocnum.toUInt();

	mf.close();

	if (inum == 0){
		QLOG_ERROR() << "TOC version was returned as 0...";
		mf.close();
		g_WMV->updateStatusBar(QObject::tr("Error gathering TOC version. Value was returned as 0."));
		err.setText(QObject::tr("Error gathering the TOC version.\n\nIf this problem persists, try repairing or reinstalling WoW."));
		err.setWindowTitle(QObject::tr("Error gathering TOC vrsion."));
		err.exec();
		return st_WoWDir();
	}

	QLOG_INFO() << "TOC version:" << tocnum.toInt();
	g_WMV->updateStatusBar(QObject::tr("TOC number gathered. Setting version..."));

	// Set Version Number
	wdir.Version = (int)inum;
	if (inum <= WOW_VANILLA ){
		QLOG_INFO() << "TOC indicates that this is a Vanilla WoW. Setting Directory name...";
		wdir.Name = QObject::tr("Vanilla Directory","Vanilla Folder Default Name");
	}else if (inum <= WOW_TBC ){
		QLOG_INFO() << "TOC indicates that this is a Burning Crusade WoW. Setting Directory name...";
		wdir.Name = QObject::tr("Burning Crusade Directory","Burning Crusade Folder Default Name");
	}else if (inum <= WOW_WOTLK ){
		QLOG_INFO() << "TOC indicates that this is a Wrath of the Lich King WoW. Setting Directory name...";
		wdir.Name = QObject::tr("Wrath of the Lich King Directory","WotLK Folder Default Name");
	}else if (inum <= WOW_CATACLYSM ){
		QLOG_INFO() << "TOC indicates that this is a Cataclysm WoW. Setting Directory name...";
		wdir.Name = QObject::tr("Cataclysm Directory","Burning Crusade Folder Default Name");
	}else if (inum <= WOW_BETA ){
		QLOG_INFO() << "TOC indicates that this is a Beta WoW. Setting Directory name...";
		wdir.Name = QObject::tr("Beta Directory","Beta Folder Default Name");
	}else if (inum <= WOW_MOP ){
		QLOG_INFO() << "TOC indicates that this is a Mists of Pandaria WoW. Setting Directory name...";
		wdir.Name = QObject::tr("Mists of Pandaria Directory","Mists of Pandaria Folder Default Name");
	}
	MPQList_Get(wdir);

	g_WMV->updateStatusBar(QObject::tr("Scanning Complete. TOC Version Found: %1").arg(inum));
	
	*/
	QLOG_INFO() << "WoW Scan completed successfully! Returning WoW Directory...";
	return wdir;
}

// Re-orders the WoW Directory List, moving the current directory (in_pos) up/down by offset.
template <class UnReal, class Real>
void WoWDirManager::ReOrderList(UnReal in_pos, Real offset)
{
	/*
	// Check and fix out-of-bounds issues.
	if ((in_pos == 0) && (offset<0))
		offset = 0;
	if (offset > (WoWDirList.count()-1)-(Real)in_pos)
		offset = (WoWDirList.count()-1)-(Real)in_pos;

	// If there is no offset, finish.
	if (offset == 0)
		return;

	QLOG_INFO() << "Reordering Directory List...";

	bool isNeg = false;
	if (offset<0)
		isNeg = true;
	
	QMap<int,st_WoWDir> reorder;
	QStringList groups = sWoWDirs.childGroups();

	for (size_t i=0;i<groups.count();i++){
		st_WoWDir a = WoWDirList.value(groups.value((int)i),st_WoWDir());
		if (a == st_WoWDir())
			continue;

		int pos = a.Position;
		int npos = pos;

		if (pos == in_pos){
			npos = pos+offset;
			QLOG_TRACE() << "Changing" << a.Name << "position from" << pos << "to" << npos;
		}else{
			if (offset>0){	// Positive offset
				if ((pos>(int)in_pos)&&(pos<=(int)(in_pos+offset))){
					npos = pos-1;
					QLOG_TRACE() << "Changing" << a.Name << "position from" << pos << "to" << npos;
				}else{
					QLOG_TRACE() << "Keeping" << a.Name << "position the same at" << npos;
				}
			}else{			// Negative offset
				if ((pos<(int)in_pos)&&(pos>=(int)(in_pos+offset))){
					npos = pos+1;
					QLOG_TRACE() << "Changing" << a.Name << "position from" << pos << "to" << npos;
				}else{
					QLOG_TRACE() << "Keeping" << a.Name << "position the same at" << npos;
				}
			}
		}
		a.Position = npos;
		reorder.insertMulti(npos,a);	// Used Multi just in case. We don't want to overwrite directories with the same position number.
	}

	// Save the new Directories
	for (size_t i=0;i<reorder.size();i++){
		st_WoWDir a = reorder.value((int)i);
		saveDir(a, false);
	}
	UpdateList();
	QLOG_INFO() << "Finished re-ordering list.";
	*/
}

// --= WoW Directory Manager Directory Editor =--

WDM_Edit::WDM_Edit(QWidget *parent) : QDialog(parent), ui_WDM_Edit(new Ui::WDM_Edit)
{
	ui_WDM_Edit->setupUi(this);
}

WDM_Edit::~WDM_Edit()
{
	delete ui_WDM_Edit;
}

void WDM_Edit::init()
{

}

void WDM_Edit::on_btn_Okay_clicked()
{
	// Save the data

	// Close the Window
	QDialog::close();
}