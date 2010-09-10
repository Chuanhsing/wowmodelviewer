#include "util.h"
//#include "CxImage/ximage.h"
#ifdef _WINDOWS
#include <windows.h>
#include <wx/msw/winundef.h>
#endif
#include <wx/choicdlg.h>
#include <wx/dir.h>
#include <wx/dirdlg.h>

#include "UserSkins.h"

wxString gamePath;
wxString cfgPath;
wxString bgImagePath;

wxArrayString mpqArchives;

UserSkins userSkins;
UserSkins& gUserSkins = userSkins;;

int gameVersion = 0;

bool useLocalFiles = false;
bool useRandomLooks = true;
bool bHideHelmet = false;
bool bKnightEyeGlow = true;
bool bShowParticle = true;
bool bZeroParticle = true;
bool bAlternate = false; // for zhCN alternate.MPQ

// Model Export Options
// General Options
int Perfered_Exporter = -1; // Defaults to None
bool modelExportInitOnly = true;
bool modelExport_PreserveDir = true;
bool modelExport_UseWMVPosRot = false;

// Lightwave Options
bool modelExport_LW_PreserveDir = true;
bool modelExport_LW_ExportLights = true;
bool modelExport_LW_ExportDoodads = true;
int modelExport_LW_DoodadsAs = 0;
// X3D Options
bool modelExport_X3D_ExportAnimation = false;
bool modelExport_X3D_CenterModel = true;
// M3 Options
float modelExport_M3_BoundScale = 0.5f;
float modelExport_M3_SphereScale = 0.5f;
wxString modelExport_M3_TexturePath = _T("");
std::vector<uint32> modelExport_M3_Anims;
wxArrayString modelExport_M3_AnimNames;

long langID = -1;
long langOffset = -1;
long interfaceID = -1;
int ssCounter = 100; // ScreenShot Counter
int imgFormat = 0;

wxString locales[] = {_T("enUS"), _T("koKR"), _T("frFR"), _T("deDE"), _T("zhCN"),  _T("zhTW"),  _T("esES"),  _T("ruRU")};

/*
wxString langCSConv[] =
{
	_T("iso-8859-1"),
	wxEmptyString,
	wxEmptyString,
	wxEmptyString,
	_T("gb2312"),
	_T("big5"),
	wxEmptyString,
	wxEmptyString,
};
wxString CSConvStr;
*/

// Slash correction, just in case.
wxString fixMPQPath(wxString path)
{
#ifdef	_WINDOWS
    return path;
#else
    wxString str = path;
    str.Replace(wxString(MPQ_SLASH), wxString(SLASH));
    return str;
#endif
}

// Convert UTF8 string to local string
wxString CSConv(wxString str)
{
	if (langID <= 0) // || langCSConv[langID].IsEmpty())
		return str;
	return wxConvLocal.cWC2WX(wxConvUTF8.cMB2WC(str.mb_str())); // from private.h
	// old way
	//return wxCSConv(langCSConv[langID]).cWC2WX(wxConvUTF8.cMB2WC(str));
}

float frand()
{
    return rand()/(float)RAND_MAX;
}

float randfloat(float lower, float upper)
{
	return lower + (upper-lower)*(rand()/(float)RAND_MAX);
}

int randint(int lower, int upper)
{
    return lower + (int)((upper+1-lower)*frand());
}

