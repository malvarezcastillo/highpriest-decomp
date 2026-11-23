#include "common.h"
#include "resource/Model.h"
#include "resource/World.h"
#include "resource/Attr.h"
#include "resource/Ground.h"
#include "device/Sound.h"
#include "3dActor.h"
#include "GameActor.h"
#include "GameWorld.h"
#include "GameMode.h"
#include "Session.h"
#include "pdb_enums.h"
#include "3dGround.h"
#include "base/File.h"
#include "PathFinder.h"
#include "window/UISys.h"
#include <stdio.h>
#include <list>
#include <vector>
#include <algorithm>

extern CSession g_session;
extern CModeMgr g_modeMgr;
extern CRenderer* g_renderer;
extern UIWindowMgr g_windowMgr;
extern void* g_farModelThread;
extern std::vector<std::pair<float, C3dWorldRes::actorInfo*> > g_3dActors;
typedef std::vector<C3dActor *> BgObjList;


extern vector3d ambientCol;
extern vector3d diffuseCol;
extern vector3d lightDir;
extern vector3d player_pos;

vector3d lightDir;
vector3d diffuseCol;
vector3d ambientCol;
std::vector<std::pair<float, C3dWorldRes::actorInfo*> > g_3dActors;
void* g_farModelThread = NULL;
vector3d player_pos(0.0f, 0.0f, 0.0f);
unsigned long __stdcall LoadFarModels(void* pvoid);




typedef C3dWorldRes::actorInfo _ActorInfo_t;
typedef std::pair<float, _ActorInfo_t *> _ActorInfoPair_t;

unsigned long __stdcall LoadFarModels(void *pvoid)
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	std::sort(g_3dActors.begin(), g_3dActors.end());

	for (std::vector<_ActorInfoPair_t>::iterator i = g_3dActors.begin();
		 i != g_3dActors.end();
		 ++i) {
		_ActorInfo_t* actor_info = i->second;
		C3dModelRes* newRes = (C3dModelRes*)g_resMgr().Get(actor_info->modelName, false);

		if (!newRes) {
			Trace("%s model not found.", actor_info->modelName);
			continue;
		}

		C3dNodeRes* newNode = newRes->FindNode(actor_info->nodeName);
		if (!newNode)
			newNode = newRes->GetFirstNode();
		if (!newNode) {
			Trace("%s model has no node.", actor_info->modelName);
			continue;
		}

		C3dActor* newObj = new C3dActor;
		newObj->SetAnimSpeed(actor_info->animSpeed);
		newObj->SetVolumeBoxInfo(&newRes->m_volumeBoxList);
		newObj->SetPos(actor_info->pos);
		newObj->SetRot(actor_info->rot);
		newObj->SetScale(actor_info->scale);
		newObj->AssignModel(newRes);
		newObj->SetBlockType(actor_info->blockType);
		newObj->SetIsHideCheck(FALSE);
		newObj->SetName(actor_info->name);
		newObj->UpdateVertexColor(lightDir, diffuseCol, ambientCol);

		((CWorld*)pvoid)->Add3DActorToWorld(newObj);
		Sleep(0);
	}

	Trace("Load Model :: Lazy Model Load Complete");
	return 0;
}

SceneGraphNode :: SceneGraphNode()
{
	m_parent = NULL;
	for (int i=0; i<4; ++i)
	{
		m_child[i] = NULL;
	}
	m_needUpdate = true;
}

SceneGraphNode :: ~SceneGraphNode()
{
	for (int i=0; i<4; ++i)
	{
		if (m_child[i])
		{
			delete	m_child[i];
			m_child[i] = NULL;
		}
	}
}

void SceneGraphNode::Build(int level)
{
	m_center.x = (m_aabb.min.x + m_aabb.max.x) / 2;
	m_center.z = (m_aabb.min.z + m_aabb.max.z) / 2;
	m_center.y = 0;
	m_aabb.min.y = -999999;
	m_aabb.max.y = 999999;

	if (level == 5)
		return;

	m_child[0] = new SceneGraphNode;
	m_child[0]->m_parent = this;
	m_child[0]->m_aabb.min.x = m_aabb.min.x;
	m_child[0]->m_aabb.max.x = m_center.x;
	m_child[0]->m_aabb.min.z = m_aabb.min.z;
	m_child[0]->m_aabb.max.z = m_center.z;

	m_child[1] = new SceneGraphNode;
	m_child[1]->m_parent = this;
	m_child[1]->m_aabb.min.x = m_center.x;
	m_child[1]->m_aabb.max.x = m_aabb.max.x;
	m_child[1]->m_aabb.min.z = m_aabb.min.z;
	m_child[1]->m_aabb.max.z = m_center.z;

	m_child[2] = new SceneGraphNode;
	m_child[2]->m_parent = this;
	m_child[2]->m_aabb.min.x = m_aabb.min.x;
	m_child[2]->m_aabb.max.x = m_center.x;
	m_child[2]->m_aabb.min.z = m_center.z;
	m_child[2]->m_aabb.max.z = m_aabb.max.z;

	m_child[3] = new SceneGraphNode;
	m_child[3]->m_parent = this;
	m_child[3]->m_aabb.min.x = m_center.x;
	m_child[3]->m_aabb.max.x = m_aabb.max.x;
	m_child[3]->m_aabb.min.z = m_center.z;
	m_child[3]->m_aabb.max.z = m_aabb.max.z;

	for (int i = 0; i < 4; ++i)
		m_child[i]->Build(level + 1);
}

static bool TestAABBvsOBBIntersection(const C3dOBB& obb, const C3dAABB& aabb);

void	SceneGraphNode :: InsertObject(C3dActor *actor, int level)
{
	if (level == 5)
	{
		m_actorList.push_back(actor);
		SceneGraphNode *p;
		for (p = this; p; p=p->m_parent)
		{
			p->m_needUpdate = true;
		}
		return;
	}

	for (int i=0; i<4; ++i)
	{
		if (TestAABBvsOBBIntersection(actor->m_oBoundingBox, m_child[i]->m_aabb))
		{
			m_child[i]->InsertObject(actor, level+1);
		}
	}
}


