
#include <wx/wfstream.h>

#include "modelexport.h"
#include "lwoheader.h"
#include "ms3dheader.h"
#include "3dsheader.h"
#include "modelcanvas.h"

#include "CxImage/ximage.h"

unsigned short numVerts = 0;
unsigned short numFaces = 0;
unsigned short numGroups = 0;

unsigned short vertIndex = 0;
unsigned short grpIndex = 0;

struct ModelData {
	Vertex3f vertex;
	float tu;
	float tv;
	Vertex3f normal;
	unsigned short groupIndex;
	char boneid;
};

struct GroupData {
	ModelRenderPass p;
	Model *m;
};

ModelData *verts = NULL;
GroupData *groups = NULL;

// 2 methods to go,  just export the entire m2 model.
// or use our "drawing" routine to export only whats being drawn.

template <typename T>
inline T reverse_endian(T n) // for 32bits
{
#ifdef _MSC_VER
	_asm
	{
		mov EAX, n;
		bswap EAX;
		mov n, EAX;
	}

	return n;
#else
	uint32 m = *reinterpret_cast<uint32 *>(&n);
	T temp = ((m & 0xFF000000) >> 24) | ((m & 0x00FF0000) >> 8) |
			 ((m & 0x0000FF00) << 8)  | ((m & 0x000000FF) << 24);
	return *reinterpret_cast<T *>(&temp);
#endif
}

inline unsigned short ByteSwap16 (unsigned short nValue) // 16bit
{
   return ((((nValue & 0xFF00)>> 8)) | ((nValue & 0xFF) << 8));
}


void SaveTexture(wxString fn)
{
	unsigned char *pixels = NULL;

	GLint width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

	pixels = new unsigned char[width * height * 4];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

	CxImage *newImage = new CxImage(0);
	newImage->CreateFromArray(pixels, width, height, 32, (width*4), true);


	if (fn.Last() == 'g')
		newImage->Save(fn.mb_str(), CXIMAGE_FORMAT_PNG);
	else
		newImage->Save(fn.mb_str(), CXIMAGE_FORMAT_TGA);

	newImage->Destroy();
	wxDELETE(newImage);
	wxDELETEA(pixels);
}

void AddCount(Model *m)
{
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			numGroups++;
			
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				numVerts++;
			}
		}
	}
}

void AddVertices(Model *m, Attachment *att)
{
	int boneID = -1;
	Model *mParent = NULL;

	if (att->parent) {
		mParent = static_cast<Model*>(att->parent->model);
		if (mParent)
			boneID = mParent->attLookup[att->id];
	}

	Vec3D pos(0,0,0);
	Vec3D scale(1,1,1);
	if (boneID>-1) {
		pos = mParent->atts[boneID].pos;
		Matrix mat = mParent->bones[mParent->atts[boneID].bone].mat;
		/*
		pos.x += mat.m[0][4];
		pos.y += mat.m[1][4];
		pos.z += mat.m[2][4];
		*/
		scale.x = mat.m[0][0];
		scale.y = mat.m[1][1];
		scale.z = mat.m[2][2];
	}

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {			
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				
				if (m->vertices) {
					verts[vertIndex].vertex.x = ((m->vertices[a].x * scale.x) + pos.x);
					verts[vertIndex].vertex.y = ((m->vertices[a].y * scale.y) + pos.y);
					verts[vertIndex].vertex.z = ((m->vertices[a].z * scale.z) + pos.z);

					if (video.supportVBO) {
						verts[vertIndex].normal.x = (m->vertices[m->header.nVertices + a].x + pos.x);
						verts[vertIndex].normal.y = (m->vertices[m->header.nVertices + a].y + pos.y);
						verts[vertIndex].normal.z = (m->vertices[m->header.nVertices + a].z + pos.z);
					} else {
						verts[vertIndex].normal.x = m->normals[a].x;
						verts[vertIndex].normal.y = m->normals[a].y;
						verts[vertIndex].normal.z = m->normals[a].z;
					}
				} else {
					verts[vertIndex].vertex.x = ((m->origVertices[a].pos.x * scale.x) + pos.x);
					verts[vertIndex].vertex.y = ((m->origVertices[a].pos.y * scale.y) + pos.y);
					verts[vertIndex].vertex.z = ((m->origVertices[a].pos.z * scale.z) + pos.z);

					verts[vertIndex].normal.x = m->origVertices[a].normal.x;
					verts[vertIndex].normal.y = m->origVertices[a].normal.y;
					verts[vertIndex].normal.z = m->origVertices[a].normal.z;
				}

				verts[vertIndex].tu = m->origVertices[a].texcoords.x;
				verts[vertIndex].tv = m->origVertices[a].texcoords.y;

				verts[vertIndex].groupIndex = grpIndex;
				verts[vertIndex].boneid = m->origVertices[a].bones[0];

				vertIndex++;
			}
			groups[grpIndex].p = p;
			groups[grpIndex].m = m;
			grpIndex++;
		}
	}
}

