#include "common.h"







#include "../rag_entropy_pad.h"
RAG_ENTROPY_FILL_256(action_0)
RAG_ENTROPY_FILL_256(action_1)
RAG_ENTROPY_FILL_256(action_2)
RAG_ENTROPY_FILL_256(action_3)
RAG_ENTROPY_FILL_256(action_4)
RAG_ENTROPY_FILL_256(action_5)
RAG_ENTROPY_FILL_256(action_6)
RAG_ENTROPY_FILL_256(action_7)
RAG_ENTROPY_FILL_256(action_8)
RAG_ENTROPY_FILL_256(action_9)
RAG_ENTROPY_FILL_256(action_a)
RAG_ENTROPY_FILL_256(action_b)
RAG_ENTROPY_FILL_256(action_c)
RAG_ENTROPY_FILL_256(action_d)
RAG_ENTROPY_FILL_256(action_e)
RAG_ENTROPY_FILL_256(action_f)
#include "Action.h"
#include "Sprite.h"
#include "../base/File.h"
#include "../base/Util.h"
#include "../3dlib/Renderer.h"
#include "../3dlib/DrawUtil.h"

struct CSprClip_v11 {
	int x, y;
	int sprIndex;
	int flags;
};

using std::string;

void	CMotion :: Create(unsigned int clip_count)
{
	m_eventId = -1;
	numClips = clip_count;
	if (clip_count > 1000)
		clip_count = 1000;
	sprClips.resize(clip_count);
	attachInfo.clear();
	attachCnt = 0;
}




CSprClip*	CMotion :: GetClip(unsigned int clip_number)
{
	static CSprClip dummy_clip = { 0, 0, 0, 0, 255, 255, 255, 255, 1.0f, 1.0f, 0, 0 };

	if (clip_number < sprClips.size())
		return &sprClips[clip_number];

	return &dummy_clip;
}

void	CAction :: Create(unsigned int motion_count)
{
	motions.resize(motion_count);
}


CMotion*	CAction :: GetMotion(unsigned int motion_number)
{
	if(motion_number < motions.size())
		return &motions[motion_number];
	else
	{
		static CMotion dummy_motion;

		return &dummy_motion;
	}
}

static CActRes CActResInstance(Exemplar(), "act", "sprite\\");


CActRes :: CActRes()
	: CRes()
{
	numMaxClipPerMotion = 0;
}

void	CActRes :: Create(unsigned int action_count)
{
	Reset();
	actions.resize(action_count);
	m_delay.resize(action_count, 4.0f);
}

void    CActRes :: Reset()
{
	actions.clear();
	m_events.clear();
}

void    CActRes :: ReCalcClipXY(CSprClip *	clip, int cx, int cy, int isHalfW, int isHalfH)
{
	if (clip == NULL)
	{
		Reset();
		ErrorMsg("Error While Load Act");
		return;
	}
	if (isHalfW)
		cx *= 2;
	if (isHalfH)
		cy *= 2;

	clip->x = (clip->x - (cx / 2 + cx % 2) * clip->zoomx) / clip->zoomx;
	clip->y = (clip->y - (cy / 2 + cy % 2) * clip->zoomy) / clip->zoomy;

}

const char *	CActRes :: GetEventName(int eventId)
{
	if(eventId < m_events.size())
		return m_events[eventId].c_str();
	else
		return "";
}