static bool TestAABBvsOBBIntersection(const C3dOBB& obb, const C3dAABB& aabb)
{
	float afEA[3], afEB[3];

	afEA[0] = obb.halfSize.x;
	afEA[1] = obb.halfSize.y;
	afEA[2] = obb.halfSize.z;
	afEB[0] = (aabb.max.x - aabb.min.x) / 2;
	afEB[1] = (aabb.max.y - aabb.min.y) / 2;
	afEB[2] = (aabb.max.z - aabb.min.z) / 2;

	vector3d aabbCenter;
	aabbCenter.Set((aabb.max.x + aabb.min.x) / 2,
				   (aabb.max.y + aabb.min.y) / 2,
				   (aabb.max.z + aabb.min.z) / 2);
	vector3d kD = obb.center - aabbCenter;

	float aafC[3][3];
	float aafAbsC[3][3];
	float afAD[3];

	double fR0, fR1, fR;
	double fR01;
	double error_range = 0.000001f;

	aafC[0][0] = obb.u.x;
	aafC[0][1] = obb.u.y;
	aafC[0][2] = obb.u.z;
	afAD[0] = kD.DotProduct(obb.u);
	aafAbsC[0][0] = fabs(aafC[0][0]);
	aafAbsC[0][1] = fabs(aafC[0][1]);
	aafAbsC[0][2] = fabs(aafC[0][2]);
	fR = fabs(afAD[0]);
	fR1 = afEB[0] * aafAbsC[0][0] + afEB[1] * aafAbsC[0][1] + afEB[2] * aafAbsC[0][2];
	fR01 = afEA[0] + fR1;
	if (fR - fR01 > error_range)
		return false;

	aafC[1][0] = obb.v.x;
	aafC[1][1] = obb.v.y;
	aafC[1][2] = obb.v.z;
	afAD[1] = kD.DotProduct(obb.v);
	aafAbsC[1][0] = fabs(aafC[1][0]);
	aafAbsC[1][1] = fabs(aafC[1][1]);
	aafAbsC[1][2] = fabs(aafC[1][2]);
	fR = fabs(afAD[1]);
	fR1 = afEB[0] * aafAbsC[1][0] + afEB[1] * aafAbsC[1][1] + afEB[2] * aafAbsC[1][2];
	fR01 = afEA[1] + fR1;
	if (fR - fR01 > error_range)
		return false;

	aafC[2][0] = obb.w.x;
	aafC[2][1] = obb.w.y;
	aafC[2][2] = obb.w.z;
	afAD[2] = kD.DotProduct(obb.w);
	aafAbsC[2][0] = fabs(aafC[2][0]);
	aafAbsC[2][1] = fabs(aafC[2][1]);
	aafAbsC[2][2] = fabs(aafC[2][2]);
	fR = fabs(afAD[2]);
	fR1 = afEB[0] * aafAbsC[2][0] + afEB[1] * aafAbsC[2][1] + afEB[2] * aafAbsC[2][2];
	fR01 = afEA[2] + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(kD.x);
	fR0 = afEA[0] * aafAbsC[0][0] + afEA[1] * aafAbsC[1][0] + afEA[2] * aafAbsC[2][0];
	fR01 = fR0 + afEB[0];
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(kD.y);
	fR0 = afEA[0] * aafAbsC[0][1] + afEA[1] * aafAbsC[1][1] + afEA[2] * aafAbsC[2][1];
	fR01 = fR0 + afEB[1];
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(kD.z);
	fR0 = afEA[0] * aafAbsC[0][2] + afEA[1] * aafAbsC[1][2] + afEA[2] * aafAbsC[2][2];
	fR01 = fR0 + afEB[2];
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[2] * aafC[1][0] - afAD[1] * aafC[2][0]);
	fR0 = afEA[1] * aafAbsC[2][0] + afEA[2] * aafAbsC[1][0];
	fR1 = afEB[1] * aafAbsC[0][2] + afEB[2] * aafAbsC[0][1];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[2] * aafC[1][1] - afAD[1] * aafC[2][1]);
	fR0 = afEA[1] * aafAbsC[2][1] + afEA[2] * aafAbsC[1][1];
	fR1 = afEB[0] * aafAbsC[0][2] + afEB[2] * aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[2] * aafC[1][2] - afAD[1] * aafC[2][2]);
	fR0 = afEA[1] * aafAbsC[2][2] + afEA[2] * aafAbsC[1][2];
	fR1 = afEB[0] * aafAbsC[0][1] + afEB[1] * aafAbsC[0][0];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[0] * aafC[2][0] - afAD[2] * aafC[0][0]);
	fR0 = afEA[0] * aafAbsC[2][0] + afEA[2] * aafAbsC[0][0];
	fR1 = afEB[1] * aafAbsC[1][2] + afEB[2] * aafAbsC[1][1];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[0] * aafC[2][1] - afAD[2] * aafC[0][1]);
	fR0 = afEA[0] * aafAbsC[2][1] + afEA[2] * aafAbsC[0][1];
	fR1 = afEB[0] * aafAbsC[1][2] + afEB[2] * aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[0] * aafC[2][2] - afAD[2] * aafC[0][2]);
	fR0 = afEA[0] * aafAbsC[2][2] + afEA[2] * aafAbsC[0][2];
	fR1 = afEB[0] * aafAbsC[1][1] + afEB[1] * aafAbsC[1][0];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[1] * aafC[0][0] - afAD[0] * aafC[1][0]);
	fR0 = afEA[0] * aafAbsC[1][0] + afEA[1] * aafAbsC[0][0];
	fR1 = afEB[1] * aafAbsC[2][2] + afEB[2] * aafAbsC[2][1];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[1] * aafC[0][1] - afAD[0] * aafC[1][1]);
	fR0 = afEA[0] * aafAbsC[1][1] + afEA[1] * aafAbsC[0][1];
	fR1 = afEB[0] * aafAbsC[2][2] + afEB[2] * aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	fR = fabs(afAD[1] * aafC[0][2] - afAD[0] * aafC[1][2]);
	fR0 = afEA[0] * aafAbsC[1][2] + afEA[1] * aafAbsC[0][2];
	fR1 = afEB[0] * aafAbsC[2][1] + afEB[1] * aafAbsC[2][0];
	fR01 = fR0 + fR1;
	if (fR - fR01 > error_range)
		return false;

	return true;
}

