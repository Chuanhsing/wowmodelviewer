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

uint32 nSkinnedBones(Model *m, MPQFile *mpqf, uint32 start, uint32 num)
{
	ModelVertex *verts = (ModelVertex*)(mpqf->getBuffer() + m->header.ofsVertices);
	uint8 *skinned = new uint8[m->header.nBones];
	memset(skinned, 0, sizeof(uint8)*m->header.nBones);
	uint32 nSkinnedBones = 0;
	for(uint32 i=start; i<(start+num); i++) {
		for(uint32 j=0; j<4; j++) {
			if (verts[i].weights[j] != 0)
				skinned[verts[i].bones[j]] = 1;
		}
	}
	for(uint32 i=0; i<m->header.nBones; i++) {
		if (skinned[i] == 1)
			nSkinnedBones ++;
	}
	wxDELETEA(skinned);
	return nSkinnedBones;
}

void ExportM2toM3(Model *m, const char *fn, bool init)
{
	if (!m)
		return;

	wxFFile f(wxString(fn, wxConvUTF8), wxT("w+b"));
	reList.clear();

	if (!f.IsOpened()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}
	LogExportData(_T("M3"),wxString(fn, wxConvUTF8).BeforeLast(SLASH),_T("M2"));

	MPQFile mpqf((char *)m->modelname.c_str());
	MPQFile mpqfv((char *)m->lodname.c_str());

	// 1. FileHead
	RefEntry("43DM", f.Tell(), 1, 0xB);

	struct MD34 fHead;
	memset(&fHead, 0, sizeof(fHead));
	strcpy(fHead.id, "43DM");
	fHead.mref.nEntries = 1;
	fHead.mref.ref = ++fHead.nRefs;
	memset(&fHead.padding, 0xAA, sizeof(fHead.padding));
	f.Seek(sizeof(fHead), wxFromCurrent);

	// 2. ModelHead
	RefEntry("LDOM", f.Tell(), 1, 0x17);

	struct MODL mdata;
	memset(&mdata, 0, sizeof(mdata));
	f.Seek(sizeof(mdata), wxFromCurrent);

	// 3. Content
	// Modelname
	wxString n(fn, wxConvUTF8);
	n = n.AfterLast('\\').BeforeLast('.');
	n.Append(_T(".max"));

	mdata.name.nEntries = n.Len()+1;
	mdata.name.ref = ++fHead.nRefs;
	RefEntry("RAHC", f.Tell(), mdata.name.nEntries, 0);
	f.Write(n.c_str(), n.Len()+1);
	padding(&f);

	mdata.type = 0x180d53;
	
	// mSEQS
	uint32 nAnimations = 0;
	uint32 *logAnimations = new uint32[m->header.nAnimations];
	wxString *nameAnimations = new wxString[m->header.nAnimations];
	int Walks = 0, Stands =0, Attacks = 0, Deaths = 0;
	for(uint32 i=0; i<m->header.nAnimations; i++) {
		wxString strName;
		try {
			AnimDB::Record rec = animdb.getByAnimID(m->anims[i].animID);
			strName = rec.getString(AnimDB::Name);
		} catch (AnimDB::NotFound) {
			strName = _T("???");
		}
		if (!strName.StartsWith(_T("Run")) && !strName.StartsWith(_T("Stand")) && 
				!strName.StartsWith(_T("Attack")) && !strName.StartsWith(_T("Death")))
			continue;
		if (strName.StartsWith(_T("StandWound")))
			continue;
		nameAnimations[i] = strName;
		if (strName.StartsWith(_T("Run"))) {
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
	mdata.mSEQS.ref = ++fHead.nRefs;
	SEQS *seqs = new SEQS[nAnimations];
	memset(seqs, 0, sizeof(SEQS)*nAnimations);
	f.Seek(sizeof(SEQS)*nAnimations, wxFromCurrent);
	padding(&f);
	for(uint32 i=0; i<nAnimations; i++) {
		wxString strName = nameAnimations[i];
		seqs[i].name.nEntries = strName.Len()+1;
		seqs[i].name.ref = ++fHead.nRefs;
		RefEntry("RAHC", f.Tell(), seqs[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.Len()+1);
		padding(&f);
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<nAnimations; i++) {
		int j = logAnimations[i];
		seqs[i].d1[0] = -1;
		seqs[i].d1[1] = -1;
		seqs[i].length = m->anims[j].timeEnd;
		seqs[i].moveSpeed = m->anims[j].moveSpeed;
		seqs[i].frequency = m->anims[j].playSpeed; // ?
		seqs[i].ReplayStart = 1;
		seqs[i].ReplayEnd = 1;
		seqs[i].d4[0] = 0x64;
		seqs[i].boundSphere.min = m->header.ps.VertexBox[0];
		seqs[i].boundSphere.max = m->header.ps.VertexBox[1];
		seqs[i].boundSphere.radius = m->header.ps.VertexRadius;
		f.Write(&seqs[i], sizeof(SEQS));
	}
	wxDELETEA(seqs);
	f.Seek(datachunk_offset, wxFromStart);

	// mSTC
	RefEntry("_CTS", f.Tell(), nAnimations, 4);
	chunk_offset = f.Tell();
	mdata.mSTC.nEntries = nAnimations;
	mdata.mSTC.ref = ++fHead.nRefs;
	STC *stcs = new STC[mdata.mSTC.nEntries];
	memset(stcs, 0, sizeof(STC)*mdata.mSTC.nEntries);
	f.Seek(sizeof(STC)*mdata.mSTC.nEntries, wxFromCurrent);
	padding(&f);
	for(uint32 i=0; i<mdata.mSTC.nEntries; i++) {
		int anim_offset = logAnimations[i];

		// name
		wxString strName = nameAnimations[i];
		strName.Append(_T("_full"));
		stcs[i].name.nEntries = strName.Len()+1;
		stcs[i].name.ref = ++fHead.nRefs;
		RefEntry("RAHC", f.Tell(), stcs[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.Len()+1);
		padding(&f);

		// animid
		for(int j=0; j<m->header.nBones; j++) {
			if (m->bones[j].trans.uses(anim_offset)) {
				stcs[i].Trans.nEntries++;
			}
			if (m->bones[j].rot.uses(anim_offset)) {
				stcs[i].Rot.nEntries++;
			}
		}
		stcs[i].animid.nEntries = stcs[i].Trans.nEntries + stcs[i].Rot.nEntries;
		stcs[i].animid.ref = ++fHead.nRefs;
		RefEntry("_23U", f.Tell(), stcs[i].animid.nEntries, 0);
		for(int j=0; j<m->header.nBones; j++) {
			if (m->bones[j].trans.uses(anim_offset)) {
				int16 p = 2;
				f.Write(&j, sizeof(int16));
				f.Write(&p, sizeof(int16));
			}
			if (m->bones[j].rot.uses(anim_offset)) {
				int16 p = 3;
				f.Write(&j, sizeof(int16));
				f.Write(&p, sizeof(int16));
			}
		}
		padding(&f);

		// animindex
		stcs[i].animindex.nEntries = stcs[i].animid.nEntries;
		stcs[i].animindex.ref = ++fHead.nRefs;
		RefEntry("_23U", f.Tell(), stcs[i].animindex.nEntries, 0);
		for(int j=0; j<m->header.nBones; j++) {
			if (m->bones[j].trans.uses(anim_offset)) {
				int16 p = 2;
				f.Write(&j, sizeof(int16));
				f.Write(&p, sizeof(int16));
			}
			if (m->bones[j].rot.uses(anim_offset)) {
				int16 p = 3;
				f.Write(&j, sizeof(int16));
				f.Write(&p, sizeof(int16));
			}
		}
		padding(&f);


		SD *sds;
		int ii;
		int chunk_offset2, datachunk_offset2;

		// Events, VEDS
		stcs[i].Events.nEntries = 1;
		stcs[i].Events.ref = ++fHead.nRefs;
		RefEntry("VEDS", f.Tell(), stcs[i].Events.nEntries, 0);
		chunk_offset2 = f.Tell();
		SD sd;
		memset(&sd, 0, sizeof(sd));
		f.Seek(sizeof(sd), wxFromCurrent);
		for(int j=0; j<m->header.nBones; j++) {
			if (m->bones[j].trans.uses(anim_offset)) {
				int counts = m->bones[j].trans.data[anim_offset].size();
				sd.length = m->bones[j].trans.times[anim_offset][counts-1];
				break;
			}
		}
		sd.timeline.nEntries = 1;
		sd.timeline.ref = ++fHead.nRefs;
		RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
		f.Write(&sd.length, sizeof(int32));
		padding(&f);
		sd.data.nEntries = 1;
		sd.data.ref = ++fHead.nRefs;
		RefEntry("TNVE", f.Tell(), sd.data.nEntries, 0);
		EVNT evnt;
		// name
		strName = _T("Evt_SeqEnd");
		evnt.name.nEntries = strName.Len()+1;
		evnt.name.ref = ++fHead.nRefs;
		evnt.d1 = -1;
		evnt.s1 = -1;
		evnt.a = Vec4D(1.0f, 0.0f, 0.0f, 0.0f);
		evnt.b = Vec4D(0.0f, 1.0f, 0.0f, 0.0f);
		evnt.c = Vec4D(0.0f, 0.0f, 1.0f, 0.0f);
		evnt.d = Vec4D(0.0f, 0.0f, 0.0f, 1.0f);
		f.Write(&evnt, sizeof(evnt));
		padding(&f);
		RefEntry("RAHC", f.Tell(), evnt.name.nEntries, 0);
		f.Write(strName.c_str(), strName.Len()+1);
		padding(&f);
		datachunk_offset2 = f.Tell();
		f.Seek(chunk_offset2, wxFromStart);
		f.Write(&sd, sizeof(sd));
		f.Seek(datachunk_offset2, wxFromStart);

		// Trans, V3DS
		if (stcs[i].Trans.nEntries > 0) {
			stcs[i].Trans.ref = ++fHead.nRefs;
			RefEntry("V3DS", f.Tell(), stcs[i].Trans.nEntries, 0);
			chunk_offset2 = f.Tell();
			sds = new SD[stcs[i].Trans.nEntries];
			memset(sds, 0, sizeof(SD)*stcs[i].Trans.nEntries);
			f.Seek(sizeof(sd)*stcs[i].Trans.nEntries, wxFromCurrent);
			ii=0;
			for(int j=0; j<m->header.nBones; j++) {
				if (m->bones[j].trans.uses(anim_offset)) {
					int counts = m->bones[j].trans.data[anim_offset].size();
					sds[ii].timeline.nEntries = counts;
					sds[ii].timeline.ref = ++fHead.nRefs;
					RefEntry("_23I", f.Tell(), sds[ii].timeline.nEntries, 0);
					for (int k=0; k<counts; k++) {
						f.Write(&m->bones[j].trans.times[anim_offset][k], sizeof(int32));
					}
					padding(&f);
					sds[ii].length = m->bones[j].trans.times[anim_offset][counts-1];
					sds[ii].data.nEntries = counts;
					sds[ii].data.ref = ++fHead.nRefs;
					RefEntry("3CEV", f.Tell(), sds[ii].data.nEntries, 0);
					for (int k=0; k<counts; k++) {
						f.Write(&m->bones[j].trans.data[anim_offset][k].x, sizeof(int32));
						f.Write(&m->bones[j].trans.data[anim_offset][k].y, sizeof(int32));
						f.Write(&m->bones[j].trans.data[anim_offset][k].z, sizeof(int32));
					}
					padding(&f);
					ii++;
				}
			}
			datachunk_offset2 = f.Tell();
			f.Seek(chunk_offset2, wxFromStart);
			for(int j=0; j<stcs[i].Trans.nEntries; j++) {
				f.Write(&sds[j], sizeof(sd));
			}
			wxDELETEA(sds);
			f.Seek(datachunk_offset2, wxFromStart);
		}

		// Rot, Q4DS
		if (stcs[i].Rot.nEntries > 0) {
			stcs[i].Rot.ref = ++fHead.nRefs;
			RefEntry("Q4DS", f.Tell(), stcs[i].Rot.nEntries, 0);
			chunk_offset2 = f.Tell();
			sds = new SD[stcs[i].Rot.nEntries];
			memset(sds, 0, sizeof(SD)*stcs[i].Rot.nEntries);
			f.Seek(sizeof(sd)*stcs[i].Rot.nEntries, wxFromCurrent);
			ii=0;
			for(int j=0; j<m->header.nBones; j++) {
				if (m->bones[j].rot.uses(anim_offset)) {
					int counts = m->bones[j].rot.data[anim_offset].size();
					sds[ii].timeline.nEntries = counts;
					sds[ii].timeline.ref = ++fHead.nRefs;
					RefEntry("_23I", f.Tell(), sds[ii].timeline.nEntries, 0);
					for (int k=0; k<counts; k++) {
						f.Write(&m->bones[j].rot.times[anim_offset][k], sizeof(int32));
					}
					padding(&f);
					sds[ii].length = m->bones[j].rot.times[anim_offset][counts-1];
					sds[ii].data.nEntries = counts;
					sds[ii].data.ref = ++fHead.nRefs;
					RefEntry("TAUQ", f.Tell(), sds[ii].data.nEntries, 0);
					for (int k=0; k<counts; k++) {
						f.Write(&m->bones[j].rot.data[anim_offset][k].x, sizeof(int32));
						f.Write(&m->bones[j].rot.data[anim_offset][k].y, sizeof(int32));
						f.Write(&m->bones[j].rot.data[anim_offset][k].z, sizeof(int32));
						f.Write(&m->bones[j].rot.data[anim_offset][k].w, sizeof(int32));
					}
					padding(&f);
					ii++;
				}
			}
			datachunk_offset2 = f.Tell();
			f.Seek(chunk_offset2, wxFromStart);
			for(int j=0; j<stcs[i].Rot.nEntries; j++) {
				f.Write(&sds[j], sizeof(sd));
			}
			wxDELETEA(sds);
			f.Seek(datachunk_offset2, wxFromStart);
		}
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mSTC.nEntries; i++) {
		stcs[i].indSEQ[0] = stcs[i].indSEQ[1] = i;
		f.Write(&stcs[i], sizeof(STC));
	}
	f.Seek(datachunk_offset, wxFromStart);

	// mSTG
	RefEntry("_GTS", f.Tell(), nAnimations, 0);
	chunk_offset = f.Tell();
	mdata.mSTG.nEntries = nAnimations;
	mdata.mSTG.ref = ++fHead.nRefs;
	STG *stgs = new STG[mdata.mSTG.nEntries];
	memset(stgs, 0, sizeof(STG)*mdata.mSTG.nEntries);
	f.Seek(sizeof(STG)*mdata.mSTG.nEntries, wxFromCurrent);
	padding(&f);
	for(uint32 i=0; i<mdata.mSTG.nEntries; i++) {
		// name
		wxString strName = nameAnimations[i];
		stgs[i].name.nEntries = strName.Len()+1;
		stgs[i].name.ref = ++fHead.nRefs;
		RefEntry("RAHC", f.Tell(), stgs[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.Len()+1);
		padding(&f);
		
		// stcID
		stgs[i].stcID.nEntries = 1;
		stgs[i].stcID.ref = ++fHead.nRefs;
		RefEntry("_23U", f.Tell(), stgs[i].stcID.nEntries, 0);
		f.Write(&i, sizeof(uint32));
		padding(&f);
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mSTG.nEntries; i++) {
		f.Write(&stgs[i], sizeof(STG));
	}
	wxDELETEA(stgs);
	f.Seek(datachunk_offset, wxFromStart);

	// mSTS
	mdata.mSTS.nEntries = nAnimations;
	mdata.mSTS.ref = ++fHead.nRefs;
	RefEntry("_STS", f.Tell(), mdata.mSTS.nEntries, 0);
	chunk_offset = f.Tell();
	STS *stss = new STS[mdata.mSTS.nEntries];
	memset(stss, 0, sizeof(STS)*mdata.mSTS.nEntries);
	f.Seek(sizeof(STS)*mdata.mSTS.nEntries, wxFromCurrent);
	padding(&f);
	for(uint32 i=0; i<mdata.mSTS.nEntries; i++) {
		int anim_offset = logAnimations[i];

		stss[i].animid.nEntries = stcs[i].animid.nEntries;
		stss[i].animid.ref = ++fHead.nRefs;
		RefEntry("_23U", f.Tell(), stss[i].animid.nEntries, 0);
		for(int j=0; j<m->header.nBones; j++) {
			if (m->bones[j].trans.uses(anim_offset)) {
				int16 p = 2;
				f.Write(&j, sizeof(int16));
				f.Write(&p, sizeof(int16));
			}
			if (m->bones[j].rot.uses(anim_offset)) {
				int16 p = 3;
				f.Write(&j, sizeof(int16));
				f.Write(&p, sizeof(int16));
			}
		}
		padding(&f);
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mSTS.nEntries; i++) {
		stss[i].d1[0] = -1;
		stss[i].d1[1] = -1;
		stss[i].d1[2] = -1;
		stss[i].s1 = -1;
		f.Write(&stss[i], sizeof(STS));
	}
	wxDELETEA(stss);
	f.Seek(datachunk_offset, wxFromStart);

	// mBone
	mdata.mBone.nEntries = m->header.nBones;
	mdata.mBone.ref = ++fHead.nRefs;
	RefEntry("ENOB", f.Tell(), mdata.mBone.nEntries, 1);
	chunk_offset = f.Tell();
	BONE *bones = new BONE[mdata.mBone.nEntries];
	memset(bones, 0, sizeof(BONE)*mdata.mBone.nEntries);
	f.Seek(sizeof(BONE)*mdata.mBone.nEntries, wxFromCurrent);
	padding(&f);
	ModelBoneDef *mb = (ModelBoneDef*)(mpqf.getBuffer() + m->header.ofsBones);
	for(uint32 i=0; i<mdata.mBone.nEntries; i++) {
		// name
		wxString strName = wxString::Format(_T("Bone_%d"), i);
		bones[i].name.nEntries = strName.Len()+1;
		bones[i].name.ref = ++fHead.nRefs;
		RefEntry("RAHC", f.Tell(), bones[i].name.nEntries, 0);
		f.Write(strName.c_str(), strName.Len()+1);
		padding(&f);
	}
	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mBone.nEntries; i++) {
		bones[i].d1 = -1;
		bones[i].flags = 0xA00;
		bones[i].parent = mb[i].parent;
		for(uint32 j=0; j<mdata.mSTC.nEntries; j++) {
			int anim_offset = logAnimations[j];
			if (m->bones[i].trans.uses(anim_offset)) {
				bones[i].initTrans.AnimRef.animid = i | (2 << 16);
				break;
			}
		}
		bones[i].initTrans.AnimRef.animid = i | (2 << 16);
		bones[i].initTrans.value.x = mb[i].pivot.x;
		bones[i].initTrans.value.y = mb[i].pivot.z;
		bones[i].initTrans.value.z = mb[i].pivot.y;
		for(uint32 j=0; j<mdata.mSTC.nEntries; j++) {
			int anim_offset = logAnimations[j];
			if (m->bones[i].rot.uses(anim_offset)) {
				bones[i].initRot.AnimRef.animid = i | (3 << 16);
				break;
			}
		}
		bones[i].initRot.AnimRef.animid = i | (3 << 16);
		bones[i].initRot.value = Vec4D(0.0f, 0.0f, 0.0f, 1.0f);
		bones[i].initRot.unValue = Vec4D(0.0f, 0.0f, 0.0f, 1.0f);
		bones[i].initScale.AnimRef.animid = i | (5 << 16);
		bones[i].initScale.value = Vec3D(1.0f, 1.0f, 1.0f);
		bones[i].initScale.unValue = Vec3D(1.0f, 1.0f, 1.0f);
		bones[i].ar1.AnimRef.animid = i | (6 << 16);
		bones[i].ar1.value = 1;
		bones[i].ar1.unValue = 1;
		f.Write(&bones[i], sizeof(BONE));
	}
	wxDELETEA(bones);
	f.Seek(datachunk_offset, wxFromStart);

	// nSkinnedBones
	mdata.nSkinnedBones = nSkinnedBones(m, &mpqf, 0, m->header.nVertices);
	// vertFlags
	mdata.vertFlags = 0x182007D;

	// mVert
	ModelVertex *verts = (ModelVertex*)(mpqf.getBuffer() + m->header.ofsVertices);
	mdata.mVert.nEntries = m->header.nVertices*sizeof(Vertex32);
	mdata.mVert.ref = ++fHead.nRefs;
	RefEntry("__8U", f.Tell(), mdata.mVert.nEntries, 0);
	for(uint32 i=0; i<m->header.nVertices; i++) {
		Vertex32 vert;
		memset(&vert, 0, sizeof(vert));
		vert.pos = verts[i].pos;
		memcpy(vert.pos, verts[i].pos, sizeof(vert.pos));
		memcpy(vert.weBone, verts[i].weights, 4);
		memcpy(vert.weIndice, verts[i].bones, 4);
		// Vec3D normal -> char normal[4], TODO
		vert.normal[0] = (verts[i].normal.x+1)*0xFF/2;
		vert.normal[1] = (verts[i].normal.y+1)*0xFF/2;
		vert.normal[2] = (verts[i].normal.z+1)*0xFF/2;
		// Vec2D texcoords -> uint16 uv[2], TODO
		vert.uv[0] = verts[i].texcoords.x*0x800;
		vert.uv[1] = verts[i].texcoords.y*0x800;
		f.Write(&vert, sizeof(vert));
	}
	padding(&f);

	// mDIV
	mdata.mDIV.nEntries = 1;
	mdata.mDIV.ref = ++fHead.nRefs;
	RefEntry("_VID", f.Tell(), mdata.mDIV.nEntries, 2);
	chunk_offset = f.Tell();
	DIV div;
	memset(&div, 0, sizeof(div));
	f.Seek(sizeof(div), wxFromCurrent);
	padding(&f);
	// mDIV.faces = m->view.nTriangles
	ModelView *view = (ModelView*)(mpqfv.getBuffer());
	div.faces.nEntries = view->nTris;
	div.faces.ref = ++fHead.nRefs;
	RefEntry("_61U", f.Tell(), div.faces.nEntries, 0);
	uint16 *triangles = (uint16*)(mpqfv.getBuffer() + view->ofsTris);
	for(uint16 i=0; i<div.faces.nEntries; i++) {
		f.Write(&triangles[i], sizeof(uint16)); // Error
	}
	padding(&f);
	// mDiv.meash
	div.REGN.nEntries = view->nTex;
	div.REGN.ref = ++fHead.nRefs;
	RefEntry("NGER", f.Tell(), div.REGN.nEntries, 3);
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
		regn.boneCount = nSkinnedBones(m, &mpqf, regn.indVert, regn.numVert);
		regn.indBone = indBone;
		regn.numBone = regn.boneCount;
		regn.b1[0] = regn.b1[1] = 1;
		indBone += regn.boneCount;
		f.Write(&regn, sizeof(regn));
	}
	padding(&f);
	// mDiv.BAT
	div.BAT.nEntries = view->nTex;
	div.BAT.ref = ++fHead.nRefs;
	RefEntry("_TAB", f.Tell(), div.BAT.nEntries, 1);
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
	div.MSEC.ref = ++fHead.nRefs;
	RefEntry("CESM", f.Tell(), div.MSEC.nEntries, 1);
	MSEC msec;
	memset(&msec, 0, sizeof(msec));
	f.Write(&msec, sizeof(msec));
	padding(&f);

	datachunk_offset = f.Tell();
	f.Seek(chunk_offset, wxFromStart);
	div.unk = 1;
	f.Write(&div, sizeof(div));
	f.Seek(datachunk_offset, wxFromStart);

	// mBoneLU
	uint16 *boneLookup = (uint16 *)(mpqf.getBuffer() + m->header.ofsBoneLookup);
	std::vector<uint16> bLookup;
	for (size_t j=0; j<view->nTex; j++) {
		size_t geoset = tex[j].op;
		std::vector<uint16> bLookup2; // local bLookup
		bLookup2.clear();

		for(uint32 i=ops[geoset].vstart; i<(ops[geoset].vstart+ops[geoset].vcount); i++) {
			for(uint32 k=0; k<4; k++) {
				if (verts[i].weights[k] != 0) {
					bool bFound = false;
					for(uint32 m=0; m<bLookup2.size(); m++) {
						if (bLookup2[m] == verts[i].bones[k])
							bFound = true;
					}
					if (bFound == false)
						bLookup2.push_back(verts[i].bones[k]);
				}
			}
		}
		// push local bLookup to global, and lookup it in boneLookup
		for(uint32 i=0; i<bLookup2.size(); i++) {
			bLookup.push_back(boneLookup[bLookup2[i]]);
		}
	}
	mdata.mBoneLU.nEntries = bLookup.size();
	mdata.mBoneLU.ref = ++fHead.nRefs;
	RefEntry("_61U", f.Tell(), mdata.mBoneLU.nEntries, 0);
	for(uint16 i=0; i<bLookup.size(); i++) {
		f.Write(&bLookup[i], sizeof(uint16));
	}
	padding(&f);

	// boundSphere
	mdata.boundSphere.min = m->header.ps.VertexBox[0];
	mdata.boundSphere.max = m->header.ps.VertexBox[1];
	mdata.boundSphere.radius = m->header.ps.VertexRadius;

	// mAttach
	if (m->header.nAttachments) {
		ModelAttachmentDef *attachments = (ModelAttachmentDef*)(mpqf.getBuffer() + m->header.ofsAttachments);
		mdata.mAttach.nEntries = m->header.nAttachments;
		mdata.mAttach.ref = ++fHead.nRefs;
		RefEntry("_TTA", f.Tell(), mdata.mAttach.nEntries, 0);
		chunk_offset = f.Tell();
		ATT *atts = new ATT[mdata.mAttach.nEntries];
		memset(atts, 0, sizeof(ATT)*mdata.mAttach.nEntries);
		f.Seek(sizeof(ATT)*mdata.mAttach.nEntries, wxFromCurrent);
		padding(&f);
		for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
			// name
			wxString strName = wxString::Format(_T("ATT_%d"), i);
			atts[i].name.nEntries = strName.Len()+1;
			atts[i].name.ref = ++fHead.nRefs;
			RefEntry("RAHC", f.Tell(), atts[i].name.nEntries, 0);
			f.Write(strName.c_str(), strName.Len()+1);
			padding(&f);
		}
		datachunk_offset = f.Tell();
		f.Seek(chunk_offset, wxFromStart);
		for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
			atts[i].flag = -1;
			atts[i].bone = attachments[i].bone;
			f.Write(&atts[i], sizeof(ATT));
		}
		wxDELETEA(atts);
		f.Seek(datachunk_offset, wxFromStart);
	}

	// mAttachLU
	//int16 *attachLookup = (int16 *)(mpqf.getBuffer() + m->header.ofsAttachLookup);
	if (mdata.mAttach.nEntries) {
		mdata.mAttachLU.nEntries = mdata.mAttach.nEntries;
		mdata.mAttachLU.ref = ++fHead.nRefs;
		RefEntry("_61U", f.Tell(), mdata.mAttachLU.nEntries, 0);
		for(uint16 i=0; i<mdata.mAttachLU.nEntries; i++) {
			int16 ii = -1;
			f.Write(&ii, sizeof(int16));
		}
		padding(&f);
	}

	// mMatLU
	if (view->nTex) {
		mdata.mMatLU.nEntries = view->nTex;
		mdata.mMatLU.ref = ++fHead.nRefs;
		RefEntry("MTAM", f.Tell(), mdata.mMatLU.nEntries, 0);
		for(uint32 i=0; i<mdata.mMatLU.nEntries; i++) {
			MATM matm;
			matm.nmat = 1;
			matm.matind = i;
			f.Write(&matm, sizeof(matm));
		}
		padding(&f);
	}

	// mMat
	if (view->nTex) {
		mdata.mMat.nEntries = view->nTex;
		mdata.mMat.ref = ++fHead.nRefs;
		RefEntry("_TAM", f.Tell(), mdata.mMat.nEntries, 0xF);
		chunk_offset = f.Tell();
		MAT *mats = new MAT[mdata.mMat.nEntries];
		memset(mats, 0, sizeof(MAT)*mdata.mMat.nEntries);
		f.Seek(sizeof(MAT)*mdata.mMat.nEntries, wxFromCurrent);
		padding(&f);
		for(uint32 i=0; i<mdata.mMat.nEntries; i++) {
			// name
			wxString strName = wxString::Format(_T("Mat_%2d"), i);
			mats[i].name.nEntries = strName.Len()+1;
			mats[i].name.ref = ++fHead.nRefs;
			RefEntry("RAHC", f.Tell(), mats[i].name.nEntries, 0);
			f.Write(strName.c_str(), strName.Len()+1);
			padding(&f);

			// layers
			int chunk_offset2, datachunk_offset2;
			for(uint32 j=0; j<13; j++) {
				mats[i].layers[j].nEntries = 1;
				mats[i].layers[j].ref = ++fHead.nRefs;
				RefEntry("RYAL", f.Tell(), mats[i].layers[j].nEntries, 0x16);
				chunk_offset2 = f.Tell();
				LAYR layer;
				memset(&layer, 0, sizeof(layer));
				layer.brightness_mult1.value = 1.0f;
				layer.uvTiling.value = Vec2D(1.0f, 1.0f);
				layer.d20 = -1;
				f.Write(&layer, sizeof(layer));
				padding(&f);

				if (j == 0) { // LAYER_Diff
					wxString strName = wxString::Format(_T("Mat_%2d.tga"), i);
					layer.name.nEntries = strName.Len()+1;
					layer.name.ref = ++fHead.nRefs;
					RefEntry("RAHC", f.Tell(), layer.name.nEntries, 0);
					f.Write(strName.c_str(), strName.Len()+1);
					padding(&f);
				}
				datachunk_offset2 = f.Tell();
				f.Seek(chunk_offset2, wxFromStart);
				f.Write(&layer, sizeof(layer));
				f.Seek(datachunk_offset2, wxFromStart);
			}
		}
		datachunk_offset = f.Tell();
		f.Seek(chunk_offset, wxFromStart);
		for(uint32 i=0; i<mdata.mMat.nEntries; i++) {
			mats[i].cutoutThresh = 0xC8;
			mats[i].SpecMult = 1;
			mats[i].EmisMult = 1;
			mats[i].layerBlend = 2;
			mats[i].emisBlend = 2;
			f.Write(&mats[i], sizeof(MAT));
		}
		wxDELETE(mats);
		f.Seek(datachunk_offset, wxFromStart);
	}

	// mIREF
	mdata.mIREF.nEntries = mdata.mBone.nEntries;
	mdata.mIREF.ref = ++fHead.nRefs;
	RefEntry("FERI", f.Tell(), mdata.mIREF.nEntries, 0);
	for(uint32 i=0; i<mdata.mIREF.nEntries; i++) {
		IREF iref;
		memset(&iref, 0, sizeof(iref));
		iref.matrix[0][1] = 2.0f;
		iref.matrix[1][0] = -2.0f;
		iref.matrix[2][2] = 2.0f;
		iref.matrix[3][0] = -m->bones[i].pivot.x;
		iref.matrix[3][1] = -m->bones[i].pivot.z;
		iref.matrix[3][2] = -m->bones[i].pivot.y;
		iref.matrix[3][3] = 1.0f;
		f.Write(&iref, sizeof(iref));
	}
	padding(&f);

	//
	mdata.mat.a = Vec4D(1.0f, 0.0f, 0.0f, 0.0f);
	mdata.mat.b = Vec4D(0.0f, 1.0f, 0.0f, 0.0f);
	mdata.mat.c = Vec4D(0.0f, 0.0f, 1.0f, 0.0f);
	mdata.mat.d = Vec4D(0.0f, 0.0f, 0.0f, 1.0f);

	// 4. ReferenceEntry
	fHead.ofsRefs = f.Tell();
	fHead.nRefs = reList.size(); // MODL Ref

	for(size_t i=0; i<reList.size(); i++) {
		f.Write(&reList[i], sizeof(ReferenceEntry));
	}

	// 5. rewrite head
	f.Seek(0, wxFromStart);
	f.Write(&fHead, sizeof(fHead));
	f.Write(&mdata, sizeof(mdata));

	wxDELETEA(stcs);

	mpqf.close();
	mpqfv.close();
	f.Close();
}