bool	CActRes :: Load(const char *fName)
{
	struct {
		WORD	id, ver;
		WORD	count;
		char	reserved[10];
	} FileHeader;
	int		motionCount, spriteCount;
	RECT	trange1, trange2;


	struct CSprOldClip {
		int x, y;
		int sprIndex;
		int flags;
		BYTE r, g, b, a;
		float zoom;
		int angle;
		int clipType;
	};


	CFile	fp;
	if (!fp.Open(fName, CFile::modeRead))
	{
		ErrorMsg((string("Cannot find File : ") + string(fName)).c_str());
		return	false;
	}


	fp.Read(&FileHeader, sizeof(FileHeader));
	if (FileHeader.id != (int('C')*256+'A'))
	{
		ErrorMsg("Illegal file format");
		ErrorMsg(fName);
		fp.Close();
		return	false;
	}

	string sprName = fName;
	sprName.resize(sprName.size()-4);
	sprName += ".spr";

	if (FileHeader.ver > 0x205)
	{

		ErrorMsg("Unsupported Version. Content may be corrupted");
		fp.Close();
		return false;
	}





	Create(FileHeader.count);
	int i ;
	for (i=0; i<actions.size(); i++)
	{

		CAction  &	naction = actions[i];

		fp.Read((void *)&motionCount, sizeof(int));
		naction.Create(motionCount);

		for (int j = 0; j < motionCount; j++)
		{
			fp.Read((void *)&trange1, sizeof(RECT));
			fp.Read((void *)&trange2, sizeof(RECT));
			fp.Read((void *)&spriteCount, sizeof(int));

			CMotion&	nmotion = naction.motions[j];
			nmotion.Create(spriteCount);

			nmotion.range1 = trange1;
			nmotion.range2 = trange2;

			if (numMaxClipPerMotion < spriteCount)
			{
				numMaxClipPerMotion = spriteCount;
			}

			for (int k=0; k<spriteCount; k++)
			{

				CSprClip &	nsprite = nmotion.sprClips[k];

				if (FileHeader.ver < 0x0200)
				{

					fp.Read(&nsprite, sizeof(CSprClip_v11));
					nsprite.a = 0xff;
					nsprite.r = 0xff;
					nsprite.g = 0xff;
					nsprite.b = 0xff;
					nsprite.zoomx = 1.0f;
					nsprite.zoomy = 1.0f;
					nsprite.angle = 0;
					nsprite.clipType = 0;

				}
				else if(FileHeader.ver < 0x0204)
				{

					CSprOldClip	nOldSprite;

					fp.Read(&nOldSprite, sizeof(CSprOldClip));
					nsprite.a = nOldSprite.a;
					nsprite.r = nOldSprite.r;
					nsprite.g = nOldSprite.g;
					nsprite.b = nOldSprite.b;
					nsprite.x = nOldSprite.x;
					nsprite.y = nOldSprite.y;
					nsprite.angle = nOldSprite.angle;
					nsprite.flags = nOldSprite.flags;

					nsprite.clipType = nOldSprite.clipType;
					nsprite.sprIndex = nOldSprite.sprIndex;
					nsprite.zoomx = nOldSprite.zoom;
					nsprite.zoomy = nOldSprite.zoom;

					if (nsprite.sprIndex != -1)
					{
						SprImg *	img = ((CSprRes*)g_resMgr().Get(sprName.c_str(), 0))->m_sprites[nsprite.clipType][nsprite.sprIndex];
						ReCalcClipXY(&nsprite, img->width, img->height, img->isHalfW, img->isHalfH);
					}
				}
				else
				{
					fp.Read(&nsprite, sizeof(CSprClip));

					if(nsprite.clipType < 0)
					{
						Trace("NO~~~!!");
					}

					if (FileHeader.ver >= 0x0205)
					{
						int wi, he;
						fp.Read(&wi, sizeof(int));
						fp.Read(&he, sizeof(int));
						if (nsprite.sprIndex != -1)
						{
							ReCalcClipXY(&nsprite, wi, he, 0, 0);
						}
					}
					else
					{
						if (nsprite.sprIndex != -1)
						{
							SprImg *	img = ((CSprRes*)g_resMgr().Get(sprName.c_str(), 0))->m_sprites[nsprite.clipType][nsprite.sprIndex];
							if (img != NULL)
								ReCalcClipXY(&nsprite, img->width, img->height, img->isHalfW, img->isHalfH);
						}
					}

				}
			}

			if (FileHeader.ver >= 0x0200)
			{
				fp.Read(&nmotion.m_eventId, sizeof(int));
				if (FileHeader.ver == 0x0200)
					nmotion.m_eventId = -1;
			}

			if (FileHeader.ver >= 0x0203)
			{
				int attachCnt;
				fp.Read(&attachCnt, sizeof(int));
				nmotion.attachCnt = attachCnt;
				nmotion.attachInfo.resize(attachCnt);

				CAttachPointInfo nInfo[2];
				for(int a = 0; a < attachCnt; a++)
				{
					fp.Read(&nInfo[0].m_attr, 16);
					memcpy(&nmotion.attachInfo[a], &nInfo[1], sizeof(CAttachPointInfo));
				}
			}
		}
	}

	if (FileHeader.ver >= 0x0201)
	{
		int numEvent;
		fp.Read(&numEvent, sizeof(int));
		char buf[40];
		m_events.resize(numEvent);
		for (int i=0; i<numEvent; i++)
		{
			fp.Read(&buf, 40);
			m_events[i] = buf;
		}
	}

	if(FileHeader.ver >= 0x0202)
	{
		for (i = 0; i < FileHeader.count; i++)
			fp.Read(&m_delay[i], sizeof(float));
	}

	fp.Close();
	return	true;
}

float	CActRes :: GetDelay(unsigned int actIndex)
{
	if(actIndex < m_delay.size())
		return	m_delay[actIndex];
	else
		return 4.0f;
}

CAction *	CActRes :: GetAction(unsigned int action_number)
{
	if(action_number < actions.size())
		return &actions[action_number];
	else
	{
		static CAction dummy_action;
		return &dummy_action;
	}
}