void SceneGraphNode::InsertObjectAfter(C3dActor *actor, int level)
{
	if (level == 5)
	{
		m_actorList.push_back(actor);
		SceneGraphNode *p;
		for (p = this; p; p=p->m_parent)
		{
			p->m_needUpdate = true;
		}
		return;
	}

	for (int i=0; i<4; ++i)
	{
		if (TestAABBvsOBBIntersection(actor->m_oBoundingBox, m_child[i]->m_aabb))
		{
			m_child[i]->InsertObject(actor, level+1);
		}
	}
}

void	SceneGraphNode :: InsertObject(C3dGround *ground, int level)
{
	m_needUpdate = true;
	m_ground = ground;
	m_ground->ConvertToCellCoor(m_aabb.min.x, m_aabb.min.z, m_groundArea.left, m_groundArea.top);
	m_ground->ConvertToCellCoor(m_aabb.max.x, m_aabb.max.z, m_groundArea.right, m_groundArea.bottom);
	if (level == 5)
		return;
	for (int i=0; i<4; ++i)
	{
		m_child[i]->InsertObject(ground, level+1);
	}
}

void	SceneGraphNode :: InsertObject(C3dAttr *attr, int level)
{
	m_needUpdate = true;
	m_attr = attr;
	m_attr->ConvertToCellCoor(m_aabb.min.x, m_aabb.min.z, m_attrArea.left, m_attrArea.top);
	m_attr->ConvertToCellCoor(m_aabb.max.x, m_aabb.max.z, m_attrArea.right, m_attrArea.bottom);
	if (level == 5)
		return;
	for (int i=0; i<4; ++i)
	{
		m_child[i]->InsertObject(attr, level+1);
	}
}

void	SceneGraphNode :: RemoveObject(C3dActor *actor, int level)
{
	if (level == 5)
	{
		bool	nodeChanged = false;
		std::vector<C3dActor *>::iterator i = m_actorList.begin();
		while (i != m_actorList.end())
		{
			if (*i == actor)
			{
				i = m_actorList.erase(i);
				nodeChanged = true;
			}
			else
			{
				++i;
			}
		}
		if (nodeChanged)
		{
			SceneGraphNode *p;
			for (p = this; p; p = p->m_parent)
				p->m_needUpdate = true;
		}
		return;
	}

	for (int i = 0; i < 4; ++i)
	{
		if (m_child[i])
			m_child[i]->RemoveObject(actor, level + 1);
	}
}

void SceneGraphNode::UpdateVolume(int level)
{
	m_needUpdate = false;
	if (level == 5) {
		float gndTop, gndBottom;
		float attrTop, attrBottom;

		m_ground->GetHeightMinMax(m_groundArea, gndTop, gndBottom);
		m_attr->GetHeightMinMax(m_attrArea, attrTop, attrBottom);
		m_aabb.max.y = gndBottom > attrBottom ? gndBottom : attrBottom;
		m_aabb.min.y = gndTop < attrTop ? gndTop : attrTop;

		BgObjList::iterator i = m_actorList.begin();
		while (i != m_actorList.end()) {
			if ((*i) == NULL)
				continue;
			if (m_aabb.min.y > (*i)->m_aaBoundingBox.min.y)
				m_aabb.min.y = (*i)->m_aaBoundingBox.min.y;
			if (m_aabb.max.y < (*i)->m_aaBoundingBox.max.y)
				m_aabb.max.y = (*i)->m_aaBoundingBox.max.y;
			++i;
		}

		SceneGraphNode* p;
		for (p = this; p; p = p->m_parent) {
			if (p->m_aabb.min.y > m_aabb.min.y)
				p->m_aabb.min.y = m_aabb.min.y;
			if (p->m_aabb.max.y < m_aabb.max.y)
				p->m_aabb.max.y = m_aabb.max.y;
			p->m_center.y = (p->m_aabb.min.y + p->m_aabb.max.y) / 2;
			p->m_halfSize.Set(p->m_aabb.max.x - p->m_center.x,
							  p->m_aabb.max.y - p->m_center.y,
							  p->m_aabb.max.z - p->m_center.z);
		}
		return;
	}

	for (int i = 0; i < 4; ++i) {
		if (m_child[i]->m_needUpdate)
			m_child[i]->UpdateVolume(level + 1);
	}
}

void SceneGraphNode::UpdateVolumeAfter(int level)
{
	m_needUpdate = false;
	if (level == 5) {
		float gndTop, gndBottom;
		float attrTop, attrBottom;

		m_ground->GetHeightMinMax(m_groundArea, gndTop, gndBottom);
		m_attr->GetHeightMinMax(m_attrArea, attrTop, attrBottom);
		m_aabb.max.y = gndBottom > attrBottom ? gndBottom : attrBottom;
		m_aabb.min.y = gndTop < attrTop ? gndTop : attrTop;

		std::vector<C3dActor *>::iterator i = m_actorList.begin();
		while (i != m_actorList.end()) {
			if (m_aabb.min.y > (*i)->m_aaBoundingBox.min.y)
				m_aabb.min.y = (*i)->m_aaBoundingBox.min.y;
			if (m_aabb.max.y < (*i)->m_aaBoundingBox.max.y)
				m_aabb.max.y = (*i)->m_aaBoundingBox.max.y;
			++i;
		}

		SceneGraphNode* p;
		for (p = this; p; p = p->m_parent) {
			if (p->m_aabb.min.y > m_aabb.min.y)
				p->m_aabb.min.y = m_aabb.min.y;
			if (p->m_aabb.max.y < m_aabb.max.y)
				p->m_aabb.max.y = m_aabb.max.y;
			p->m_center.y = (p->m_aabb.min.y + p->m_aabb.max.y) / 2;
			p->m_halfSize.Set(p->m_aabb.max.x - p->m_center.x,
							  p->m_aabb.max.y - p->m_center.y,
							  p->m_aabb.max.z - p->m_center.z);
		}
		return;
	}

	for (int i = 0; i < 4; ++i) {
		if (m_child[i]->m_needUpdate)
			m_child[i]->UpdateVolumeAfter(level + 1);
	}
}

