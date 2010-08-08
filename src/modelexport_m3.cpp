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
	fHead.mref.nEntries = 1;
	fHead.mref.ref = ++fHead.nRefs;
	memset(&fHead.padding, 0xAA, sizeof(fHead.padding));
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
	f.Write(&end, sizeof(end));
	padding(&f);
	mdata.Modelname.nEntries = Modelname.nEntries;
	mdata.Modelname.ref = fHead.nRefs++;

	mdata.type = 0x180d53;
	
	ReferenceEntry mSEQS;
	strcpy(mSEQS.id, "SQES");
	mSEQS.offset = f.Tell();
	mSEQS.nEntries = m->header.nAnimations;
	mSEQS.vers = 1;
	mdata.mSEQS.nEntries = mSEQS.nEntries;
	mdata.mSEQS.ref = fHead.nRefs++;
	SEQS *seqs = new SEQS[mSEQS.nEntries];
	memset(seqs, 0, sizeof(SEQS)*mSEQS.nEntries);
	ReferenceEntry *reSeqs = new ReferenceEntry[mSEQS.nEntries];
	f.Seek(sizeof(SEQS)*mSEQS.nEntries, wxFromCurrent);
	for(int i=0; i<mSEQS.nEntries; i++) {
		wxString strName;
		try {
			AnimDB::Record rec = animdb.getByAnimID(g_selModel->anims[i].animID);
			strName = rec.getString(AnimDB::Name);
		} catch (AnimDB::NotFound) {
			strName = _T("???");
		}
		seqs[i].name.nEntries = strName.length();
		seqs[i].name.ref = fHead.nRefs++;
		strcpy(reSeqs[i].id, "RAHC");
		reSeqs[i].offset = f.Tell();
		reSeqs[i].nEntries = strName.length();
		reSeqs[i].vers = 0;
		f.Write(strName.c_str(), strName.length());
		f.Write(&end, sizeof(end));
		padding(&f);
	}
	int after_seqs = f.Tell();
	f.Seek(mSEQS.offset, wxFromStart);
	for(int i=0; i<mSEQS.nEntries; i++) {
		seqs[i].d1 = -1;
		seqs[i].d2 = -1;
		seqs[i].d3 = 0;
		seqs[i].length = m->anims[i].timeEnd;
		seqs[i].moveSpeed = m->anims[i].moveSpeed;
		seqs[i].flags = 1;
		f.Write(&seqs[i], sizeof(SEQS));
	}
	wxDELETEA(seqs);
	f.Seek(after_seqs, wxFromStart);

	// 4. ReferenceEntry
	fHead.ofsRefs = f.Tell();

	f.Write(&htag, sizeof(ReferenceEntry));
	f.Write(&mtag, sizeof(ReferenceEntry));
	f.Write(&Modelname, sizeof(ReferenceEntry));
	f.Write(&mSEQS, sizeof(ReferenceEntry));
	for(int i=0; i<mSEQS.nEntries; i++) {
		f.Write(&reSeqs[i], sizeof(ReferenceEntry));
	}

	// 5. rewrite head
	f.Seek(0, wxFromStart);
	f.Write(&fHead, sizeof(fHead));
	f.Write(&mdata, sizeof(mdata));

	f.Close();
}