CMotion *	CActRes :: GetMotion(unsigned int actIndex, unsigned int motIndex)
{
	return GetAction(actIndex)->GetMotion(motIndex);
}

int		CActRes :: GetMotionCount(unsigned int actIndex)
{
	return max(GetAction(actIndex)->motions.size(), 1);
}

static CRmaRes CRmaResInstance(Exemplar(), "rma", "model\\");




CRmaRes :: CRmaRes()
{
	m_interiorZoom = 1.0f;
	m_interiorOffsetX = 0;
	m_interiorOffsetY = 0;
	m_interiorStartFrame = 0;
	m_interiorEndFrame = 0;
	m_shadowZoom = 1.0f;
}

CRmaRes :: ~CRmaRes()
{
	Reset();
}

void	CRmaRes :: Reset()
{
	m_actionList.erase(m_actionList.begin(), m_actionList.end());
	m_rsxNameList.erase(m_rsxNameList.begin(), m_rsxNameList.end());
	m_rsxNameIndexList.erase(m_rsxNameIndexList.begin(), m_rsxNameIndexList.end());
}

string & CRmaRes :: GetRsxName(int action)
{
	return m_rsxNameList[m_rsxNameIndexList[action]];
}

int	CRmaRes :: GetMotionSpeed(int action)
{
	return m_actionList[action].motionSpeed;
}

int	CRmaRes :: GetStartFrame(int action)
{
	return m_actionList[action].startFrame;
}

int	CRmaRes :: GetEndFrame(int action)
{
	return m_actionList[action].endFrame;
}

int CRmaRes :: GetFirstAttackFrame(int action)
{
	int size = m_actionList[action].attackFrameList.size();
	for (int i=0; i<size; ++i)
		return m_actionList[action].attackFrameList[i];
	return -1;
}

BOOL CRmaRes :: IsAttackFrame(int action, int frame)
{
	int size = m_actionList[action].attackFrameList.size();
	for (int i=0; i<size; ++i) {
		if (frame == m_actionList[action].attackFrameList[i])
			return true;
	}
	return FALSE;
}

BOOL	CRmaRes :: IsValidAction(int action)
{
	if (action >= m_actionList.size() || action < 0)
		return FALSE;
	if (m_actionList[action].startFrame != -1)
		return true;
	return FALSE;
}

bool	CRmaRes :: Load(const char *fName)
{
	char *	buf;
	long	i;
	CTokenizer	a;
	CFile	fp;

	fp.Open(fName, CFile::modeRead);
	i = fp.GetLength();

	buf = new char[i+1];
	fp.Read(buf, i);
	fp.Close();

	buf[i] = 0;
	std::istrstream st(buf);

	int curRsxNameIndex = -1;
	while (st) {
		a.GetLine(st);
		if (a.NumTokens()) {
			if (a[0] == "") {
			} else if (a[0] == "#rsxname") {
				curRsxNameIndex++;
				m_rsxNameList.resize(curRsxNameIndex+1);
				m_rsxNameList[curRsxNameIndex] = a[1];
			} else if (a[0] == "#action") {
				int actNum = atoi(a[1].c_str());
				if (actNum >= m_actionList.size()) {
					m_actionList.resize(actNum+1);
					m_rsxNameIndexList.resize(actNum+1);
				}

				if (m_actionList[actNum].startFrame != -1)
					ErrorMsg("Duplicate action found");
				m_actionList[actNum].motionSpeed = atoi(a[2].c_str());
				m_actionList[actNum].startFrame = atoi(a[3].c_str());
				m_actionList[actNum].endFrame = atoi(a[4].c_str());
				if (a.NumTokens() >= 6) {
					m_actionList[actNum].attackFrameList.resize(a.NumTokens()-5);
					for (int i=0; i<(a.NumTokens()-5); i++)
						m_actionList[actNum].attackFrameList[i] = atoi(a[5+i].c_str());
				}
				m_rsxNameIndexList[actNum] = curRsxNameIndex;

			} else if (a[0] == "#interiorzoom") {
				m_interiorZoom = atof(a[1].c_str());
			} else if (a[0] == "#interioroffsetx") {
				m_interiorOffsetX = atoi(a[1].c_str());
			} else if (a[0] == "#interioroffsety") {
				m_interiorOffsetY = atoi(a[1].c_str());
			} else if (a[0] == "#interiorframes") {
				m_interiorStartFrame = atoi(a[1].c_str());
				m_interiorEndFrame = atoi(a[2].c_str());
			} else if (a[0] == "#shadowzoom") {
				m_shadowZoom = atof(a[1].c_str());
			} else {
				ErrorMsg("Syntax Error in Rma");
			}
		}
	}
	delete [] buf;
	return true;
}