void InitCommon(Attachment *att)
{
	if (verts)
		wxDELETEA(verts);
	if (groups)
		wxDELETEA(groups);

	numVerts = 0;
	numGroups = 0;
	numFaces = 0;
	vertIndex = 0;
	grpIndex = 0;

	if (!att)
		return;

	Model *m = NULL;
	if (att->model) {
		m = static_cast<Model*>(att->model);
		if (!m)
			return;

		AddCount(m);
	}

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddCount(mAtt);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddCount(mAttChild);
		}
	}

	numFaces = numVerts / 3;

	verts = new ModelData[numVerts];
	//indices = new float[numVerts];
	groups = new GroupData[numGroups];

	if (m)
		AddVertices(m, att);

	// children:
	for (size_t i=0; i<att->children.size(); i++) {
		Model *mAtt = static_cast<Model*>(att->children[i]->model);
		if (mAtt)
			AddVertices(mAtt, att->children[i]);

		Attachment *att2 = att->children[i];
		for (size_t j=0; j<att2->children.size(); j++) {
			Model *mAttChild = static_cast<Model*>(att2->children[j]->model);
			if (mAttChild)
				AddVertices(mAttChild, att2->children[j]);
		}
	}
}

void ExportM2to3DS(Model *m, const char *fn)
{
	if (!m)
		return;

	//ofstream f(fn, ios::out | ios::binary | ios::trunc);
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	unsigned short numVerts = 0;

	/*
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				numVerts++;
			}
		}
	}
	*/
	numVerts = m->header.nVertices;

	Vertex3f *verts = new Vertex3f[numVerts];
	for (int i=0; i<numVerts; i++) {
		verts[i].x = m->vertices[i].x;
		verts[i].y = m->vertices[i].y;
		verts[i].z = m->vertices[i].z;
	}

	unsigned int numIndices = 0;

	// Indice count
	for (size_t i=0; i<m->passes.size(); i++) {
		if (m->passes[i].init(m)) {
			numIndices += m->passes[i].indexCount;
		}
	}

	numFaces = numIndices / 3;
	MAX3DS_FACE *tris = new MAX3DS_FACE[numFaces];

	int count=0;
	int triCount = 0;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			//int tri=0;
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k+=3,b+=3) {
				tris[triCount].x = m->indices[b];
				tris[triCount].y = m->indices[b+1];
				tris[triCount].z = m->indices[b+2];

				triCount++;

				/*
				uint16 a = m->indices[b];

				verts[count].x = m->vertices[a].x;
				verts[count].y = m->vertices[a].y;
				verts[count].z = m->vertices[a].z;
				count++;
				tri++;

				if (tri == 3) {
					tri = 0;
					tris[tricount].x = count-2;
					tris[tricount].y = count-1;
					tris[tricount].z = count;
					tricount++;
				}
				*/
			}
		}
	}


	// Primary chunk
	MAX3DS_HEADER header;

	// EDIT3DS chunk
	MAX3DS_CHUNK chunk1;
	chunk1.id = EDIT3DS;

	// EDIT_CONFIG2 chunk
	MAX3DS_CHUNK chunk2_1;
	chunk2_1.id = MESH_VERSION;
	
	int meshVersion = 3;
	chunk2_1.size += sizeof(int);

	// EDIT_MATERIAL (0xAFFF)
	MAX3DS_CHUNK chunk2_2;
	chunk2_2.id = EDIT_MATERIAL;

	// MATNAME (0xA000) (See mli Doc) 
	MAX3DS_CHUNK chunk2_2_1;
	chunk2_2_1.id = MATNAME;

	// Material name
	wxString matName(_T("Material1\0"));
	const char *materialName = matName.mb_str();
	chunk2_2_1.size += int(matName.size() + 1);

	//COL_RGB  

	//EDIT_CONFIG1  (0x0100) chunk
	MAX3DS_CHUNK chunk2_3;
	chunk2_3.id = EDIT_CONFIG1;
	float config1 = 1.0f;
	chunk2_3.size += sizeof(float);

	// EDIT OBJECT chunk
	MAX3DS_CHUNK chunk2_4;
	chunk2_4.id = EDIT_OBJECT;

	// Model name
	wxString modName(_T("Model1\0"));
	const char *modelName = modName.mb_str();
	chunk2_4.size += int(modName.size() + 1);

	// OBJ_MESH chunk
	MAX3DS_CHUNK chunk3;
	chunk3.id = OBJ_MESH;

	// Object name
	wxString objName(m->name.c_str(), wxConvUTF8);
	const char *objectName = objName.mb_str();

	//MESH_VERTICES	0x4110 // The objects vertices
	MAX3DS_CHUNK chunk4_1;
	chunk4_1.id = MESH_VERTICES;

	// Vertice count
	// ^ numVerts
	chunk4_1.size += 2;
	// Vertices
	chunk4_1.size += (numVerts * sizeof(Vertex3f));

	//MESH_FACEL1		0x4120 // The objects faces
	MAX3DS_CHUNK chunk4_2;
	chunk4_2.id = MESH_FACEL1;

	// number of triangle faces
	// ^numIndices
	chunk4_2.size += 4;
	// Indices
	chunk4_2.size += (numFaces * sizeof(MAX3DS_FACE));
	// ------------------------------------


	// ------------------------------------
	// Correct the chunk sizes
	chunk3.size += chunk4_2.size;
	chunk3.size += chunk4_1.size;
	chunk2_2.size += chunk2_2_1.size;
	chunk2_4.size += chunk3.size;
	chunk1.size += chunk2_1.size;
	chunk1.size += chunk2_2.size;
	chunk1.size += chunk2_3.size;
	chunk1.size += chunk2_4.size;
	header.primary.size += chunk1.size;

	// Now write the data to file
	f.Write(&header, sizeof(MAX3DS_HEADER));
	f.Write(&chunk1, sizeof(MAX3DS_CHUNK));

	f.Write(&chunk2_1, sizeof(MAX3DS_CHUNK));
	f.Write(&meshVersion, sizeof(int));

	f.Write(&chunk2_2, sizeof(MAX3DS_CHUNK));

	f.Write(&chunk2_2_1, sizeof(MAX3DS_CHUNK));
	f.Write(materialName, (matName.size() + 1));

	//COL_RGB  

	f.Write(&chunk2_3, sizeof(MAX3DS_CHUNK));
	f.Write(&config1, sizeof(float));

	f.Write(&chunk2_4, sizeof(MAX3DS_CHUNK));
	f.Write(modelName, (modName.size() + 1));

	f.Write(&chunk3, sizeof(MAX3DS_CHUNK));
	//f.Write(objectName, (objName.size() + 1));

	// Mesh chunk Vertice count, and vertices
	f.Write(&chunk4_1, sizeof(MAX3DS_CHUNK));
	f.Write(&numVerts, sizeof(unsigned short));
	for (int i=0; i<numVerts; i++)
		f.Write(&verts[i], sizeof(Vertex3f));
	
	// Index chunk - Indice count, and indices
	f.Write(&chunk4_2, sizeof(MAX3DS_CHUNK));
	f.Write(&numFaces, sizeof(unsigned int));
	for (int i=0; i<numFaces; i++)
		f.Write(&tris[i], sizeof(MAX3DS_FACE));

	f.Close();

	wxDELETEA(tris);
	wxDELETEA(verts);
}

