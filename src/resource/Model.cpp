#include "common.h"
#include "../Session.h"
#include "../GameActor.h"
#include "Model.h"
#include "../View.h"
#include "../GameWorld.h"
#include "../GameMode.h"
#include "../RagEffect.h"
#include "../3dlib/Prim.h"
#include "Attr.h"
#include "Wave.h"
#include "../device/Sound.h"
#include <algorithm>
#include "../3dlib/Anim.h"
#include "../3dlib/Mesh.h"
#include "../3dlib/Texture.h"
#include "../base/File.h"
#include "Skybox.h"
#include "Ground.h"






extern DWORD const g_GlobalVertexFormat = (D3DFVF_XYZ|D3DFVF_NORMAL|
									D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0));

using std::string;

inline void C3dMesh::SortFace()
{
	std::sort(m_face, m_face + m_numFace);
}





typedef std::list<C3dNodeRes *>     C3dNodeResList;
typedef std::list<CVolumeBox *>     CVolumeBoxList;
typedef std::list<C3dAnimNodeRes *> C3dAnimNodeResList;
typedef std::list<int>              IntList;


#ifndef Trace
#define Trace (void)
#endif






#ifndef MAX_SHADEGROUP
#endif
#ifndef MAX_GROUP
#define MAX_GROUP MAX_SHADEGROUP
#endif
#ifndef MAX_FACE_PER_MESH
#define MAX_FACE_PER_MESH 1000
#endif
#ifndef MAX_VERTEX_PER_MESH
#define MAX_VERTEX_PER_MESH 1000
#endif
#ifndef MAX_BONE_TYPE
#define MAX_BONE_TYPE 100
#endif
#ifndef MAX_GRANNY_CONTROL
#define MAX_GRANNY_CONTROL 20
#endif
#ifndef MAX_GRANNY_VERTEX
#define MAX_GRANNY_VERTEX 10000
#endif


extern CRenderer* g_renderer;

C3dGrannyBoneRes g_GrannyBoneRes;

extern CSession g_session;


#ifndef GrannyColorMapTextureType
#define GrannyColorMapTextureType 0
#endif
#ifndef GrannyDiffuseColorTexture
#define GrannyDiffuseColorTexture 2
#endif
#ifdef GrannyDeformPositionNormal
#undef GrannyDeformPositionNormal
#endif
#define GrannyDeformPositionNormal 2
#ifndef GrannyStandardRigidVertexSection
#define GrannyStandardRigidVertexSection 1
#endif
#ifndef GrannyStandardRigidIndexSection
#define GrannyStandardRigidIndexSection 2
#endif
#ifndef GrannyStandardDeformableIndexSection
#define GrannyStandardDeformableIndexSection 4
#endif
#ifndef GrannyStandardTextureSection
#define GrannyStandardTextureSection 5
#endif



#include <windows.h>
#include <ddraw.h>


#ifndef sprintf_s
#include <stdio.h>
#include <stdarg.h>
#define sprintf_s rag_sprintf_s
inline int rag_sprintf_s(char* buf, size_t, const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int n = vsprintf(buf, fmt, ap);
	va_end(ap);
	return n;
}
#endif



typedef _W3D_TLVERTEX w3dtlvertex;

#include <math.h>


C3dNodeRes :: C3dNodeRes()
{
	memset(parentname, 0, sizeof(parentname));
	parent = NULL;
	texture = NULL;
	mesh = NULL;
	numTexture = 0;
}

C3dNodeRes :: ~C3dNodeRes()
{
	Release();
}

void	C3dNodeRes :: Release()
{
	if (texture) {
		for (int i = 0; i < numTexture; i++) {
			texture[i]->Unlock();
			g_texMgr.DestroyTexture(texture[i]);
		}
		delete [] texture;
		texture = NULL;
	}
	if (mesh) {
		delete mesh;
		mesh = NULL;
	}
	child.erase(child.begin(), child.end());
}

static C3dModelRes C3dModelResInstance(Exemplar(), "rsm", "model\\");


C3dModelRes :: C3dModelRes()
	: CRes()
{
	m_numMaterials = 0;
	m_material = NULL;
}

void	C3dModelRes :: Reset()
{
	if (m_material) {
		delete [] m_material;
		m_material = NULL;
		m_numMaterials = 0;
	}
	C3dNodeResList::iterator i = m_objectList.begin();
	while (i != m_objectList.end()) {
		delete *i;
		i++;
	}
	m_objectList.erase(m_objectList.begin(), m_objectList.end());
	m_rootObjList.erase(m_rootObjList.begin(), m_rootObjList.end());
	m_meshList.erase(m_meshList.begin(), m_meshList.end());

	CVolumeBoxList::iterator j = m_volumeBoxList.begin();
	while (j != m_volumeBoxList.end()) {
		delete *j;
		j++;
	}
	m_volumeBoxList.erase(m_volumeBoxList.begin(), m_volumeBoxList.end());
}


