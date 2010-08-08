#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_m3.h"
#include "modelcanvas.h"

void padding(wxFFile *f, int pads=16)
{
	char pad=0xAA;
	if (f->Tell()%pads != 0) {
		int j=pads-(f->Tell()%pads);
		for(int i=0; i<j; i++) {
			f->Write(&pad, sizeof(pad));
		}
	}
}

void ExportM2toM3(Model *m, const char *fn, bool init)
{
	if (!m)
		return;

	wxFFile f(fn, wxT("w+b"));

	if (!f.IsOpened()) { // IsOk
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("M3"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));

	// 1. FileHead
	ReferenceEntry htag;
	strcpy(htag.id, "43DM");
	htag.offset = f.Tell();;
	htag.nEntries = 1;
	htag.vers = 0xB;

	struct MD34 fHead;
	memset(&fHead, 0, sizeof(fHead));
	strcpy(fHead.id, "43DM");
	fHead.nRefs ++;
	f.Write(&fHead, sizeof(fHead));

	// 2. ModelHead
	ReferenceEntry mtag;
	strcpy(mtag.id, "LDOM");
	mtag.offset = f.Tell(); // offset to MODL
	mtag.nEntries = 1;
	mtag.vers = 0x17;
	fHead.nRefs ++;

	struct MODL mdata;
	memset(&mdata, 0, sizeof(mdata));
	f.Write(&mdata, sizeof(mdata));

	// 3. Content
	// Modelname
	ReferenceEntry Modelname;
	strcpy(Modelname.id, "RAHC");
	Modelname.offset = f.Tell();
	Modelname.nEntries = strlen(fn);
	Modelname.vers = 0;
	f.Write(fn, strlen(fn));
	char end=0;
	char pad=0xAA;
	f.Write(&end, sizeof(end));
	padding(&f);
	mdata.Modelname.nEntries = strlen(fn);
	mdata.Modelname.ref = fHead.nRefs;
	fHead.nRefs ++;

	mdata.type = 0x180d53;

	// 4. ReferenceEntry
	fHead.ofsRefs = f.Tell();

	f.Write(&htag, sizeof(htag));
	f.Write(&mtag, sizeof(mtag));
	f.Write(&Modelname, sizeof(Modelname));

	// 5. rewrite head
	f.Seek(0, wxFromStart);
	f.Write(&fHead, sizeof(fHead));
	f.Write(&mdata, sizeof(mdata));

	f.Close();
}