std::vector<C3dActor *>* SceneGraphNode::GetActorList(float x, float z, int level)
{
	if (level == 5)
	{
		return	&m_actorList;
	}
	else
	{
		if (x < m_center.x)
		{
			if (z < m_center.z)
			{
				return	m_child[0]->GetActorList(x, z, level+1);
			}
			else
			{
				return	m_child[2]->GetActorList(x, z, level+1);
			}
		}
		else
		{
			if (z < m_center.z)
			{
				return	m_child[1]->GetActorList(x, z, level+1);
			}
			else
			{
				return	m_child[3]->GetActorList(x, z, level+1);
			}
		}
	}
}

void SceneGraphNode :: LoadSceneGraph(CFile *fp, int Level)
{
	fp->Read(&m_aabb.max.x, 4);
	fp->Read(&m_aabb.max.y, 4);
	fp->Read(&m_aabb.max.z, 4);
	fp->Read(&m_aabb.min.x, 4);
	fp->Read(&m_aabb.min.y, 4);
	fp->Read(&m_aabb.min.z, 4);
	fp->Read(&m_halfSize.x, 4);
	fp->Read(&m_halfSize.y, 4);
	fp->Read(&m_halfSize.z, 4);
	fp->Read(&m_center.x, 4);
	fp->Read(&m_center.y, 4);
	fp->Read(&m_center.z, 4);
	if (Level == 5)
		return;
	for (int i = 0; i < 4; ++i)
	{
		m_child[i]->LoadSceneGraph(fp, Level + 1);
	}
}

void SceneGraphNode::CopySceneGraph(int Level, SceneGraphNode *graph)
{
	m_needUpdate = false;
	m_aabb.max.x = graph->m_aabb.max.x;
	m_aabb.max.y = graph->m_aabb.max.y;
	m_aabb.max.z = graph->m_aabb.max.z;

	m_aabb.min.x = graph->m_aabb.min.x;
	m_aabb.min.y = graph->m_aabb.min.y;
	m_aabb.min.z = graph->m_aabb.min.z;

	m_halfSize.x = graph->m_halfSize.x;
	m_halfSize.y = graph->m_halfSize.y;
	m_halfSize.z = graph->m_halfSize.z;

	m_center.x = graph->m_center.x;
	m_center.y = graph->m_center.y;
	m_center.z = graph->m_center.z;

	if (Level == 5 || graph == NULL)
		return;

	for (int a = 0; a < 4; a++)
	{
		m_child[a]->CopySceneGraph(Level + 1, graph->m_child[a]);
	}
}

CRayPicker :: CRayPicker()
{
}





void	CRayPicker :: Build(const ray3d &ray, SceneGraphNode *rootNode)
{
	m_ray.dir = ray.dir;
	m_ray.org = ray.org;
	m_ray.limit = infinity;
	CullSceneNode(rootNode, 0);
}

void	CRayPicker :: Build(const lineSegment3d &ray, SceneGraphNode *rootNode)
{
	m_ray.org = ray.org;
	m_ray.dir = ray.dir;
	m_ray.limit = ray.limit;
	CullSceneNode(rootNode, 0);
}

void	CRayPicker :: Free()
{
	m_cubeletList.erase(m_cubeletList.begin(), m_cubeletList.end());
}

void	CRayPicker :: CullSceneNode(SceneGraphNode *node, int level)
{
	if (level == 5)
	{
		m_cubeletList.push_back(node);
		return;
	}
	for (int c=0; c<4; c++)
	{
		int		result = CheckAABBIntersect(node->m_child[c]);

		if (result == 0)
			continue;
		else
			CullSceneNode(node->m_child[c], level+1);
	}
}




int CRayPicker::CheckAABBIntersect(SceneGraphNode *node)
{
	return CheckAABBIntersect(node->m_aabb);
	float	tMin = -infinity;
	float	tMax = infinity;

	vector3d	p = node->m_center-m_ray.org;

	float	e, f;

	e = p.x;
	f = m_ray.dir.x;
	if (fabs(f) > epsilon)
	{
		float	t1, t2;
		float	oof = 1.0f/f;

		t1 = (e+node->m_halfSize.x)*oof;
		t2 = (e-node->m_halfSize.x)*oof;
		if (t1>t2)
		{
			if (t2 > tMin) tMin = t2;
			if (t1 < tMax) tMax = t1;
		}
		else
		{
			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
		}
		if (tMin > tMax)
		{
			return	FALSE;
		}
		if (tMax < 0)
		{
			return FALSE;
		}
	}
	else if (-e-node->m_halfSize.x > 0 || -e+node->m_halfSize.x < 0)
	{
		return FALSE;
	}

	e = p.y;
	f = m_ray.dir.y;
	if (fabs(f) > epsilon)
	{
		float	t1, t2;
		float	oof = 1.0f/f;

		t1 = (e+node->m_halfSize.y)*oof;
		t2 = (e-node->m_halfSize.y)*oof;
		if (t1>t2)
		{
			if (t2 > tMin) tMin = t2;
			if (t1 < tMax) tMax = t1;
		}
		else
		{
			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
		}
		if (tMin > tMax)
		{
			return	FALSE;
		}
		if (tMax < 0)
		{
			return FALSE;
		}
	}
	else if (-e-node->m_halfSize.y > 0 || -e+node->m_halfSize.y < 0)
	{
		return FALSE;
	}

	e = p.z;
	f = m_ray.dir.z;
	if (fabs(f) > epsilon)
	{
		float	t1, t2;
		float	oof = 1.0f/f;

		t1 = (e+node->m_halfSize.z)*oof;
		t2 = (e-node->m_halfSize.z)*oof;
		if (t1>t2)
		{
			if (t2 > tMin) tMin = t2;
			if (t1 < tMax) tMax = t1;
		}
		else
		{
			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
		}
		if (tMin > tMax) return	FALSE;
		if (tMax < 0) return FALSE;
	}
	else if (-e-node->m_halfSize.z > 0 || -e+node->m_halfSize.z < 0) return FALSE;
	if (tMin > 0)
	{
		if (tMin > m_ray.limit)
			return	FALSE;
	}
	else
	{
		if (tMax > m_ray.limit)
			return	FALSE;
	}
	return	TRUE;
}