void fixname(wxString &name)
{
	for (uint32 i=0; i<name.length(); i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}
void fixnamen(char *name, uint32 len)
{
	for (uint32 i=0; i<len; i++) {
		if (i>0 && name[i]>='A' && name[i]<='Z' && isalpha(name[i-1])) {
			name[i] |= 0x20;
		} else if ((i==0 || !isalpha(name[i-1])) && name[i]>='a' && name[i]<='z') {
			name[i] &= ~0x20;
		}
	}
}

// Byteswap for 2 Bytes
unsigned short _SwapTwoBytes (unsigned short w)
{
	unsigned short tmp;
	tmp =  (w & 0x00ff);
	tmp = ((w & 0xff00) >> 0x08) | (tmp << 0x08);
	return tmp;
}

// Round a float, down to the specified decimal
float round(float input, int limit = 2){
	if (limit > 0){
		input *= (10^limit);
	}
	input = int(input+0.5);
	if (limit > 0){
		input /= (10^limit);
	}
	return input;
}

void MakeDirs(wxString PathBase, wxString ExtPaths){
	wxString NewBase = PathBase;
	//wxLogMessage("MKDIR Paths\nBasePath: %s\nOthers Paths: %s", PathBase, ExtPaths);
	wxString Paths[30];
	unsigned int PathNum = 0;
	while (ExtPaths.Find(SLASH)>0){
		Paths[PathNum] = ExtPaths.BeforeFirst(SLASH);
		wxString rep = Paths[PathNum]+SLASH;
		ExtPaths.Replace(rep, wxEmptyString, true);
		//wxLogMessage("\nBuilding Paths: %s\npaths:%s",Paths[PathNum],ExtPaths);
		PathNum++;
	}
	Paths[PathNum] = ExtPaths;
	PathNum++;

	for (unsigned int x=0;x<PathNum;x++){
		NewBase = wxString(NewBase << SLASH << Paths[x]);
		if (wxDirExists(NewBase) == false){
			//wxLogMessage("Attempting to create the following directory: %s",NewBase);
			wxMkdir(NewBase);
		}
	}
}

void getGamePath()
{
#ifdef _WINDOWS
	HKEY key;
	unsigned long t, s;
	long l;
	unsigned char path[1024];
	memset(path, 0, sizeof(path));

	wxArrayString sNames;
	gamePath = wxEmptyString;

	// if it failed, look for World of Warcraft install
	const wxString regpaths[] = {
		// _WIN64
		_T("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft"),
		_T("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft\\PTR"),
		_T("SOFTWARE\\Wow6432Node\\Blizzard Entertainment\\World of Warcraft\\Beta")
		//_WIN32, but for compatible
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft"),
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\PTR"),
		_T("SOFTWARE\\Blizzard Entertainment\\World of Warcraft\\Beta")
	};

	for (uint32 i=0; i<WXSIZEOF(regpaths); i++) {
		l = RegOpenKeyEx((HKEY)HKEY_LOCAL_MACHINE, regpaths[i], 0, KEY_QUERY_VALUE, &key);

		if (l == ERROR_SUCCESS) {
			s = sizeof(path);
			l = RegQueryValueEx(key, _T("InstallPath"), 0, &t,(LPBYTE)path, &s);
			if (l == ERROR_SUCCESS && wxDir::Exists(path)) {
				sNames.Add(path);
			}
			RegCloseKey(key);
		}
	}

	if (sNames.size() == 1)
		gamePath = sNames[0];
	else if (sNames.size() > 1)
		gamePath = wxGetSingleChoice(_T("Please select a Path:"), _T("Path"), sNames);

	// If we found an install then set the game path, otherwise just set to C:\ for now
	if (gamePath == wxEmptyString) {
		gamePath = _T("C:\\Program Files\\World of Warcraft\\");
		if (!wxFileExists(gamePath+_T("Wow.exe"))){
			gamePath = wxDirSelector(wxT("Please select your World of Warcraft folder:"), gamePath);
		}
	}
	if (gamePath.Last() != SLASH)
		gamePath.Append(SLASH);
	gamePath.Append(_T("Data\\"));
#elif _MAC // Mac OS X
    gamePath = wxT("/Applications/World of Warcraft/");
	if (!wxFileExists(gamePath+_T("Data/common.MPQ")) && !wxFileExists(gamePath+_T("Data/art.MPQ")) ){
        gamePath = wxDirSelector(wxT("Please select your World of Warcraft folder:"), gamePath);
    }
	if (gamePath.Last() != SLASH)
		gamePath.Append(SLASH);
	gamePath.Append(_T("Data/"));
#else // Linux
	gamePath = _T(".")+SLASH;
	if (!wxFileExists(gamePath+_T("Wow.exe"))){
		gamePath = wxDirSelector(wxT("Please select your World of Warcraft folder:"), gamePath);
	}
	if (gamePath.Last() != SLASH)
		gamePath.Append(SLASH);
	gamePath.Append(_T("Data/"));
#endif
}
