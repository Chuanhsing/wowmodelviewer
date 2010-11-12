#ifndef EXPORTERS_H
#define EXPORTERS_H

// Exporter Data
struct Exporter_Type {
	wxString Name;
	uint16 ID;
	wxString MenuText;
	bool canM2;
	bool canWMO;
	bool canADT;

	Exporter_Type(wxString name, uint16 id, wxString menutext, bool can_m2 = false, bool can_wmo = false, bool can_adt = false){
		Name = name;
		ID = id;
		MenuText = menutext;
		canM2 = can_m2;
		canWMO = can_wmo;
		canADT = can_adt;
	}
};

// Exporters
const static int ExporterTypeCount = 10;

// This list should be alphabetical.
const static Exporter_Type Exporter_Types[ExporterTypeCount] = {
	Exporter_Type(_T("3D Studio Max (3DS)"),ID_MODELEXPORT_3DS,_T("3DS..."),true),
	Exporter_Type(_T("Collada"),ID_MODELEXPORT_COLLADA,_T("Collada...")),
#ifdef	_WINDOWS
	Exporter_Type(_T("FBX"),ID_MODELEXPORT_FBX,_T("FBX..."),true),
#else
	Exporter_Type(_T("FBX"),ID_MODELEXPORT_FBX,_T("FBX...")),	// Disabled like Collada until we get it working on non-windows systems.
#endif
#ifdef _DEBUG
	Exporter_Type(_T("Lightwave 3D"),ID_MODELEXPORT_LWO,_T("Lightwave 3D..."),true,true,true),
#else
	Exporter_Type(_T("Lightwave 3D"),ID_MODELEXPORT_LWO,_T("Lightwave 3D..."),true,true),
#endif
	Exporter_Type(_T("M3"),ID_MODELEXPORT_M3,_T("M3..."),true),
	Exporter_Type(_T("Milkshake"),ID_MODELEXPORT_MS3D,_T("Milkshape..."),true),
	Exporter_Type(_T("Ogre XML"),ID_MODELEXPORT_OGRE,_T("Ogre XML..."),true),
	Exporter_Type(_T("Wavefront OBJ"),ID_MODELEXPORT_OBJ,_T("Wavefront OBJ..."),true,true),
	Exporter_Type(_T("X3D"),ID_MODELEXPORT_X3D,_T("X3D..."),true),
	Exporter_Type(_T("X3D in XHTML"),ID_MODELEXPORT_XHTML,_T("X3D in XHTML..."),true),
};


#endif