#include "rag_symbol_shift.h"
bool	C3dModelRes :: Load(const char *fName)
{
	RAG_SYMBOL_SHIFT(1)
	CFile	fp;

	if(fp.Open(fName, CFile::modeRead) == false)
		return false;

	char	magicId[] = "GRSM";
	char	magicBuf[5];

	fp.Read(magicBuf, 4);
	magicBuf[4]=0;
	if (strcmp(magicBuf, magicId))
	{
		ErrorMsg("Invalid model file format.");
		return	false;
	}
	BYTE	verMajor;
	BYTE	verMinor;
	fp.Read(&verMajor, sizeof(BYTE));
	fp.Read(&verMinor, sizeof(BYTE));
	if (verMajor != 1 || verMinor > 5)
	{
		ErrorMsg("Unsupported version. The contents may be corrupted.");
		return	false;
	}

	fp.Read(&m_animLen, sizeof(int));
	fp.Read(&m_shadeType, sizeof(int));
	if (verMinor >= 4)
		fp.Read(&m_alpha, sizeof(BYTE));
	else
		m_alpha = 0xff;
	char	reserved[16];
	fp.Read(reserved, 16);
	int		numTexture;
	fp.Read(&numTexture, sizeof(int));
	std::string *textureNames = new std::string[numTexture];
	char	tmp[40];
	int i;
	for (i=0; i<numTexture; i++)
	{
		fp.Read(tmp, 40);
		textureNames[i] = std::string(tmp);
	}
	char	mainNode[64];

	fp.Read(tmp, 40);
	strcpy(mainNode, tmp);

	int		numNodes;
	fp.Read(&numNodes, sizeof(int));

	for (i = 0; i < numNodes; i++)
	{
		C3dNodeRes *	temp = new C3dNodeRes;
		temp->scene = this;
		temp->alpha = m_alpha;


		fp.Read(tmp, 40);
		strcpy(temp->name, tmp);


		fp.Read(tmp, 40);
		strcpy(temp->parentname, tmp);


		fp.Read(&temp->numTexture, sizeof(int));


		temp->texture = new CTexture* [temp->numTexture];

		for (int j=0; j<temp->numTexture; j++)
		{
			int	index;
			fp.Read(&index, sizeof(int));
			temp->texture[j] = g_texMgr.GetTexture(textureNames[index].c_str(), 0);
			temp->texture[j]->Lock();
		}


		matrix	offsetTM;
		fp.Read(&offsetTM, sizeof(matrix));


		fp.Read(&temp->pos, sizeof(vector3d));

		fp.Read(&temp->rotangle, sizeof(float));

		fp.Read(temp->rotaxis, sizeof(float)*3);

		fp.Read(&temp->scale, sizeof(float)*3);

		temp->mesh = new C3dMesh;

		fp.Read(&temp->mesh->m_numVert, sizeof(int));

		temp->mesh->m_vert = new vector3d[temp->mesh->m_numVert];
		fp.Read(temp->mesh->m_vert, sizeof(vector3d)*temp->mesh->m_numVert);


		for (int v=0; v<temp->mesh->m_numVert; v++)
		{
			vector3d	vertWithOffset;
			vertWithOffset.MatrixMult(temp->mesh->m_vert[v], offsetTM);
			temp->mesh->m_vert[v] = vertWithOffset;
		}

		fp.Read(&temp->mesh->m_numTVert, sizeof(int));
		temp->mesh->m_tvert = new tvertex3d[temp->mesh->m_numTVert];

		if (verMinor == 1)
		{
			for (int t=0; t<temp->mesh->m_numTVert; t++)
			{
				texCoor		tempvert;
				fp.Read(&tempvert, sizeof(texCoor));
				temp->mesh->m_tvert[t].color = 0xffffffff;
				temp->mesh->m_tvert[t].u = tempvert.u;
				temp->mesh->m_tvert[t].v = tempvert.v;
			}
		}
		else
		{
			fp.Read(temp->mesh->m_tvert, sizeof(tvertex3d)*temp->mesh->m_numTVert);
		}

		fp.Read(&temp->mesh->m_numFace, sizeof(int));
		temp->mesh->m_face = new face3d[temp->mesh->m_numFace];

		if (verMinor == 1)
		{
			for (int t=0; t<temp->mesh->m_numFace; t++) {
				face3d_v11	tempface;
				fp.Read(&tempface, sizeof(face3d_v11));
				temp->mesh->m_face[t] = tempface;
				temp->mesh->m_face[t].smoothGroup = 0;
			}
		}
		else
		{
			fp.Read(temp->mesh->m_face, sizeof(face3d)*temp->mesh->m_numFace);
		}

		temp->mesh->SortFace();
		temp->mesh->UpdateNormal();

		if (verMinor >= 5)
		{
			temp->posanim.Load(&fp);
		}
		temp->rotanim.Load(&fp);

		m_objectList.push_back(temp);
		m_meshList[temp->name] = temp->mesh;
	}

	delete[] textureNames;

	if (verMinor < 5)
	{
		C3dNodeRes *	main = FindNode(mainNode);
		main->posanim.Load(&fp);
	}


	C3dNodeResList::iterator j = m_objectList.begin();
	while (j != m_objectList.end())
	{
		if ((*j)->parentname[0])
		{
			C3dNodeRes *	t = FindNode((*j)->parentname);
			(*j)->parent = t;
			t->child.push_back(*j);
		}
		j++;
	}
	m_rootObjList.push_back(mainNode);

	int		numVol;
	fp.Read(&numVol, sizeof(int));

	if (verMajor == 1 && verMinor > 2)
	{
		for (i=0; i<numVol; i++)
		{
			CVolumeBox *	n = new CVolumeBox;
			CVolumeBox_v21	t;
			fp.Read(&t, sizeof(CVolumeBox_v21));
			n->flag = t.flag;
			n->m_pos = t.m_pos;
			n->m_rot = t.m_rot;
			n->m_size = t.m_size;
			if (n->flag == 0) {
				n->m_size.x += 3;
				n->m_size.z += 3;
			}
			m_volumeBoxList.push_back(n);
		}
	}
	else if (verMajor == 1 && verMinor == 2)
	{
		for (int i=0; i<numVol; i++) {
			CVolumeBox *	n = new CVolumeBox;
			CVolumeBox_v11	t;
			fp.Read(&t, sizeof(CVolumeBox_v11));
			n->flag = 0;
			n->m_pos = t.m_pos;
			n->m_rot = t.m_rot;
			n->m_size = t.m_size;
			m_volumeBoxList.push_back(n);
		}
	}

	fp.Close();
	strcpy(name, fName);
	return	true;
}


C3dNodeRes *	C3dModelRes :: FindNode(const char *name)
{
	C3dNodeResList::iterator i = m_objectList.begin();
	while (i != m_objectList.end()) {
		if (!(strcmp((*i)->name, name))) {
			return	*i;
		}
		i++;
	}
	return	NULL;
}

C3dFrameState :: C3dFrameState()
{
	m_vertices = NULL;
	m_tvertices = NULL;
}

void	C3dFrameState :: CreateVertex(int numVertex)
{
	m_vertices = new vector3d[numVertex];
}

void	C3dFrameState :: CreateTVertex(int numVertex)
{
	m_tvertices = new tvertex3d[numVertex];
}

C3dFrameState :: ~C3dFrameState()
{
	if (m_vertices)
		delete [] m_vertices;
	if (m_tvertices)
		delete [] m_tvertices;
}

C3dAnimNodeRes :: C3dAnimNodeRes()
{
	parent = NULL;
	model = NULL;
	m_numVertices = 0;
	m_numFaces = 0;
	m_frames = NULL;
	m_faceLight = NULL;
	m_faces = NULL;
	m_textures = NULL;
}

C3dAnimNodeRes :: ~C3dAnimNodeRes()
{
	Release();
}