int CRayPicker::CheckAABBIntersect(const C3dAABB &aabb) const
{
	float	tMin = -infinity;
	float	tMax = infinity;

	vector3d center((aabb.min.x+aabb.max.x)/2, (aabb.min.y+aabb.max.y)/2, (aabb.min.z+aabb.max.z)/2);
	vector3d halfSize(aabb.max.x - center.x, aabb.max.y - center.y, aabb.max.z - center.z);
	vector3d	p = center-m_ray.org;

	float	e, f;

	e = p.x;
	f = m_ray.dir.x;
	if (fabs(f) > epsilon)
	{
		float	t1, t2;
		float	oof = 1.0f/f;

		t1 = (e+halfSize.x)*oof;
		t2 = (e-halfSize.x)*oof;
		if (t1>t2)
		{
			if (t2 > tMin) tMin = t2;
			if (t1 < tMax) tMax = t1;
		}
		else
		{
			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
		}
		if (tMin > tMax)
		{
			return	FALSE;
		}

		if (tMax < 0)
		{
			return FALSE;
		}
	}
	else if (-e-halfSize.x*0.7f > 0 || -e+halfSize.x*0.7f < 0)
	{
		return FALSE;
	}

	e = p.y;
	f = m_ray.dir.y;
	if (fabs(f) > epsilon)
	{
		float	t1, t2;
		float	oof = 1.0f/f;

		t1 = (e+halfSize.y)*oof;
		t2 = (e-halfSize.y)*oof;
		if (t1>t2)
		{
			if (t2 > tMin) tMin = t2;
			if (t1 < tMax) tMax = t1;
		}
		else
		{
			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
		}
		if (tMin > tMax)
		{
			return	FALSE;
		}
		if (tMax < 0)
		{
			return FALSE;
		}
	}
	else if (-e-halfSize.y*0.7f > 0 || -e+halfSize.y*0.7f < 0)
	{
		return FALSE;
	}

	e = p.z;
	f = m_ray.dir.z;
	if (fabs(f) > epsilon)
	{
		float	t1, t2;
		float	oof = 1.0f/f;

		t1 = (e+halfSize.z)*oof;
		t2 = (e-halfSize.z)*oof;
		if (t1>t2)
		{
			if (t2 > tMin) tMin = t2;
			if (t1 < tMax) tMax = t1;
		}
		else
		{
			if (t1 > tMin) tMin = t1;
			if (t2 < tMax) tMax = t2;
		}
		if (tMin > tMax)
		{
			return	FALSE;
		}
		if (tMax < 0)
		{
			return FALSE;
		}
	}
	else if (-e-halfSize.z*0.7f > 0 || -e+halfSize.z*0.7f < 0)
	{
		return FALSE;
	}

	if (tMin > 0)
	{
		if (tMin > m_ray.limit)
			return	FALSE;
	}
	else
	{
		if (tMax > m_ray.limit)
			return	FALSE;
	}
	return	TRUE;
}

CWorld::CWorld()
{
	m_Calculated = NULL;
	m_bgObjThread = 0;
	m_ground = NULL;
	m_player = NULL;
	m_isPKZone = FALSE;
	m_isSiegeMode = FALSE;
	m_isBattleFieldMode = FALSE;
	g_session.m_camp_A = -1;
	g_session.m_camp_B = -1;
	g_session.m_MaxItemIndex = 1;
	m_isEventPVPMode = FALSE;
}

CWorld :: ~CWorld()
{
	if(m_ground)
		delete m_ground;
}