void ExportM2toMS3D(Attachment *att, Model *m, const char *fn)
{
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	InitCommon(att);

	// Write the header
	ms3d_header_t header;
	strncpy(header.id, "MS3D000000", sizeof(header.id));
	header.version = 4;

	// Header
	f.Write(reinterpret_cast<char *>(&header), sizeof(ms3d_header_t));
	// Vertex Count
	f.Write(reinterpret_cast<char *>(&numVerts), sizeof(numVerts));
	
	// Write Vertex data?
	for (unsigned int i=0; i<numVerts; i++) {
		ms3d_vertex_t vert;
		vert.boneId = verts[i].boneid;
		vert.flags = 0; //SELECTED;
		vert.referenceCount = 0; // what the?
		vert.vertex[0] = verts[i].vertex.x;
		vert.vertex[1] = verts[i].vertex.y;
		vert.vertex[2] = verts[i].vertex.z;
		f.Write(reinterpret_cast<char *>(&vert), sizeof(ms3d_vertex_t));
	}
	// ---------------------------

	// Triangle Count
	f.Write(reinterpret_cast<char *>(&numFaces), sizeof(numFaces));

	// Write Triangle Data?
	for (unsigned int i=0; i<numVerts; i+=3) {
		ms3d_triangle_t tri;
		tri.flags = 0; //SELECTED;
		tri.groupIndex = (unsigned char)verts[i].groupIndex;
		tri.smoothingGroup = 1; // 1 - 32

		for (int j=0; j<3; j++) {
			tri.vertexIndices[j] = i+j;
			tri.s[j] = verts[i+j].tu;
			tri.t[j] = verts[i+j].tv;
			
			tri.vertexNormals[j][0] = verts[i+j].normal.x;
			tri.vertexNormals[j][1] = verts[i+j].normal.y;
			tri.vertexNormals[j][2] = verts[i+j].normal.z;
		}

		f.Write(reinterpret_cast<char *>(&tri), sizeof(ms3d_triangle_t));
	}
	// ---------------------------

	// Number of groups
	f.Write(reinterpret_cast<char *>(&numGroups), sizeof(numGroups));

	unsigned short indiceCount = 0;
	for (unsigned short i=0; i<numGroups; i++) {
		wxString groupName(wxString::Format(_T("Geoset_%i"), i));

		const char flags = 0; // SELECTED
		f.Write(&flags, sizeof(flags));

		char name[32];
		strncpy(name, groupName.mb_str(), sizeof(name));
		f.Write(name, sizeof(name));

		unsigned short faceCount = groups[i].p.indexCount / 3;
		f.Write(reinterpret_cast<char *>(&faceCount), sizeof(faceCount));
		
		for (int k=0; k<faceCount; k++) {
			//triIndices[k] = indiceCount;
			f.Write(reinterpret_cast<char *>(&indiceCount), sizeof(indiceCount));
			indiceCount++;
		}

		unsigned char gIndex = (char)i;
		f.Write(reinterpret_cast<char *>(&gIndex), sizeof(gIndex));
	}

	// Number of materials (pretty much identical to groups, each group has its own material)
	f.Write(reinterpret_cast<char *>(&numGroups), sizeof(numGroups));
	
	for (unsigned short i=0; i<numGroups; i++) {
		wxString matName(wxString::Format(_T("Material_%i"), i));

		ModelRenderPass p = groups[i].p;
		if (p.init(groups[i].m)) {
			ms3d_material_t mat;
			memset(mat.alphamap, '\0', sizeof(mat.alphamap));

			strncpy(mat.name, matName.mb_str(), sizeof(mat.name));
			mat.ambient[0] = 0.7f;
			mat.ambient[1] = 0.7f;
			mat.ambient[2] = 0.7f;
			mat.ambient[3] = 1.0f;
			mat.diffuse[0] = p.ocol.x;
			mat.diffuse[1] = p.ocol.y;
			mat.diffuse[2] = p.ocol.z;
			mat.diffuse[3] = p.ocol.w;
			mat.specular[0] = 0.0f;
			mat.specular[1] = 0.0f;
			mat.specular[2] = 0.0f;
			mat.specular[3] = 1.0f;
			mat.emissive[0] = p.ecol.x;
			mat.emissive[1] = p.ecol.y;
			mat.emissive[2] = p.ecol.z;
			mat.emissive[3] = p.ecol.w;
			mat.transparency = p.ocol.w;

			if (p.useEnvMap) {
				mat.shininess = 30.0f;
				mat.mode = 1;
			} else {
				mat.shininess = 0.0f;
				mat.mode = 0;
			}

			unsigned int bindtex = 0;
			if (groups[i].m->specialTextures[p.tex]==-1) 
				bindtex = groups[i].m->textures[p.tex];
			else 
				bindtex = groups[i].m->replaceTextures[groups[i].m->specialTextures[p.tex]];

			wxString texName(fn, wxConvUTF8);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << wxT("_") << bindtex << wxT(".tga");
			strncpy(mat.texture, texName.mb_str(), sizeof(mat.texture));

			f.Write(reinterpret_cast<char *>(&mat), sizeof(ms3d_material_t));

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}

#if 0
	// save some keyframe data
	float fps = 1.0f;
	float fCurTime = 0.0f;
	int totalFrames = 0;

	f.Write(reinterpret_cast<char *>(&fps), sizeof(fps));
	f.Write(reinterpret_cast<char *>(&fCurTime), sizeof(fCurTime));
	f.Write(reinterpret_cast<char *>(&totalFrames), sizeof(totalFrames));
	
	
	// number of joints
	unsigned short numJoints = 0; //(unsigned short)m->header.nBones;

	f.Write(reinterpret_cast<char *>(&numJoints), sizeof(numJoints));
#else
	// TODO
	// save some keyframe data
	float fps = 1.0f; //m->anims[m->anim].playSpeed;
	float fCurTime = 0.0f;
	int totalFrames = 0; // (m->anims[m->anim].timeEnd - m->anims[m->anim].timeStart);

	f.Write(reinterpret_cast<char *>(&fps), sizeof(fps));
	f.Write(reinterpret_cast<char *>(&fCurTime), sizeof(fCurTime));
	f.Write(reinterpret_cast<char *>(&totalFrames), sizeof(totalFrames));
	
	// number of joints

	unsigned short numJoints = (unsigned short)m->header.nBones;

	f.Write(reinterpret_cast<char *>(&numJoints), sizeof(numJoints));

	for (int i=0; i<numJoints; i++) {
		ms3d_joint_t joint;

		joint.flags = 0; // SELECTED
		memset(joint.name, '\0', sizeof(joint.name));
		snprintf(joint.name, sizeof(joint.name), "Bone_%i_%i", m->anim, i);
		memset(joint.parentName, '\0', sizeof(joint.parentName));

		joint.rotation[0] = 0; // m->bones[i].pivot.x;
		joint.rotation[1] = 0; // m->bones[i].pivot.y;
		joint.rotation[2] = 0; // m->bones[i].pivot.z;

		joint.position[0] = m->bones[i].transPivot.x;
		joint.position[1] = m->bones[i].transPivot.y;
		joint.position[2] = m->bones[i].transPivot.z;

		int parent = m->bones[i].parent;
		if (parent > -1) {
			snprintf(joint.parentName, sizeof(joint.parentName), "Bone_%i_%i", m->anim, parent);

			joint.position[0] -= m->bones[parent].transPivot.x;
			joint.position[1] -= m->bones[parent].transPivot.y;
			joint.position[2] -= m->bones[parent].transPivot.z;
		}

		joint.numKeyFramesRot = 0; //(unsigned short)m->bones[i].rot.data[m->anim].size();
		joint.numKeyFramesTrans = 0; //(unsigned short)m->bones[i].trans.data[m->anim].size();

		f.Write(reinterpret_cast<char *>(&joint), sizeof(ms3d_joint_t));

		if (joint.numKeyFramesRot > 0) {
			ms3d_keyframe_rot_t *keyFramesRot = new ms3d_keyframe_rot_t[joint.numKeyFramesRot];
			for (int j=0; j<joint.numKeyFramesRot; j++) {
				keyFramesRot[j].time = m->bones[i].rot.times[m->anim][j]; // Error, time in seconds;
				keyFramesRot[j].rotation[0] = m->bones[i].rot.data[m->anim][j].x;
				keyFramesRot[j].rotation[1] = m->bones[i].rot.data[m->anim][j].y;
				keyFramesRot[j].rotation[2] = m->bones[i].rot.data[m->anim][j].z;
			}

			f.Write(reinterpret_cast<char *>(keyFramesRot), sizeof(ms3d_keyframe_rot_t) * joint.numKeyFramesRot);
			wxDELETEA(keyFramesRot);
		}

		if (joint.numKeyFramesTrans > 0) {
			ms3d_keyframe_pos_t *keyFramesTrans = new ms3d_keyframe_pos_t[joint.numKeyFramesTrans];
			for (int j=0; j<joint.numKeyFramesTrans; j++) {
				keyFramesTrans[j].time = m->bones[i].trans.times[m->anim][j]; // Error,time in seconds;;
				keyFramesTrans[j].position[0] = m->bones[i].trans.data[m->anim][j].x;
				keyFramesTrans[j].position[1] = m->bones[i].trans.data[m->anim][j].y;
				keyFramesTrans[j].position[2] = m->bones[i].trans.data[m->anim][j].z;
				if (parent > -1) {
					keyFramesTrans[j].position[0] -= m->bones[parent].transPivot.x;
					keyFramesTrans[j].position[1] -= m->bones[parent].transPivot.y;
					keyFramesTrans[j].position[2] -= m->bones[parent].transPivot.z;
					if (m->bones[parent].trans.data[m->anim].size() > j) {
						keyFramesTrans[j].position[0] -= m->bones[parent].trans.data[m->anim][j].x;
						keyFramesTrans[j].position[1] -= m->bones[parent].trans.data[m->anim][j].y;
						keyFramesTrans[j].position[2] -= m->bones[parent].trans.data[m->anim][j].z;
					}
				}
			}

			f.Write(reinterpret_cast<char *>(keyFramesTrans), sizeof(ms3d_keyframe_pos_t) * joint.numKeyFramesTrans);
			wxDELETEA(keyFramesTrans);
		}
	}
#endif
	f.Close();

	if (verts)
		wxDELETEA(verts);
	if (groups)
		wxDELETEA(groups);
}

/*
http://gpwiki.org/index.php/LWO
*/
void ExportM2toLWO2(Attachment *att, Model *m, const char *fn)
{
	int i32;
	uint32 u32;
	float f32;
	uint16 u16;
	unsigned char ub;
	
	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	InitCommon(att);

	// LightWave object files use the IFF syntax described in the EA-IFF85 document. Data is stored in a collection of chunks. 
	// Each chunk begins with a 4-byte chunk ID and the size of the chunk in bytes, and this is followed by the chunk contents.

	unsigned int fileLen = 0;

	// --
	// Formally, a LightWave object file is a single IFF FORM chunk of type LWO2. The first 4 bytes are the characters 'F', 'O', 'R', 'M', 
	// and this is followed by a 4-byte integer containing the chunk size (the size of the file minus 8) and the FORM type (the 
	// characters 'L', 'W', 'O', '2'). As with all numbers in LWO2 files, the chunk size is always written in big-endian (Motorola, 
	// network) byte order.
	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);
	f.Write("LWO2", 4);
	fileLen += 4;
	// ================


	// --
	// The TAGS chunk contains an array of strings. Whenever something is identified by name in the file, the ID is often a 
	// 0-based index into the TAGS array. The only named element in this file is its single surface, named "Default".
	f.Write("TAGS", 4);
	u32 = reverse_endian<uint32>(8);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.Write("Default", 7);
	ub = 0;
	f.Write(reinterpret_cast<char *>(&ub), 1);
	fileLen += 4+4+8;
	// ================


	// --
	// The layer header signals the start of a new layer. All geometry elements that appear in the file after this and before 
	// the next LAYR chunk belong to this layer. The layer header contains an index, a flags word, the pivot point of the layer, 
	// the layer's name, and the index of the parent layer. This is the first (and only) layer, so its index is 0 and the optional 
	// parent index is omitted. The bits in the flags word are also 0, and the layer hasn't been given a name.
	// The pivot point is the origin for rotations in this layer and is expressed in world coordinates. Pivots typically differ from 
	// (0, 0, 0) when layers and layer parenting are used to create an object hierarchy.
	f.Write("LAYR", 4);
	u32 = reverse_endian<uint32>(0);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	// ================


	// --
	// POINTS CHUNK, this is the vertice data
	// The PNTS chunk contains triples of floating-point numbers, the coordinates of a list of points. The numbers are written 
	// as IEEE 32-bit floats in network byte order. The IEEE float format is the standard bit pattern used by almost all CPUs 
	// and corresponds to the internal representation of the C language float type. In other words, this isn't some bizarre 
	// proprietary encoding. You can process these using simple fread and fwrite calls (but don't forget to correct the byte 
	// order if necessary).
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the vertice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				vert.x = reverse_endian<float>(m->vertices[a].x);
				vert.y = reverse_endian<float>(m->vertices[a].y);
				vert.z = reverse_endian<float>(m->vertices[a].z);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				fileLen += 12;
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
	// ================


	// --
	// The bounding box for the layer, just so that readers don't have to scan the PNTS chunk to find the extents.
	f.Write("BBOX", 4);
	u32 = reverse_endian<uint32>(24);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	Vec3D vert;
	vert.x = reverse_endian<float>(m->header.ps.BoundingBox[0].x);
	vert.y = reverse_endian<float>(m->header.ps.BoundingBox[0].y);
	vert.z = reverse_endian<float>(m->header.ps.BoundingBox[0].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	vert.x = reverse_endian<float>(m->header.ps.BoundingBox[1].x);
	vert.y = reverse_endian<float>(m->header.ps.BoundingBox[1].y);
	vert.z = reverse_endian<float>(m->header.ps.BoundingBox[1].z);
	f.Write(reinterpret_cast<char *>(&vert.x), 4);
	f.Write(reinterpret_cast<char *>(&vert.y), 4);
	f.Write(reinterpret_cast<char *>(&vert.z), 4);
	// ================


	// --
	// POLYGON CHUNK
	// The POLS chunk contains a list of polygons. A "polygon" in this context is anything that can be described using an 
	// ordered list of vertices. A POLS of type FACE contains ordinary polygons, but the POLS type can also be CURV, 
	// PTCH, MBAL or BONE, for example.
	//
	// The high 6 bits of the vertex count for each polygon are reserved for flags, which in effect limits the number of 
	// vertices per polygon to 1023. Don't forget to mask the high bits when reading the vertex count. The flags are 
	// currently only defined for CURVs.
	// 
	// The point indexes following the vertex count refer to the points defined in the most recent PNTS chunk. Each index 
	// can be a 2-byte or a 4-byte integer. If the high order (first) byte of the index is not 0xFF, the index is 2 bytes long. 
	// This allows values up to 65279 to be stored in 2 bytes. If the high order byte is 0xFF, the index is 4 bytes long and 
	// its value is in the low three bytes (index & 0x00FFFFFF). The maximum value for 4-byte indexes is 16,777,215 (224 - 1). 
	// Objects with more than 224 vertices can be stored using multiple pairs of PNTS and POLS chunks.
	// 
	// The cube has 6 square faces each defined by 4 vertices. LightWave polygons are single-sided by default 
	// (double-sidedness is a possible surface property). The vertices are listed in clockwise order as viewed from the 
	// visible side, starting with a convex vertex. (The normal is defined as the cross product of the first and last edges.)
	int32 polySize = (numVerts / 3) * sizeof(POLYCHUNK);

	f.Write("POLS", 4);
	i32 = reverse_endian<int32>(polySize);
	f.Write(reinterpret_cast<char *>(&i32), 4);
	fileLen += 8; // an extra 4 bytes for chunk size
	f.Write("FACE", 4);
	fileLen += 4;

	uint16 counter=0;
	int16 surfCounter=0;
	POLYCHUNK tri;
	
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {

			surfCounter++;

			for (unsigned int k=0; k<p.indexCount; k+=3) {
				u16 = 3;
				tri.numVerts = ByteSwap16(u16);
				for (uint16 b=0; b<3; b++) {
					tri.indice[b] = ByteSwap16(counter);
					counter++;
				}
				tri.surfIndex = ByteSwap16(surfCounter);

				f.Write(reinterpret_cast<char *>(&tri), sizeof(POLYCHUNK));

				fileLen += 10;
			}
		}
	}
	
	// Now lets go back and correct our data lengths
	//f.seekp(-4 - polySize, ios::cur);
	//i32 = reverse_endian<int32>(polySize);
	//f.Write(reinterpret_cast<char *>(&i32), 4);
	// ========

	// --
	// The PTAG chunk associates tags with polygons. In this case, it identifies which surface is assigned to each polygon. 
	// The first number in each pair is a 0-based index into the most recent POLS chunk, and the second is a 0-based 
	// index into the TAGS chunk.
	f.Write("PTAG", 4);
	u32 = reverse_endian<uint32>(0);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	// ================


	// --
	f.Write("SURF", 4);
	u32 = reverse_endian<uint32>(0);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	// ================

//	f.Write("", 4);

	f.Close();
}