void	C3dAnimNodeRes :: CalcFaceLight(matrix &ptm, int time, vector3d &light, vector3d &diffuseCol, vector3d &ambientCol, int shading, int alpha)
{
	int		i, j, g;

	static	vector3d	faceNormals[MAX_FACE_PER_MESH];
	static	vector3d	vertNormals[MAX_VERTEX_PER_MESH];
	IntList	GroupList[MAX_GROUP];
	IntList::iterator	li;

	switch (shading) {
		case	0:
			for (i=0; i<m_numFaces; i++) {
				for (int k=0; k<3; k++)
					m_faceLight[i].color[k] = (alpha<<24) + 0x00ffffff;
			}
			break;
		case	1:
			for (i=0; i<m_numFaces; i++) {

				face3d &	face = m_faces[i];
				vector3d &	v1 = m_frames[time].m_vertices[face.vertindex[0]];
				vector3d &	v2 = m_frames[time].m_vertices[face.vertindex[1]];
				vector3d &	v3 = m_frames[time].m_vertices[face.vertindex[2]];

				vector3d	wv1, wv2, wv3;
				wv1.MatrixMult(v1, ptm);
				wv2.MatrixMult(v2, ptm);
				wv3.MatrixMult(v3, ptm);
				vector3d	wnormal;
				wnormal.CrossProduct(wv3-wv2, wv3-wv1);
				wnormal.Normalize();

				float	product = wnormal.x*light.x + wnormal.y*light.y + wnormal.z*light.z;


				if (product < 0)
					product = 0;

				float	r, g, b;
				int		intr, intg, intb;

				r = ambientCol.x + product*diffuseCol.x;
				if (r > 1)
					r = 1;
				g = ambientCol.y + product*diffuseCol.y;
				if (g > 1)
					g = 1;
				b = ambientCol.z + product*diffuseCol.z;
				if (b > 1)
					b = 1;
				for (int k=0; k<3; k++) {
					int		time2 = time;
					if (m_tvertIdentity) {
						time2 = 0;
					}
					tvertex3d	&	tv = m_frames[time2].m_tvertices[face.tvertindex[k]];
					if (tv.color) {
						intr = r * 128;
						intg = g * 128;
						intb = b * 128;
					} else {
						intr = r * 255;
						intg = g * 255;
						intb = b * 255;
					}
					m_faceLight[i].color[k] = (alpha<<24) | (intr << 16) | (intg << 8) | intb;
				}


			}
			break;
		case	2:

			for (i=0; i<m_numFaces; i++) {

				face3d &	face = m_faces[i];
				vector3d &	v1 = m_frames[time].m_vertices[face.vertindex[0]];
				vector3d &	v2 = m_frames[time].m_vertices[face.vertindex[1]];
				vector3d &	v3 = m_frames[time].m_vertices[face.vertindex[2]];

				vector3d	wv1, wv2, wv3;
				wv1.MatrixMult(v1, ptm);
				wv2.MatrixMult(v2, ptm);
				wv3.MatrixMult(v3, ptm);
				faceNormals[i].CrossProduct(wv3-wv2, wv3-wv1);
				faceNormals[i].Normalize();
				GroupList[face.smoothGroup].push_back(i);
			}

			for (g=0; g<MAX_GROUP; g++) {
				if (GroupList[g].size() == 0)
					continue;
				for (int i=0; i<m_numVertices; i++) {
					vertNormals[i].Set(0, 0, 0);
					for (j=0; j<m_numFaces; j++) {
						face3d &	face = m_faces[j];
						if (face.smoothGroup != g)
							continue;
						if (face.vertindex[0] == i || face.vertindex[1] == i || face.vertindex[2] == i)
							vertNormals[i] += faceNormals[j];
					}
					if (vertNormals[i].Magnitude() != 0)
						vertNormals[i].Normalize();
				}
				for (li=GroupList[g].begin(); li!=GroupList[g].end(); li++) {
					face3d &	face = m_faces[(*li)];
					for (int k=0; k<3; k++) {
						vector3d &	wnormal = vertNormals[face.vertindex[k]];
						float	product = wnormal.x*light.x + wnormal.y*light.y + wnormal.z*light.z;

						if (product < 0)
							product = 0;

						float	r, g, b;
						int		intr, intg, intb;

						r = ambientCol.x + product*diffuseCol.x;
						if (r > 1)
							r = 1;
						g = ambientCol.y + product*diffuseCol.y;
						if (g > 1)
							g = 1;
						b = ambientCol.z + product*diffuseCol.z;
						if (b > 1)
							b = 1;
						int		time2 = time;
						if (m_tvertIdentity) {
							time2 = 0;
						}
						tvertex3d	&	tv = m_frames[time2].m_tvertices[face.tvertindex[k]];
						if (tv.color) {
							intr = r * 128;
							intg = g * 128;
							intb = b * 128;
						} else {
							intr = r * 255;
							intg = g * 255;
							intb = b * 255;
						}
						m_faceLight[*li].color[k] = (alpha<<24) | (intr << 16) | (intg << 8) | intb;


					}
				}
			}
			break;
		default:
			{
				int i;
				for (i=0; i<m_numFaces; i++) {
					for (int k=0; k<3; k++)
						m_faceLight[i].color[k] = (alpha<<24) + 0x00ffffff;
				}
			}
			ErrorMsg("Invalid Shade Type");
			break;
	}
	C3dAnimNodeResList::iterator it = child.begin();
	while (it != child.end()) {
		(*it)->CalcFaceLight(ptm, time, light, diffuseCol, ambientCol, shading, alpha);
		it++;
	}
}


void	C3dAnimNodeRes :: Release()
{
	if (m_frames) {
		delete [] m_frames;
		m_frames = NULL;
	}
	if (m_faces) {
		delete [] m_faces;
		m_faces = NULL;
	}
	if (m_faceLight) {
		delete [] m_faceLight;
		m_faceLight = NULL;
	}
	if (m_textures) {
		for (int i=0; i<m_numTextures; i++) {
			m_textures[i]->Unlock();
			g_texMgr.DestroyTexture(m_textures[i]);
		}
		delete [] m_textures;
		m_textures = NULL;
	}
}


static C3dAnimModelRes C3dAnimModelResInstance(Exemplar(), "rsx", "model\\");


C3dAnimModelRes :: C3dAnimModelRes()
{
	m_numMaterials = 0;
	m_material = NULL;
	m_numFrame = 0;
	m_tickPerFrame = 0;
	m_zoom = 1.0f;
	m_shade = 2;
	m_pivot.Set(0, 0, 0);
	m_isShaded = FALSE;
	m_alpha = 0xff;
}

C3dAnimModelRes :: ~C3dAnimModelRes()
{
	Release();
}

void	C3dAnimModelRes :: Release()
{
	if (m_material) {
		delete [] m_material;
		m_material = NULL;
		m_numMaterials = 0;
	}
	C3dAnimNodeResList::iterator i = m_objectList.begin();
	while (i != m_objectList.end()) {
		delete *i;
		i++;
	}
	m_objectList.erase(m_objectList.begin(), m_objectList.end());
}

C3dAnimNodeRes *	C3dAnimModelRes :: FindNode(const char *name)
{
	C3dAnimNodeResList::iterator i = m_objectList.begin();
	while (i != m_objectList.end()) {
		if ((*i)->name == name) {
			return	*i;
		}
		i++;
	}
	return	NULL;
}

