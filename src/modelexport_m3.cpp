#include "globalvars.h"
#include "modelexport.h"
#include "modelexport_m3.h"
#include "modelcanvas.h"

#define	ROOT_BONE	(1)

typedef enum M3_Class {
	AR_Default,
	AR_Bone,
	AR_MSEC,
	AR_Mat,
	AR_Layer,
	AR_Par
};

static wxString M3_Attach_Names[] = {
	_T("Ref_Hardpoint"),   // 0
	_T("Ref_Weapon Right"),
	_T("Ref_Weapon Left"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),   //5
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),   //10
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Hardpoint"),
	_T("Ref_Target"),     //15
	_T("Ref_Target"), 
	_T("Ref_Hardpoint"), 
	_T("Ref_Head"), 
	_T("Ref_Origin"),
	_T("Ref_Overhead"),   //20
};

static std::vector<ReferenceEntry> reList;

typedef struct {
	uint16 texid;
	uint16 flags;
	uint16 blend;
	int16  animid;
	int16  color;
	int16  eye;
} MATmap;

typedef struct {
	uint16 regnIndex;
	uint16 matmIndex;
} MeshMap;

typedef	struct {
	int16 index;
	int16 type;
} AnimOffset;

typedef struct {
	std::vector <int>	timeline;
	std::vector <float>	data;
} Anim_Float;

typedef struct {
	std::vector <int>	timeline;
	std::vector <Vec2D>	data;
} Anim_Vec2D;

typedef struct {
	std::vector <int>	timeline;
	std::vector <Vec3D>	data;
} Anim_Vec3D;

typedef struct {
	std::vector <int>	timeline;
	std::vector <Vec4D>	data;
} Anim_Vec4D;

typedef	struct {
	std::vector <uint32> animid;
	std::vector <AnimOffset> animoff;
	std::vector <Anim_Float> animfloat;
	std::vector <Anim_Vec2D> animvec2d;
	std::vector <Anim_Vec3D> animvec3d;
	std::vector <Anim_Vec4D> animvec4d;
	std::vector <SD>		 animsdfloat;
	std::vector <SD>		 animsdvec2d;
	std::vector <SD>		 animsdvec3d;
	std::vector <SD>		 animsdvec4d;
} STCExtra;

uint32 CreateAnimID(int m3class, int key, int subkey, int idx)
{
	return (m3class & 0xFF) << 24 | (key & 0xFF) << 16 | (subkey & 0xFF) << 8 | (idx & 0xFF);
}

void SetAnimed(AnimationReference &anim)
{
	anim.flags = INTERPOLATION_LINEAR;
	anim.animflag = 6;
}

Vec3D fixCoord(Vec3D v)
{
	return Vec3D(v.y, -v.x, v.z);
}

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

void NameRefEntry(Reference *name, wxString strName, wxFFile *f)
{
	name->nEntries = strName.Len()+1;
	name->ref = reList.size();
	RefEntry("RAHC", f->Tell(), name->nEntries, 0);
	f->Write(strName.c_str(), strName.Len()+1);
	padding(f);
}

uint32 nSkinnedBones(Model *m, MPQFile *mpqf)
{
	ModelVertex *verts = (ModelVertex*)(mpqf->getBuffer() + m->header.ofsVertices);
	ModelBoneDef *mb = (ModelBoneDef*)(mpqf->getBuffer() + m->header.ofsBones);
	uint8 *skinned = new uint8[m->header.nBones];
	memset(skinned, 0, sizeof(uint8)*m->header.nBones);
	uint32 nSkinnedBones = 0;

	for(uint32 i=0; i < m->header.nVertices; i++) {
		for(uint32 j=0; j<4; j++) {
			if (verts[i].weights[j] != 0)
				skinned[verts[i].bones[j]] = 1;
		}
	}

	for(uint32 i=0; i<m->header.nBones; i++) {
		if (skinned[i] == 1)
		{
			uint32 j = i;
			while(1)
			{
				if(mb[j].parent > -1)
				{
					j = mb[j].parent;
					skinned[j] = 1;
				}
				else
					break;
			}
		}
	}
	for(uint32 i=0; i<m->header.nBones; i++) {
		if (skinned[i] == 1)
			nSkinnedBones ++;
	}
	wxDELETEA(skinned);
	nSkinnedBones++;
	return nSkinnedBones;
}

void ExportM2toM3(Attachment *att, Model *m, const char *fn, bool init)
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

	MPQFile mpqf(m->modelname);
	MPQFile mpqfv(m->lodname);

	// 1. FileHead
	RefEntry("43DM", f.Tell(), 1, 0xB);

	struct MD34 fHead;
	memset(&fHead, 0, sizeof(fHead));
	strcpy(fHead.id, "43DM");
	fHead.mref.nEntries = 1;
	fHead.mref.ref = reList.size();
	memset(&fHead.padding, 0xAA, sizeof(fHead.padding));
	f.Seek(sizeof(fHead), wxFromCurrent);

	// 2. ModelHead
	RefEntry("LDOM", f.Tell(), 1, 0x17);

	struct MODL mdata;
	memset(&mdata, 0, sizeof(mdata));
	mdata.init();
	f.Seek(sizeof(mdata), wxFromCurrent);

	// 3. Content
	// Prepare
	ModelView *view = (ModelView*)(mpqfv.getBuffer());
	ModelGeoset *ops = (ModelGeoset*)(mpqfv.getBuffer() + view->ofsSub);
	ModelTexUnit *tex = (ModelTexUnit*)(mpqfv.getBuffer() + view->ofsTex);
	uint16 *trianglelookup = (uint16*)(mpqfv.getBuffer() + view->ofsIndex);
	uint16 *triangles = (uint16*)(mpqfv.getBuffer() + view->ofsTris);

	ModelTextureDef *texdef = (ModelTextureDef*)(mpqf.getBuffer() + m->header.ofsTextures);
	uint16 *texlookup = (uint16*)(mpqf.getBuffer() + m->header.ofsTexLookup);
	uint16 *texunitlookup = (uint16*)(mpqf.getBuffer() + m->header.ofsTexUnitLookup);
	uint16 *texanimlookup = (uint16*)(mpqf.getBuffer() + m->header.ofsTexAnimLookup);
	ModelBoneDef *mb = (ModelBoneDef*)(mpqf.getBuffer() + m->header.ofsBones);
	ModelVertex *verts = (ModelVertex*)(mpqf.getBuffer() + m->header.ofsVertices);
	ModelRenderFlags *renderflags = (ModelRenderFlags *)(mpqf.getBuffer() + m->header.ofsTexFlags);
	//ModelTexAnimDef *texanim = (ModelTexAnimDef *)(mpqf.getBuffer() + m->header.ofsTexAnims);
	//uint16 *boneLookup = (uint16 *)(mpqf.getBuffer() + m->header.ofsBoneLookup);
	ModelParticleEmitterDef *particle = (ModelParticleEmitterDef *)(mpqf.getBuffer() + m->header.ofsParticleEmitters);

	std::vector <Vertex32>	Verts;
	std::vector <uint16>	Faces;
	std::vector <REGN>		Regns;
	std::vector <BONE>		Bones;
	std::vector <SEQS>		Seqss;
	std::vector <STC>		Stcs;
	std::vector <STCExtra>	StcExtras;

	std::vector <int>		MeshM2toM3;

	std::vector <uint16>	bLookup;

	std::vector <std::vector <uint32> > M3OpacityAnimid;
	std::vector <std::vector <uint32> > M2OpacityIdx;

	std::vector <uint32>	logAnimations;

	wxArrayString vAnimations;
	wxArrayString nameAnimations;
	int chunk_offset;