void ExportM2toLWO(Model *m, const char *fn)
{
	int i32;
	uint32 u32;
	float f32;
	uint16 u16;
	unsigned char ub;

	wxFFileOutputStream f(wxString(fn, wxConvUTF8), wxT("w+b"));

	if (!f.IsOk()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	unsigned int fileLen = 0;

	f.Write("FORM", 4);
	f.Write(reinterpret_cast<char *>(&fileLen), 4);

	f.Write("LWOB", 4);
	fileLen += 4;


	// --
	// POINTS CHUNK, this is the vertice data
	uint32 pointsSize = 0;
	f.Write("PNTS", 4);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	fileLen += 8;

	// output all the vertice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				Vec3D vert;
				vert.x = reverse_endian<float>(m->vertices[a].x);
				vert.y = reverse_endian<float>(m->vertices[a].y);
				vert.z = reverse_endian<float>(m->vertices[a].z);
				f.Write(reinterpret_cast<char *>(&vert.x), 4);
				f.Write(reinterpret_cast<char *>(&vert.y), 4);
				f.Write(reinterpret_cast<char *>(&vert.z), 4);
				fileLen += 12;
				pointsSize += 12;

				numVerts++;
			}
			numGroups++;
		}
	}
	// ================


	// --
	// SURFACE CHUNK,
	uint32 surfaceSize = 0;
	f.Write("SRFS", 4);
	f.Write(reinterpret_cast<char *>(&surfaceSize), 4);
	fileLen += 8;

	wxString surfName;
	int surfaceCounter = 0;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			surfName = _T("geoset_");
			surfName << surfaceCounter;
			surfaceCounter++;
			
			surfName.Append(_T('\0'));
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append(_T('\0'));

			f.Write(surfName.c_str(), (int)surfName.length());
			
			fileLen += (uint32)surfName.length();
			surfaceSize += (uint32)surfName.length();
		}
	}
	
	f.SeekO(-4 - surfaceSize, wxFromCurrent);
	u32 = reverse_endian<uint32>(surfaceSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	f.SeekO(0, wxFromEnd);
	// =================

	// --
	// POLYGON CHUNK
	int32 polySize = (numVerts / 3) * sizeof(POLYCHUNK);

	f.Write("POLS", 4);
	i32 = reverse_endian<int32>(polySize);
	f.Write(reinterpret_cast<char *>(&i32), 4);
	fileLen += 8; // an extra 4 bytes for chunk size

	uint16 counter=0;
	int16 surfCounter=0;
	POLYCHUNK tri;
	
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {

			surfCounter++;

			for (unsigned int k=0; k<p.indexCount; k+=3) {
				u16 = 3;
				tri.numVerts = ByteSwap16(u16);
				for (uint16 b=0; b<3; b++) {
					tri.indice[b] = ByteSwap16(counter);
					counter++;
				}
				tri.surfIndex = ByteSwap16(surfCounter);

				f.Write(reinterpret_cast<char *>(&tri), sizeof(POLYCHUNK));

				fileLen += 10;
			}
		}
	}
	
	// Now lets go back and correct our data lengths
	//f.seekp(-4 - polySize, ios::cur);
	//i32 = reverse_endian<int32>(polySize);
	//f.write(reinterpret_cast<char *>(&i32), 4);
	// ========


	// Now we need to write our surface definitions (ie. RenderPass data)
	// --------------------------
	//f.seekp(0, ios::end);

	surfaceCounter = 0;

	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			uint32 surfaceDefSize = 0;
			f.Write("SURF", 4);
			f.Write(reinterpret_cast<char *>(&surfaceDefSize), 4);
			fileLen += 8;

			// Surface name
			surfName = _T("geoset_");
			surfName << surfaceCounter;
			surfaceCounter++;
			
			surfName.Append(_T('\0'));
			if (fmod((float)surfName.length(), 2.0f) > 0)
				surfName.Append(_T('\0'));

			f.Write(surfName.data(), (int)surfName.length());
			
			fileLen += (uint32)surfName.length();
			surfaceDefSize += (uint32)surfName.length();

			// Surface Attributes
			// COLOUR, size 4, bytes 2
			f.Write("COLR", 4);
			u16 = 4;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			
			// value
			ub = (unsigned char)(p.ocol.x * 255);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			ub = (unsigned char)(p.ocol.y * 255);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			ub = (unsigned char)(p.ocol.z * 255);
			f.Write(reinterpret_cast<char *>(&ub), 1);
			ub = '\0';
			f.Write(reinterpret_cast<char *>(&ub), 1);

			fileLen += 10;
			surfaceDefSize += 10;
			
			// FLAGS
			f.Write("FLAG", 4);
			// size
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			// value
			u16 = 0;
			if (!p.cull)
				u16 &= SUF_DOUBLESIDED;
			if (p.blendmode>2)
				u16 &= SUF_ADDITIVE;
			if (p.blendmode==1)
				u16 &= SUF_EDGETRANSPARENT;
			
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			fileLen += 8;
			surfaceDefSize += 8;

			// GLOSSINESS
			f.Write("GLOS", 4);
			// size
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			// Value
			// try 50% gloss for 'relfection surfaces
			if (p.useEnvMap)
				u16 = 128;
			else
				u16 = 0;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			fileLen += 8;
			surfaceDefSize += 8;
			
			if (p.useEnvMap) {
				// REFLECTION
				f.Write("FRFL", 4);
				// size
				u16 = 4;
				u16 = ByteSwap16(u16);
				f.Write(reinterpret_cast<char *>(&u16), 2);

				// value
				f32 = 0.2f;
				f32 = reverse_endian<float>(f32);
				f.Write(reinterpret_cast<char *>(&f32), 4);

				fileLen += 10;
				surfaceDefSize += 10;
				
				// REFLECTION
				f.Write("RFLT", 4);
				// size
				u16 = 2;
				u16 = ByteSwap16(u16);
				f.Write(reinterpret_cast<char *>(&u16), 2);

				// value
				u16 = 1;
				u16 = ByteSwap16(u16);
				f.Write(reinterpret_cast<char *>(&u16), 2);

				fileLen += 8;
				surfaceDefSize += 8;
			}
			

			// TRANSPARENCY
			f.Write("FTRN", 4);
			u16 = 4; // size
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			// value
			f32 = p.ocol.w;
			f32 = reverse_endian<float>(f32);
			f.Write(reinterpret_cast<char *>(&f32), 4);

			fileLen += 10;
			surfaceDefSize += 10;

			// TEXTURE FLAGS
			f.Write("TFLG", 4);
			// size
			u16 = 2;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			// value
			u16 = 0; // don't know the flag info yet
			if (p.trans)
				u16 &= TXF_PIXBLEND;
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);

			fileLen += 8;
			surfaceDefSize += 8;

			wxString texName(fn, wxConvUTF8);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << _T("_") << p.tex << _T(".tga") << '\0';
			if (fmod((float)texName.length(), 2.0f) > 0)
				texName.Append(_T('\0'));

			// TEXTURE filename
			f.Write("TIMG", 4);
			u16 = (unsigned short)texName.length();
			u16 = ByteSwap16(u16);
			f.Write(reinterpret_cast<char *>(&u16), 2);
			f.Write(texName.data(), (int)texName.length());

			fileLen += (unsigned int)texName.length() + 6;
			surfaceDefSize += (unsigned int)texName.length() + 6;

			// update the chunks length
			f.SeekO(-4 - surfaceDefSize, wxFromCurrent);
			u32 = reverse_endian<uint32>(surfaceDefSize);
			f.Write(reinterpret_cast<char *>(&u32), 4);
			f.SeekO(0, wxFromEnd);

			wxString texFilename(fn, wxConvUTF8);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}
	
	f.SeekO(4, wxFromStart);
	u32 = reverse_endian<uint32>(fileLen);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	
	f.SeekO(16, wxFromStart);
	u32 = reverse_endian<uint32>(pointsSize);
	f.Write(reinterpret_cast<char *>(&u32), 4);
	// ===========================


	f.Close();
}