#include "rag_budget_fill.h"
bool	C3dAnimModelRes :: Load(const char *fName)
{
	CFile	fp;

	if(fp.Open(fName, CFile::modeRead) == false)
		return false;


	char	magicBuf[5];

	fp.Read(magicBuf, 4);
	magicBuf[4]=0;
	BYTE	verMajor;
	BYTE	verMinor;
	fp.Read(&verMajor, sizeof(BYTE));
	fp.Read(&verMinor, sizeof(BYTE));


	if (verMajor == 1 && verMinor >=  2) {
		fp.Read(&m_zoom, sizeof(float));
		fp.Read(&m_pivot, sizeof(vector3d));
	}
	if (verMajor == 1 && verMinor ==  3)
		fp.Read(&m_alpha, sizeof(int));
	fp.Read(&m_shade, sizeof(int));
	fp.Read(&m_numFrame, sizeof(int));
	fp.Read(&m_tickPerFrame, sizeof(int));
	char	reserved[16];
	fp.Read(reserved, 16);

	char	tmp[40];
	int		numTexture;
	string *textureNames;

	fp.Read(&numTexture, sizeof(int));
	textureNames = new string[numTexture];
	int i ;
	for (i=0; i<numTexture; i++) {
		fp.Read(tmp, 40);
		textureNames[i] = string(tmp);
		g_texMgr.GetTexture(tmp, 0);
	}
	string	mainNode;

	fp.Read(tmp, 40);
	mainNode = string(tmp);

	int		numNodes;
	fp.Read(&numNodes, sizeof(int));

	for (i=0; i<numNodes; i++)
	{
		C3dAnimNodeRes *	temp = new C3dAnimNodeRes;
		temp->model = this;
		temp->m_frames = new C3dFrameState[m_numFrame];

		int		nodeHeader;
		fp.Read(&nodeHeader, sizeof(int));
		if (nodeHeader != 0x12345678) {
			ErrorMsg("data corrupt");
			return	false;
		}

		fp.Read(tmp, 40);
		temp->name = string(tmp);


		fp.Read(tmp, 40);
		temp->parentname = string(tmp);


		fp.Read(&temp->m_numTextures, sizeof(int));


		temp->m_textures = new CTexture* [temp->m_numTextures];
		for (int j=0; j<temp->m_numTextures; j++) {
			int	index;
			fp.Read(&index, sizeof(int));
			string	texName = textureNames[index];
			temp->m_textures[j] = g_texMgr.GetTexture(texName.c_str(), 0);
			temp->m_textures[j]->Lock();
		}

		RAG_BUDGET_FILL(4)
		fp.Read(&temp->m_numVertices, sizeof(int));

		for (int k=0; k<m_numFrame; k++) {
			temp->m_frames[k].CreateVertex(temp->m_numVertices);
			fp.Read(temp->m_frames[k].m_vertices, sizeof(vector3d)*temp->m_numVertices);
		}

		fp.Read(&temp->m_numTVertices, sizeof(int));

		fp.Read(&temp->m_tvertIdentity, sizeof(BOOL));
		if (temp->m_tvertIdentity) {
			temp->m_frames[0].CreateTVertex(temp->m_numTVertices);
			fp.Read(temp->m_frames[0].m_tvertices, sizeof(tvertex3d)*temp->m_numTVertices);
		} else {
			for (k=0; k<m_numFrame; k++) {
				temp->m_frames[k].CreateTVertex(temp->m_numTVertices);
				fp.Read(temp->m_frames[k].m_tvertices, sizeof(tvertex3d)*temp->m_numTVertices);
			}
		}

		fp.Read(&temp->m_numFaces, sizeof(int));

		temp->m_faceLight = new faceLight[temp->m_numFaces];
		temp->m_faces = new face3d[temp->m_numFaces];
		fp.Read(temp->m_faces, sizeof(face3d)*temp->m_numFaces);

		m_objectList.push_back(temp);
		int		nodeFooter;
		fp.Read(&nodeFooter, sizeof(int));
		if (nodeFooter != 0x12345678) {
			ErrorMsg("data corrupt");
			return	false;
		}
	}
	fp.Close();
	delete [] textureNames;


	C3dAnimNodeResList::iterator j = m_objectList.begin();
	while (j != m_objectList.end()) {
		if (!(*j)->parentname.empty()) {
			C3dAnimNodeRes *	t = FindNode((*j)->parentname.c_str());
			(*j)->parent = t;
			t->child.push_back(*j);
		}
		j++;
	}
	m_rootObjList.push_back(mainNode.c_str());

	return	true;
}

static C3dGrannyModelRes C3dGrannyModelResInstance(Exemplar(), "gr2", "model\\3dmob\\");


C3dGrannyModelRes :: C3dGrannyModelRes()
{
	m_Scene.Model = NULL;
	m_pModel = NULL;
	m_nBoneType = 0;
	m_hair = NULL;
}

C3dGrannyModelRes :: ~C3dGrannyModelRes()
{
	if( m_Scene.Model ) Release();
}

bool C3dGrannyModelRes :: Load( const char* fName )
{
  bool Result = false;
	GrannyModel *Model;

  if(!GrannyVersionsMatch) {
	ErrorMsg("doesn't match the .h file used during compilation");
	return false;
  }

  m_pBoneRes = &g_GrannyBoneRes;
  m_Scene.ModelCount = 0;
  m_Scene.MaxBoneCount = 0;
  m_Scene.TextureCount = 0;
  m_Scene.MaxMutableVertexBufferCount = 0;

	const char* pCh = strchr( fName, '_' );
	if( pCh==NULL ) return false;

	int i=0;
	char	str[10];
	pCh++;
	while( *pCh!='.' && i<4 ) {
		str[i] = *pCh;
		i++, pCh++;
	}
	str[i] = 0;
	m_nBoneType = atoi(str);

	CFile	fp;

	if (!fp.Open(fName, CFile::modeRead)) {
		ErrorMsg((string("Cannot find File : ") + string(fName)).c_str());
		return	false;
	}

	m_Scene.LoadedFile = GrannyReadEntireFileFromMemory(fp.GetLength (), (void*)fp.GetBuf ());
	fp.Close();
  if(m_Scene.LoadedFile)
  {
	granny_file_info *FileInfo = GrannyGetFileInfo(m_Scene.LoadedFile);
	  strcpy( m_Scene.BaseFileName, fName);

	if(FileInfo) {
		m_Scene.TextureCount = FileInfo->TextureCount;
		m_Scene.Textures = new GrannyTexture[m_Scene.TextureCount];
		{
					for(int TextureIndex = 0;TextureIndex < m_Scene.TextureCount;TextureIndex++)
		  {
						m_Scene.Textures[TextureIndex].tex = NULL;
			CreateTexture(&m_Scene.Textures[TextureIndex], FileInfo->Textures[TextureIndex]);
					}
				}

		m_Scene.ModelCount = FileInfo->ModelCount;

		sAssert(m_Scene.Model == NULL);

		m_Scene.Model = new GrannyModel;


		{

		  granny_model *GrannyModel = FileInfo->Models[0];
		  Model = m_Scene.Model;
		  CreateModel(Model, GrannyModel);
					m_pSkeleton = FileInfo->Skeletons[0];
					m_pModel = GrannyModel;
		}

		m_Scene.SharedLocalPose = GrannyNewLocalPose(m_Scene.MaxBoneCount);

		if(m_Scene.MaxMutableVertexBufferSize)
		{
		  m_Scene.MaxMutableVertexBufferCount = 4;
		}

		Result = true;
	  }
	  else
	  {
		ErrorMsg("Unable to access file data .gr2");
	  }

	  GrannyFreeFileSection(m_Scene.LoadedFile,
							GrannyStandardRigidVertexSection);
	  GrannyFreeFileSection(m_Scene.LoadedFile,
							GrannyStandardRigidIndexSection);
	  GrannyFreeFileSection(m_Scene.LoadedFile,
							GrannyStandardDeformableIndexSection);
	  GrannyFreeFileSection(m_Scene.LoadedFile,
							GrannyStandardTextureSection);
  }
  else
  {
	ErrorMsg("Unable to load file .gr2 (C3dGrannyModelRes :: Load)");
  }

  SetProjectZ(1600.0f);

	int nHighVertex = 0, nVertex;
	for( i=0;i<m_Scene.Model->MeshCount;i++ ) {
		nVertex = GetMeshIndexCount(i);
		if( nVertex>nHighVertex ) nHighVertex = nVertex;
		if( nVertex>MAX_GRANNY_VERTEX ) {
			ErrorMsg("too many vertex granny model!");
		}
	}

  m_HitareaPos.x = 0.0f, m_HitareaPos.y = 0.0f, m_HitareaPos.z = 0.0f;
	m_HitRadx = 10.0f, m_HitRady = 10.0f;
  return(Result);
}