/************************************************************************************/
/* Prepare Data                                                                     */
/************************************************************************************/

	// init mesh, vertex, face, mat
	int boneidx = 0;
	int vertidx = 0;
	int faceidx = 0;

	for (size_t j=0; j<view->nSub; j++) 
	{
		if (m->showGeosets[j] != true)
		{
			MeshM2toM3.push_back(-1);
			continue;
		}

		std::vector<uint16> bLookup2; // local bLookup
		bLookup2.clear();

		REGN regn;
		memset(&regn, 0, sizeof(regn));
		regn.init();
		Vertex32 vert;

		for(uint32 i=ops[j].vstart; i<(ops[j].vstart+ops[j].vcount); i++) {
			memset(&vert, 0, sizeof(vert));
			vert.pos = fixCoord(verts[trianglelookup[i]].pos); 
			memcpy(vert.weBone, verts[trianglelookup[i]].weights, 4);
			for (uint32 k=0; k<4; k++)
			{
				if (verts[trianglelookup[i]].weights[k] != 0) {
					bool bFound = false;
					for(uint32 m=0; m<bLookup2.size(); m++) {
						if (bLookup2[m] == verts[trianglelookup[i]].bones[k])
						{
							bFound = true;
							vert.weIndice[k] = m;
							break;
						}
					}
					if (bFound == false)
					{
						vert.weIndice[k] = bLookup2.size();
						bLookup2.push_back(verts[trianglelookup[i]].bones[k]);
					}
				}
			}
			// Vec3D normal -> char normal[4]
			vert.normal[0] = (verts[trianglelookup[i]].normal.x+1)*0xFF/2;
			vert.normal[1] = (verts[trianglelookup[i]].normal.y+1)*0xFF/2;
			vert.normal[2] = (verts[trianglelookup[i]].normal.z+1)*0xFF/2;
			// Vec2D texcoords -> uint16 uv[2]
			vert.uv[0] = verts[trianglelookup[i]].texcoords.x*0x800;
			vert.uv[1] = verts[trianglelookup[i]].texcoords.y*0x800;

			Verts.push_back(vert);
		}

		for(uint32 i = ops[j].istart; i < ops[j].istart + ops[j].icount; i++) {
			uint16 face;
			face = triangles[i] - ops[j].vstart;
			Faces.push_back(face);
		}

		// push local bLookup to global, and lookup it in boneLookup
		for(uint32 i=0; i<bLookup2.size(); i++) {
			bLookup.push_back(bLookup2[i] + ROOT_BONE);
		}

		regn.boneCount = bLookup2.size();
		regn.numBone = bLookup2.size();
		regn.numFaces = ops[j].icount;
		regn.numVert = ops[j].vcount;
		regn.indBone = boneidx;
		regn.indFaces = faceidx;
		regn.indVert = vertidx;

		MeshM2toM3.push_back(Regns.size());
		Regns.push_back(regn);

		boneidx += regn.numBone;
		faceidx += regn.numFaces;
		vertidx += regn.numVert;
	}

	//prepare BAT and MeshtoMat Table
	std::vector<MATmap> MATtable;
	std::vector<MeshMap> MeshtoMat;
	for (uint32 i=0; i<view->nTex; i++)
	{
		if ((gameVersion < 40000 && tex[i].texunit < m->header.nTexUnitLookup && texunitlookup[tex[i].texunit] == 0) || gameVersion >= 40000) // cataclysm lost this table
		{	
			int idx = -1;

			for(uint32 j=0; j<MATtable.size(); j++)
			{

				if (MATtable[j].texid == texlookup[tex[i].textureid] && 
					MATtable[j].blend == renderflags[tex[i].flagsIndex].blend &&
					MATtable[j].flags == renderflags[tex[i].flagsIndex].flags &&
					MATtable[j].color == tex[i].colorIndex)
				{
					idx = j;
					break;
				}
			}
			if (idx < 0)
			{
				if (m->showGeosets[tex[i].op])
				{
					MATmap bm;
					bm.texid = texlookup[tex[i].textureid];
					bm.flags = renderflags[tex[i].flagsIndex].flags;
					bm.blend = renderflags[tex[i].flagsIndex].blend;
					bm.animid = texanimlookup[tex[i].texanimid];
					bm.color = tex[i].colorIndex;
					if (m->charModelDetails.isChar && ops[tex[i].op].id == 0 && tex[i].colorIndex >= 0)
						bm.eye = 1;
					else
						bm.eye = 0;
					idx = MATtable.size();
					MATtable.push_back(bm);
					
					MeshMap mm;
					mm.regnIndex = tex[i].op;
					mm.matmIndex = idx;
					MeshtoMat.push_back(mm);
				}
			}
			else
			{
				int found = 0;
				for(uint32 k=0; k<MeshtoMat.size(); k++)
				{
					if (MeshtoMat[k].regnIndex == tex[i].op && MeshtoMat[k].matmIndex == idx)
					{
						found = 1;
						break;
					}
				}
				if (found == 0)
				{
					if (m->showGeosets[tex[i].op])
					{
						MeshMap mm;
						mm.regnIndex = tex[i].op;
						mm.matmIndex = idx;
						MeshtoMat.push_back(mm);
					}
				}
			}
		}
	}

	std::vector <uint32> M3TexAnimId;
	std::vector <uint32> M2TexAnimId;
	for(uint32 i=0; i<MATtable.size(); i++) {
		for(uint32 j=0; j<13; j++) {
			if (j == MAT_LAYER_DIFF && (MATtable[i].blend != BM_ADDITIVE_ALPHA && MATtable[i].blend != BM_ADDITIVE)) 
			{
				if (MATtable[i].animid != -1)
				{
					M3TexAnimId.push_back(CreateAnimID(AR_Layer, i, j, 7));
					M2TexAnimId.push_back(MATtable[i].animid);
				}
			}

			if (j == MAT_LAYER_EMISSIVE && (MATtable[i].blend == BM_ADDITIVE_ALPHA || MATtable[i].blend == BM_ADDITIVE)) 
			{
				if (MATtable[i].animid != -1)
				{
					M3TexAnimId.push_back(CreateAnimID(AR_Layer, i, j, 7));
					M2TexAnimId.push_back(MATtable[i].animid);
				}
			}

			if (j == MAT_LAYER_ALPHA && 
				(MATtable[i].blend == BM_ALPHA_BLEND || MATtable[i].blend == BM_ADDITIVE_ALPHA || MATtable[i].blend == BM_TRANSPARENT)) // LAYER_Alpha
			{
				if (MATtable[i].animid != -1)
				{
					M3TexAnimId.push_back(CreateAnimID(AR_Layer, i, j, 7));
					M2TexAnimId.push_back(MATtable[i].animid);
				}
			}
		}
	}