void	CWorld :: OnEnterFrame()
{
	std::string rswName = (char *)m_curMode->SendMsg(MM_QUERYRSWNAME, 0, 0, 0);
	C3dWorldRes *world = (C3dWorldRes *)g_resMgr().Get(rswName.c_str(), false);
	if (world == NULL)
	{
		ErrorMsg("NULL was passed for world");
		return;
	}

	m_Calculated = world->m_CalculatedNode;
	((CGameMode *)m_curMode)->m_areaLeft = world->m_groundLeft;
	((CGameMode *)m_curMode)->m_areaRight = world->m_groundRight;
	((CGameMode *)m_curMode)->m_areaTop = world->m_groundTop;
	((CGameMode *)m_curMode)->m_areaBottom = world->m_groundBottom;

	lightDir = world->m_lightDir;
	diffuseCol = world->m_diffuseCol;
	ambientCol = world->m_ambientCol;
	g_renderer->SetLight(lightDir, diffuseCol, ambientCol);

	if (world->m_attrFile.empty())
	{
		ErrorMsg("No attr file is specified in the rsw");
		return;
	}

	m_attr = (C3dAttr *)g_resMgr().Get(world->m_attrFile.c_str(), false);
	if (m_attr == NULL)
	{
		ErrorMsg("Failed to read attr file");
		return;
	}
	m_attr->m_zoom = 5;

	if (world->m_gndFile.empty())
	{
		ErrorMsg("No gnd file is specified in the rsw");
		return;
	}

	CGndRes *ground = (CGndRes *)g_resMgr().Get(world->m_gndFile.c_str(), false);
	if (ground == NULL)
	{
		ErrorMsg("NULL was passed for ground.");
		return;
	}

	if (ground->m_newVer)
		m_ground = new C3dGround15;
	else
	{
		ErrorMsg("Map is an old version");
		return;
	}

	m_ground->AssignGnd(ground, lightDir, diffuseCol, ambientCol);
	m_ground->m_attr = m_attr;
	g_resMgr().Unload(ground);

	int num3dActor = world->m_3dActors.size();
	int curCount = 0;
	int percent = -1;
	std::list<C3dWorldRes::actorInfo *>::iterator i = world->m_3dActors.begin();

	m_bgObjList.clear();
	m_bgObjList.reserve(num3dActor);

	while (i != world->m_3dActors.end())
	{
		curCount++;
		if (percent != curCount * 100 / num3dActor)
		{
			percent = curCount * 100 / num3dActor;
			g_windowMgr.SendMsg(UIM_LOADINGPERCENT, percent, 0, 0, 0);
			g_windowMgr.RenderWallPaper();
			g_windowMgr.Render();
			if (g_renderer->DrawScene())
				g_renderer->Flip(false);
			CheckSystemMessage();
		}

		C3dWorldRes::actorInfo *ai = *i;
		g_modeMgr.GetGameMode()->GetClientCoor(g_session.m_posX, g_session.m_posY, player_pos.x, player_pos.z);
		float distance = (ai->pos - player_pos).Magnitude();
		if (distance > 200.0f)
		{
			g_3dActors.push_back(std::pair<float, C3dWorldRes::actorInfo *>(distance, ai));
			++i;
			continue;
		}

		C3dModelRes *newRes = (C3dModelRes *)g_resMgr().Get(ai->modelName, false);
		if (!newRes) {
			Trace("%s model not found.", ai->modelName);
			++i;
			continue;
		}

		C3dNodeRes *newNode = newRes->FindNode(ai->nodeName);

		if (!newNode) newNode = newRes->GetFirstNode();

		if (!newNode) {
			Trace("%s model has no node.", ai->modelName);
			++i;
			continue;
		}

		C3dActor *newObj = new C3dActor;
		newObj->SetAnimSpeed(ai->animSpeed);
		newObj->SetVolumeBoxInfo(&newRes->m_volumeBoxList);
		newObj->SetPos(ai->pos);
		newObj->SetRot(ai->rot);
		newObj->SetScale(ai->scale);
		newObj->AssignModel(newRes);
		newObj->SetBlockType(ai->blockType);
		newObj->SetIsHideCheck(FALSE);
		newObj->SetName(ai->name);
		newObj->UpdateVertexColor(lightDir, diffuseCol, ambientCol);
		m_bgObjList.push_back(newObj);

		++i;
	}

	BuildSceneGraph();
	MakeFixedActor();
	m_ground->CalcLmIntensityFloatRGBAll();
	m_ground->SetWaterInfo(
		world->m_waterLevel,
		world->m_waterType,
		world->m_waterAnimSpeed,
		(int)world->m_wavePitch,
		(int)world->m_waveSpeed,
		world->m_waveHeight);

	m_bgObjCount = m_bgObjList.size();
	DWORD threadId;
	g_farModelThread = CreateThread(NULL, 0, LoadFarModels, this, 0, &threadId);
	if (!g_farModelThread)
	{
		Trace("Error Creating Thread.");
		LoadFarModels(this);
		WaitForThreadEnd();
	}

	InitPlayer();
	g_session.m_isShowGameOver = FALSE;
	Trace("Load Model :: Early Model Load Complete");
}


void	CWorld :: OnExitFrame()
{
	WaitForThreadEnd();
	FreeSceneGraph();
	RemoveGameActorAll();
	std::vector<C3dActor *>::iterator j = m_bgObjList.begin();
	while (j != m_bgObjList.end())
	{
		delete  *j;
		j++;
	}
	m_bgObjList.erase(m_bgObjList.begin(), m_bgObjList.end());

	delete m_ground;
	m_ground = NULL;

	g_resMgr().UnloadRarelyUsedRes();
	g_texMgr.UnloadRarelyUsedTexture();
}

void	CWorld :: MakeFixedActor()
{
	std::string rswName = (char *)m_curMode->SendMsg(MM_QUERYRSWNAME, 0, 0, 0);
	C3dWorldRes *	world = (C3dWorldRes *)g_resMgr().Get(rswName.c_str(), false);

	std::list<C3dWorldRes::effectSrcInfo *>::iterator i4 = world->m_particles.begin();
	while (i4 != world->m_particles.end()) {
		C3dWorldRes::effectSrcInfo *	ai = *i4;
		CEffectMaker * nEffect = new CEffectMaker;
		nEffect->SendMsg(NULL, AM_SETPOS, int(ai), 0, 0);
		m_gameObjectList.push_back(nEffect);
		i4++;
	}

	if (!g_soundMode)
		return;
	std::list<C3dWorldRes::soundSrcInfo *>::iterator i5 = world->m_sounds.begin();
	while (i5 != world->m_sounds.end()) {
		C3dWorldRes::soundSrcInfo *	ai = *i5;
		CSoundMaker * nSound = new CSoundMaker;
		nSound->SendMsg(NULL, AM_SETPOS, int(ai), 0, 0);
		m_gameObjectList.push_back(nSound);
		i5++;
	}
}