granny_control* C3dGrannyModelRes :: ActAnimation( int idx, granny_model_instance* instance, granny_control* control, int nLoop )
{
	if ( control==NULL ) {
		granny_animation* animation = m_pBoneRes->GetAnimation(m_nBoneType, idx);
		if( animation==NULL ) return NULL;
		control = GrannyPlayControlledAnimation(0.0f, animation, instance);
		if( control==NULL ) return NULL;
		GrannySetControlLoopCount(control, nLoop);

	}
	else {
		GrannySetControlActive(control, true);
	}
	return control;
}

void C3dGrannyModelRes :: DeActAnimation( granny_control* control )
{
	if( control ) {
		GrannySetControlActive(control, false);
		GrannyFreeControl(control);

	}
}

void C3dGrannyModelRes :: Release()
{

	if( m_Scene.Model )
  {
	GrannyModel *Model = m_Scene.Model;

	for(int MeshIndex = 0;MeshIndex < Model->MeshCount;MeshIndex++)
	{
	  GrannyMesh &Mesh = Model->Meshes[MeshIndex];

	  GrannyFreeMeshBinding(Mesh.Binding);
	  GrannyFreeMeshDeformer(Mesh.Deformer);

	  delete [] Mesh.TextureReferences;
			delete Mesh.VerIdx ;
			if( Mesh.Vertices ) delete Mesh.Vertices ;
	}

	GrannyFreeModelInstance(Model->GrannyInstance);
	GrannyFreeWorldPose(Model->WorldPose);
	delete [] Model->Meshes;
  }

	for(int TextureIndex = 0;TextureIndex < m_Scene.TextureCount;TextureIndex++)
	{
		GrannyTexture &Texture = m_Scene.Textures[TextureIndex];
		delete Texture.tex;
	}
	delete [] m_Scene.Textures;

  GrannyFreeLocalPose(m_Scene.SharedLocalPose);
  GrannyFreeFile(m_Scene.LoadedFile);

  delete m_Scene.Model;
	m_Scene.Model = NULL;
}

void C3dGrannyModelRes :: CreateTexture(GrannyTexture *Texture, granny_texture *GrannyTexture)
{
  Texture->Name = GrannyTexture->FromFileName;

  if((GrannyTexture->TextureType == GrannyColorMapTextureType) && (GrannyTexture->ImageCount == 1))
  {

	{
	  Texture->tex = CreateRGBATexture(GrannyTexture, 0);
	}
  }
  else
  {
	ErrorMsg("This sample only works with single-image : ColorMapTextureType textures");
  }
}

CTexture* C3dGrannyModelRes :: CreateRGBATexture(granny_texture *GrannyTexture, int ImageIndex)
{
  LPDIRECTDRAWSURFACE7 D3DTexture = NULL;
	CTexture* RetD3DTexture = NULL;

  BYTE HasAlpha = GrannyTextureHasAlpha(GrannyTexture);
  int Width = GrannyTexture->Width;
  int Height = GrannyTexture->Height;
	unsigned char *PixelBuffer = new BYTE[Width * Height * 4];
  unsigned char *PixelBuffer2 = new BYTE[Width * Height * 4];
	BYTE	r, g, b, a;

  GrannyCopyTextureImage(GrannyTexture, 0, 0, GrannyRGBA8888PixelFormat,
						 Width, Height, Width * 4, PixelBuffer);

	int sidx=0, didx=0;;

	if( HasAlpha ) {
		for( int i=0;i<Height;i++ ) {
			for( int j=0;j<Width;j++ ) {

				r = PixelBuffer[sidx+0];
				g = PixelBuffer[sidx+1];
				b = PixelBuffer[sidx+2];
				a = PixelBuffer[sidx+3];
 				PixelBuffer2[didx+0] =  b;
				PixelBuffer2[didx+1] = 	g;
				PixelBuffer2[didx+2] = 	r;
				PixelBuffer2[didx+3] =  a;
				didx+=4, sidx+=4;
			}
		}

		RetD3DTexture = g_texMgr.CreateTexture( Width, Height, (DWORD*)PixelBuffer2, PF_A4R4G4B4, 0 );
	}
	else {
		for( int i=0;i<Height;i++ ) {
			for( int j=0;j<Width;j++ ) {

				r = PixelBuffer[sidx+0];
				g = PixelBuffer[sidx+1];
				b = PixelBuffer[sidx+2];
				a = PixelBuffer[sidx+3];
 				PixelBuffer2[didx+0] =  b;
				PixelBuffer2[didx+1] = 	g;
				PixelBuffer2[didx+2] = 	r;
				PixelBuffer2[didx+3] =  255;
				didx+=4, sidx+=4;
			}
		}
		RetD3DTexture = g_texMgr.CreateTexture( Width, Height, (DWORD*)PixelBuffer2, PF_A1R5G5B5, 0 );
	}

	delete [] PixelBuffer2;
	delete [] PixelBuffer;

  return(RetD3DTexture);
}

void C3dGrannyModelRes :: CreateModel(GrannyModel *Model, granny_model *GrannyModel)
{
  int BoneCount = GrannyModel->Skeleton->BoneCount;
  if(m_Scene.MaxBoneCount < BoneCount)
  {
	m_Scene.MaxBoneCount = BoneCount;
  }

  Model->GrannyInstance = GrannyInstantiateModel(GrannyModel);

  Model->WorldPose = GrannyNewWorldPose(BoneCount);

  Model->MeshCount = GrannyModel->MeshBindingCount;
  Model->Meshes = new GrannyMesh[Model->MeshCount];
  for(int MeshIndex = 0;MeshIndex < Model->MeshCount;MeshIndex++)
  {
	CreateMesh(&Model->Meshes[MeshIndex], GrannyModel->MeshBindings[MeshIndex].Mesh, Model->GrannyInstance);
  }
}

