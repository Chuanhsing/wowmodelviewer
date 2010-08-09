#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_m3.h"
#include "modelcanvas.h"

static std::vector<ReferenceEntry> reList;

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

void RefEntry(const char *id, uint32 offset, uint32 nEntries, uint32 vers)
{
	ReferenceEntry re;
	strncpy(re.id, id, 4);
	re.offset = offset;
	re.nEntries = nEntries;
	re.vers = vers;
	reList.push_back(re);
}

void ExportM2toM3(Model *m, const char *fn, bool init)
{
	if (!m)
		return;

	wxFFile f(wxString(fn, wxConvUTF8), wxT("w+b"));
	reList.clear();

	if (!f.IsOpened()) { // IsOk
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("M3"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));

	// 1. FileHead
	RefEntry("43DM", f.Tell(), 1, 0xB);

	struct MD34 fHead;
	memset(&fHead, 0, sizeof(fHead));
	strcpy(fHead.id, "43DM");
	fHead.mref.nEntries = 1;
	fHead.mref.ref = ++fHead.nRefs;
	memset(&fHead.padding, 0xAA, sizeof(fHead.padding));
	f.Write(&fHead, sizeof(fHead));

	// 2. ModelHead
	RefEntry("LDOM", f.Tell(), 1, 0x17);
	fHead.nRefs ++;

	struct MODL mdata;
	memset(&mdata, 0, sizeof(mdata));
	f.Write(&mdata, sizeof(mdata));

	// 3. Content
	// Modelname
	wxString n(fn, wxConvUTF8);
	n = n.AfterLast('\\').BeforeLast('.');
	n.Append(_T(".max"));

	RefEntry("RAHC", f.Tell(), n.length()+1, 0);
	f.Write(n.c_str(), n.length());
	char end=0;
	f.Write(&end, sizeof(end));
	padding(&f);
	mdata.name.nEntries = n.length()+1;
	mdata.name.ref = fHead.nRefs++;

	mdata.type = 0x180d53;
	
	// mSEQS
	uint32 nAnimations = 0;
	uint32 *logAnimations = new uint32[m->header.nAnimations];
	wxString *nameAnimations = new wxString[m->header.nAnimations];
	int Walks = 0, Stands =0, Attacks = 0, Deaths = 0;
	for(uint32 i=0; i<m->header.nAnimations; i++) {
		wxString strName;
		try {
			AnimDB::Record rec = animdb.getByAnimID(g_selModel->anims[i].animID);
			strName = rec.getString(AnimDB::Name);
		} catch (AnimDB::NotFound) {
			strName = _T("???");
		}
		if (!strName.StartsWith(_T("Walk")) && !strName.StartsWith(_T("Stand")) && 
				!strName.StartsWith(_T("Attack")) && !strName.StartsWith(_T("Death")))
			continue;
		if (strName.StartsWith(_T("StandWound")))
			continue;
		nameAnimations[i] = strName;
		if (strName.StartsWith(_T("Walk"))) {
			if (Walks == 0)
				nameAnimations[nAnimations] = _T("Walk");
			else
				nameAnimations[nAnimations] = wxString::Format(_T("Walk %02d"), Walks);
			Walks ++;
		}
		if (strName.StartsWith(_T("Stand"))) {
			if (Stands == 0)
				nameAnimations[nAnimations] = _T("Stand");
			else
				nameAnimations[nAnimations] = wxString::Format(_T("Stand %02d"), Stands);
			Stands ++;
		}
		if (strName.StartsWith(_T("Attack"))) {
			if (Attacks == 0)
				nameAnimations[nAnimations] = _T("Attack");
			else
				nameAnimations[nAnimations] = wxString::Format(_T("Attack %02d"), Attacks);
			Attacks ++;
		}
		if (strName.StartsWith(_T("Death"))) {
			if (Deaths == 0)
				nameAnimations[nAnimations] = _T("Death");
			else
				nameAnimations[nAnimations] = wxString::Format(_T("Death %02d"), Deaths);
			Deaths ++;
		}
		logAnimations[nAnimations] = i;
		nAnimations++;
	}

	RefEntry("SQES", f.Tell(), nAnimations, 1);
	int seqs_offset = f.Tell();
	mdata.mSEQS.nEntries = nAnimations;
	mdata.mSEQS.ref = fHead.nRefs++;
	SEQS *seqs = new SEQS[nAnimations];
	memset(seqs, 0, sizeof(SEQS)*nAnimations);
	f.Seek(sizeof(SEQS)*nAnimations, wxFromCurrent);

	for(uint32 i=0; i<nAnimations; i++) {
		wxString strName = nameAnimations[i];
		seqs[i].name.nEntries = strName.length()+1;
		seqs[i].name.ref = fHead.nRefs++;
		RefEntry("RAHC", f.Tell(), seqs[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.length());
		f.Write(&end, sizeof(end));
		padding(&f);
	}
	int after_seqs = f.Tell();
	f.Seek(seqs_offset, wxFromStart);
	for(uint32 i=0; i<nAnimations; i++) {
		int j = logAnimations[i];
		seqs[i].d1 = -1;
		seqs[i].d2 = -1;
		seqs[i].length = m->anims[j].timeEnd;
		seqs[i].moveSpeed = m->anims[j].moveSpeed;
		seqs[i].frequency = m->anims[j].playSpeed; //
		seqs[i].ReplayStart = 1;
		seqs[i].d4[0] = 1;
		seqs[i].d4[1] = 0x64;
		seqs[i].boundSphere.radius = m->anims[j].rad; //
		f.Write(&seqs[i], sizeof(SEQS));
	}
	wxDELETEA(seqs);
	f.Seek(after_seqs, wxFromStart);

	// mSTC
	RefEntry("_CTS", f.Tell(), nAnimations, 4);
	int stc_offset = f.Tell();
	mdata.mSTC.nEntries = nAnimations;
	mdata.mSTC.ref = fHead.nRefs++;
	STC *stcs = new STC[nAnimations];
	memset(stcs, 0, sizeof(STC)*nAnimations);
	for(uint32 i=0; i<nAnimations; i++) {
		stcs[i].indSEQ[0] = stcs[i].indSEQ[0] = i;
		f.Write(&stcs[i], sizeof(STC));
	}
	padding(&f);
	for(uint32 i=0; i<nAnimations; i++) {
		// name
		wxString strName = nameAnimations[i];
		strName.Append(_T("_full"));
		stcs[i].name.nEntries = strName.length()+1;
		stcs[i].name.ref = fHead.nRefs++;
		RefEntry("RAHC", f.Tell(), stcs[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.length());
		f.Write(&end, sizeof(end));
		padding(&f);

		// animid
		stcs[i].animid.nEntries = 4;
		stcs[i].animid.ref = fHead.nRefs++;
		RefEntry("_23U", f.Tell(), stcs[i].animid.nEntries, 0);
		uint32 pad = 0;
		for(uint32 j=0; j<stcs[i].animid.nEntries; j++)
			f.Write(&pad, sizeof(pad));
		padding(&f);

		// animindex
		stcs[i].animindex.nEntries = 4;
		stcs[i].animindex.ref = fHead.nRefs++;
		RefEntry("_23U", f.Tell(), stcs[i].animindex.nEntries, 0);
		for(uint32 j=0; j<stcs[i].animindex.nEntries; j++)
			f.Write(&pad, sizeof(pad));
		padding(&f);

		// Events, VEDS
		stcs[i].Events.nEntries = 1;
		stcs[i].Events.ref = fHead.nRefs++;
		RefEntry("VEDS", f.Tell(), stcs[i].Events.nEntries, 0);
		SD sd;
		memset(&sd, 0, sizeof(sd));
		f.Write(&sd, sizeof(sd));

		// Trans, V3DS
		stcs[i].Trans.nEntries = 1;
		stcs[i].Trans.ref = fHead.nRefs++;
		RefEntry("V3DS", f.Tell(), stcs[i].Trans.nEntries, 0);
		f.Write(&sd, sizeof(sd));

		// Rot, Q4DS
		stcs[i].Rot.nEntries = 1;
		stcs[i].Rot.ref = fHead.nRefs++;
		RefEntry("Q4DS", f.Tell(), stcs[i].Rot.nEntries, 0);
		f.Write(&sd, sizeof(sd));
	}
	after_seqs = f.Tell();
	f.Seek(stc_offset, wxFromStart);
	for(uint32 i=0; i<nAnimations; i++) {
		f.Write(&stcs[i], sizeof(STC));
	}
	wxDELETEA(stcs);
	f.Seek(after_seqs, wxFromStart);

	// mSTG
	RefEntry("_GTS", f.Tell(), nAnimations, 0);
	int stg_offset = f.Tell();
	mdata.mSTG.nEntries = nAnimations;
	mdata.mSTG.ref = fHead.nRefs++;
	STG *stgs = new STG[nAnimations];
	memset(stgs, 0, sizeof(STG)*nAnimations);
	for(uint32 i=0; i<nAnimations; i++) {
		f.Write(&stgs[i], sizeof(STG));
	}
	padding(&f);
	for(uint32 i=0; i<nAnimations; i++) {
		// name
		wxString strName = nameAnimations[i];
		stgs[i].name.nEntries = strName.length()+1;
		stgs[i].name.ref = fHead.nRefs++;
		RefEntry("RAHC", f.Tell(), stgs[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.length());
		f.Write(&end, sizeof(end));
		padding(&f);
		
		// stcID
		stgs[i].stcID.nEntries = 1;
		stgs[i].stcID.ref = fHead.nRefs++;
		RefEntry("_23U", f.Tell(), stgs[i].stcID.nEntries, 0);
		f.Write(&i, sizeof(uint32));
		padding(&f);
	}
	after_seqs = f.Tell();
	f.Seek(stg_offset, wxFromStart);
	for(uint32 i=0; i<nAnimations; i++) {
		f.Write(&stgs[i], sizeof(STG));
	}
	wxDELETEA(stgs);
	f.Seek(after_seqs, wxFromStart);

	// mSTS
/*
	RefEntry("_STS", f.Tell(), mSEQS.nEntries, 0);
	mdata.mSTS.nEntries = mSTS.nEntries;
	mdata.mSTS.ref = fHead.nRefs++;

	// mBone
	
	// mVert
	RefEntry("__8U", f.Tell(), 0, 0);
	mdata.mVert.nEntries = mVert.nEntries;
	mdata.mVert.ref = fHead.nRefs++;
*/
	// mDIV
	
	// mBoneLU
	
	// boundSphere
	
	// mAttach
	
	// mAttachLU
	
	// mLite
	
	// mSHBX
	
	// mCam
	
	// mMatLU
	
	// mMat
	
	// mDIS
	
	// mCMP
	
	// mTER
	
	// mVOL
	
	// mCREP
	
	// mPar
	
	// mParc
	
	// mRibbon
	
	// mPROJ
	
	// mFOR
	
	// mWRP
	
	// mPHRB
	
	// mIKJT
	
	// mPATU
	
	// mTRGD
	
	// mIREF
	
	// mSGSS
	
	// mATVL
	
	// mBBSC
	
	// mTMD

	// 4. ReferenceEntry
	fHead.ofsRefs = f.Tell();

	for(size_t i=0; i<reList.size(); i++) {
		f.Write(&reList[i], sizeof(ReferenceEntry));
	}

	// 5. rewrite head
	f.Seek(0, wxFromStart);
	f.Write(&fHead, sizeof(fHead));
	f.Write(&mdata, sizeof(mdata));

	f.Close();
}