/*
	if (att->children.size() > 0)
	{
		for (uint32 i = 0; i < att->children[0]->children.size(); i++)
		{
			Attachment *att2 = att->children[0]->children[i];
			Model *am = static_cast<Model*>(att2->model);
			if (am)
			{
				MPQFile ampqf(am->modelname);
				MPQFile ampqfv(am->lodname);

				ampqf.close();
				ampqfv.close();
			}
		}
	}
*/

	// mSEQS
	if (modelExport_M3_Anims.size() > 0) {
		logAnimations = modelExport_M3_Anims;
		for(uint32 i=0; i<m->header.nAnimations; i++) {
			SEQS seqs;
			bool bFound = false;
			uint32 pos;
			for(pos=0; pos<logAnimations.size(); pos++) {
				if (logAnimations[pos] == i) {
					bFound = true;
					break;
				}
			}
			if (bFound == false)
				continue;
			wxString strName = modelExport_M3_AnimNames[pos];

			// make name unique
			uint32 counts = 0;
			for(uint32 j=0; j<vAnimations.size(); j++) {
				if (vAnimations[j] == strName) {
					counts ++;
				}
			}
			vAnimations.push_back(strName);
			if (counts > 0)
				strName += wxString::Format(_T(" %02d"), counts);

			nameAnimations.push_back(strName);

			memset(&seqs, 0, sizeof(seqs));
			seqs.init();
			seqs.length = m->anims[i].timeEnd;
			seqs.moveSpeed = m->anims[i].moveSpeed;
			seqs.frequency = m->anims[i].playSpeed;
			seqs.boundSphere.min = fixCoord(m->anims[logAnimations[0]].boundSphere.min) * modelExport_M3_SphereScale;
			seqs.boundSphere.max = fixCoord(m->anims[logAnimations[0]].boundSphere.max) * modelExport_M3_SphereScale;
			seqs.boundSphere.radius = m->anims[logAnimations[0]].boundSphere.radius * modelExport_M3_SphereScale;
			Seqss.push_back(seqs);

		}
	} else {
		for(uint32 i=0; i<m->header.nAnimations; i++) {
			SEQS seqs;
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

			if (strName.StartsWith(_T("Run"))) {
				strName = _T("Walk");
			}
			if (strName.StartsWith(_T("Stand"))) {
				strName = _T("Stand");
			}
			if (strName.StartsWith(_T("Attack"))) {
				strName = _T("Attack");
			}
			if (strName.StartsWith(_T("Death"))) {
				strName = _T("Death");
			}

			// make name unique
			uint32 counts = 0;
			for(uint32 j=0; j<vAnimations.size(); j++) {
				if (vAnimations[j] == strName) {
					counts ++;
				}
			}
			vAnimations.push_back(strName);
			if (counts > 0)
				strName += wxString::Format(_T(" %02d"), counts);

			nameAnimations.push_back(strName);

			logAnimations.push_back(i);

			memset(&seqs, 0, sizeof(seqs));
			seqs.init();
			seqs.length = m->anims[i].timeEnd;
			seqs.moveSpeed = m->anims[i].moveSpeed;
			seqs.frequency = m->anims[i].playSpeed;
			seqs.boundSphere.min = fixCoord(m->anims[logAnimations[0]].boundSphere.min) * modelExport_M3_SphereScale;
			seqs.boundSphere.max = fixCoord(m->anims[logAnimations[0]].boundSphere.max) * modelExport_M3_SphereScale;
			seqs.boundSphere.radius = m->anims[logAnimations[0]].boundSphere.radius * modelExport_M3_SphereScale;
			Seqss.push_back(seqs);
		}
	}

	// init bones
	if (ROOT_BONE == 1) {
		BONE bone;
		memset(&bone, 0, sizeof(BONE));
		bone.init();
		bone.parent = -1;
		bone.initTrans.AnimRef.animid = CreateAnimID(AR_Bone, 0, 0, 2);
		bone.initRot.AnimRef.animid = CreateAnimID(AR_Bone, 0, 0, 3);
		bone.initRot.value = Vec4D(0.0f, 0.0f, -sqrt(0.5f), sqrt(0.5f));
		bone.initScale.AnimRef.animid = CreateAnimID(AR_Bone, 0, 0, 5);
		bone.initScale.value = Vec3D(1.0f, 1.0f, 1.0f)*modelExport_M3_BoundScale;
		bone.ar1.AnimRef.animid = CreateAnimID(AR_Bone, 0, 0, 6);
		
		Bones.push_back(bone);
	}

	for(uint32 i=0; i<m->header.nBones; i++) {
		BONE bone;
		int  idx = Bones.size();
		memset(&bone, 0, sizeof(BONE));
		bone.init();
		bone.parent = mb[i].parent + ROOT_BONE;
		bone.initTrans.AnimRef.animid = CreateAnimID(AR_Bone, idx, 0, 2);
		for (uint32 j=0; j<Seqss.size(); j++)
		{
			int anim_offset = logAnimations[j];
			if (m->bones[i].trans.data[anim_offset].size() > 0)
			{
				SetAnimed(bone.initTrans.AnimRef);
				break;
			}
		}

		bone.initTrans.value = mb[i].pivot;
		if (bone.parent > (ROOT_BONE - 1))
			bone.initTrans.value -= mb[bone.parent - ROOT_BONE].pivot ;

		bone.initRot.AnimRef.animid = CreateAnimID(AR_Bone, idx, 0, 3);
		for (uint32 j=0; j<Seqss.size(); j++)
		{
			int anim_offset = logAnimations[j];
			if (m->bones[i].rot.data[anim_offset].size() > 0)
			{
				SetAnimed(bone.initRot.AnimRef);
				break;
			}
		}

		bone.initScale.AnimRef.animid = CreateAnimID(AR_Bone, idx, 0, 5);
		for (uint32 j=0; j<Seqss.size(); j++)
		{
			int anim_offset = logAnimations[j];
			if (m->bones[i].scale.data[anim_offset].size() > 0)
			{
				SetAnimed(bone.initScale.AnimRef);
				break;
			}
		}
		bone.ar1.AnimRef.animid = CreateAnimID(AR_Bone, idx, 0, 6);
		Bones.push_back(bone);
	}