void C3dGrannyModelRes :: CreateMesh(GrannyMesh *Mesh, granny_mesh *GrannyMesh, granny_model_instance *InModel)
{
  Mesh->Mesh = GrannyMesh;

  granny_skeleton *Skeleton = GrannyGetSourceSkeleton(InModel);
  Mesh->Binding = GrannyNewMeshBinding(GrannyMesh, Skeleton, Skeleton);

  int IndexCount = GrannyGetMeshIndexCount(GrannyMesh);
  int IndexBufferSize = sizeof(unsigned short) * IndexCount;

  Mesh->VerIdx = new WORD[IndexCount];

  if(IndexCount) {
	  BYTE *Indices = (BYTE*)Mesh->VerIdx;
	  GrannyCopyMeshIndices(GrannyMesh, sizeof(WORD), Indices);
  }

  int VertexCount = GrannyGetMeshVertexCount(GrannyMesh);
  int VertexBufferSize = VertexCount * sizeof(granny_pnt332_vertex);

	Mesh->Vertices = NULL;

  if(GrannyMeshIsRigid(GrannyMesh)) {
		Mesh->Vertices = new BYTE[sizeof(granny_pnt332_vertex)*VertexCount];
		GrannyCopyMeshVertices(GrannyMesh, GrannyPNT332VertexType, Mesh->Vertices);

	Mesh->Deformer = 0;
  }
  else
  {
	if(m_Scene.MaxMutableVertexBufferSize < VertexBufferSize) {
		m_Scene.MaxMutableVertexBufferSize = VertexBufferSize;
	}

	Mesh->Deformer = GrannyNewMeshDeformer(
		GrannyGetMeshVertexType(GrannyMesh), GrannyPNT332VertexType, GrannyDeformPositionNormal);

	if(!Mesh->Deformer){
	  ErrorMsg("Granny didn't find a matching deformer for the vertex format used by mesh");
	}
  }

  Mesh->TextureCount = GrannyMesh->MaterialBindingCount;
  Mesh->TextureReferences = new GrannyTexture *[Mesh->TextureCount];
  for(int TextureIndex = 0;TextureIndex < Mesh->TextureCount;TextureIndex++) {
	Mesh->TextureReferences[TextureIndex] = FindTexture(GrannyMesh->MaterialBindings[TextureIndex].Material);
  }
}

GrannyTexture* C3dGrannyModelRes :: FindTexture(granny_material *GrannyMaterial)
{
	GrannyTexture *Texture = NULL;

  granny_texture *GrannyTexture = GrannyGetMaterialTextureByType(GrannyMaterial, GrannyDiffuseColorTexture);

  if(GrannyTexture)
  {

		for(int TextureIndex = 0;TextureIndex < m_Scene.TextureCount;TextureIndex++)
	{
	  Texture = &m_Scene.Textures[TextureIndex];
	  if(strcmp(Texture->Name, GrannyTexture->FromFileName) == 0) {
		return(Texture);
	  }
		}
  }
  return(0);
}

void C3dGrannyModelRes :: SetProjectZ(float fClip)
{
	m_ProjectZ = fClip + fClip / 5.0f;
}

granny_world_pose*  C3dGrannyModelRes :: UpdateAni(float GameClock, granny_model_instance* instance)
{

  GrannyModel *Model = m_Scene.Model;
	if( Model==NULL ) return NULL;

	granny_skeleton *Skeleton = GrannyGetSourceSkeleton(instance);

  int BoneCount = Skeleton->BoneCount;

  GrannySetModelClock(instance, GameClock);
  GrannySampleModelAnimations(instance, 0, BoneCount, m_Scene.SharedLocalPose);
  GrannyBuildWorldPose( Skeleton, 0, BoneCount, m_Scene.SharedLocalPose, 0, Model->WorldPose);
  GrannyFreeCompletedModelControls(instance);


	return Model->WorldPose;
}

VOID* C3dGrannyModelRes :: RenderMesh(int nMeshIdx, int* RetTex, int* RetFaceArr, int* nIndexNo, matrix* poseMat,
																			granny_world_pose* pWorldpose )
{
  return RenderModel(m_Scene.Model, nMeshIdx, RetTex, RetFaceArr, nIndexNo, poseMat, pWorldpose );
}

VOID* C3dGrannyModelRes :: GetRigidMesh( int nMeshIdx )
{
	GrannyMesh *Mesh;
	Mesh = &m_Scene.Model->Meshes[nMeshIdx];
	if(GrannyMeshIsRigid(Mesh->Mesh)) {
		return (VOID*)Mesh->Vertices;
	}
	return NULL;
}

bool C3dGrannyModelRes :: SetHairAndHeadMesh( void )
{
	if( !m_hair ) return false;

	for( int i=0;i<m_Scene.Model->MeshCount;i++ ) {
		GrannyMesh* Mesh = &m_Scene.Model->Meshes[i];

		if( strstr(Mesh->Mesh->Name, "hair_pivot") ) {
			m_hair->nSubMeshIdx = i;
			return true;
		}
	}
	return false;
}

VOID* C3dGrannyModelRes :: RenderModel(GrannyModel *Model, int nMeshIdx, int* RetTex, int* RetFaceArr, int* nIndexNo,
																			 matrix* poseMat, granny_world_pose* pWorldpose )
{
	GrannyMesh *Mesh;
	VOID*	ret = NULL;





	if( pWorldpose==NULL ) {
		pWorldpose = Model->WorldPose;
	}
  granny_matrix_4x4 *CompositeBuffer = GrannyGetWorldPoseComposite4x4Array(pWorldpose);


  {
	Mesh = &Model->Meshes[nMeshIdx];



	int *ToBoneIndices = GrannyGetMeshBindingToBoneIndices(Mesh->Binding);
		int VertexCount = GrannyGetMeshVertexCount(Mesh->Mesh);
		granny_tri_material_group *Group = GrannyGetMeshTriangleGroups(Mesh->Mesh);

		GrannyTexture *Texture = Mesh->TextureReferences[Group->MaterialIndex];
		if(Texture && Mesh->TextureCount>Group->MaterialIndex) {
			*RetTex = (int)Texture->tex;
		}
		else {
			*RetTex = NULL;
		}

		if(GrannyMeshIsRigid(Mesh->Mesh)) {

		float* Transform = (float*)GrannyGetWorldPoseComposite4x4(Model->WorldPose, ToBoneIndices[0]);

				poseMat->SetRow1(Transform[0],Transform[1],Transform[2]);
				poseMat->SetRow2(Transform[4],Transform[5],Transform[6]);
				poseMat->SetRow3(Transform[8],Transform[9],Transform[10]);
				poseMat->SetRow4(Transform[12],Transform[13],Transform[14]);

				ret = (VOID*)Mesh->Vertices;
				*RetFaceArr = (int)Mesh->VerIdx;
				*nIndexNo = GrannyGetMeshIndexCount(Mesh->Mesh)/3;

				if( strstr(Mesh->Mesh->Name, "hair_pivot") ) {
					granny_pnt332_vertex* srcVert = (granny_pnt332_vertex*)Mesh->Vertices;
					if( m_hair ) {



						m_hair->poseMat = *poseMat;
					}
				}

	}
		else
	if(Mesh->Deformer) {

	  GrannyDeformVertices(Mesh->Deformer,
						   ToBoneIndices, (float *)CompositeBuffer,
						   VertexCount,
						   GrannyGetMeshVertices(Mesh->Mesh),
						   m_Scene.MatVer);

	  	int IndexSize = GrannyGetMeshBytesPerIndex(Mesh->Mesh);
			unsigned char *Indices = (unsigned char *)GrannyGetMeshIndices(Mesh->Mesh);

			ret = (VOID*)m_Scene.MatVer;
			*RetFaceArr = (int)Mesh->VerIdx;
			*nIndexNo = GrannyGetMeshIndexCount(Mesh->Mesh)/3;

			poseMat->MakeIdentity();
	}
  }
  return ret;
}