void ExportWMOtoOBJ(WMO *m, const char *fn)
{
	// Open file
	ofstream f(fn, ios_base::out | ios_base::trunc);

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	int counter = 1;
	for (int i=0; i<m->nGroups; i++) {
		for (int j=0; j<m->groups[i].nBatches; j++)
		{
			WMOBatch *batch = &m->groups[i].batches[j];
			f << "# Chunk Indice Count: " << batch->indexCount << endl;
			for(int ii=0;ii<batch->indexCount;ii++)
			{
				int a = m->groups[i].indices[batch->indexStart + ii];
				f << "v " << m->groups[i].vertices[a].x << " " << m->groups[i].vertices[a].z << " " << -m->groups[i].vertices[a].y << " " << "1.0" << endl;
				f << "vn " << m->groups[i].normals[a].x << " " << m->groups[i].normals[a].z << " " << -m->groups[i].normals[a].y << endl;
				f << "vt " << m->groups[i].texcoords[a].x << " " << (1 - m->groups[i].texcoords[a].y) << endl;
			}
			
			f << "g Geoset_" << i << "_" << j << "_tex_" << int(batch->texture) << endl;
			f << "s 1" << endl;
			f << "usemtl Material_" << counter << endl;
			for (unsigned int k=0; k<batch->indexCount; k+=3) {
				f << "f ";
				f << counter << "/" << counter << "/" << counter << " ";
				f << (counter+1) << "/" << (counter+1) << "/" << (counter+1) << " ";
				f << (counter+2) << "/" << (counter+2) << "/" << (counter+2) << endl;
				counter += 3;
			}
		}
	}
	for(int i=0;i<m->nTextures;i++)
	{
		glBindTexture(GL_TEXTURE_2D, m->mat[i].tex);

		wxString texName(fn, wxConvUTF8);
		texName = texName.AfterLast('\\').BeforeLast('.');
		texName << _T("_") << i << _T(".tga");

		wxString texFilename(fn, wxConvUTF8);
		texFilename = texFilename.BeforeLast('\\');
		texFilename += '\\';
		texFilename += texName;
		SaveTexture(texFilename);
	}

	// Close file
	f.close();
}

