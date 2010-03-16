#ifndef UTIL_H
#define UTIL_H

// STL headers
#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif
#include <algorithm>
//#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <set>
#include <sstream>
#include <vector>

// Standard C++ headers
#include <stdio.h>

// wx
#include <wx/filename.h>
#include <wx/fileconf.h>
#include <wx/string.h>
#include <wx/log.h>

// Our other utility headers
#include "vec3d.h"

using namespace std;

extern wxString gamePath;
extern wxString cfgPath;
extern wxString bgImagePath;

extern bool useLocalFiles;
extern bool useRandomLooks;
extern bool bHideHelmet;
extern bool bKnightEyeGlow;
extern bool bV310;
extern bool bShowParticle;
extern bool modelExportInitOnly;
extern bool modelExport_PreserveDir;
extern bool modelExport_LW_PreserveDir;
extern bool modelExport_LW_ExportLights;
extern bool modelExport_LW_ExportDoodads;
extern int modelExport_LW_DoodadsAs;
extern bool modelExport_X3D_ExportAnimation;
extern bool modelExport_X3D_CenterModel;

//extern bool hasBeta;

extern wxArrayString mpqArchives;

extern long langID;
extern long interfaceID;
extern int ssCounter;
extern int imgFormat;
extern long versionID;

#ifdef _WINDOWS
	typedef unsigned char uint8;
	typedef char int8;
	typedef unsigned __int16 uint16;
	typedef __int16 int16;
	typedef unsigned __int32 uint32;
	typedef __int32 int32;
#else
	#include <stdint.h>
	typedef uint8_t uint8;
	typedef int8_t int8;
	typedef uint16_t uint16;
	typedef int16_t int16;
	typedef uint32_t uint32;
	typedef int32_t int32;
#endif

float frand();
float randfloat(float lower, float upper);
int randint(int lower, int upper);

template <class T>
bool from_string(T& t, const string& s, ios_base& (*f)(ios_base&))
{
  istringstream iss(s);
  return !(iss >> f >> t).fail();
}

wxString CSConv(wxString str);
void fixname(std::string &name);
void fixnamen(char *name, uint32 len);
wxString Vec3DToString(Vec3D vec);
int wxStringToInt(const wxString& str);
float round(float input, int limit);
void MakeDirs(wxString base, wxString paths);

void getGamePath();

#endif