bool C3dGrannyModelRes :: CalculateHitArea(RENDER_INFO_RECT &info, matrix &view, matrix &dispTrans, vector3d &cam)
{
	w3dtlvertex		cpos;
	float			halfrecsizex, halfrecsizey;
	float			px, py, adj;
	vector3d		sour, chkvec;
	matrix			tr;

	tr.MakeZRotation(3.141592f);
	sour.MatrixMult(m_HitareaPos, tr);
	tr.MultMatrix(dispTrans,view);

	cpos.sx = sour.x*tr.v11 + sour.y*tr.v21 + sour.z*tr.v31 + tr.v41;
	cpos.sy = sour.x*tr.v12 + sour.y*tr.v22 + sour.z*tr.v32 + tr.v42;
	cpos.sz = sour.x*tr.v13 + sour.y*tr.v23 + sour.z*tr.v33 + tr.v43;

	chkvec.x = cam.x - sour.x;
	chkvec.y = cam.y - sour.y;
	chkvec.z = cam.z - sour.z;
	chkvec.Normalize();

	chkvec.y = (float)fabs(chkvec.y);
	if (chkvec.y > 1.0f)
		chkvec.y = 1.0f;

	adj = m_HitRady - ((m_HitRady - m_HitRadx) * chkvec.y);

	halfrecsizex = _sqrt((m_HitRadx*m_HitRadx) * 0.5f);
	halfrecsizey = _sqrt((adj*adj) * 0.5f);
	px = cpos.sx - halfrecsizex;
	py = cpos.sy - halfrecsizey;
	info.oow	= 1.0f / cpos.sz;
	info.left	= g_renderer->m_xoffset + (px*g_renderer->m_hpc) * info.oow;
	info.top	= g_renderer->m_yoffset + (py*g_renderer->m_vpc) * info.oow;

	px = cpos.sx + halfrecsizex;
	py = cpos.sy + halfrecsizey;

	info.right = g_renderer->m_xoffset + (px*g_renderer->m_hpc) * info.oow;
	info.bottom = g_renderer->m_yoffset + (py*g_renderer->m_vpc) * info.oow;

	if ((DWORD)info.left>(DWORD)g_renderer->m_width && (DWORD)info.right>(DWORD)g_renderer->m_width &&
		(DWORD)info.top>(DWORD)g_renderer->m_height && (DWORD)info.bottom>(DWORD)g_renderer->m_height)
		return	FALSE;

	return	TRUE;
}

granny_file* C3dGrannyModelRes :: LoadFile(char const *BaseFileName, char const *Suffix)
{
	static char FileNameBuffer[512];
	sprintf(FileNameBuffer, "%s%s.gr2", BaseFileName, Suffix);

		CFile	fp;
		if (!fp.Open(FileNameBuffer, CFile::modeRead)) {
			ErrorMsg((string("Cannot find File : ") + string(FileNameBuffer)).c_str());
			return	false;
		}
		granny_file *File = GrannyReadEntireFileFromMemory(fp.GetLength (), (void*)fp.GetBuf ());
		fp.Close();

	return(File);
}

int C3dGrannyModelRes :: GetMeshIndexCount( int nMeshNo )
{
	return GrannyGetMeshIndexCount(m_Scene.Model->Meshes[nMeshNo].Mesh)/3;
}

void C3dGrannyModelRes :: BindMeshToSkeleton( C3dGrannyModelRes* pDstRes )
{
	GrannyModel* pSrcModel = m_Scene.Model;

	for( int i=0;i<GetMeshNo();i++ ) {
		GrannyMesh* mesh = &pSrcModel->Meshes[i];
		granny_mesh* pMesh = mesh->Mesh;
	GrannyFreeMeshBinding(mesh->Binding);
		mesh->Binding = GrannyNewMeshBinding(mesh->Mesh, GetSkeleton(), pDstRes->GetSkeleton() );
	}
}

C3dGrannyBoneRes :: C3dGrannyBoneRes( void )
{
	m_Scene.Model = NULL;
	for( int i=0;i<MAX_BONE_TYPE;i++ ) {
		for( int j=0;j<MAX_GRANNY_CONTROL;j++ ) {
			m_Animation[i][j] = NULL;
		}
	}
}

C3dGrannyBoneRes :: ~C3dGrannyBoneRes()
{
	if( m_Scene.Model ) Release();
}

void C3dGrannyBoneRes :: Init( void )
{
}

bool C3dGrannyBoneRes :: AddBone( const char* fName, int nType, int nAniIdx )
{
  bool Result = false;
	GrannyModel *Model;

  if(!GrannyVersionsMatch) {
	ErrorMsg("doesn't match the .h file used during compilation");
	return false;
  }

  m_Scene.ModelCount = 0;
  m_Scene.MaxBoneCount = 0;
  m_Scene.TextureCount = 0;
  m_Scene.MaxMutableVertexBufferCount = 0;


	CFile	fp;

	if (fp.Open(fName, CFile::modeRead)) {
		m_Scene.LoadedFile = GrannyReadEntireFileFromMemory(fp.GetLength (), (void*)fp.GetBuf ());
		fp.Close();

		if(m_Scene.LoadedFile) {
			granny_file_info *FileInfo = GrannyGetFileInfo(m_Scene.LoadedFile);
			strcpy( m_Scene.BaseFileName, fName );

			if(FileInfo) {
				m_Scene.TextureCount = FileInfo->TextureCount;
				m_Scene.ModelCount = FileInfo->ModelCount;

				if( m_Scene.Model ) {
					FreeModel();
				}

				m_Scene.Model = new GrannyModel;

				{

					granny_model *GrannyModel = FileInfo->Models[0];
					Model = m_Scene.Model;
					CreateModel(Model, GrannyModel);

					if(FileInfo->AnimationCount)
					{

						granny_animation *Animation = 0;
						char *c = (char*)strchr(m_Scene.BaseFileName, '.');
						if( c!=NULL ) *c = 0;

						granny_file *File = LoadFile(m_Scene.BaseFileName, "");
						if(File)
						{
							granny_file_info *FileInfo = GrannyGetFileInfo(File);
							if(FileInfo)
							{
								if(Model->GrannyInstance && (FileInfo->AnimationCount == 1))
								{
									Animation = FileInfo->Animations[0];
								}
								else
								{
									ErrorMsg("This sample only supports single-animation files");
								}
							}
						}

						m_Animation[nType][nAniIdx] = Animation;
						if( !Animation ) return false;
					}
				}

				m_Scene.SharedLocalPose = GrannyNewLocalPose(m_Scene.MaxBoneCount);

				if(m_Scene.MaxMutableVertexBufferSize)
				{
					m_Scene.MaxMutableVertexBufferCount = 4;
				}

				Result = true;
			}
			else
			{
				ErrorMsg("Unable to access file data .gr2");
			}

			GrannyFreeFileSection(m_Scene.LoadedFile, GrannyStandardRigidVertexSection);
			GrannyFreeFileSection(m_Scene.LoadedFile, GrannyStandardRigidIndexSection);
			GrannyFreeFileSection(m_Scene.LoadedFile, GrannyStandardDeformableIndexSection);
			GrannyFreeFileSection(m_Scene.LoadedFile, GrannyStandardTextureSection);

			SetProjectZ(1600.0f);
			return Result;
		}
	}

	return false;
}