int		CWorld :: CheckAttr(vector3d pos)
{
	if (m_attr->GetAttr(pos.x, pos.z) == 1)
		return	3;
	if (m_attr->GetAttr(pos.x, pos.z) == 2)
		return	4;
	if (m_attr->GetAttr(pos.x, pos.z) == 3)
		return	5;
	if (m_attr->GetAttr(pos.x, pos.z) == 4)
		return	6;
	if (m_attr->GetAttr(pos.x, pos.z) == 5)
		return	7;
	if (m_attr->GetHeight(pos.x, pos.z) < pos.y)
		return	1;
	std::vector<C3dActor *> *actorList = m_rootNode.GetActorList(pos.x, pos.z, 0);
	if (!actorList)
		return	0;
	std::vector<C3dActor *>::iterator	i = actorList->begin();
	while (i != actorList->end()) {
		C3dActor *	a = *i;
		if (a->m_volumeBoxList.size() && a->m_blockType == 0)
			if (a->CheckCollision(pos))
				return	2;
		++i;
	}
	return	0;
}

int		CWorld :: CheckGround(vector3d pos)
{
	if (m_attr->GetHeight(pos.x, pos.z) < pos.y)
		return	1;
	std::vector<C3dActor *> *actorList = m_rootNode.GetActorList(pos.x, pos.z, 0);
	if (!actorList)
		return	0;
	std::vector<C3dActor *>::iterator	i = actorList->begin();
	while (i != actorList->end()) {
		C3dActor *	a = *i;
		if (a->m_volumeBoxList.size() && a->m_blockType == 0)
			if (a->CheckCollision(pos))
				return	2;
		++i;
	}
	return	0;
}

void	CWorld :: AdjustPos(vector3d &pos)
{
	std::vector<C3dActor *> *actorList = m_rootNode.GetActorList(pos.x, pos.z, 0);
	if (!actorList)
		return;
	std::vector<C3dActor *>::iterator	i = actorList->begin();
	while (i != actorList->end()) {
		C3dActor *	a = *i;
		if (a->m_volumeBoxList.size() && a->m_blockType == 0) {
			if (a->CheckCollision(pos)) {
				Trace("AdjustPos::trying adjust\n");
				a->AdjustCollision(pos);
				return;
			}
		}
		++i;
	}
}

void CWorld::AdjustPos2(vector3d &pos)
{
	float cellSize = m_attr->m_zoom;

	float dx = fmod(pos.x + 10000, cellSize);
	float dz = fmod(pos.z + 10000, cellSize);

	if (dx < cellSize / 2)
		dx = -dx;
	else
		dx = cellSize - dx;

	if (dz < cellSize / 2)
		dz = -dz;
	else
		dz = cellSize - dz;

	vector3d org = pos;
	if (fabs(dx) > fabs(dz)) {
		dx = 0;
		if (dz < 0)
			dz -= 0.5f;
		else
			dz += 0.5f;
	} else {
		dz = 0;
		if (dx < 0)
			dx -= 0.5f;
		else
			dx += 0.5f;
	}

	pos.x += dx;
	pos.z += dz;
	if (m_attr->GetAttr(pos.x, pos.z) == 1) {
		pos = org;
		dx = fmod(pos.x + 10000, cellSize);
		dz = fmod(pos.z + 10000, cellSize);

		if (dx < cellSize / 2)
			dx = -dx;
		else
			dx = cellSize - dx;

		if (dz < cellSize / 2)
			dz = -dz;
		else
			dz = cellSize - dz;

		if (fabs(dx) < fabs(dz)) {
			dx = 0;
			if (dz < 0)
				dz -= 0.5f;
			else
				dz += 0.5f;
		} else {
			dz = 0;
			if (dx < 0)
				dx -= 0.5f;
			else
				dx += 0.5f;
		}

		pos.x += dx;
		pos.z += dz;
	}
}

void	CWorld :: GetServerCoor(float cx, float cz, int &sx, int &sy)
{
	sx = int(cx)/m_attr->m_zoom+m_attr->m_width/2;
	sy = int(cz)/m_attr->m_zoom+m_attr->m_height/2;
}

void	CWorld :: GetClientCoor(int sx, int sy, float &cx, float &cz)
{
	cx = (float)((sx-m_attr->m_width/2)*m_attr->m_zoom);
	cz = (float)((sy-m_attr->m_height/2)*m_attr->m_zoom);
}

void	CWorld :: GetClientCoor(float sx, float sy, float &cx, float &cz)
{
	cx = (sx-m_attr->m_width*0.5f)*m_attr->m_zoom;
	cz = (sy-m_attr->m_height*0.5f)*m_attr->m_zoom;
}

void	CWorld :: ProcessActors()
{
	if (m_bgObjThread)
	{
		int targetBgObjCount = m_bgObjCount + m_bgObjThread;
		for ( ; m_bgObjCount < targetBgObjCount; ++m_bgObjCount )
		{
			m_rootNode.InsertObject(m_bgObjList[m_bgObjCount], 0);
			InterlockedDecrement(&m_bgObjThread);
		}
	}

	std::list<CGameObject *>::iterator i = m_gameObjectList.begin();
	while (i != m_gameObjectList.end())
	{
		bool isSurvive = (*i)->OnProcess();

		if (!isSurvive)
		{
			RemoveGameActor(*i);
			delete (*i);
			i = m_gameObjectList.erase(i);
		}
		else
		{
			++i;
		}
	}
}

bool	CWorld :: RemoveGameActor(CGameObject *gameActor)
{
	std::list<CGameActor *>::iterator it = std::find(m_actorList.begin(), m_actorList.end(), (CGameActor *)gameActor);
	if (it != m_actorList.end()) {
		m_actorList.erase(it);
		return true;
	}
	return false;
}

bool	CWorld :: RemoveGameActor(DWORD GID)
{
	std::list<CGameActor *>::iterator i = m_actorList.begin();
	while (i != m_actorList.end()) {
		if ((*i)->GetGID() == GID) {
			RemoveGameObject(*i);
			m_actorList.erase(i);
			return true;
		} else
			++i;
	}
	return false;
}

