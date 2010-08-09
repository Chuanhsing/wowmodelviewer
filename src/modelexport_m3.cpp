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
	int chunk_offset, datachunk_offset;

	RefEntry("SQES", f.Tell(), nAnimations, 1);
	chunk_offset = f.Tell();
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
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
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
	f.Seek(datachunk_offset, wxFromStart);

	// mSTC
	RefEntry("_CTS", f.Tell(), nAnimations, 4);
	chunk_offset = f.Tell();
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
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<nAnimations; i++) {
		f.Write(&stcs[i], sizeof(STC));
	}
	wxDELETEA(stcs);
	f.Seek(datachunk_offset, wxFromStart);

	// mSTG
	RefEntry("_GTS", f.Tell(), nAnimations, 0);
	chunk_offset = f.Tell();
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
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<nAnimations; i++) {
		f.Write(&stgs[i], sizeof(STG));
	}
	wxDELETEA(stgs);
	f.Seek(datachunk_offset, wxFromStart);

	// mSTS
/*
	RefEntry("_STS", f.Tell(), mSEQS.nEntries, 0);
	mdata.mSTS.nEntries = mSTS.nEntries;
	mdata.mSTS.ref = fHead.nRefs++;
*/

	// mBone
	mdata.mBone.nEntries = m->header.nBones;
	mdata.mBone.ref = fHead.nRefs++;
	RefEntry("ENOB", f.Tell(), mdata.mBone.nEntries, 0);
	chunk_offset = f.Tell();
	BONE *bones = new BONE[mdata.mBone.nEntries];
	memset(bones, 0, sizeof(BONE)*mdata.mBone.nEntries);
	for(uint32 i=0; i<mdata.mBone.nEntries; i++) {
		bones[i].d1 = -1;
		f.Write(&bones[i], sizeof(BONE));
	}
	padding(&f);
	for(uint32 i=0; i<mdata.mBone.nEntries; i++) {
		// name
		wxString strName = wxString::Format(_T("Bone_%d"), i);
		bones[i].name.nEntries = strName.length()+1;
		bones[i].name.ref = fHead.nRefs++;
		RefEntry("RAHC", f.Tell(), bones[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.length());
		f.Write(&end, sizeof(end));
		padding(&f);
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mBone.nEntries; i++) {
		f.Write(&bones[i], sizeof(BONE));
	}
	wxDELETEA(bones);
	f.Seek(datachunk_offset, wxFromStart);

	// mVert
	mdata.mVert.nEntries = m->header.nVertices*sizeof(Vertex32);
	mdata.mVert.ref = fHead.nRefs++;
	RefEntry("__8U", f.Tell(), mdata.mVert.nEntries, 0);
	MPQFile mpqf((char *)m->modelname.c_str());
	ModelVertex *verts = (ModelVertex*)(mpqf.getBuffer() + m->header.ofsVertices);
	for(uint32 i=0; i<m->header.nVertices; i++) {
		Vertex32 vert;
		memset(&vert, 0, sizeof(vert));
		vert.pos = verts[i].pos;
		memcpy(vert.pos, verts[i].pos, sizeof(vert.pos));
		memcpy(vert.weBone, verts[i].weights, 4);
		memcpy(vert.weIndice, verts[i].bones, 4);
		// Vec3D normal -> char normal[4], TODO
		vert.normal[0] = verts[i].normal.x*0x80;
		vert.normal[1] = verts[i].normal.y*0x80;
		vert.normal[2] = verts[i].normal.z*0x80;
		// Vec2D texcoords -> uint16 uv[2], TODO
		vert.uv[0] = verts[i].texcoords.x*0x800;
		vert.uv[1] = verts[i].texcoords.y*0x800;
		f.Write(&vert, sizeof(vert));
	}
	padding(&f);


	// mDIV
	mdata.mDIV.nEntries = 1;
	mdata.mDIV.ref = fHead.nRefs++;
	RefEntry("_VID", f.Tell(), mdata.mDIV.nEntries, 0);
	chunk_offset = f.Tell();
	DIV div;
	memset(&div, 0, sizeof(div));
	f.Write(&div, sizeof(div));
	padding(&f);
	// mDIV.faces = m->view.nTriangles
	MPQFile mpqfv((char *)m->lodname.c_str());
	ModelView *view = (ModelView*)(mpqfv.getBuffer());
	div.faces.nEntries = view->nTris;
	div.faces.ref = fHead.nRefs++;
	RefEntry("_61U", f.Tell(), div.faces.nEntries, 0);
	for(uint16 i=1; i<=div.faces.nEntries; i++) {
		f.Write(&i, sizeof(uint16));
	}
	padding(&f);
	// mDiv.meash
	div.REGN.nEntries = view->nTex;
	div.REGN.ref = fHead.nRefs++;
	RefEntry("NGER", f.Tell(), div.REGN.nEntries, 0);
	ModelGeoset *ops = (ModelGeoset*)(mpqfv.getBuffer() + view->ofsSub);
	ModelTexUnit *tex = (ModelTexUnit*)(mpqfv.getBuffer() + view->ofsTex);
	int indBone = 0;
	for (size_t j=0; j<view->nTex; j++) {
		size_t geoset = tex[j].op;

		REGN regn;
		memset(&regn, 0, sizeof(regn));
		regn.indFaces = ops[geoset].istart;
		regn.numFaces = ops[geoset].icount;
		regn.indVert = ops[geoset].vstart;
		regn.numVert = ops[geoset].vcount;
		regn.boneCount = ops[geoset].nBones;
		regn.indBone = indBone;
		regn.numBone = ops[geoset].nBones;
		indBone += regn.boneCount;
		f.Write(&regn, sizeof(regn));
	}
	padding(&f);
	// mDiv.BAT
	div.BAT.nEntries = view->nTex;
	div.BAT.ref = fHead.nRefs++;
	RefEntry("_TAB", f.Tell(), div.BAT.nEntries, 0);
	for (size_t j=0; j<view->nTex; j++) {
		BAT bat;
		memset(&bat, 0, sizeof(bat));
		bat.subid = j;
		bat.matid = j;
		bat.s2 = -1;
		f.Write(&bat, 0xE); // sizeof(bat) is buggy to 0x10
	}
	padding(&f);
	// mDiv.MSEC
	div.MSEC.nEntries = 1;
	div.MSEC.ref = fHead.nRefs++;
	RefEntry("CESM", f.Tell(), div.MSEC.nEntries, 0);
	MSEC msec;
	memset(&msec, 0, sizeof(msec));
	f.Write(&msec, sizeof(msec));
	padding(&f);

	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	f.Write(&div, sizeof(div));
	f.Seek(datachunk_offset, wxFromStart);

	// mBoneLU
/*
	uint16 *boneLookup = (uint16 *)mpqf.getBuffer() + m->header.ofsBoneLookup;
	mdata.mBoneLU.nEntries = m->header.nBoneLookup;
	mdata.mBoneLU.ref = fHead.nRefs++;
	RefEntry("_61U", f.Tell(), mdata.mBoneLU.nEntries, 0);
	f.Write(&boneLookup, mdata.mBoneLU.nEntries*sizeof(uint16));
*/

	// boundSphere
	
	// mAttach
	ModelAttachmentDef *attachments = (ModelAttachmentDef*)(mpqf.getBuffer() + m->header.ofsAttachments);
	mdata.mAttach.nEntries = m->header.nAttachments;
	mdata.mAttach.ref = fHead.nRefs++;
	RefEntry("_TTA", f.Tell(), mdata.mAttach.nEntries, 0);
	chunk_offset = f.Tell();
	ATT *atts = new ATT[mdata.mAttach.nEntries];
	memset(atts, 0, sizeof(ATT)*mdata.mAttach.nEntries);
	for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
		atts[i].flag = -1;
		atts[i].bone = attachments[i].bone;
		f.Write(&atts[i], sizeof(ATT));
	}
	padding(&f);
	for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
		// name
		wxString strName = wxString::Format(_T("ATT_%d"), i);
		atts[i].name.nEntries = strName.length()+1;
		atts[i].name.ref = fHead.nRefs++;
		RefEntry("RAHC", f.Tell(), atts[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.length());
		f.Write(&end, sizeof(end));
		padding(&f);
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
		f.Write(&atts[i], sizeof(ATT));
	}
	wxDELETEA(atts);
	f.Seek(datachunk_offset, wxFromStart);

	// mAttachLU

	// mMatLU
	
	// mMat
	
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

	mpqf.close();
	mpqfv.close();
	f.Close();
}