void ExportWMOto3DS(WMO *m, const char *fn)
{
	wxLogMessage(_T("Error: WMO to 3DS export not implemented."));
}

void ExportWMOtoMS3D(WMO *m, const char *fn)
{
	wxLogMessage(_T("Error: WMO to MS3D export not implemented."));
}

void ExportWMOtoLWO(WMO *m, const char *fn)
{
	wxLogMessage(_T("Error: WMO to LWO export not implemented."));
}


void ExportM2toOBJ(Model *m, const char *fn)
{
	// Open file
	//locale::global(locale(""));
	ofstream f(fn, ios_base::out | ios_base::trunc);
	//locale::global(locale("C"));

	if (!f.is_open()) {
		wxLogMessage(_T("Error: Unable to open file '%s'. Could not export model."), fn);
		return;
	}

	wxString matName(fn, wxConvUTF8);
	matName = matName.BeforeLast('.');
	matName << _T(".mtl");

	//locale::global(locale(""));
	ofstream fm(matName.mb_str(), ios_base::out | ios_base::trunc);
	//locale::global(locale("C"));
	
	// output all the vertice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			f << "# Chunk Indice Count: " << p.indexCount << endl;

			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				if (m->vertices == NULL) {
					f << "v " << m->origVertices[a].pos.x << " " << m->origVertices[a].pos.y << " " << m->origVertices[a].pos.z << " " << "1.0" << endl;
				} else {
					f << "v " << m->vertices[a].x << " " << m->vertices[a].y << " " << m->vertices[a].z << " " << "1.0" << endl;
				}
			}

			wxString texName(fn, wxConvUTF8);
			texName = texName.AfterLast('\\').BeforeLast('.');
			texName << _T("_") << p.tex << _T(".tga");
			fm << "newmtl " << "Material_" << p.geoset << endl;
			fm << "Ka " << 0.7f << " " << 0.7f << " " << 0.7f << endl;
			fm << "Kd " << p.ocol.x << " " << p.ocol.y << " " << p.ocol.z << endl;
			fm << "Ks " << p.ecol.x << " " << p.ecol.y << " " << p.ecol.z << endl;
			fm << "Ns " << p.ocol.w << endl;
			fm << "map_Kd " << texName.c_str() << endl << endl;

			wxString texFilename(fn);
			texFilename = texFilename.BeforeLast('\\');
			texFilename += '\\';
			texFilename += texName;
			SaveTexture(texFilename);
		}
	}

	fm.close();

	f << "mtllib " << matName.c_str() << endl;

	// output all the texture coordinate data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		// we don't want to render completely transparent parts
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				f << "vt " << m->origVertices[a].texcoords.x << " " << (1 - m->origVertices[a].texcoords.y) << endl;
			}
		}
	}

	// output all the vertice normals data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];
		if (p.init(m)) {
			for (size_t k=0, b=p.indexStart; k<p.indexCount; k++,b++) {
				uint16 a = m->indices[b];
				f << "vn " << m->origVertices[a].normal.x << " " << m->origVertices[a].normal.y << " " << m->origVertices[a].normal.z << endl;
			}
		}
	}

	int counter=1;
	// output the indice data
	for (size_t i=0; i<m->passes.size(); i++) {
		ModelRenderPass &p = m->passes[i];

		if (p.init(m)) {
			f << "g Geoset_" << p.geoset << endl;
			f << "s 1" << endl;
			f << "usemtl Material_" << p.geoset << endl;
			for (unsigned int k=0; k<p.indexCount; k+=3) {
				f << "f ";
				f << counter << "/" << counter << "/" << counter << " ";
				counter ++;
				f << counter << "/" << counter << "/" << counter << " ";
				counter ++;
				f << counter << "/" << counter << "/" << counter << endl;
				counter ++;
			}
		}
	}

	f << "# Total Indices: " << counter << endl;
	
	// Close file
	f.close();
}

void ExportM2toRAW(Model *m)
{
	
}