granny_animation* C3dGrannyBoneRes :: GetAnimation( int nType, int nAniIdx )
{
	granny_animation **pAni = &m_Animation[nType][nAniIdx];
	if ( *pAni == NULL )
	{
		const int MAX_ANIIDX = 5;
		const int MAX_TYPE = 10;
		static char * name[MAX_ANIIDX] = {"stand","move","attack","dead","damage" };
		static JOBTYPE STANDNAME[MAX_TYPE] = { JT_EMPELIUM,JT_GUILD_FLAG,JT_TREASURE_BOX1,
			JT_MON_BEGIN,JT_MON_BEGIN,JT_MON_BEGIN,JT_MON_BEGIN,
			JT_KNIGHT_GUARDIAN,JT_ARCHER_GUARDIAN,JT_SOLDIER_GUARDIAN };
		static char buf[MAX_PATH];
		buf[0] = NULL;

		if ( nType >= 0 && nType < MAX_TYPE && nAniIdx >= 0 && nAniIdx < MAX_ANIIDX )
		{
			if ( nAniIdx == ST_STAND )
			{

				sprintf(buf,"model\\3dmob\\%s",g_session.GetJobName( STANDNAME[nType] ));
			}
			else
			{
				sprintf(buf,"model\\3dmob_bone\\%d_%s.gr2",nType,name[nAniIdx]);
			}
		}
		else
			return NULL;

		if ( buf[0] != NULL && CFile::IsFileExist(buf) )
		{
			if (nType != 1)
				AddBone(buf,nType,nAniIdx);
		}
		else
			return NULL;
	}

	return *pAni;
}

void C3dGrannyBoneRes :: FreeModel( void )
{
	if( m_Scene.Model )
  {
	GrannyModel *Model = m_Scene.Model;

	for(int MeshIndex = 0;MeshIndex < Model->MeshCount;MeshIndex++)
	{
	  GrannyMesh &Mesh = Model->Meshes[MeshIndex];

	  GrannyFreeMeshBinding(Mesh.Binding);
	  GrannyFreeMeshDeformer(Mesh.Deformer);

			delete Mesh.VerIdx ;
	}

	GrannyFreeModelInstance(Model->GrannyInstance);
	GrannyFreeWorldPose(Model->WorldPose);
	delete [] Model->Meshes;
  }
  delete m_Scene.Model;
	m_Scene.Model = NULL;
}

void C3dGrannyBoneRes :: Release()
{

	FreeModel();

	for( int i=0;i<MAX_BONE_TYPE;i++ ) {
		for( int j=0;j<MAX_GRANNY_CONTROL;j++ ) {
			if( m_Animation[i][j] ) {
				m_Animation[i][j] = NULL;
			}
		}
	}

  GrannyFreeLocalPose(m_Scene.SharedLocalPose);
  GrannyFreeFile(m_Scene.LoadedFile);
	}

void C3dGrannyBoneRes :: CreateModel(GrannyModel *Model, granny_model *GrannyModel)
{
  int BoneCount = GrannyModel->Skeleton->BoneCount;
  if(m_Scene.MaxBoneCount < BoneCount)
  {
	m_Scene.MaxBoneCount = BoneCount;
  }

  Model->GrannyInstance = GrannyInstantiateModel(GrannyModel);

  Model->WorldPose = GrannyNewWorldPose(BoneCount);

  Model->MeshCount = GrannyModel->MeshBindingCount;
  Model->Meshes = new GrannyMesh[Model->MeshCount];
  for(int MeshIndex = 0;MeshIndex < Model->MeshCount;MeshIndex++)
  {
	CreateMesh(&Model->Meshes[MeshIndex], GrannyModel->MeshBindings[MeshIndex].Mesh, Model->GrannyInstance);
  }
}

void C3dGrannyBoneRes :: CreateMesh(GrannyMesh *Mesh, granny_mesh *GrannyMesh, granny_model_instance *InModel)
{
  Mesh->Mesh = GrannyMesh;

  granny_skeleton *Skeleton = GrannyGetSourceSkeleton(InModel);
  Mesh->Binding = GrannyNewMeshBinding(GrannyMesh, Skeleton, Skeleton);

  int IndexCount = GrannyGetMeshIndexCount(GrannyMesh);
  int IndexBufferSize = sizeof(unsigned short) * IndexCount;

  Mesh->VerIdx = new WORD[IndexCount];

  if(IndexCount) {
	  BYTE *Indices = (BYTE*)Mesh->VerIdx;
	  GrannyCopyMeshIndices(GrannyMesh, sizeof(WORD), Indices);
  }

  int VertexCount = GrannyGetMeshVertexCount(GrannyMesh);
  int VertexBufferSize = VertexCount * sizeof(granny_pnt332_vertex);

  if(GrannyMeshIsRigid(GrannyMesh)) {

	  Mesh->Deformer = 0;
  }
  else
  {
	if(m_Scene.MaxMutableVertexBufferSize < VertexBufferSize) {
		m_Scene.MaxMutableVertexBufferSize = VertexBufferSize;
	}

	Mesh->Deformer = GrannyNewMeshDeformer(
		GrannyGetMeshVertexType(GrannyMesh), GrannyPNT332VertexType, GrannyDeformPositionNormal);

	if(!Mesh->Deformer){
	  ErrorMsg("Granny didn't find a matching deformer for the vertex format used by mesh");
	}
  }
}

void C3dGrannyBoneRes :: SetProjectZ(float fClip)
{
	m_ProjectZ = fClip + fClip / 5.0f;
}

granny_animation* C3dGrannyBoneRes :: LoadAnimation(GrannyModel *Model, char const *BaseFileName, char const *Suffix)
{
  granny_animation *Animation = 0;

  char *c = (char*)strchr(BaseFileName, '.');
	if( c!=NULL ) *c = 0;

  granny_file *File = LoadFile(BaseFileName, Suffix);
  if(File)
  {
	granny_file_info *FileInfo = GrannyGetFileInfo(File);
	if(FileInfo)
	{
	  if(Model->GrannyInstance && (FileInfo->AnimationCount == 1))
	  {
		Animation = FileInfo->Animations[0];
	  }
	  else
	  {
		ErrorMsg("This sample only supports single-animation files");
	  }
	}
  }

  return(Animation);
}

granny_file* C3dGrannyBoneRes :: LoadFile(char const *BaseFileName, char const *Suffix)
{
  static char FileNameBuffer[512];
	CFile	fp;
  sprintf(FileNameBuffer, "%s%s.gr2", BaseFileName, Suffix);

	if (!fp.Open(FileNameBuffer, CFile::modeRead)) {
		ErrorMsg((string("Cannot find File : ") + string(FileNameBuffer)).c_str());
		return	false;
	}

	granny_file *File = GrannyReadEntireFileFromMemory(fp.GetLength (), (void*)fp.GetBuf ());
	fp.Close();
  return(File);
}