/************************************************************************************/
/* Write Data                                                                       */
/************************************************************************************/

	// Modelname
	wxString n = wxString(fn, wxConvUTF8).AfterLast('\\');
	NameRefEntry(&mdata.name, n, &f);

	// mSEQ
	for(uint32 i=0; i < Seqss.size(); i++) {
		NameRefEntry(&Seqss[i].name, nameAnimations[i], &f);
	}

	mdata.mSEQS.nEntries = Seqss.size();
	mdata.mSEQS.ref = reList.size();
	RefEntry("SQES", f.Tell(), mdata.mSEQS.nEntries, 1);
	f.Write(&Seqss.front(), sizeof(SEQS) * mdata.mSEQS.nEntries);
	padding(&f);

	// mSTC
	for(uint32 i=0; i < Seqss.size(); i++) {
		int anim_offset = logAnimations[i];
		STC stc;
		memset(&stc, 0, sizeof(stc));
		STCExtra extra;

		M3OpacityAnimid.push_back(std::vector<uint32> ());
		M2OpacityIdx.push_back(std::vector<uint32> ());

		// name
		wxString strName = nameAnimations[i];
		strName.Append(_T("_full"));
		NameRefEntry(&stc.name, strName, &f);

		// animid
		for (uint32 j=0; j<MATtable.size(); j++)
		{
			if (MATtable[j].color < 0)
				continue;
			if ((m->colors[MATtable[j].color].opacity.seq == -1 &&  m->colors[MATtable[j].color].opacity.data[anim_offset].size() > 0) ||
				(m->colors[MATtable[j].color].opacity.seq != -1 &&  m->colors[MATtable[j].color].opacity.data[0].size() > 0))
			{
				for(uint32 k=0; k<13; k++) 
				{

					if (k == MAT_LAYER_EMISSIVE && (MATtable[j].blend == BM_ADDITIVE_ALPHA || MATtable[j].blend == BM_ADDITIVE)) 
					{
						M3OpacityAnimid.back().push_back(CreateAnimID(AR_Layer, j, k, 2));
						M2OpacityIdx.back().push_back(MATtable[j].color);
					}

					if (k == MAT_LAYER_ALPHA && (MATtable[j].blend == BM_OPAQUE || MATtable[j].blend == BM_ALPHA_BLEND || MATtable[j].blend == BM_ADDITIVE_ALPHA))
					{
						M3OpacityAnimid.back().push_back(CreateAnimID(AR_Layer, j, k, 2));
						M2OpacityIdx.back().push_back(MATtable[j].color);
					}
				}
			}
		}

		int16 v2dcount = 0;
		int16 v3dcount = 0;
		int16 v4dcount = 0;
		int16 fcount = 0;
		AnimOffset animoff;
	
		// bone anim id
		for(uint32 j=0; j<m->header.nBones; j++) {
			if (m->bones[j].trans.data[anim_offset].size() > 0) {
				extra.animid.push_back(CreateAnimID(AR_Bone, j+ROOT_BONE, 0, 2));
				animoff.index = v3dcount++;
				animoff.type = STC_INDEX_VEC3D;
				extra.animoff.push_back(animoff);
			}
			if (m->bones[j].scale.data[anim_offset].size() > 0) {
				extra.animid.push_back(CreateAnimID(AR_Bone, j+ROOT_BONE, 0, 5));
				animoff.index = v3dcount++;
				animoff.type = STC_INDEX_VEC3D;
				extra.animoff.push_back(animoff);
			}
			if (m->bones[j].rot.data[anim_offset].size() > 0) {
				extra.animid.push_back(CreateAnimID(AR_Bone, j+ROOT_BONE, 0, 3));
				animoff.index = v4dcount++;
				animoff.type = STC_INDEX_QUAT;
				extra.animoff.push_back(animoff);
			}
		}

		// tex anim id
		for(uint32 j=0; j<M3TexAnimId.size(); j++) {
			extra.animid.push_back(M3TexAnimId[j]);
			animoff.index = v2dcount++;
			animoff.type = STC_INDEX_VEC2D;
			extra.animoff.push_back(animoff);
		}

		// mesh opacity id
		for (uint32 j=0; j<M3OpacityAnimid.back().size(); j++) {
			extra.animid.push_back(M3OpacityAnimid.back()[j]);
			animoff.index = fcount++;
			animoff.type = STC_INDEX_FLOAT;
			extra.animoff.push_back(animoff);
		}

		// particle rate id
		if (bShowParticle  && gameVersion < 40000)
		{
			for (uint32 j=0; j<m->header.nParticleEmitters; j++)
			{
				if (particle[j].en.nTimes > 0 && 
					((m->particleSystems[j].enabled.seq == -1 &&  m->particleSystems[j].enabled.data[anim_offset].size() > 0) ||
					 (m->particleSystems[j].enabled.seq != -1 &&  m->particleSystems[j].enabled.data[0].size() > 0)))
				{
					extra.animid.push_back(CreateAnimID(AR_Par, j, 0, 14));
					animoff.index = fcount++;
					animoff.type = STC_INDEX_FLOAT;
					extra.animoff.push_back(animoff);
				}
			}
		}

		// anim reference id
		stc.animid.nEntries = extra.animid.size();
		if (stc.animid.nEntries > 0)
		{
			stc.animid.ref = reList.size();
			RefEntry("_23U", f.Tell(), stc.animid.nEntries, 0);
			f.Write(&extra.animid.front(), sizeof(uint32) * stc.animid.nEntries);
			padding(&f);
		}

		// animindex
		stc.animindex.nEntries = extra.animoff.size();
		if (stc.animindex.nEntries > 0){
			stc.animindex.ref = reList.size();
			RefEntry("_23U", f.Tell(), stc.animindex.nEntries, 0);
			f.Write(&extra.animoff.front(), sizeof(AnimOffset) * stc.animindex.nEntries);
			padding(&f);
		}

		// Events, VEDS
		{
			SD sd;
			memset(&sd, 0, sizeof(sd));
			sd.init();
			for(uint32 j=0; j<m->header.nBones; j++) {
				if (m->bones[j].trans.data[anim_offset].size() > 0) {
					sd.length = Seqss[i].length;  
					break;
				}
			}

			sd.timeline.nEntries = 1;
			sd.timeline.ref = reList.size();
			RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
			f.Write(&sd.length, sizeof(int32));
			padding(&f);

			EVNT evnt;
			memset(&evnt, 0, sizeof(evnt));
			evnt.init();

			// name
			strName = _T("Evt_SeqEnd");
			NameRefEntry(&evnt.name, strName, &f);

			sd.data.nEntries = 1;
			sd.data.ref = reList.size();
			RefEntry("TNVE", f.Tell(), sd.data.nEntries, 0);
			f.Write(&evnt, sizeof(evnt));
			padding(&f);

			stc.Events.nEntries = 1;
			stc.Events.ref = reList.size();
			RefEntry("VEDS", f.Tell(), stc.Events.nEntries, 0);
			f.Write(&sd, sizeof(sd));
			padding(&f);
		}

		// V2DS
		for(uint32 j=0; j<M3TexAnimId.size(); j++) {
			SD sd;
			memset(&sd, 0, sizeof(sd));
			sd.init();
			Anim_Vec2D av2d;

			for (uint32 k=0; k < m->texAnims[M2TexAnimId[j]].trans.times[0].size(); k++) {
				av2d.timeline.push_back(m->texAnims[M2TexAnimId[j]].trans.times[0][k]);
				Vec2D tran;
				tran.x = -m->texAnims[M2TexAnimId[j]].trans.data[0][k].x;
				tran.y = -m->texAnims[M2TexAnimId[j]].trans.data[0][k].y;
				av2d.data.push_back(tran);
			}

			sd.length = m->texAnims[M2TexAnimId[j]].trans.times[0][1];

			sd.timeline.nEntries = av2d.timeline.size();
			sd.timeline.ref = reList.size();
			RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
			f.Write(&av2d.timeline.front(), sizeof(int32) * sd.timeline.nEntries);
			padding(&f);

			sd.data.nEntries = av2d.data.size();
			sd.data.ref = reList.size();
			RefEntry("2CEV", f.Tell(), sd.data.nEntries, 0);
			f.Write(&av2d.data.front(), sizeof(Vec2D) * sd.data.nEntries);
			padding(&f);

			extra.animsdvec2d.push_back(sd);
			extra.animvec2d.push_back(av2d);
		}
		
		stc.arVec2D.nEntries = extra.animvec2d.size();
		if (stc.arVec2D.nEntries > 0) {
			stc.arVec2D.ref = reList.size();
			RefEntry("V2DS", f.Tell(), stc.arVec2D.nEntries, 0);
			f.Write(&extra.animsdvec2d.front(), sizeof(SD) * stc.arVec2D.nEntries);
			padding(&f);
		}


		// Trans and Scale, V3DS
		for(uint32 j=0; j<m->header.nBones; j++) {
			// trans
			if (m->bones[j].trans.data[anim_offset].size() > 0) {
				SD sd;
				memset(&sd, 0, sizeof(sd));
				sd.init();
				Anim_Vec3D av3d;

				int counts = m->bones[j].trans.data[anim_offset].size();
				for (int k=0; k<counts; k++) {
					av3d.timeline.push_back(m->bones[j].trans.times[anim_offset][k]);
					Vec3D tran;
					if (m->bones[j].parent > -1)
						tran = m->bones[j].pivot - m->bones[m->bones[j].parent].pivot;
					else
						tran = m->bones[j].pivot;
					tran += m->bones[j].trans.data[anim_offset][k];
					tran.z *= -1.0f;
					av3d.data.push_back(Vec3D(tran.x, tran.z, tran.y));
				}

				sd.length = Seqss[i].length; 

				sd.timeline.nEntries = counts;
				sd.timeline.ref = reList.size();
				RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
				f.Write(&av3d.timeline.front(), sizeof(int32) * sd.timeline.nEntries);
				padding(&f);

				sd.data.nEntries = counts;
				sd.data.ref = reList.size();
				RefEntry("3CEV", f.Tell(), sd.data.nEntries, 0);
				f.Write(&av3d.data.front(), sizeof(Vec3D) * sd.data.nEntries);
				padding(&f);

				extra.animsdvec3d.push_back(sd);
				extra.animvec3d.push_back(av3d);
			}
			//scale
			if (m->bones[j].scale.data[anim_offset].size() > 0) {
				SD sd;
				memset(&sd, 0, sizeof(sd));
				sd.init();
				Anim_Vec3D av3d;

				int counts = m->bones[j].scale.data[anim_offset].size();
				for (int k=0; k<counts; k++) {
					av3d.timeline.push_back(m->bones[j].scale.times[anim_offset][k]);
					Vec3D scale;
					scale.x = m->bones[j].scale.data[anim_offset][k].x;
					scale.y = m->bones[j].scale.data[anim_offset][k].z;
					scale.z = m->bones[j].scale.data[anim_offset][k].y;
					av3d.data.push_back(scale);
				}

				sd.length = Seqss[i].length;

				sd.timeline.nEntries = counts;
				sd.timeline.ref = reList.size();
				RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
				f.Write(&av3d.timeline.front(), sizeof(int32) * sd.timeline.nEntries);
				padding(&f);

				sd.data.nEntries = counts;
				sd.data.ref = reList.size();
				RefEntry("3CEV", f.Tell(), sd.data.nEntries, 0);
				f.Write(&av3d.data.front(), sizeof(Vec3D) * sd.data.nEntries);
				padding(&f);

				extra.animsdvec3d.push_back(sd);
				extra.animvec3d.push_back(av3d);
			}
		}

		stc.arVec3D.nEntries = extra.animvec3d.size();
		if (stc.arVec3D.nEntries > 0) {
			stc.arVec3D.ref = reList.size();
			RefEntry("V3DS", f.Tell(), stc.arVec3D.nEntries, 0);
			f.Write(&extra.animsdvec3d.front(), sizeof(SD) * stc.arVec3D.nEntries);
			padding(&f);
		}


		// Rot, Q4DS
		for(uint32 j=0; j<m->header.nBones; j++) {
			if (m->bones[j].rot.data[anim_offset].size() > 0) {
				SD sd;
				memset(&sd, 0, sizeof(sd));
				sd.init();
				Anim_Vec4D av4d;

				int counts = m->bones[j].rot.data[anim_offset].size();
				for (int k=0; k<counts; k++) {
					av4d.timeline.push_back(m->bones[j].rot.times[anim_offset][k]);
					Vec4D rot;
					rot.x = -m->bones[j].rot.data[anim_offset][k].x;
					rot.y = m->bones[j].rot.data[anim_offset][k].z;
					rot.z = -m->bones[j].rot.data[anim_offset][k].y;
					rot.w = m->bones[j].rot.data[anim_offset][k].w;
					av4d.data.push_back(rot);
				}

				sd.length = Seqss[i].length;

				sd.timeline.nEntries = counts;
				sd.timeline.ref = reList.size();
				RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
				f.Write(&av4d.timeline.front(), sizeof(int32) * sd.timeline.nEntries);
				padding(&f);

				sd.data.nEntries = counts;
				sd.data.ref = reList.size();
				RefEntry("TAUQ", f.Tell(), sd.data.nEntries, 0);
				f.Write(&av4d.data.front(), sizeof(Vec4D) * sd.data.nEntries);
				padding(&f);

				extra.animsdvec4d.push_back(sd);
				extra.animvec4d.push_back(av4d);
			}
		}

		stc.arQuat.nEntries = extra.animvec4d.size();
		if (stc.arQuat.nEntries > 0) {
			stc.arQuat.ref = reList.size();
			RefEntry("Q4DS", f.Tell(), stc.arQuat.nEntries, 0);
			f.Write(&extra.animsdvec4d.front(), sizeof(SD) * stc.arQuat.nEntries);
			padding(&f);
		}

		// Float, 3RDS
		for (uint32 j=0; j<M3OpacityAnimid.back().size(); j++)
		{
			if ((m->colors[M2OpacityIdx.back()[j]].opacity.seq == -1 &&  m->colors[M2OpacityIdx.back()[j]].opacity.data[anim_offset].size() > 0) ||
			    (m->colors[M2OpacityIdx.back()[j]].opacity.seq != -1 &&  m->colors[M2OpacityIdx.back()[j]].opacity.data[0].size() > 0))
			{
				SD sd;
				memset(&sd, 0, sizeof(sd));
				sd.init();
				Anim_Float af;

				int animidx;
				if (m->colors[M2OpacityIdx.back()[j]].opacity.seq == -1)
					animidx = anim_offset;
				else
					animidx = 0;
				int counts = m->colors[M2OpacityIdx.back()[j]].opacity.data[animidx].size();
				for (int k=0; k<counts; k++) {
					af.timeline.push_back(m->colors[M2OpacityIdx.back()[j]].opacity.times[animidx][k]);
					af.data.push_back(m->colors[M2OpacityIdx.back()[j]].opacity.data[animidx][k]);
				}

				sd.length = Seqss[i].length;

				sd.timeline.nEntries = counts;
				sd.timeline.ref = reList.size();
				RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
				f.Write(&af.timeline.front(), sizeof(int32) * sd.timeline.nEntries);
				padding(&f);

				sd.data.nEntries = counts;
				sd.data.ref = reList.size();
				RefEntry("LAER", f.Tell(), sd.data.nEntries, 0);
				f.Write(&af.data.front(), sizeof(float) * sd.data.nEntries);
				padding(&f);

				extra.animsdfloat.push_back(sd);
				extra.animfloat.push_back(af);
			}
		}

		// particle rate anim
		if (bShowParticle  && gameVersion < 40000)
		{
			for (uint32 j=0; j<m->header.nParticleEmitters; j++)
			{
				if (particle[j].en.nTimes > 0)
				{
					if ((m->particleSystems[j].enabled.seq == -1 &&  m->particleSystems[j].enabled.data[anim_offset].size() > 0) ||
						(m->particleSystems[j].enabled.seq != -1 &&  m->particleSystems[j].enabled.data[0].size() > 0))
					{
						SD sd;
						memset(&sd, 0, sizeof(sd));
						sd.init();
						Anim_Float af;

						int animidx;
						if (m->particleSystems[j].enabled.seq == -1)
							animidx = anim_offset;
						else
							animidx = 0;
						int counts = m->particleSystems[j].enabled.data[animidx].size();
						for (int k=0; k<counts; k++) {
							af.timeline.push_back(m->particleSystems[j].enabled.times[animidx][k]);
							float rate;
							if (m->particleSystems[j].enabled.data[animidx][k] && m->particleSystems[j].rate.data[0].size() > 0)
								rate = m->particleSystems[i].rate.data[0][0];
							else
								rate = 0;	
							af.data.push_back(rate);
						}

						sd.length = Seqss[i].length;

						sd.timeline.nEntries = counts;
						sd.timeline.ref = reList.size();
						RefEntry("_23I", f.Tell(), sd.timeline.nEntries, 0);
						f.Write(&af.timeline.front(), sizeof(int32) * sd.timeline.nEntries);
						padding(&f);

						sd.data.nEntries = counts;
						sd.data.ref = reList.size();
						RefEntry("LAER", f.Tell(), sd.data.nEntries, 0);
						f.Write(&af.data.front(), sizeof(float) * sd.data.nEntries);
						padding(&f);

						extra.animsdfloat.push_back(sd);
						extra.animfloat.push_back(af);
					}
				}
			}
		}

		stc.arFloat.nEntries = extra.animfloat.size();
		if (stc.arFloat.nEntries > 0) {
			stc.arFloat.ref = reList.size();
			RefEntry("3RDS", f.Tell(), stc.arFloat.nEntries, 0);
			f.Write(&extra.animsdfloat.front(), sizeof(SD) * stc.arFloat.nEntries);
			padding(&f);
		}

		Stcs.push_back(stc);
		StcExtras.push_back(extra);
	}

	mdata.mSTC.nEntries = Seqss.size();
	mdata.mSTC.ref = reList.size();
	RefEntry("_CTS", f.Tell(), mdata.mSTC.nEntries, 4);
	f.Write(&Stcs.front(), sizeof(STC) * mdata.mSTC.nEntries);
	padding(&f);

	// mSTG
	mdata.mSTG.nEntries = mdata.mSEQS.nEntries;
	mdata.mSTG.ref = reList.size();
	RefEntry("_GTS", f.Tell(), mdata.mSTG.nEntries, 0);
	chunk_offset = f.Tell();
	STG *stgs = new STG[mdata.mSTG.nEntries];
	memset(stgs, 0, sizeof(STG)*mdata.mSTG.nEntries);
	f.Seek(sizeof(STG)*mdata.mSTG.nEntries, wxFromCurrent);
	padding(&f);
	for(uint32 i=0; i<mdata.mSTG.nEntries; i++) {
		// name
		NameRefEntry(&stgs[i].name, nameAnimations[i], &f);
		
		// stcID
		stgs[i].stcID.nEntries = 1;
		stgs[i].stcID.ref = reList.size();
		RefEntry("_23U", f.Tell(), stgs[i].stcID.nEntries, 0);
		f.Write(&i, sizeof(uint32));
		padding(&f);
	}
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mSTG.nEntries; i++) {
		f.Write(&stgs[i], sizeof(STG));
	}
	wxDELETEA(stgs);
	f.Seek(0, wxFromEnd);

	// mSTS
	mdata.mSTS.nEntries = mdata.mSEQS.nEntries;
	mdata.mSTS.ref = reList.size();
	RefEntry("_STS", f.Tell(), mdata.mSTS.nEntries, 0);
	chunk_offset = f.Tell();
	STS *stss = new STS[mdata.mSTS.nEntries];
	memset(stss, 0, sizeof(STS)*mdata.mSTS.nEntries);
	f.Seek(sizeof(STS)*mdata.mSTS.nEntries, wxFromCurrent);
	padding(&f);
	for(uint32 i=0; i<mdata.mSTS.nEntries; i++) {
		int anim_offset = logAnimations[i];

		stss[i].animid.nEntries = StcExtras[i].animid.size();
		if (stss[i].animid.nEntries) {
			stss[i].animid.ref = reList.size();
			RefEntry("_23U", f.Tell(), stss[i].animid.nEntries, 0);

			for(uint32 j=0; j<m->header.nBones; j++) {
				if (m->bones[j].trans.data[anim_offset].size() > 0) {
					uint32 p = CreateAnimID(AR_Bone, j+ROOT_BONE, 0, 2);
					f.Write(&p, sizeof(uint32));
				}
				if (m->bones[j].scale.data[anim_offset].size() > 0) {
					uint32 p = CreateAnimID(AR_Bone, j+ROOT_BONE, 0, 5);
					f.Write(&p, sizeof(uint32));
				}
				if (m->bones[j].rot.data[anim_offset].size() > 0) {
					uint32 p = CreateAnimID(AR_Bone, j+ROOT_BONE, 0, 3);
					f.Write(&p, sizeof(uint32));
				}
			}
			for(uint32 j=0; j<M3TexAnimId.size(); j++) 
			{
				uint32 p = M3TexAnimId[j];
				f.Write(&p, sizeof(uint32));
			}

			for (uint32 j=0; j<M3OpacityAnimid[i].size(); j++)
			{
				uint32 p = M3OpacityAnimid[i][j];
				f.Write(&p, sizeof(uint32));
			}

			if (bShowParticle  && gameVersion < 40000)
			{
				for (uint32 j=0; j<m->header.nParticleEmitters; j++)
				{
					if (particle[j].en.nTimes > 0 && 
					    ((m->particleSystems[j].enabled.seq == -1 &&  m->particleSystems[j].enabled.data[anim_offset].size() > 0) ||
					     (m->particleSystems[j].enabled.seq != -1 &&  m->particleSystems[j].enabled.data[0].size() > 0)))
					{
						uint32 p = CreateAnimID(AR_Par, j, 0, 14);
						f.Write(&p, sizeof(uint32));
					}
				}
			}
			padding(&f);
		}
	}
	f.Seek(chunk_offset, wxFromStart);
	for(uint32 i=0; i<mdata.mSTS.nEntries; i++) {
		stss[i].init();
		f.Write(&stss[i], sizeof(STS));
	}
	wxDELETEA(stss);
	f.Seek(0, wxFromEnd);

	// mBone
	for(uint32 i=0; i<Bones.size(); i++) {
		// name
		wxString strName = wxString(fn, wxConvUTF8).AfterLast(SLASH).BeforeLast('.')+_T('_');
		if (i < ROOT_BONE)
			strName += _T("Bone_Root");
		else
			strName += wxString::Format(_T("Bone%d"), i-ROOT_BONE);

		for(uint32 j=0; j < BONE_MAX; j++) {
			if (i >= ROOT_BONE && m->keyBoneLookup[j] == i-ROOT_BONE) {
				strName += _T("_")+Bone_Names[j];
				break;
			}
		}
		NameRefEntry(&Bones[i].name, strName, &f);
	}

	mdata.mBone.nEntries = Bones.size();
	mdata.mBone.ref = reList.size();
	RefEntry("ENOB", f.Tell(), mdata.mBone.nEntries, 1);
	f.Write(&Bones.front(), sizeof(BONE) * mdata.mBone.nEntries);
	padding(&f);

	// nSkinnedBones
	mdata.nSkinnedBones = nSkinnedBones(m, &mpqf);

	// mVert
	mdata.mVert.nEntries = Verts.size() * sizeof(Vertex32);//m->header.nVertices*sizeof(Vertex32);
	mdata.mVert.ref = reList.size();
	RefEntry("__8U", f.Tell(), mdata.mVert.nEntries, 0);
	f.Write(&Verts.front(), mdata.mVert.nEntries);
	padding(&f);

	// mDIV
	mdata.mDIV.nEntries = 1;
	mdata.mDIV.ref = reList.size();
	RefEntry("_VID", f.Tell(), mdata.mDIV.nEntries, 2);
	chunk_offset = f.Tell();
	DIV div;
	memset(&div, 0, sizeof(div));
	div.init();
	f.Seek(sizeof(div), wxFromCurrent);
	padding(&f);

	// mDiv.Faces
	div.faces.nEntries = Faces.size(); //view->nTris;
	div.faces.ref = reList.size();
	RefEntry("_61U", f.Tell(), div.faces.nEntries, 0);
	f.Write(&Faces.front(), sizeof(uint16) * div.faces.nEntries);
	padding(&f);

	// mDiv.meash
	div.REGN.nEntries = Regns.size(); //view->nSub;
	div.REGN.ref = reList.size();
	RefEntry("NGER", f.Tell(), div.REGN.nEntries, 3);
	f.Write(&Regns.front(), sizeof(REGN) * div.REGN.nEntries);
	padding(&f);

	// mDiv.BAT
	div.BAT.nEntries = MeshtoMat.size();
	div.BAT.ref = reList.size();
	RefEntry("_TAB", f.Tell(), div.BAT.nEntries, 1);
	for (size_t j=0; j<div.BAT.nEntries; j++) {
		BAT bat;
		memset(&bat, 0, sizeof(bat));
		bat.init();
		bat.regnIndex = MeshM2toM3[MeshtoMat[j].regnIndex];
		bat.matmIndex = MeshtoMat[j].matmIndex;
		f.Write(&bat, 0xE); // sizeof(bat) is buggy to 0x10
	}
	padding(&f);

	// mDiv.MSEC
	div.MSEC.nEntries = 1;
	div.MSEC.ref = reList.size();
	RefEntry("CESM", f.Tell(), div.MSEC.nEntries, 1);
	MSEC msec;
	memset(&msec, 0, sizeof(msec));
	msec.bndSphere.AnimRef.animid = CreateAnimID(AR_MSEC, 0, 0, 1);
	f.Write(&msec, sizeof(msec));
	padding(&f);
	f.Seek(chunk_offset, wxFromStart);
	f.Write(&div, sizeof(div));
	f.Seek(0, wxFromEnd);

	// mBoneLU
	mdata.mBoneLU.nEntries = bLookup.size();
	mdata.mBoneLU.ref = reList.size();
	RefEntry("_61U", f.Tell(), mdata.mBoneLU.nEntries, 0);
	f.Write(&bLookup.front(), sizeof(uint16) * mdata.mBoneLU.nEntries );
	padding(&f);

	// boundSphere, m->header.boundSphere is too big
	int anim_offset = logAnimations[0];
	mdata.boundSphere.min = fixCoord(m->anims[anim_offset].boundSphere.min) * modelExport_M3_SphereScale;
	mdata.boundSphere.max = fixCoord(m->anims[anim_offset].boundSphere.max) * modelExport_M3_SphereScale;
	mdata.boundSphere.radius = m->anims[anim_offset].boundSphere.radius * modelExport_M3_SphereScale;

	// mAttach
	// this makes some read errors in sc2 editor
	wxArrayString AttRefName;

	if (m->header.nAttachments) {
		ModelAttachmentDef *attachments = (ModelAttachmentDef*)(mpqf.getBuffer() + m->header.ofsAttachments);
		mdata.mAttach.nEntries = m->header.nAttachments;
		mdata.mAttach.ref = reList.size();
		RefEntry("_TTA", f.Tell(), mdata.mAttach.nEntries, 1);
		chunk_offset = f.Tell();
		ATT *atts = new ATT[mdata.mAttach.nEntries];
		memset(atts, 0, sizeof(ATT)*mdata.mAttach.nEntries);
		f.Seek(sizeof(ATT)*mdata.mAttach.nEntries, wxFromCurrent);
		padding(&f);
		for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
			// name
			wxString strName = _T("Ref_Hardpoint");

			if (attachments[i].id < WXSIZEOF(M3_Attach_Names))
				strName = wxString(M3_Attach_Names[attachments[i].id], wxConvUTF8);

			AttRefName.push_back(strName);

			int count = 0;
			for(uint32 j=0; j < AttRefName.size(); j++) {
				if (AttRefName[j] == strName)
					count++;
			}

			if (count > 1)
				strName += wxString::Format(_T(" %02d"), count - 1);

			NameRefEntry(&atts[i].name, strName, &f);
		}
		f.Seek(chunk_offset, wxFromStart);
		for(uint32 i=0; i<mdata.mAttach.nEntries; i++) {
			atts[i].init();
			atts[i].bone = attachments[i].bone + ROOT_BONE;
			f.Write(&atts[i], sizeof(ATT));
		}
		wxDELETEA(atts);
		f.Seek(0, wxFromEnd);
	}

	// mAttachLU
	if (mdata.mAttach.nEntries) {
		mdata.mAttachLU.nEntries = mdata.mAttach.nEntries;
		mdata.mAttachLU.ref = reList.size();
		RefEntry("_61U", f.Tell(), mdata.mAttachLU.nEntries, 0);
		for(uint16 i=0; i<mdata.mAttachLU.nEntries; i++) {
			int16 ii = -1;
			f.Write(&ii, sizeof(int16));
		}
		padding(&f);
	}

	uint32 partexstart = MATtable.size();
	std::vector <int32> M3ParticleMap;
	if (bShowParticle && gameVersion < 40000)
	{
		// prepare particle texture
		for (uint32 i=0; i < m->header.nParticleEmitters; i++)
		{
			int idx = -1;
			if (particle[i].texture >= 0)
			{
				for (uint32 j = partexstart; j < MATtable.size(); j++)
				{
					if (MATtable[j].texid == particle[i].texture)
					{
						idx = j;
						break;
					}
				}
				if (idx == -1)
				{
					idx = MATtable.size();
					MATmap bm;
					bm.texid = particle[i].texture;
					bm.flags = 0;
					bm.blend = BM_ADDITIVE_ALPHA; //particle[i].blend;
					bm.animid = -1;
					bm.color = -1;
					MATtable.push_back(bm);
				}
			}
			M3ParticleMap.push_back(idx);
		}
	}


	// mMatLU
	if (MATtable.size() > 0) {
		mdata.mMatLU.nEntries = MATtable.size();
		mdata.mMatLU.ref = reList.size();
		RefEntry("MTAM", f.Tell(), mdata.mMatLU.nEntries, 0);
		for(uint32 i=0; i<mdata.mMatLU.nEntries; i++) {
			MATM matm;
			matm.init();
			matm.matind = i;
			f.Write(&matm, sizeof(matm));
		}
		padding(&f);
	}

	// mMat
	if (MATtable.size() > 0) {
		mdata.mMat.nEntries = MATtable.size();
		mdata.mMat.ref = reList.size();
		RefEntry("_TAM", f.Tell(), mdata.mMat.nEntries, 0xF);
		chunk_offset = f.Tell();
		MAT *mats = new MAT[mdata.mMat.nEntries];
		memset(mats, 0, sizeof(MAT)*mdata.mMat.nEntries);
		f.Seek(sizeof(MAT)*mdata.mMat.nEntries, wxFromCurrent);
		padding(&f);
		for(uint32 i=0; i<mdata.mMat.nEntries; i++) {
			// name
			wxString ff = wxString(fn, wxConvUTF8).BeforeLast('.').AfterLast(SLASH);
			wxString strName = wxString::Format(_T("%sMat_%02d"), ff.c_str(), i+1);
			NameRefEntry(&mats[i].name, strName, &f);

			// layers
			int chunk_offset2;
			for(uint32 j=0; j<13; j++) {
				mats[i].layers[j].nEntries = 1;
				mats[i].layers[j].ref = reList.size();
				RefEntry("RYAL", f.Tell(), mats[i].layers[j].nEntries, 0x16);
				chunk_offset2 = f.Tell();
				LAYR layer;
				memset(&layer, 0, sizeof(layer));
				layer.init();

				layer.Colour.AnimRef.animid =			CreateAnimID(AR_Layer, i, j, 1);
				layer.brightness_mult1.AnimRef.animid = CreateAnimID(AR_Layer, i, j, 2);
				layer.brightness_mult2.AnimRef.animid = CreateAnimID(AR_Layer, i, j, 3);
				layer.ar1.AnimRef.animid =				CreateAnimID(AR_Layer, i, j, 4);
				layer.ar2.AnimRef.animid =				CreateAnimID(AR_Layer, i, j, 5);
				layer.ar3.AnimRef.animid =				CreateAnimID(AR_Layer, i, j, 6);
				layer.ar4.AnimRef.animid =				CreateAnimID(AR_Layer, i, j, 7);
				layer.uvAngle.AnimRef.animid =			CreateAnimID(AR_Layer, i, j, 8);
				layer.uvTiling.AnimRef.animid =			CreateAnimID(AR_Layer, i, j, 9);
				layer.ar5.AnimRef.animid =				CreateAnimID(AR_Layer, i, j, 10);
				layer.ar6.AnimRef.animid =				CreateAnimID(AR_Layer, i, j, 11);
				layer.brightness.AnimRef.animid =		CreateAnimID(AR_Layer, i, j, 12);
				
				f.Write(&layer, sizeof(layer));
				padding(&f);

				int texid = MATtable[i].texid;
				wxString texName = m->TextureList[texid].BeforeLast('.').AfterLast(SLASH) + _T(".tga");

				if (modelExport_M3_TexturePath.Len() > 0)
				{
					if (modelExport_M3_TexturePath.Last() != '/' && modelExport_M3_TexturePath.Last() != '\\')
						texName = modelExport_M3_TexturePath + SLASH + texName;
					else
						texName = modelExport_M3_TexturePath + texName;
				}

				if (j == MAT_LAYER_DIFF && (MATtable[i].blend != BM_ADDITIVE_ALPHA && MATtable[i].blend != BM_ADDITIVE)) 
				{
					NameRefEntry(&layer.name, texName, &f);

					if (MATtable[i].animid != -1)
						SetAnimed(layer.ar4.AnimRef);
					if (bShowParticle && gameVersion < 40000 && i >= partexstart)
						layer.flags |= LAYR_FLAGS_SPLIT;
				}

				if (j == MAT_LAYER_EMISSIVE && (MATtable[i].blend == BM_ADDITIVE_ALPHA || MATtable[i].blend == BM_ADDITIVE)) 
				{
					NameRefEntry(&layer.name, texName, &f);

					if (MATtable[i].animid != -1)
						SetAnimed(layer.ar4.AnimRef);
					if (MATtable[i].color != -1)
					{
						if (m->colors[MATtable[i].color].opacity.sizes != 0)
							SetAnimed(layer.brightness_mult1.AnimRef);
					}
					if (bShowParticle && gameVersion < 40000 && i >= partexstart)
						layer.flags |= LAYR_FLAGS_SPLIT;
				}

				if (j == MAT_LAYER_ALPHA && MATtable[i].blend == BM_OPAQUE && MATtable[i].color != -1)
				{
					NameRefEntry(&layer.name, _T("NoTexture"), &f);
					layer.alphaFlags = LAYR_ALPHAFLAGS_ALPHAONLY;
					SetAnimed(layer.brightness_mult1.AnimRef);
					if (MATtable[i].eye == 1)
						layer.brightness_mult1.value = 0;
				}
				else if (j == MAT_LAYER_ALPHA && 
					(MATtable[i].blend == BM_ALPHA_BLEND || MATtable[i].blend == BM_ADDITIVE_ALPHA || MATtable[i].blend == BM_TRANSPARENT))
				{
					layer.alphaFlags = LAYR_ALPHAFLAGS_ALPHAONLY;
					NameRefEntry(&layer.name, texName, &f);

					if (MATtable[i].animid != -1)
						SetAnimed(layer.ar4.AnimRef);
					if (MATtable[i].color != -1)
						SetAnimed(layer.brightness_mult1.AnimRef);
					if (bShowParticle && gameVersion < 40000 && i >= partexstart)
						layer.flags |= LAYR_FLAGS_SPLIT;
				}

				f.Seek(chunk_offset2, wxFromStart);
				f.Write(&layer, sizeof(layer));
				f.Seek(0, wxFromEnd);
			}
		}
		f.Seek(chunk_offset, wxFromStart);
		for(uint32 i=0; i<mdata.mMat.nEntries; i++) {
			mats[i].init();

			mats[i].ar1.AnimRef.animid = CreateAnimID(AR_Mat, i, 0, 1);
			mats[i].ar2.AnimRef.animid = CreateAnimID(AR_Mat, i, 0, 2);

			switch(MATtable[i].blend)
			{
				case BM_OPAQUE: 
					mats[i].blendMode = MAT_BLEND_OPAQUE; 
					mats[i].cutoutThresh = 1;
					break;
				case BM_TRANSPARENT: 
					mats[i].blendMode = MAT_BLEND_OPAQUE; 
					mats[i].cutoutThresh = 180;
					break;
				case BM_ALPHA_BLEND: 
					mats[i].blendMode = MAT_BLEND_ALPHABLEND; 
					mats[i].cutoutThresh = 16;
					break;
				case BM_ADDITIVE: 
					mats[i].blendMode = MAT_BLEND_ADD; 
					mats[i].cutoutThresh = 0;
					break;
				case BM_ADDITIVE_ALPHA: 
					mats[i].blendMode = MAT_BLEND_ALPHAADD; 
					mats[i].cutoutThresh = 16;
					break;
				case BM_MODULATE: 
					mats[i].blendMode = MAT_BLEND_MOD; 
					mats[i].cutoutThresh = 0;
					break;
				default: 
					mats[i].blendMode = MAT_BLEND_OPAQUE;
					mats[i].cutoutThresh = 0;
			}

			if (MATtable[i].flags & RENDERFLAGS_UNLIT)
				mats[i].flags |= MAT_FLAG_UNSHADED;
			if (MATtable[i].flags & RENDERFLAGS_UNFOGGED)
				mats[i].flags |= MAT_FLAG_UNFOGGED;
			if (MATtable[i].flags & RENDERFLAGS_TWOSIDED)
				mats[i].flags |= MAT_FLAG_TWOSIDED;
			if (MATtable[i].flags & RENDERFLAGS_BILLBOARD)
				mats[i].flags |= MAT_FLAG_DEPTHPREPASS;
			f.Write(&mats[i], sizeof(MAT));
		}
		wxDELETE(mats);
		f.Seek(0, wxFromEnd);
	}

	if (bShowParticle && gameVersion < 40000)
	{
		// mPar
		mdata.mPar.nEntries = m->header.nParticleEmitters;
		mdata.mPar.ref = reList.size();
		RefEntry("_RAP", f.Tell(), mdata.mPar.nEntries, 12);
		for(uint32 i=0; i<mdata.mPar.nEntries; i++) {
			PAR par;
			memset(&par, 0, sizeof(PAR));

			par.emisSpeedStart.AnimRef.animid = CreateAnimID(AR_Par, i, 0, 1);
			par.speedVariation.AnimRef.animid = CreateAnimID(AR_Par, i, 0, 2);
			par.yAngle.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 3);
			par.xAngle.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 4);
			par.xSpread.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 5);
			par.ySpread.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 6);
			par.lifespan.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 7);
			par.decay.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 8);
			par.scale1.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 9);
			par.speedUnk1.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 10);
			par.col1Start.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 11);
			par.col1Mid.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 12);
			par.col1End.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 13);
			par.emissionRate.AnimRef.animid =	CreateAnimID(AR_Par, i, 0, 14);
			par.emissionArea.AnimRef.animid =	CreateAnimID(AR_Par, i, 0, 15);
			par.tailUnk1.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 16);
			par.pivotSpread.AnimRef.animid =	CreateAnimID(AR_Par, i, 0, 17);
			par.spreadUnk1.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 18);
			par.ar19.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 19);
			par.rotate.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 20);
			par.col2Start.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 21);
			par.col2Mid.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 22);
			par.col2End.AnimRef.animid =		CreateAnimID(AR_Par, i, 0, 23);
			par.ar24.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 24);
			par.ar25.AnimRef.animid =			CreateAnimID(AR_Par, i, 0, 25);
			par.ar26.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 26);
			par.ar27.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 27);
			par.ar28.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 28);
			par.ar29.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 29);
			par.ar30.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 30);
			par.ar31.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 31);
			par.ar32.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 32);
			par.ar33.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 33);
			par.ar34.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 34);
			par.ar35.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 35);
			par.ar36.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 36);
			par.ar37.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 37);
			par.ar38.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 38);
			par.ar39.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 39);
			par.ar40.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 40);
			par.ar41.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 41);
			par.ar42.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 42);
			par.ar43.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 43);
			par.ar44.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 44);
			par.ar45.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 45);
			par.ar46.AnimRef.animid = 			CreateAnimID(AR_Par, i, 0, 46);

			par.matmIndex = M3ParticleMap[i];
			par.bone = particle[i].bone + 1;
			par.maxParticles = 1000;
			par.emisSpeedStart.value = m->particleSystems[i].speed.data[0][0];  //particle[i].EmissionSpeed;
			par.emisSpeedMid = m->particleSystems[i].speed.data[0][0];
			par.emisSpeedEnd = m->particleSystems[i].speed.data[0][0];
			par.speedVariation.value = m->particleSystems[i].variation.data[0][0] * m->particleSystems[i].speed.data[0][0];  //particle[i].SpeedVariation.
			par.xSpread.value = m->particleSystems[i].spread.data[0][0];
			par.ySpread.value = m->particleSystems[i].spread.data[0][0]; //m->particleSystems[i].lat.data[0][0];
			par.lifespan.value = m->particleSystems[i].lifespan.data[0][0];
			par.scaleRatio = 0.5f;
			par.scale1.value.x = m->particleSystems[i].sizes[0] * 2.0f;//particle[i].p.scales[0];
			par.scale1.value.y = m->particleSystems[i].sizes[1] * 2.0f;//particle[i].p.scales[1];
			par.scale1.value.z = m->particleSystems[i].sizes[2] * 2.0f;//particle[i].p.scales[2];
			Vec3D  colors2[3];
			uint16 opacity[3];
			memcpy(colors2, mpqf.getBuffer()+particle[i].p.colors.ofsKeys, sizeof(Vec3D)*3);
			memcpy(opacity, mpqf.getBuffer()+particle[i].p.opacity.ofsKeys, sizeof(uint16)*3);
			par.col1Start.value[0] = (int)colors2[0].z;
			par.col1Start.value[1] = (int)colors2[0].y;
			par.col1Start.value[2] = (int)colors2[0].x;
			par.col1Start.value[3] = (int)opacity[0] >> 7;
			par.col1Mid.value[0] = (int)colors2[1].z;
			par.col1Mid.value[1] = (int)colors2[1].y;
			par.col1Mid.value[2] = (int)colors2[1].x;
			par.col1Mid.value[3] = (int)opacity[1] >> 7;
			par.col1End.value[0] = (int)colors2[2].z;
			par.col1End.value[1] = (int)colors2[2].y;
			par.col1End.value[2] = (int)colors2[2].x;
			par.col1End.value[3] = (int)opacity[2] >> 7;


			par.emissionArea.value.x = m->particleSystems[i].areaw.data[0][0];
			par.emissionArea.value.y = m->particleSystems[i].areal.data[0][0];

			par.f9[0] = 1.0f;
			par.f9[1] = 1.0f;

			par.d17[1] = 1;

			par.f5[0] = 1;
			par.f5[1] = 0.5;
			par.f5[2] = 0.5;

			if (particle[i].p.rotation > 0)
			{
				par.enableRotate = 1;
				par.rotate.value.x = 0;
				par.rotate.value.y = particle[i].p.rotation;
				par.rotate.value.z = particle[i].p.rotation;
			}

			if (m->particleSystems[i].rate.data[0].size() > 0)
				par.emissionRate.value = m->particleSystems[i].rate.data[0][0];
			else
				par.emissionRate.value = 0;	
			par.columns = particle[i].cols;
			par.rows = particle[i].rows;

			if (par.columns > 1)
				par.f15[0] = 1.0f / par.columns;
			if (par.rows > 1)
				par.f15[1] = 1.0f / par.rows;

			if (par.columns > 1 || par.rows > 1)
			{
				par.parFlags |= PARTICLEFLAG_randFlipbookStart;
			}
			par.parFlags |= PARTICLEFLAG_useVertexAlpha;

			par.enableSpeedVariation = 1;

			if (particle[i].EmitterType == 1)
				par.ptenum = 1;
			else if (particle[i].EmitterType == 2)
				par.ptenum = 2;

			if (particle[i].en.nTimes > 0)
				SetAnimed(par.emissionRate.AnimRef);

			f.Write(&par, sizeof(PAR));
		}
		padding(&f);
	}

	// mIREF
	mdata.mIREF.nEntries = mdata.mBone.nEntries;
	mdata.mIREF.ref = reList.size();
	RefEntry("FERI", f.Tell(), mdata.mIREF.nEntries, 0);
	for(uint32 i=0; i<mdata.mIREF.nEntries; i++) {
		IREF iref;
		memset(&iref, 0, sizeof(iref));
		iref.init();
		if (i >= ROOT_BONE) {
			iref.matrix[3][0] = -m->bones[i-ROOT_BONE].pivot.x;
			iref.matrix[3][1] = m->bones[i-ROOT_BONE].pivot.z;
			iref.matrix[3][2] = -m->bones[i-ROOT_BONE].pivot.y;
		} 
		f.Write(&iref, sizeof(iref));
	}
	padding(&f);

	// 4. ReferenceEntry
	fHead.ofsRefs = f.Tell();
	fHead.nRefs = reList.size();

	for(size_t i=0; i<reList.size(); i++) {
		f.Write(&reList[i], sizeof(ReferenceEntry));
	}

	// 5. rewrite head
	f.Seek(0, wxFromStart);
	f.Write(&fHead, sizeof(fHead));
	f.Write(&mdata, sizeof(mdata));

	
	// save textures
	wxString texFilename(fn, wxConvUTF8);
	texFilename = texFilename.BeforeLast(SLASH);
	if (modelExport_M3_TexturePath.Len() > 0)
		MakeDirs(texFilename, modelExport_M3_TexturePath);

	for (size_t i=0; i < MATtable.size(); i++)
	{
		int texid = MATtable[i].texid;
		wxString texName;

		if (texdef[texid].type == TEXTURE_BODY)
		{
			texName = m->modelname;
			texName = texName.BeforeLast('.').AfterLast(SLASH);
			texName += _T("_body"); 
			glBindTexture(GL_TEXTURE_2D, m->replaceTextures[m->specialTextures[texid]]);
		}
		else
		{
			texName = m->TextureList[texid];
			GLuint bindtex = texturemanager.add(texName);
			glBindTexture(GL_TEXTURE_2D, bindtex);
			texName = texName.BeforeLast('.').AfterLast(SLASH);
		}

		texName.Append(_T(".tga"));
		
		texName = texFilename + SLASH + modelExport_M3_TexturePath + SLASH + texName;
		//wxLogMessage(_T("Exporting Image: %s"),texName.c_str());
		SaveTexture(texName);
	}

	mpqf.close();
	mpqfv.close();
	f.Close();
	reList.clear();
}
