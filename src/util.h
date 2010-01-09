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
extern bool modelExport_PreserveLWDir;
extern bool modelExport_LW_ExportLights;
extern bool modelExport_LW_ExportDoodads;
extern int modelExport_LW_DoodadsAs;

//extern bool hasBeta;

extern wxArrayString mpqArchives;

extern long langID;
extern long interfaceID;
extern int ssCounter;
extern int imgFormat;
extern long versionID;

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
void fixnamen(char *name, size_t len);
wxString Vec3DToString(Vec3D vec);
int wxStringToInt(const wxString& str);
float round(float input, int limit);
void MakeDirs(wxString base, wxString paths);

void getGamePath();

#endif