bool	CWorld :: RemoveItem(DWORD ITAID)
{
	std::list<CItem *>::iterator i = m_itemList.begin();
	for(; i != m_itemList.end(); ++i) {
		if ((*i)->GetAID() == ITAID) {
			RemoveGameObject(*i);
			m_itemList.erase(i);
			return true;
		}
	}
	return false;
}

void	CWorld :: RemoveGameObject(CGameObject *object)
{
	std::list<CGameObject *>::iterator it = std::find(m_gameObjectList.begin(), m_gameObjectList.end(), object);
	if (it != m_gameObjectList.end()) {
		delete  object;
		m_gameObjectList.erase(it);
	}
}


void	CWorld :: RemoveGameActorAll()
{
	std::list<CGameObject *>::iterator i = m_gameObjectList.begin();

	while (i != m_gameObjectList.end()) {
		delete *i;
		++i;
	}
	m_player = NULL;

	m_gameObjectList.clear();
	m_actorList.clear();
	m_itemList.clear();
	m_skillList.clear();
}

CGameActor * CWorld :: GetGameActorByAID(DWORD GID)
{
	std::list<CGameActor *>::iterator i = m_actorList.begin();
	while (i != m_actorList.end()) {
		if ((*i)->GetGID() == GID)
			return (*i);
		++i;
	}
	return NULL;
}

CItem * CWorld :: GetItemByAID(DWORD AID)
{
	std::list<CItem *>::iterator i = m_itemList.begin();
	for(; i != m_itemList.end(); ++i) {
		if ((*i)->GetAID() == AID) return (*i);
	}
	return NULL;
}

bool	CWorld :: RemoveSkill(DWORD AID)
{
	std::list<CSkill *>::iterator i = m_skillList.begin();
	while (i != m_skillList.end()) {
		if ((*i)->GetAID() == AID) {
			RemoveGameObject(*i);
			m_skillList.erase(i);
			return true;
		} else
			++i;
	}
	return false;
}

CSkill * CWorld :: GetSkillByAID(DWORD AID)
{
	std::list<CSkill *>::iterator i = m_skillList.begin();
	while (i != m_skillList.end()) {
		if ((*i)->GetAID() == AID)
			return (*i);
		++i;
	}
	return NULL;
}

CSkill * CWorld::MakeSkill(int val_JT, int val_X, int val_Y)
{
	unsigned long AID = 199;
	std::list<CSkill *>::iterator i = m_skillList.begin();
	while (i != m_skillList.end()) {
		if ((*i)->GetAID() > AID)
			AID = (*i)->GetAID();
		i++;
	}

	SKILLSTANDENTRYSTRUCT ss;
	ss.job = val_JT;
	ss.AID = AID + 1;
	ss.xPos = val_X;
	ss.yPos = val_Y;
	ss.isVisible = 1;
	ss.creatorAID = 1;

	CSkill* skill = new CSkill;
	m_gameObjectList.push_back(skill);
	m_skillList.push_back(skill);
	skill->SendMsg(NULL, AM_SKILLSTANDENTRY, (int)&ss, 0, 0);
	return skill;
}

bool CWorld::RemoveSkillbyJT(int val_JT)
{
	std::list<CSkill *>::iterator i = m_skillList.begin();
	while (i != m_skillList.end()) {
		if ((*i)->GetJob() == val_JT) {
			RemoveGameObject(*i);
			m_skillList.erase(i);
			return true;
		} else
			++i;
	}
	return false;
}

float	CWorld :: CalcShadowHeight(vector3d &pos)
{
	float	groundHeight = m_attr->GetHeight(pos.x, pos.z);

	if (pos.y == groundHeight)
		return	groundHeight;
	std::vector<C3dActor *> *actorList = m_rootNode.GetActorList(pos.x, pos.z, 0);
	if (!actorList)
		return	groundHeight;
	std::vector<C3dActor *>::iterator	i = actorList->begin();
	while (i != actorList->end()) {
		C3dActor *	a = *i;
		if (a->m_volumeBoxList.size() && a->m_blockType == 0)
			a->CalcShadowHeight(pos, groundHeight);
		++i;
	}
	return	groundHeight;
}

void CWorld::InitPlayer()
{
	m_player = new CPlayer;
	m_gameObjectList.push_back(m_player);
}

void	CWorld :: BuildSceneGraph()
{
	FreeSceneGraph();

	m_rootNode.m_aabb.min.x = -(m_ground->m_width * m_ground->m_zoom)/2;
	m_rootNode.m_aabb.max.x = (m_ground->m_width * m_ground->m_zoom)/2-1;
	m_rootNode.m_aabb.min.z = -(m_ground->m_height * m_ground->m_zoom)/2;
	m_rootNode.m_aabb.max.z = (m_ground->m_height * m_ground->m_zoom)/2-1;
	m_rootNode.m_center.x = 0;
	m_rootNode.m_center.z = 0;
	m_rootNode.Build(0);

	std::vector<C3dActor *>::iterator	i = m_bgObjList.begin();

	for(;i != m_bgObjList.end(); ++i)
	{
		m_rootNode.InsertObject(*i, 0);
	}

	m_rootNode.InsertObject(m_ground, 0);
	m_rootNode.InsertObject(m_attr, 0);
	if (m_Calculated)
		m_rootNode.CopySceneGraph(0, m_Calculated);
	else
		m_rootNode.UpdateVolume(0);
}


void	CWorld :: FreeSceneGraph()
{
	for (int i=0; i<4; ++i) {
		if (m_rootNode.m_child[i]) {
			delete	m_rootNode.m_child[i];
			m_rootNode.m_child[i] = NULL;
		}
	}
}

void CWorld :: Add3DActorToWorld(C3dActor *actor)
{
	m_bgObjList.push_back(actor);
	InterlockedIncrement(&m_bgObjThread);
}

void	CWorld ::WaitForThreadEnd()
{
	if (g_farModelThread)
		WaitForSingleObject(g_farModelThread, INFINITE);

	g_farModelThread = NULL;

	g_3dActors.clear();

	m_bgObjThread = 0;
}
