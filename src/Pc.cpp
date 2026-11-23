#include "common.h"
#include "GameMode.h"
#include "GameWorld.h"
#include "Session.h"
#include "packet.h"
#include "RagEffect.h"
#include "RagEffectPrim.h"
#include "resource/ImfRes.h"
#include "resource/PaletteRes.h"
#include "device/Control.h"
#include "resource/Wave.h"
#include "View.h"

bool IsEffectStateMarried(DWORD effectState);
bool IsEffectStateSanta(DWORD effectState);
bool IsEffectStateSummer(DWORD effectState);
bool IsEffectStateFootPrint(DWORD effectState);
bool IsEffectStateSpecialHiding(DWORD effectState);
bool IsEffectStateOrcFace(DWORD effectState);

extern CModeMgr g_modeMgr;

extern UIWindowMgr g_windowMgr;
extern CSession g_session;
extern CRenderer* g_renderer;
extern int MAX_ATTACKMT;
extern bool IsNameYellow(DWORD aid);
extern bool IsEventAID(DWORD aid);
extern float FAR_DIST;
extern float g_avgPixelRatio;
extern float g_zHover;
extern float g_zOffset;


const int PC_RENDER_LAYER_SIZE = 8;
const int Layer_Body = 0;
const int Layer_Head = 1;
const int Layer_Accessory1 = 2;
const int Layer_Accessory2 = 3;
const int Layer_Accessory3 = 4;
const int Layer_Shield = 7;
const int BL_LIGHT_BODY = 0x01;
const int BL_LIGHT_SWORD = 0x02;
const int BL_SPARK_SWORD = 0x04;
const int BL_DOUBLE_BODY = 0x08;
const int BL_REFLECT_BODY = 0x200;
const int BL_ASURA = 0x800;
const int BL_4WAY = 0x1000;
const int BL_QUAKE = 0x2000;
const int BL_RUNNING = 0x8000;
const int BL_HIGHJUMP = 0x10000;
const int BL_LANDJUMP = 0x20000;
const int BL_UNDEAD = 0x80000;
const int HEADER_ANIM_FRAME_NUM = 9;
const int JOBMINUS = 3950;
const int Layer_Weapon1 = 5;
const int Layer_Weapon2 = 6;
static unsigned long tmpShadowDotBuf[35 * 35];
#ifndef PATH_HUMAN
#define PATH_HUMAN "\xc0\xce\xb0\xa3\xc1\xb7\\"
#endif



static inline float GetShadowOow(float sprY, float tlvertY, float tvz)
{
	return 1.0f / (-g_shadowSlope*(sprY-tlvertY) + tvz);
}

static inline float GetSprOow(float sprY, float tlvertY, float tvz)
{
	return 1.0f / (g_slope*(sprY-tlvertY) + tvz);
}

inline int	GetClipIndex(int layer)
{
	switch (layer) {
		case	Layer_Head:
			return	1;
	}
	return	0;
}

inline int	GetLayer(CImfRes *imfRes, int p, int baseAction, int curAction, int curMotion)
{
	int		offAction = curAction - baseAction;
	switch (offAction)
	{
		case	2:
		case	3:
		case	4:
		case	5:
			{
				if (p == 7)
					return	7;
				switch (p)
				{
					case	6:
					case	5:
						return	imfRes->GetLayer(p-5, curAction, curMotion);
						break;
				}
				return	6 - p;
			}
			break;
		default:
			{
				switch (p)
				{
					case	7:
					case	6:
						return	imfRes->GetLayer(p-6, curAction, curMotion);
						break;
				}
				return	7 - p;
				break;
			}
	}
}

inline bool	NeedWeaponLayer(int curAction)
{
	if (curAction < 32)
		return	false;
	if (curAction > 39 && curAction < 80)
		return	false;
	if (curAction > 95)
		return	false;

	return	true;
}

CPc :: CPc()
{
	m_sprResList.resize(PC_LAYER_SIZE+4, NULL);
	m_actResList.resize(PC_LAYER_SIZE+4, NULL);
	OnInit();
	m_isPc = TRUE;
}

CPc :: ~CPc()
{
	for(int i=0; i<PC_LAYER_SIZE+4; ++i)
	{
		if(m_sprResList[i]) m_sprResList[i]->Unlock();
		if(m_actResList[i]) m_actResList[i]->Unlock();
	}

	OnExit();
}

void	CPc :: OnInit()
{
	m_gage = NULL;
	m_headPalette = 0;
	m_shield = 0;
	m_honor = 0;
	m_virtue = 0;
	m_gdid = 0;
	m_emblemVersion = 0;
	m_headDir = 0;
	m_shoe = 0;

	m_renderWithoutLayer = FALSE;
	m_pk_total = m_pk_rank = 0;
	m_isPc = TRUE;
}

void	CPc :: OnExit()
{
	if (m_gage) {
		g_windowMgr.PostQuit(m_gage);
		m_gage = NULL;
	}
}

void	CPc :: SetSprJob(int job)
{
	char	tmpName[256];
	if (job == JT_BARD && m_sex == 0)
		m_sex = 1;
	if (job == JT_BARD_H && m_sex == 0)
		m_sex = 1;
	if (job == JT_BARD_B && m_sex == 0)
		m_sex = 1;
	if (job == JT_DANCER && m_sex == 1)
		m_sex = 0;
	if (job == JT_DANCER_H && m_sex == 1)
		m_sex = 0;
	if (job == JT_DANCER_B && m_sex == 1)
		m_sex = 0;

	m_shadowZoom = g_session.GetShadowFactorWithJob(job);

	const char* actName;
	if (IsEffectStateMarried(m_effectState))
	{
		SetActNameList(0, g_session.GetJobActName(JT_MARRIED, m_sex, tmpName));
		SetSprNameList(0, g_session.GetJobSprName(JT_MARRIED, m_sex, tmpName));
		actName = m_actResList[0] ? m_actResList[0]->GetName() : "";
	}
	else if (IsEffectStateSanta(m_effectState))
	{
		SetActNameList(0, g_session.GetJobActName(JT_SANTA, m_sex, tmpName));
		SetSprNameList(0, g_session.GetJobSprName(JT_SANTA, m_sex, tmpName));
		actName = m_actResList[0] ? m_actResList[0]->GetName() : "";
	}
	else if (IsEffectStateSummer(m_effectState))
	{
		SetActNameList(0, g_session.GetJobActName(JT_SUMMER, m_sex, tmpName));
		SetSprNameList(0, g_session.GetJobSprName(JT_SUMMER, m_sex, tmpName));
		actName = m_actResList[0] ? m_actResList[0]->GetName() : "";
	}
	else
	{
		SetActNameList(0, g_session.GetJobActName(job, m_sex, tmpName));
		SetSprNameList(0, g_session.GetJobSprName(job, m_sex, tmpName));
		actName = m_actResList[0] ? m_actResList[0]->GetName() : "";
	}

	SetActName(actName);
	SetSprName(m_sprResList[0] ? m_sprResList[0]->GetName() : "");
}

void	CPc :: SetSprHead(int head)
{
	char	tmpName[256];
	m_head = head;
	SetActNameList(1, g_session.GetHeadActName(GetJob(), m_head, m_sex, tmpName));
	SetSprNameList(1, g_session.GetHeadSprName(GetJob(), m_head, m_sex, tmpName));
}

void  CPc :: SetSprJobDye(int job)
{
	char tmpName[256];
	SetActNameList(9, g_session.GetJobDyeActName(GetJob(), m_sex, tmpName));
	SetSprNameList(9, g_session.GetJobDyeSprName(GetJob(), m_sex, tmpName));
}


void	CPc :: SetSprAccessory(int accessory)
{
	m_accessory = accessory;
	if (accessory == 0x1505)
		m_accessory = 0x183;
	else if (accessory == 0x1435)
		m_accessory = 0xe3;
	else if (accessory == 0x14b4)
		m_accessory = 0x142;
	else if (accessory == 0x145c)
		m_accessory = 0x10a;
	else if (accessory == 0x148f)
		m_accessory = 0x12f;
	else if (accessory == 0x145e)
		m_accessory = 0x10c;
	else if (accessory == 0x1411)
		m_accessory = 0xd0;
	else if (accessory == 0x1408)
		m_accessory = 0x102;
	else if (accessory == 0x145a)
		m_accessory = 0x108;
	else if (accessory == 0x1504)
		m_accessory = 0x182;

	if (m_gid == 1)
		g_session.m_accessory = m_accessory;

	char	tmpName[256];
	SetActNameList(2, g_session.GetAccessoryActName(GetJob(), m_head, m_sex, m_accessory, tmpName));
	SetSprNameList(2, g_session.GetAccessorySprName(GetJob(), m_head, m_sex, m_accessory, tmpName));
}

void	CPc :: SetSprAccessory2(int accessory2)
{
	m_accessory2 = accessory2;
	if (accessory2 == 0x8c2)
		m_accessory2 = 0x1a;
	else if (accessory2 == 0x8e4)
		m_accessory2 = 0x3f;
	else if (accessory2 == 0x1437)
		m_accessory2 = 0xe5;
	else if (accessory2 == 0x140f)
		m_accessory2 = 0xcf;
	else if (accessory2 == 0x1467)
		m_accessory2 = 0xaf;
	else if (accessory2 == 0x14ed)
		m_accessory2 = 0x16d;
	else if (accessory2 == 0x8ee)
		m_accessory2 = 0x49;
	else if (accessory2 == 0x14c3)
		m_accessory2 = 0x151;
	else if (accessory2 == 0x14cd)
		m_accessory2 = 0x159;
	else if (accessory2 == 0x13ee)
		m_accessory2 = 0xb9;

	if (m_gid == 1)
		g_session.m_accessory2 = m_accessory2;

	char	tmpName[256];
	SetActNameList(3, g_session.GetAccessoryActName(GetJob(), m_head, m_sex, m_accessory2, tmpName));
	SetSprNameList(3, g_session.GetAccessorySprName(GetJob(), m_head, m_sex, m_accessory2, tmpName));

	if (IsEffectExist(0x2a6))
	{
		SendMsg(NULL, AM_DELETE_EFFECT, 0x2a6, 0, 0);
		SendMsg(NULL, AM_DELETE_EFFECT, 0x2a7, 0, 0);
		SendMsg(NULL, AM_DELETE_EFFECT, 0x2a8, 0, 0);
	}
}

void	CPc :: SetSprAccessory3(int accessory3)
{
	m_accessory3 = accessory3;
	if (accessory3 == 0x8dd)
		m_accessory3 = 0x38;
	else if (accessory3 == 0x1456)
		m_accessory3 = 0x103;
	else if (accessory3 == 0x8db)
		m_accessory3 = 0x36;
	else if (accessory3 == 0x13f9)
		m_accessory3 = 0xc2;
	else if (accessory3 == 0x8da)
		m_accessory3 = 0x35;
	else if (accessory3 == 0x13f6)
		m_accessory3 = 0xbf;
	else if (accessory3 == 0x13f3)
		m_accessory3 = 0xbc;
	else if (accessory3 == 0x8de)
		m_accessory3 = 0x39;

	if (m_gid == 1)
		g_session.m_accessory3 = m_accessory3;

	char	tmpName[256];
	SetActNameList(4, g_session.GetAccessoryActName(GetJob(), m_head, m_sex, m_accessory3, tmpName));
	SetSprNameList(4, g_session.GetAccessorySprName(GetJob(), m_head, m_sex, m_accessory3, tmpName));
}

void	CPc :: SetSprWeapon(int weapon)
{
	char tmpName[256];
	char bufStr[256];
	m_weapon = weapon;

	sprintf(bufStr, "Weapon no. - %d, Job - %d", weapon, GetJob());
	g_windowMgr.SendMsg(UIM_PUSHINTOCHATHISTORY, int(bufStr), 0xfa969b, 0, 0);

	if (!g_session.IsProperWeapon(GetJob(), m_weapon))
	{
		SetActNameList(5, "");
		SetSprNameList(5, "");
		SetActNameList(6, "");
		SetSprNameList(6, "");
	}
	else
	{
		sprintf(tmpName, "\xc0\xce\xb0\xa3\xc1\xb7\\%s%s%s.act", g_session.m_pcJobNameTable[GetJob()],
			g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[m_weapon]);
		SetActNameList(5, tmpName);
		sprintf(tmpName, "\xc0\xce\xb0\xa3\xc1\xb7\\%s%s%s.spr", g_session.m_pcJobNameTable[GetJob()],
			g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[m_weapon]);
		SetSprNameList(5, tmpName);
		switch (m_weapon)
		{
			case	1:
			case	2:
			case	3:
			case	4:
			case	5:
			case	6:
			case	7:
			case	16:
			case	17:
			case	18:
			case	25:
			case	26:
			case	27:
			case	28:
			case	29:
			case	30:
			{
				sprintf(tmpName, "\xc0\xce\xb0\xa3\xc1\xb7\\%s%s%s_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[GetJob()], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[m_weapon]);
				SetActNameList(6, tmpName);
				sprintf(tmpName, "\xc0\xce\xb0\xa3\xc1\xb7\\%s%s%s_\xb0\xcb\xb1\xa4.spr", g_session.m_pcJobNameTable[GetJob()], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[m_weapon]);
				SetSprNameList(6, tmpName);
				break;
			}
			default:
				SetActNameList(6, "");
				SetSprNameList(6, "");
				break;
		}
	}
}

void	CPc :: SetSprShield(int shield)
{
	char	tmpName[256];
	int job = GetJob();
	if (shield > 2000)
		shield = g_session.GetShieldType(shield);
	m_shield = shield;

	SetActNameList(7, g_session.GetShieldActName(job, m_sex, m_shield, tmpName));
	SetSprNameList(7, g_session.GetShieldSprName(job, m_sex, m_shield, tmpName));
}

void	CPc :: SetImfFileName()
{
	char	tmpName[256];
	m_imfName = g_session.GetImfName(GetJob(), m_head, m_sex, tmpName);
}

void	CPc :: SetHeadPaletteName()
{
	if (m_headPalette) {
		char	palName[64];
		m_headPaletteName = g_session.GetHeadPaletteName(m_head, GetJob(), m_sex, m_headPalette, palName);
	}
}

void	CPc :: SetBodyPaletteName()
{
	if (m_bodyPalette)
	{
		char	palName[64];
		m_bodyPaletteName = g_session.GetBodyPaletteName(GetJob(), m_sex, m_bodyPalette, palName);
	}
}

void	CPc :: SetBodyPalette(int bodyPalette)
{
	m_bodyPalette = bodyPalette;
	SetBodyPaletteName();
}

void	CPc :: SetHeadPalette(int headPalette)
{
	m_headPalette = headPalette;
	SetHeadPaletteName();
}

void	CPc :: SetSprAct(int job, int sex, int head, int weapon, int accessory, int accessory2, int accessory3, int shield, int headPal, int bodyPal)
{
	SetSex(sex);
	m_head = head;
	m_weapon = weapon;
	m_accessory = accessory;
	m_accessory2 = accessory2;
	m_accessory3 = accessory3;
	m_shield = shield;
	m_headPalette = headPal;
	m_bodyPalette = bodyPal;

	SetJob(job);

	if (IsEffectStateMarried(m_effectState))
		SetJob(JT_MARRIED);
	else if (IsEffectStateSanta(m_effectState))
		SetJob(JT_SANTA);
	else if (IsEffectStateSummer(m_effectState))
		SetJob(JT_SUMMER);
	else
		SetJob(job);

	SetSprHead(m_head);
	SetSprWeapon(m_weapon);
	SetSprAccessory(m_accessory);
	SetSprAccessory2(m_accessory2);
	SetSprAccessory3(m_accessory3);
	SetSprShield(m_shield);
	SetImfFileName();
	SetHeadPaletteName();
	SetBodyPaletteName();
}

void	CPc :: SetSprAct2(int job, int sex, int head, int weapon, int accessory, int accessory2, int accessory3, int shield, int headPal, int bodyPal, int effectState)
{
	SetSex(sex);
	m_head = head;
	m_weapon = weapon;
	m_accessory = accessory;
	m_accessory2 = accessory2;
	m_accessory3 = accessory3;

	int right = (weapon >> 16) & 0xffff;
	int left = (weapon & 0xffff);
	if (g_session.GetWeaponType(right) < 0)
		m_shield = right;
	else m_shield = 0;

	if (m_shield > 2000)
		m_shield = g_session.GetShieldType(m_shield);

	m_headPalette = headPal;
	m_bodyPalette = bodyPal;

	if (job > 4000 && job < 4023)
	{
		m_bodyPalette = 1;
		if (job == 4012 || job == 4009)
			m_bodyPalette = 0;
		if (job == 4011 || job == 4013)
			m_bodyPalette = 0;
		if (job == 4008 || job == 4014)
			m_bodyPalette = 0;
		if (job == 4010)
			m_bodyPalette = 0;
		m_bodyPalette = 0;
		if (job <= 4007)
			m_bodyPalette = 1;
	}
	else
	{
		m_bodyPalette = 0;
	}

	if (IsEffectStateMarried(effectState))
		SetJob(JT_MARRIED);
	else if (IsEffectStateSanta(effectState))
		SetJob(JT_SANTA);
	else if (IsEffectStateSummer(effectState))
		SetJob(JT_SUMMER);
	else
		SetJob(job);

	SetSprHead(m_head);
	if (IsEffectStateMarried(effectState) || IsEffectStateSanta(effectState) || IsEffectStateSummer(effectState))
	{
	}
	else
	{
		if (GetJob() == JT_ASSASSIN || GetJob() == JT_ASSASSIN_H || GetJob() == JT_ASSASSIN_B)
		{
			SetSprWeapon2(weapon);
		}
		else
		{
			SetSprWeapon2(left);
		}
	}

	SetSprAccessory(m_accessory);
	SetSprAccessory2(m_accessory2);
	SetSprAccessory3(m_accessory3);
	if (IsEffectStateMarried(effectState) || IsEffectStateSanta(effectState) || IsEffectStateSummer(effectState))
	{
	}
	else
		SetSprShield( m_shield );

	SetImfFileName();
	SetHeadPaletteName();
	SetBodyPaletteName();




}

bool	CPc :: OnProcess()
{
	ProcessEffectState();
	ProcessWillBeAttacked();
	ProcessChatBalloon();
	ProcessGage();
	if (m_isSingleColor) {
		if (timeGetTime() > m_singleColorStartTick + 100) {
			m_colorOfSingleColor = 0xffff0000;
			m_isSingleColor = false;
		}
	}
	if (m_sprArgb != 0xffffffff)
		ModifyArgb(8, 8, 8, 8);
	bool result = ProcessState();
	ProcessSound();
	if (m_isPlayHitWave) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		vector3d& pPos = gameMode->GetWorld()->m_player->m_pos;
		PlayWave(m_hitWaveName, m_pos.x - pPos.x, 0, m_pos.z - pPos.z);
		m_isPlayHitWave = FALSE;
	}
	return result;
}

bool	CPc :: ProcessState()
{
	if (m_freezeEndTick) {
		if (timeGetTime() > m_freezeEndTick) {
			m_isMotionFreezed = false;
			m_freezeEndTick = 0;
		}
	}

	if (!ProcessVanish()) {
		return	false;
	}
	CGameMode *		gameMode = g_modeMgr.GetGameMode();
	sAssert(gameMode);
	switch (m_stateId) {
		case	8: {
			ProcessMotion();
			float stateCnt = (timeGetTime() - m_stateStartTick) / 24.0f;
			if (m_targetGid && int(stateCnt)%34==0) {
				CGameMode *		gameMode = g_modeMgr.GetGameMode();
				sAssert(gameMode);
				CGameActor *	tActor = gameMode->GetWorld()->GetGameActorByAID(m_targetGid);
				if (tActor) {
					CalcDir(tActor->m_pos.x, tActor->m_pos.z);
				}
			}
			break;
		}
		case	7:
		{
			ProcessMotion();
			if (m_isMotionFinished)
			SetState(ST_STAND);
			break;
		}
		case	2:
		{
			ProcessMotion();
			float stateCnt = (timeGetTime() - m_stateStartTick) / 24.0f;
			if (m_isMotionFinished)
				SetState(ST_STAND);
			break;

		}
		case	6:
			ProcessMotion();
			break;
		case	3:
#ifdef		SECONDJOB_SKILL2
			m_isForceState = false;
			m_isForceState2 = false;
			m_isForceState3 = false;
#endif
			m_isMotionFreezed = false;
			ProcessMotion();
			break;
		default:
			return	CGameActor::ProcessState();
			break;
	}
	return	true;
}

void	CPc :: SetState(int stateId)
{
	if (m_stateId == ST_DEAD || m_isTrickDead)
		return;

	if (stateId == -1)
		return;

	if (m_stateId == ST_DAMAGE) {
		m_pos.x = m_damageDestX;
		m_pos.z = m_damageDestZ;
	}

	int prevStateId = m_stateId;
	switch (stateId)
	{
	case ST_BEGIN_SKILL:
		RegisterPos();
		SetAction(96, 6, MT_ONETIME);
		m_isMotionFinished = true;
		break;

	case ST_SIT:
		RegisterPos();
		SetAction(16, 4, MT_ONETIME);
		m_isMotionFinished = true;
		m_curMotion = m_headDir;
		break;

	case ST_MOVE:
		m_isCounter = FALSE;
		if (m_bodyState != BODYSTATE_STONECURSE && m_bodyState != BODYSTATE_FREEZING)
			m_isMotionFreezed = false;
		m_headDir = 0;
		CGameActor::SetState(stateId);
		break;

	case ST_STAND:
		CGameActor::SetState(stateId);

		if (GetJob() == JT_STAR && m_gid == g_session.GetAid())
		{
			if (g_modeMgr.GetGameMode()->GetView()->GetDestDistance() > FAR_DIST)
				g_modeMgr.GetGameMode()->GetView()->SetDestDistance(FAR_DIST);
		}
		m_curMotion = m_headDir;
		if (prevStateId == ST_DAMAGE || prevStateId == ST_ATTACK || m_pkState || prevStateId == ST_BEGIN_SKILL)
			SetAction(32, 4, MT_LOOP);
		g_modeMgr.GetGameMode()->SetWaitUseItemAck(FALSE);
		break;

	case ST_SKILL:
	{
		if (m_stateId == ST_SIT)
			return;

		int skillAction = 96;
		if (GetJob() == JT_BARD || GetJob() == JT_DANCER || GetJob() == JT_CRUSADER)
		{
			SetAction(32, 6, MT_ONETIME);
		}
		else if (GetJob() == JT_BARD_H || GetJob() == JT_DANCER_H || GetJob() == JT_CRUSADER_H)
		{
			SetAction(skillAction, 6, MT_ONETIME);
			SetForceAnimSpeed(4);
			SetForceAnimation(skillAction, 0, 0, skillAction, 0, TRUE, 400, FALSE);
		}
		else if (GetJob() == JT_BARD_B || GetJob() == JT_DANCER_B || GetJob() == JT_CRUSADER_B)
		{
			SetAction(32, 6, MT_ONETIME);
		}
		else if (GetJob() == JT_MONK || GetJob() == JT_MONK_H || GetJob() == JT_MONK_B)
		{
			if (m_sex == 0)
			{
				SetAction(32, 6, MT_ONETIME);
			}
			else
			{
				SetAction(skillAction, 6, MT_ONETIME);
				SetForceAnimSpeed(4);
				SetForceAnimation(skillAction, 0, 0, skillAction, 0, TRUE, 400, FALSE);
			}
		}
		else if (GetJob() >= JT_TAEKWON && GetJob() <= JT_STAR2)
		{
			SetAction(32, 6, MT_ONETIME);
		}
		else if (GetJob() == JT_NINJA)
		{
			SetAction(skillAction, 6, MT_ONETIME);
			SetForceAnimSpeed(4);
			SetForceAnimation(skillAction, 2, 5, skillAction, 5, TRUE, 1000, FALSE);
		}
		else if (GetJob() == JT_GUNSLINGER)
		{
			SetAction(32, 6, MT_ONETIME);
			SetForceAnimSpeed(4);
			SetForceAnimation(skillAction, 0, 3, skillAction, 3, TRUE, 1000, FALSE);
		}
		else
		{
			SetAction(skillAction, 6, MT_ONETIME);
		}
		m_attackMotion = 6;
		break;
	}

	case ST_ATTACK:
	{
		m_isCounter = FALSE;
		if (m_bodyState != BODYSTATE_STONECURSE && m_bodyState != BODYSTATE_FREEZING)
			m_isMotionFreezed = false;
		SendMsg(NULL, AM_DELETE_SKILL_RECHARGEGAGE, 0, 0, 0);

		bool isSecondAttack = g_session.IsSecondAttack(GetJob(), m_sex, m_weapon);
		if (GetJob() >= JT_TAEKWON && GetJob() <= JT_STAR2)
		{
			int R = rand() % 10;
			if (R < 7) {
				isSecondAttack = true;
				CGameActor::SetState(ST_ATTACK2);
			} else {
				isSecondAttack = false;
				CGameActor::SetState(ST_ATTACK);
			}
		}
		else
		{
			if (isSecondAttack)
				CGameActor::SetState(ST_ATTACK2);
			else
				CGameActor::SetState(ST_ATTACK);
		}

		m_attackMotion = g_session.GetPCAttackMotion(GetJob(), m_sex, m_weapon, isSecondAttack);

		int weapon = m_weapon;
		if (weapon > WEAPONTYPE_LAST)
			weapon = g_session.GetWeaponType(weapon);

		if (weapon == WEAPONTYPE_BOW) {
			g_modeMgr.GetGameMode()->MakeArrowEffect(m_pos, m_targetGid, m_attackMotion * m_motionSpeed, 0);
			m_attackMotion += 8 / m_motionSpeed;
		}
		break;
	}

	case ST_ATTACK2:
	{
		m_isCounter = FALSE;
		if (m_bodyState != BODYSTATE_STONECURSE && m_bodyState != BODYSTATE_FREEZING)
			m_isMotionFreezed = false;
		SendMsg(NULL, AM_DELETE_SKILL_RECHARGEGAGE, 0, 0, 0);

		bool isSecondAttack = g_session.IsSecondAttack(GetJob(), m_sex, m_weapon);
		if (isSecondAttack)
			CGameActor::SetState(ST_ATTACK2);
		else
			CGameActor::SetState(ST_ATTACK);
		m_attackMotion = g_session.GetPCAttackMotion(GetJob(), m_sex, m_weapon, isSecondAttack);

		if (m_weapon == WEAPONTYPE_WHIP || m_weapon == WEAPONTYPE_INSTRUMENT) {
			g_modeMgr.GetGameMode()->MakeArrowEffect(m_pos, m_targetGid, m_attackMotion * m_motionSpeed, 0);
			m_attackMotion += 8 / m_motionSpeed;
		}
		break;
	}

	default:
		CGameActor::SetState(stateId);
		return;
	}

	m_curAction = m_baseAction + Get8Dir(m_roty);
	m_stateId = stateId;
	m_stateStartTick = timeGetTime();
}


void	CPc :: ProcessGage()
{
	if (m_gage) {
		m_gage->Move(m_lastTlvertX - 30, (int)((float)(g_renderer->m_height * 12 / 480) * m_lastPixelRatio + m_lastTlvertY));
	}
}


void	CPc :: SetModifyFactorOfmotionSpeed (int attackMT)
{
	int weapon = m_weapon;
	if (weapon > 0x1f) {
		weapon = g_session.GetWeaponType(weapon);
	}

	if (weapon != 0xb) {
		if (attackMT > MAX_ATTACKMT)
		{
			attackMT = MAX_ATTACKMT;
		}
	}
	m_modifyFactorOfmotionSpeed = (float)attackMT / 432.0f;
}

void	CPc :: SetHonor(int honor)
{
	if (honor < 0)
		SendMsg(NULL, AM_EMOTION, 0x22, 0, 0);
	else {
		SendMsg(NULL, AM_DELETE_PROHIBITEFFECT, 0, 0, 0);
	}

	m_honor = honor;
}

void	CPc :: SetPropensityInfo(int honor, int virtue)
{
	SetHonor(honor);
	m_virtue = virtue;
}

void	CPc :: SendMsg(CGameObject *sender, int message, int arg1, int arg2, int arg3)
{
	CGameMode *		gameMode = g_modeMgr.GetGameMode();
	sAssert(gameMode);

	switch (message) {
		case	AM_SET_HEAD_DIR: {
			sAssert(arg1>=0 && arg1<=2);
			if (m_stateId == ST_STAND || m_stateId == ST_SIT) {
				m_curMotion = arg1;
			}
			break;
		}
		case	AM_REMOVEGAGE: {
			if (m_gage) {
				g_windowMgr.PostQuit(m_gage);
				m_gage = NULL;
			}
			break;
		}
		case	AM_UPDATEGAGE: {
			if (!m_gage) {
				m_gage = new UIPcGage();
				m_gage->Create(60, 5);
				g_windowMgr.AddWindowFront(m_gage);
			}
			m_gage->SetHp(arg1, arg2);
			m_gage->SetSp(0, 0);
			m_gage->Move(m_lastTlvertX - 30, g_renderer->m_height * 12 / 480 + m_lastTlvertY);
#ifdef _ARTICLE_BLOOD_DIS

			m_ActorTitle->SetHp(arg1, arg2 ) ;
#endif
			break;
		}
		case	AM_STANDENTRY:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;
			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);
			SetSprAct(
				s->job, s->sex, s->head,
				s->weapon,
				s->accessory ,
				s->accessory2,
				s->accessory3,
				s->shield,
				s->headpalette, s->bodypalette);

			SetAttrState(s->bodyState, s->effectState, s->healthState);

			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					SetState(ST_STAND);
					break;
			}
			break;
			MakePartyEffect(s->GID);
		}
		case	AM_STANDENTRY2:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;
			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);
			SetSprAct2(s->job, s->sex, s->head, s->weapon, s->accessory, s->accessory2, s->accessory3, s->shield, s->headpalette, s->bodypalette , s->effectState);
			SetAttrState(s->bodyState, s->effectState, s->healthState);
			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					SetState(ST_STAND);
					break;
			}
			break;
			MakePartyEffect(s->GID);
		}
		case	AM_MAKE_BLUR_ACTOR:
			break;
		case	AM_CAMPINFO:
		case	AM_MAKE_CHICKEN: {
			SetSprJob(GetJob());
			SetImfFileName();
			int weapon = GetWeapon();
			if (weapon > 0 && weapon < WEAPONTYPE_LAST)
			{
				SetSprWeapon(weapon);
			}
			else
			{
				SetSprWeapon2(weapon);
			}
			SetSprShield( m_shield );
			SetHeadPaletteName();
			SetBodyPaletteName();
		}
		break;
		case	AM_DELETE_CHICKEN: {
			SetSprJob(GetJob());
			SetImfFileName();
			int weapon = GetWeapon();
			if (weapon > 0 && weapon < WEAPONTYPE_LAST)
			{
				SetSprWeapon(weapon);
			}
			else
			{
				SetSprWeapon2(weapon);
			}
			SetSprShield(m_shield);
			SetHeadPaletteName();
			SetBodyPaletteName();
		}
		break;
		case	AM_INITSTANDENTRY:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			m_gid = s->GID;
			CGameMode *			gameMode = g_modeMgr.GetGameMode();
			sAssert(gameMode);

			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_speed = s->speed;
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;

			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);

	  SetSprAct(s->job, s->sex, s->head,
		s->weapon,
		s->accessory ,
		s->accessory2 ,
		s->accessory3 ,
		s->shield,
		s->headpalette, s->bodypalette);

	  SetAttrState(s->bodyState, s->effectState, s->healthState);
			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					sAssert(false);
					Trace("UNKNOWN PCSTATE");
					SetState(ST_STAND);
					break;
			}

			if (arg2 == 2 && !IsEffectStateSpecialHiding(m_effectState)) {
#ifdef SECONDJOB_SKILL2

				LaunchEffect(EF_ENTRY2);
#else
				LaunchEffect(EF_ENTRY);
#endif
			}
			MakePartyEffect(s->GID);
			break;
		}
		case	AM_INITSTANDENTRY2:
			{

			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			m_gid = s->GID;
			CGameMode *			gameMode = g_modeMgr.GetGameMode();
			sAssert(gameMode);

			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_speed = s->speed;
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;

			s->accessory2;
			s->accessory3;

			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);
			SetSprAct2(s->job, s->sex, s->head, s->weapon, s->accessory, s->accessory2, s->accessory3, s->shield, s->headpalette, s->bodypalette,s->effectState);
			SetAttrState(s->bodyState, s->effectState, s->healthState);
			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					sAssert(false);
					Trace("UNKNOWN PCSTATE");
					SetState(ST_STAND);
					break;
			}

			if (arg2 == 2 && !IsEffectStateSpecialHiding(m_effectState)) {
#ifdef SECONDJOB_SKILL2

				LaunchEffect(EF_ENTRY2);
#else
				LaunchEffect(EF_ENTRY);
#endif
			}
			MakePartyEffect(s->GID);
			break;
		}
		case	AM_INITMOVEENTRY: {
				MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
				m_gid = ms->GID;
				m_speed = ms->speed;
				m_moveStartTime = ms->moveStartTime;

				m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

				ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);

				SetSprAct(
					ms->job, ms->sex, ms->head,
					ms->weapon,
					ms->accessory ,
					ms->accessory2,
					ms->accessory3,
					ms->shield,
					ms->headpalette, ms->bodypalette);

				SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
				SetPKState(ms->isPKModeON);
				SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
				SetPropensityInfo(ms->honor, ms->virtue);

				gameMode->GetClientCoor(ms->sx, ms->sy, m_pos.x, m_pos.z);
				m_pos.x += (ms->cellX-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				m_pos.z += (ms->cellY-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				CWorld *	world = gameMode->GetWorld();
				m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);

				bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);

				if (result) {
					m_moveDestX = ms->dx;
					m_moveDestY = ms->dy;
					SetState(ST_MOVE);
					m_pathStartCell = 0;
				} else
					SetState(ST_STAND);
				MakePartyEffect(ms->GID);
			}
			break;
		case	AM_INITMOVEENTRY2: {
				MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
				m_gid = ms->GID;
				m_speed = ms->speed;
				m_moveStartTime = ms->moveStartTime;

				m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

				ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);
				SetSprAct2(
					ms->job, ms->sex, ms->head,
					ms->weapon,
					ms->accessory ,
					ms->accessory2 ,
					ms->accessory3 ,
					ms->shield ,
					ms->headpalette, ms->bodypalette,ms->effectState);

				SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
				SetPKState(ms->isPKModeON);
				SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
				SetPropensityInfo(ms->honor, ms->virtue);

				gameMode->GetClientCoor(ms->sx, ms->sy, m_pos.x, m_pos.z);
				m_pos.x += (ms->cellX-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				m_pos.z += (ms->cellY-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				CWorld *	world = gameMode->GetWorld();
				m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);

				bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);

				if (result)
				{
					m_moveDestX = ms->dx;
					m_moveDestY = ms->dy;
					SetState(ST_MOVE);
					m_pathStartCell = 0;
				} else
					SetState(ST_STAND);
				MakePartyEffect(ms->GID);


			}
			break;
		case	AM_MOVEENTRY: {
			MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
			m_gid = ms->GID;
			m_speed = ms->speed;
			m_moveStartTime = ms->moveStartTime;

			m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

			ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);
			SetSprAct(ms->job, ms->sex, ms->head, ms->weapon, ms->accessory, ms->accessory2, ms->accessory3, ms->shield, ms->headpalette, ms->bodypalette);
			SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
			SetPKState(ms->isPKModeON);
			SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
			SetPropensityInfo(ms->honor, ms->virtue);

			bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);
			if (result) {
				m_moveDestX = ms->dx;
				m_moveDestY = ms->dy;
				SetState(ST_MOVE);
				m_pathStartCell = 0;
			}

			MakePartyEffect(ms->GID);
			break;
		}
		case	AM_MOVEENTRY2: {
			MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
			m_gid = ms->GID;
			m_speed = ms->speed;
			m_moveStartTime = ms->moveStartTime;

			m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

			ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);
			SetSprAct2(ms->job, ms->sex, ms->head, ms->weapon, ms->accessory, ms->accessory2, ms->accessory3, ms->shield, ms->headpalette, ms->bodypalette, ms->effectState);
			SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
			SetPKState(ms->isPKModeON);
			SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
			SetPropensityInfo(ms->honor, ms->virtue);

			bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);
			if (result)
			{
				m_moveDestX = ms->dx;
				m_moveDestY = ms->dy;
				SetState(ST_MOVE);
				m_pathStartCell = 0;
			}

			MakePartyEffect(ms->GID);
			break;
		}
		case	AM_CHANGE_RANK: {
 			m_pk_rank = arg2;
			m_pk_total = arg1;
		}
		break;
		default:
			CGameActor::SendMsg(sender, message, arg1, arg2, arg3);
			break;
	}
}


void	CPc :: SetRank(int Total, int Rank) {}

void	CPc :: Render(matrix &vtm)
{
	char PASS;
	PASS = 0;

	if (IsEffectStateFootPrint(m_effectState))
	{
		RenderShoe(1);
	}

	if ( (m_BodyLight & BL_RUNNING) == BL_RUNNING )
	{
		RenderShoe(2);
	}

	for (int a=0; a<stateId.size(); a++)
	{
		if (stateId[a] == EFST_CLAIRVOYANCE)
			PASS = 1;
	}
	if ( PASS == 1 && ( m_isVisible == 0 || m_isVisibleBody == 0 ) && !IsEffectStateSpecialHiding(m_effectState) )
	{
		SetArgb(-1, 0, 0, 0);
		RenderBodyLayer(vtm, FALSE);
	}
	else if ( m_isVisible )
	{
		if (m_stateId != 6 && m_stateId != ST_DEAD)
		{
			RenderShadow(vtm, m_zoom);
		}

		if (m_stateId == 6 && GetJob() == JT_STAR2)
			RenderShadow(vtm, m_zoom);

		if (m_isVisibleBody)
		{
			if (IsEventAID(m_gid))
			{
				RenderBody2(vtm, FALSE);
			}
			else
			{
				if (m_renderWithoutLayer)
					RenderBody(vtm, FALSE);
				else
				{
					RenderBodyLayer(vtm, FALSE);
				}
			}
		}
		else
		{
			UpdateTlvertex(vtm);
		}
	}
	else
	{
		UpdateTlvertex(vtm);
	}
}

void	CPc :: RenderShadowLayer(matrix &vtm)
{
	static int head_imfOffX = 0;
	static int head_imfOffY = 0;
	vector3d	tv;
	tv.MatrixMult(m_pos, vtm);

	float		x, y, oow;
	g_renderer->ProjectVertex(m_pos, vtm, &x, &y, &oow);


	float	pixelRatio = g_renderer->GetPixelRatio(oow);
	m_lastPixelRatio = pixelRatio / g_avgPixelRatio;
	float	fSprShift = m_sprShift * m_zoom * g_renderer->GetScreenXFactor() * m_lastPixelRatio;
	float	zHover = g_zHover * m_lastPixelRatio;

	float	lmRIntensity = 1.0f;
	float	lmGIntensity = 1.0f;
	float	lmBIntensity = 1.0f;
	SetLmIntensity(lmRIntensity, lmGIntensity, lmBIntensity);
	DWORD	m_A = (m_sprArgb&0xFF000000)>>24, m_R = (m_sprArgb&0x00FF0000)>>16, m_G = (m_sprArgb&0x0000FF00)>>8, m_B = m_sprArgb&0x000000FF;
	float	m_fA = float(m_A)/255.f, m_fR = float(m_R)/255.f, m_fG = float(m_G)/255.f, m_fB = float(m_B)/255.f;
	m_fR *= lmRIntensity; m_fG *= lmGIntensity; m_fB *= lmBIntensity;

	RENDER_INFO_RECT	info = { g_renderer->m_width, g_renderer->m_height, 0, 0, oow };
	CImfRes *			imfRes = (CImfRes *) g_resMgr().Get(m_imfName.c_str(), 0);

	BOOL	bodyAndAccessoryIsExchanged = 0;
	BOOL	headLayerPassed = FALSE;
	BOOL	isInsideScreen = FALSE;

	int			drawCount = 0;

	for (int p=PC_RENDER_LAYER_SIZE-1; p>=0; --p)
	{
		int		layer = GetLayer(imfRes, p, m_baseAction, m_curAction, m_curMotion);
		int		layerQueried = layer;

		if (!headLayerPassed)
			headLayerPassed = (layerQueried==1) ? TRUE : FALSE;

		if (headLayerPassed && layerQueried==0) {
			layer = 2;
			++bodyAndAccessoryIsExchanged;
		}
		if (bodyAndAccessoryIsExchanged==1 && layerQueried==2) {
			++bodyAndAccessoryIsExchanged;
			layer = 0;
		}

		sAssert(layer>=0 && layer<PC_RENDER_LAYER_SIZE);
		if (!(layer>=0 && layer<PC_RENDER_LAYER_SIZE))
			layer = 0;

		if (strlen(GetActNameList(layer))==0 || strlen(GetSprNameList(layer))==0)
			continue;

		if (!NeedWeaponLayer(m_curAction) && layer>=5 )
			continue;

		CActRes *	actRes = GetActResList(layer);
		CSprRes *	sprRes = GetSprResList(layer);
		int			clipIndex = GetClipIndex(layer);

		if (m_curMotion >= actRes->GetMotionCount(m_curAction))
			continue;

		#ifdef _DEBUG

			if (m_curAction >= actRes->GetActionCount()) {
				sAssert(false);
				ErrorMsg("Error : CPc::Render - Illegal action");
				return;
			}
			if (m_curMotion >= actRes->GetMotionCount(m_curAction)) {
				sAssert(false);
				ErrorMsg("Error : CPc::Render - Illegal motion");
				return;
			}
		#endif

		CMotion *	mot = actRes->GetMotion(m_curAction, m_curMotion);

		sAssert(mot->numClips > clipIndex);

		CSprClip *	sprClip = mot->GetClip(clipIndex);
		if ((unsigned int)sprClip->sprIndex == 0xffffffff)
			continue;


		DWORD	argb = (255<<24) + (1<<16) + (1<< 8) + 255;

		float	zoomX = 1.0f * m_zoom;
		float	zoomY = 1.0f * m_zoom;
		float	top, btm, left, right;

		SprImg *	img = sprRes->m_sprites[sprClip->clipType][sprClip->sprIndex];

		int			imfOffX = 0;
		int			imfOffY = 0;
		switch (layer) {
			case	0:
			case	1: {
				if (layer == 1) {
					CMotion *	headMotion = mot;
					CMotion *	bodyMotion = GetActResList(Layer_Body)->GetMotion(m_curAction, m_curMotion);

					if (headMotion->attachCnt && bodyMotion->attachCnt) {
						if (headMotion->attachInfo[0].m_attr == bodyMotion->attachInfo[0].m_attr) {
							imfOffX = bodyMotion->attachInfo[0].x - headMotion->attachInfo[0].x;
							imfOffY = bodyMotion->attachInfo[0].y - headMotion->attachInfo[0].y;
							head_imfOffX = imfOffX;
							head_imfOffY = imfOffY;
						}
					}
				}
			}
			break;
			case	2:
			case	3:
			case	4: {
				CMotion *	accMotion = mot;
				CMotion *	headMotion = GetActResList(Layer_Head)->GetMotion(m_curAction, m_curMotion);
				if (accMotion->attachCnt && headMotion->attachCnt) {
					if (accMotion->attachInfo[0].m_attr == headMotion->attachInfo[0].m_attr) {
						imfOffX += headMotion->attachInfo[0].x - accMotion->attachInfo[0].x;
						imfOffY += headMotion->attachInfo[0].y - accMotion->attachInfo[0].y;
					}
				}
				imfOffX += head_imfOffX;
				imfOffY += head_imfOffY;
			}
			break;
		}

		top = y + double(sprClip->y - 5 + imfOffY) * pixelRatio * sprClip->zoomy * zoomY;
		btm = y + double(sprClip->y + 5 + img->height*(img->isHalfH+1)+imfOffY) * pixelRatio * sprClip->zoomy * zoomY + 1;

		if (sprClip->flags & 1) {
			left = x + double(sprClip->x - 5 + img->width*(img->isHalfW+1)+imfOffX) * pixelRatio * sprClip->zoomx * zoomX;
			right = x + double(sprClip->x + 5 + imfOffX) * pixelRatio * sprClip->zoomx * zoomX + 1;

			if (top >= g_renderer->m_height || btm <= 0 || right >= g_renderer->m_width || left <= 0)
				continue;
		} else {
			left = x + double(sprClip->x - 5 + imfOffX) * pixelRatio * sprClip->zoomx * zoomX;
			right = x + double(sprClip->x + 5 + img->width*(img->isHalfW+1)+imfOffX) * pixelRatio * sprClip->zoomx * zoomX + 1;

			if (top >= g_renderer->m_height || btm <= 0 || left >= g_renderer->m_width || right <= 0)
				continue;
		}
		isInsideScreen = TRUE;

		float	sprOow;
		float	sprOowup;
		if (m_alwaysTopLayer) {
			sprOow =	1.0f/NEARPLANE + g_zOffset*float(p) - g_zOffset*20 + g_zOffset*m_offsetOow;
			sprOowup =	1.0f/NEARPLANE + g_zOffset*float(p) - g_zOffset*20 + g_zOffset*m_offsetOow;
		} else {
			if (m_isLieOnGround) {
				sprOow =	GetShadowOow(btm, y, tv.z)+g_zOffset*float(drawCount+0+m_offsetOow)+zHover;
				sprOowup =	GetShadowOow(top, y, tv.z)+g_zOffset*float(drawCount+0+m_offsetOow)+zHover;
			} else {
				sprOow =	GetSprOow(btm, y, tv.z + 0.1f) + g_zOffset*float(drawCount+0+m_offsetOow) + zHover;
				sprOowup =	GetSprOow(top, y, tv.z + 0.1f) + g_zOffset*float(drawCount+0+m_offsetOow) + zHover;
			}
		}
		++drawCount;

		if (m_isSingleColor && m_colorOfSingleColor != 0xffff0000) {
			argb = m_colorOfSingleColor;
		}


		RPSprite	spr;
		spr.top			= top;
		spr.bottom		= btm;
		spr.left		= left + fSprShift;
		spr.right		= right + fSprShift;
		spr.oow			= sprOow;
		spr.oowup		= sprOowup;
		spr.spr			= img;
		spr.rgba		= argb;
		spr.angle		= sprClip->angle+m_sprAngle;

		if(layer==0 || layer==1) {
			if (m_shouldAddPickInfo) info.Update(spr);
		}

		SYSTEMPALETTE *		palette;
		switch (layer) {
			case	0:
				if (m_bodyPalette) {
					CPaletteRes *	palRes = (CPaletteRes *)g_resMgr().Get(m_bodyPaletteName.c_str(), 0);
					palette = palRes->m_pal;
				} else {
					palette = sprRes->m_pal;
				}
				break;
			case	1:
				if (m_headPalette) {
					CPaletteRes *	palRes = (CPaletteRes *)g_resMgr().Get(m_headPaletteName.c_str(), 0);
					palette = palRes->m_pal;
				} else {
					palette = sprRes->m_pal;
				}
				break;
			default:
				palette = sprRes->m_pal;
				break;
		}

		RenderSprite(spr, palette, RF_SPRITE);


	}

	if (isInsideScreen)
		SetRenderInfo(info, x, y);
	else
		SetTlvert(x, y);
}

#if 1
void	CPc :: RenderBodyLayer(matrix &vtm, BOOL isBlur)
{
	static int head_imfOffX = 0;
	static int head_imfOffY = 0;
	int DoubleAcc;



	vector3d	tv;
	tv.MatrixMult(m_pos, vtm);

	float		x, y, oow;
	g_renderer->ProjectVertex(m_pos, vtm, &x, &y, &oow);

	float	pixelRatio = g_renderer->GetPixelRatio(oow);
	m_lastPixelRatio = pixelRatio / g_avgPixelRatio;

	float	fSprShift = m_sprShift * m_zoom * g_renderer->GetScreenXFactor() * m_lastPixelRatio;
	float	zHover = g_zHover * m_lastPixelRatio;

	float	lmRIntensity = 1.0f;
	float	lmGIntensity = 1.0f;
	float	lmBIntensity = 1.0f;
	SetLmIntensity(lmRIntensity, lmGIntensity, lmBIntensity);
	DWORD	m_A = (m_sprArgb&0xFF000000)>>24, m_R = (m_sprArgb&0x00FF0000)>>16, m_G = (m_sprArgb&0x0000FF00)>>8, m_B = m_sprArgb&0x000000FF;

	float	m_fA = float(m_A)/255.f, m_fR = float(m_R)/255.f, m_fG = float(m_G)/255.f, m_fB = float(m_B)/255.f;
	m_fR *= lmRIntensity;
	m_fG *= lmGIntensity;
	m_fB *= lmBIntensity;

	RENDER_INFO_RECT	info = { g_renderer->m_width, g_renderer->m_height, 0, 0, oow };
	CImfRes *			imfRes = (CImfRes *) g_resMgr().Get(m_imfName.c_str(), 0);

	BOOL	bodyAndAccessoryIsExchanged = 0;
	BOOL	headLayerPassed = FALSE;
	BOOL	isInsideScreen = FALSE;

	SetCharacterPixelRatio(&pixelRatio);
	if  ( (GetJob() >= JT_NOVICE_B && GetJob() <= JT_THIEF_B) || GetJob() == JT_SUPERNOVICE_B )
		pixelRatio *= 0.75f;
	else if  (GetJob() > JT_THIEF_B && GetJob() < JT_SUPERNOVICE_B)
		pixelRatio *= 0.82f;

	short fpos=0;
	if ( GetJob() == JT_STAR2 )
		fpos = SetCharacterFlying();
	else if (GetJob() == JT_STAR)
	{
		if (m_FlyNow>0)
			m_FlyNow--;
	}

	DoubleAcc = 0;
	int			drawCount = 0;
	int			p;
	for (p=PC_RENDER_LAYER_SIZE-1; p>=0; --p)
	{
		int		layer = GetLayer(imfRes, p, m_baseAction, m_curAction, m_curMotion);


		int		layerQueried = layer;

		if (!headLayerPassed)
			headLayerPassed = (layerQueried==Layer_Head) ? TRUE : FALSE;

		if (headLayerPassed && layerQueried==Layer_Body)
		{
			layer = Layer_Accessory1;
			++bodyAndAccessoryIsExchanged;
		}
		if (bodyAndAccessoryIsExchanged==1 && layerQueried==Layer_Accessory1)
		{
			++bodyAndAccessoryIsExchanged;
			layer = Layer_Body;
		}

		sAssert(layer>=0 && layer<PC_RENDER_LAYER_SIZE);
		if (!(layer>=0 && layer<PC_RENDER_LAYER_SIZE))
			layer = 0;

			if(layer==Layer_Accessory1) layer=Layer_Accessory3;
			else
			if(layer==Layer_Accessory3) layer=Layer_Accessory1;

			if (strlen(GetActNameList(layer)) == 0 || strlen(GetSprNameList(layer)) == 0)
			{
				continue;
			}


			if (!NeedWeaponLayer(m_curAction) && layer>=Layer_Weapon1)
			{
				continue;
			}
			CActRes *	actRes = GetActResList(layer);
			CSprRes *	sprRes = GetSprResList(layer);

			{
				char accname[256];
				char buf[1024];

				if (DoubleAcc == 1)
					sprintf(accname, "\x5f\xbf\xe4\xc1\xa4\xc0\xc7\xb1\xcd");
				else if (DoubleAcc == 2)
					sprintf(accname, "\x5f\xb4\xab\xb0\xa1\xb8\xae\xb0\xb3");
				else if (DoubleAcc == 3)
					sprintf(accname, "\x5f\xc7\xef\xb8\xa7");

				if (DoubleAcc > 0)
				{
					sprintf(buf, "sprite\\\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.act", g_session.m_newPcSexNameTable[m_sex], g_session.m_newPcSexNameTable[m_sex], accname);
					actRes = (CActRes *)g_resMgr().Get(buf, 0);
					sprintf(buf, "sprite\\\xbe\xc7\xbc\xbc\xbb\xe7\xb8\xae\\%s\\%s%s.spr", g_session.m_newPcSexNameTable[m_sex], g_session.m_newPcSexNameTable[m_sex], accname);
					sprRes = (CSprRes *)g_resMgr().Get(buf, 0);
					DoubleAcc = 100;
				}
			}
			if (actRes == 0)
				actRes = GetActResList(layer);
			if (sprRes == 0)
				sprRes = GetSprResList(layer);

#ifdef	SECONDJOB_SKILL2
				if (layer == Layer_Head && IsEffectStateOrcFace(m_effectState))
			{
			actRes = (CActRes *)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\orcface.act", 0);
			sprRes = (CSprRes *)g_resMgr().Get("\xC0\xCC\xC6\xD1\xC6\xAE\\orcface.spr", 0);
		}
#endif
			int			clipIndex = GetClipIndex(layer);



			if (m_curMotion >= actRes->GetMotionCount(m_curAction))
			{
				continue;
			}

		CMotion *	mot = 0 ;
		int tempAction;
		char changed;

		changed = 0;


		if (GetJob() >= JT_TAEKWON && GetJob() <= JT_STAR2
			&& m_curAction >= 88
			&& layer >= Layer_Head && layer <= Layer_Accessory3 )
		{
			if (m_curAction>=88 && m_curAction<=95 && m_curMotion>=3 && m_curMotion<=5)
			{
			if (m_sex == 0 && m_curMotion != 3)
				{
					changed = 1;
					if (m_curAction<=89)
					{
						if (m_curMotion == 4)
							tempAction = 6;
						else if (m_curMotion == 5)
							tempAction = 5;
					}
					else if (m_curAction<=91)
					{
						if (m_curMotion == 4)
							tempAction = 6;
						else if (m_curMotion == 5)
							tempAction = 7;
					}
					else if (m_curAction<=93)
					{
						if (m_curMotion == 4)
							tempAction = 2;
						else if (m_curMotion == 5)
							tempAction = 1;
					}
					else if (m_curAction<=95)
					{
						if (m_curMotion == 4)
							tempAction = 2;
						else if (m_curMotion == 5)
							tempAction = 3;
					}
				}
			else if (m_sex == 1 && m_curMotion != 5)
				{
					changed = 1;
					if (m_curAction<=89)
					{
						if (m_curMotion == 3)
							tempAction = 6;
						else if (m_curMotion == 4)
							tempAction = 5;
					}
					else if (m_curAction<=91)
					{
						if (m_curMotion == 3)
							tempAction = 6;
						else if (m_curMotion == 4)
							tempAction = 7;
					}
					else if (m_curAction<=93)
					{
						if (m_curMotion == 3)
							tempAction = 2;
						else if (m_curMotion == 4)
							tempAction = 1;
					}
					else if (m_curAction<=95)
					{
						if (m_curMotion == 3)
							tempAction = 2;
						else if (m_curMotion == 4)
							tempAction = 3;
					}
				}
			}

			if (m_curAction>=96 && m_curMotion == 1)
			{
				changed = 1;
				if (m_curAction>=96 && m_curAction<=97)
					tempAction = 0;
				else if (m_curAction>=98 && m_curAction<=99)
					tempAction = 6;
				else if (m_curAction>=100 && m_curAction<=101)
					tempAction = 4;
				else
					tempAction = 2;
			}

			if (changed == 1)
			{
				mot = actRes->GetMotion(tempAction, 0);
			}
			else
			{
				mot = actRes->GetMotion(m_curAction, m_curMotion);
			}
		}
		else
		{
			mot = actRes->GetMotion(m_curAction, m_curMotion);
		}

#ifdef	SECONDJOB_SKILL2
		if (layer == Layer_Weapon2 && mot->m_eventId != -1)
		{
			std::string eventName = actRes->GetEventName(mot->m_eventId);
			if (eventName.find(".wav") != -1)
			{
				strcpy(m_hitWaveName, eventName.c_str());
			}
			m_isPlayHitWave = TRUE;
		}
#endif



		if ( layer == Layer_Accessory1 || layer == Layer_Accessory2 || layer == Layer_Accessory3 )
		{
			int headMotionCnt = GetActResList(Layer_Head)->GetMotionCount ( m_curAction ) ;
			int accMotionCnt = GetActResList(layer)->GetMotionCount ( m_curAction ) ;

			if ( headMotionCnt > 0 && accMotionCnt > 0 &&
				(headMotionCnt != accMotionCnt) && (accMotionCnt > headMotionCnt) && ((accMotionCnt % headMotionCnt) == 0) )
			{
				int		 motionGap = accMotionCnt / headMotionCnt ;
				mot = actRes->GetMotion(m_curAction, m_curMotion * motionGap);
			}
		}
			CSprClip *	sprClip = mot->GetClip(clipIndex);
			if (sprClip->sprIndex == -1)
				continue;


		DWORD	A = sprClip->a, R = sprClip->r, G = sprClip->g, B = sprClip->b;
		float	fA = float(A)/255.f, fR = float(R)/255.f, fG = float(G)/255.f, fB = float(B)/255.f;
		fA *= m_fA; fR *= m_fR; fG *= m_fG; fB *= m_fB;
		DWORD	argb = (DWORD(fA*255.f)<<24) + (DWORD(fR*255.f)<<16) + (DWORD(fG*255.f)<< 8) + DWORD(fB*255.f);

		float	zoomX = 1.0f * m_zoom;
		float	zoomY = 1.0f * m_zoom;
		float	top, btm, left, right;

		SprImg *	img = sprRes->m_sprites[sprClip->clipType][sprClip->sprIndex];

		int			imfOffX = 0;
		int			imfOffY = 0;




		switch (layer)
		{
			case	Layer_Body:
			case	Layer_Head:
				{

					if (layer == Layer_Head)
					{
						CMotion *	headMotion = mot;
						CMotion *	bodyMotion = GetActResList(Layer_Body)->GetMotion(m_curAction, m_curMotion);

						if (mot->attachCnt && bodyMotion->attachCnt)
						{
							if (headMotion->attachInfo[0].m_attr == bodyMotion->attachInfo[0].m_attr)
							{
								imfOffX += bodyMotion->attachInfo[0].x - headMotion->attachInfo[0].x;
								imfOffY += bodyMotion->attachInfo[0].y - headMotion->attachInfo[0].y;

								head_imfOffX = imfOffX;
								head_imfOffY = imfOffY;
							}
						}
					}
				}
			break;
			case	Layer_Accessory1:
			case	Layer_Accessory2:
			case	Layer_Accessory3:
			case	8:
			{
				CMotion *	accMotion = mot;
				CMotion *	headMotion;


				{
					int headMotionCnt = GetActResList(Layer_Head)->GetMotionCount ( m_curAction ) ;
					int accMotionCnt = GetActResList(layer)->GetMotionCount ( m_curAction ) ;

					if ( headMotionCnt > 0 && accMotionCnt > 0 &&
						(headMotionCnt != accMotionCnt) && (accMotionCnt > headMotionCnt) && ((accMotionCnt % headMotionCnt) == 0) )
					{
						int		 motionGap = accMotionCnt / headMotionCnt ;
						int		 motionSpeed = GetActResList(layer)->GetDelay ( 0 ) ;
						float	stateCnt = float(timeGetTime() - m_stateStartTick)/24.0f;

						if ( motionSpeed == 0 )
							motionSpeed = 4 ;

						int		 randMot = int(stateCnt/motionSpeed)%motionGap ;

						int tmpMotion = m_curMotion * motionGap + randMot ;


						accMotion = GetActResList(layer)->GetMotion(m_curAction, tmpMotion);


						sprClip = accMotion->GetClip(clipIndex);

						if ( sprClip->sprIndex != -1 )
						{
							img = sprRes->m_sprites[sprClip->clipType][sprClip->sprIndex];
						}
					}

					if (changed == 1)
						headMotion = GetActResList(Layer_Head)->GetMotion(tempAction, 0);
					else
						headMotion = GetActResList(Layer_Head)->GetMotion(m_curAction, m_curMotion);

					if (accMotion->attachCnt && headMotion->attachCnt)
					{
						if (accMotion->attachInfo[0].m_attr == headMotion->attachInfo[0].m_attr)
						{
							imfOffX += headMotion->attachInfo[0].x - accMotion->attachInfo[0].x;
							imfOffY += headMotion->attachInfo[0].y - accMotion->attachInfo[0].y;
						}
					}
					imfOffX += head_imfOffX;
					imfOffY += head_imfOffY;
				}
			}
			break;
		}

		SetCharacterPreAngle(layer,&imfOffX,&imfOffY,&head_imfOffX,&head_imfOffY);

		top = y + double(sprClip->y+imfOffY) * pixelRatio * sprClip->zoomy * zoomY;
		btm = y + double(sprClip->y+img->height*(img->isHalfH+1)+imfOffY) * pixelRatio * sprClip->zoomy * zoomY + 1;


		if (sprClip->flags & 1)
		{
			left = x + double(sprClip->x+img->width*(img->isHalfW+1)+imfOffX) * pixelRatio * sprClip->zoomx * zoomX;
			right = x + double(sprClip->x+imfOffX) * pixelRatio * sprClip->zoomx * zoomX + 1;

				if (top >= g_renderer->m_height || btm <= 0 || right >= g_renderer->m_width || left <= 0)
				{
					continue;
				}
			} else
			{
				left = x + double(sprClip->x+imfOffX) * pixelRatio * sprClip->zoomx * zoomX;
				right = x + double(sprClip->x+img->width*(img->isHalfW+1)+imfOffX) * pixelRatio * sprClip->zoomx * zoomX + 1 ;

				if (top >= g_renderer->m_height || btm <= 0 || left >= g_renderer->m_width || right <= 0)
				{
					continue;
				}
			}


		SetCharacterSquare(&top, &btm, &left, &right);

		isInsideScreen = TRUE;

		float	sprOow;
		float	sprOowup;
		if (m_alwaysTopLayer)
		{
			sprOow =	1.0f/NEARPLANE + g_zOffset*float(p) - g_zOffset*20 + g_zOffset*m_offsetOow;
			sprOowup =	1.0f/NEARPLANE + g_zOffset*float(p) - g_zOffset*20 + g_zOffset*m_offsetOow;

		}
		else
		{
			if (m_isLieOnGround) {
				sprOow =	GetShadowOow(btm, y, tv.z)+g_zOffset*float(drawCount+3+m_offsetOow)+zHover;
				sprOowup =	GetShadowOow(top, y, tv.z)+g_zOffset*float(drawCount+3+m_offsetOow)+zHover;

			} else {
				sprOow =	GetSprOow(btm, y, tv.z) + g_zOffset*float(drawCount+3+m_offsetOow) + zHover;
				sprOowup =	GetSprOow(top, y, tv.z) + g_zOffset*float(drawCount+3+m_offsetOow) + zHover;

			}
		}
		++drawCount;
		if (m_isSingleColor && m_colorOfSingleColor != 0xffff0000) {
			argb = m_colorOfSingleColor;
		}


#define NEW_RENDER_POS
#ifdef NEW_RENDER_POS






#else
#endif














		RPSprite	spr;
		spr.top		= top;
		spr.bottom	= btm;
		spr.left		= left + fSprShift;
		spr.right		= right + fSprShift;
















		if (layer==Layer_Weapon1)
		{
			spr.oow			= sprOow + 0.001f;
			spr.oowup		= sprOowup + 0.001f;
		}
		else
		{
			spr.oow			= sprOow;
			spr.oowup		= sprOowup;
		}

		spr.spr			= img;
		spr.rgba		= argb ;
		spr.angle		= sprClip->angle+m_sprAngle;

		SetCharacterAngle(&spr.angle);

		if(layer==Layer_Body || layer==Layer_Head)
		{
			if (m_shouldAddPickInfo) info.Update(spr);
		}

		SYSTEMPALETTE *		palette = 0 ;
		switch (layer)
		{
			case	Layer_Body:
				if (m_bodyPalette)
				{
					CPaletteRes *	palRes = (CPaletteRes *)g_resMgr().Get(m_bodyPaletteName.c_str(), 0);
					palette = palRes->m_pal;
				}
				else
				{
					palette = sprRes->m_pal;
				}
				break;
			case	Layer_Head:
				if (m_headPalette) {
					CPaletteRes *	palRes = (CPaletteRes *)g_resMgr().Get(m_headPaletteName.c_str(), 0);
					palette = palRes->m_pal;
				} else {
					palette = sprRes->m_pal;
				}
				break;
			default:
				palette = sprRes->m_pal;
				break;
		}

		int		renderFlag = RF_SPRITE, ret = EF_MAKEBLUR;
		SendMsg(NULL, AM_IS_HAVE_EFFECT, int(&ret), 0, 0);

		switch (layer)
		{
			case	Layer_Accessory1:
			case	Layer_Accessory2:
				renderFlag = RF_NORMAL;
				break;
			case	Layer_Accessory3:
				renderFlag = RF_NORMAL;
				break;
			default:
				if(argb < 0xff000000) renderFlag = RF_SPRITE;
				break;
		}



		{
		int headMotionCnt = GetActResList(Layer_Head)->GetMotionCount ( m_curAction ) ;
		int accMotionCnt = GetActResList(layer)->GetMotionCount ( m_curAction ) ;

		if ( headMotionCnt > 0 && accMotionCnt > 0 &&
		(headMotionCnt != accMotionCnt) && (accMotionCnt > headMotionCnt) && ((accMotionCnt % headMotionCnt) == 0) )
			renderFlag = RF_NORMAL;
		}

		if ( ( (m_BodyLight & BL_LIGHT_BODY) == BL_LIGHT_BODY) && (layer <= Layer_Accessory3) )
			renderFlag = RF_EFFECT;
		if ( ( (m_BodyLight & BL_LIGHT_SWORD) == BL_LIGHT_SWORD) && (layer == Layer_Weapon1) )
			renderFlag = RF_EFFECT;


		if ( ( (m_BodyLight & BL_SPARK_SWORD) == BL_SPARK_SWORD) && (layer == Layer_Weapon1) )
		{
			if (m_BodyFlag == 0)
			{
				RenderSprite(spr, palette, RF_EFFECT);
				m_BodyFlag = 1;
			}
			else
				m_BodyFlag = 0;
		}

#if defined (LOCAL_SERVER)
		if (layer == Layer_Head)
		{


		}
#endif


		if ( ( (m_BodyLight & BL_DOUBLE_BODY) == BL_DOUBLE_BODY) && (layer <= Layer_Accessory3) )
		{
			if ( (m_BodyLight & BL_HIGHJUMP) != BL_HIGHJUMP && (m_BodyLight & BL_LANDJUMP) != BL_LANDJUMP )
			{
				RPSprite	spr2;
				float add;

				m_BodySin ++;
				if (m_BodySin>180)
					m_BodySin=0;

				add=(g_sinTable[m_BodySin]*1.5f)+5.0f;

				add *= 400.0f / g_modeMgr.GetGameMode()->GetView()->GetDestDistance();

				spr2.top		= top -add;
				spr2.bottom		= btm +add;
				spr2.top -= fpos;
				spr2.bottom -= fpos;

				spr2.left		= left + fSprShift;
				spr2.right		= right + fSprShift;
				if (spr2.left > spr2.right)
				{
					spr2.left+=add;
					spr2.right-=add;
				}
				else
				{
					spr2.left-=add;
					spr2.right+=add;
				}
				spr2.oow			= sprOow - 0.00005f;
				spr2.oowup		= sprOowup - 0.00005f;;
				spr2.spr			= img;
				spr2.rgba		= argb;
				spr2.angle		= sprClip->angle+m_sprAngle;

				RenderSprite(spr2, palette, RF_EFFECT);

			}
		}

		if ( ((m_BodyLight & BL_ASURA) == BL_ASURA) || ((m_BodyLight & BL_UNDEAD) == BL_UNDEAD) )
		{
			if ( (m_BodyLight & BL_LANDJUMP) != BL_LANDJUMP )
			{
				RPSprite	spr2;
				float add;
				short alpha;
				float topA,btmA,leftA,rightA,pixelA,fSprShiftA;

				for (int i=1;i<=2;i++)
				{
						alpha = m_BodySin5;


						pixelA = g_renderer->GetPixelRatio(oow);

						if  (GetJob() >= JT_NOVICE_B && GetJob() <= JT_THIEF_B || GetJob() == JT_SUPERNOVICE_B)
							pixelA *= 0.75f;
						else if  (GetJob() > JT_THIEF_B && GetJob() < JT_SUPERNOVICE_B)
							pixelA *= 0.82f;

						pixelA *= (1.0f+(i*0.01f));

						fSprShiftA = m_sprShift * m_zoom * g_renderer->GetScreenXFactor() * pixelA;

						topA = y + double(sprClip->y+imfOffY) * pixelA * sprClip->zoomy * zoomY;
						btmA = y + double(sprClip->y+img->height*(img->isHalfH+1)+imfOffY) * pixelA * sprClip->zoomy * zoomY + 1;

						if (sprClip->flags & 1) {
							leftA = x + double(sprClip->x+img->width*(img->isHalfW+1)+imfOffX) * pixelA * sprClip->zoomx * zoomX;
							rightA = x + double(sprClip->x+imfOffX) * pixelA * sprClip->zoomx * zoomX + 1;

							if (top >= g_renderer->m_height || btm <= 0 || right >= g_renderer->m_width || left <= 0)
								continue;
						} else {
							leftA = x + double(sprClip->x+imfOffX) * pixelA * sprClip->zoomx * zoomX;
							rightA = x + double(sprClip->x+img->width*(img->isHalfW+1)+imfOffX) * pixelA * sprClip->zoomx * zoomX + 1;

							if (top >= g_renderer->m_height || btm <= 0 || left >= g_renderer->m_width || right <= 0)
								continue;
						}
						add = i*3;

						spr2.top		= topA;
						spr2.bottom		= btmA + ((top-topA)*0.5f);
						spr2.left		= leftA + fSprShiftA;
						spr2.right		= rightA + fSprShiftA;

						spr2.top -= fpos;
						spr2.bottom -= fpos;

						spr2.oow		= sprOow + (0.000005f*add) + 0.00001f;
						spr2.oowup		= sprOowup + (0.000005f*add) + 0.00001f;
						spr2.spr		= img;
						if ( (m_BodyLight & BL_ASURA) == BL_ASURA )
							spr2.rgba		= (alpha<<24) + (55<<16) + (55<<8) + 255;
						else if ( (m_BodyLight & BL_UNDEAD) == BL_UNDEAD )
							spr2.rgba		= (alpha<<24) + (5<<16) + (100<<8) + 5;

						spr2.angle		= sprClip->angle+m_sprAngle;

						RenderSprite(spr2, palette, RF_EFFECT);

				}
			}
		}

		if ( (m_BodyLight & BL_4WAY) == BL_4WAY )
		{
			if ( (m_BodyLight & BL_HIGHJUMP) != BL_HIGHJUMP && (m_BodyLight & BL_LANDJUMP) != BL_LANDJUMP )
			{
				RPSprite	spr2;
				float add;
				short alpha;

				for (int i=1;i<=4;i++)
				{
					add = m_BodyTime*(btm-top)*0.05f;
					alpha = 150-(m_BodyTime*5);
					if (alpha<0)
						alpha=0;

					add *= 400.0f / g_modeMgr.GetGameMode()->GetView()->GetDestDistance();

					if (i==1)
					{
						spr2.top		= top;
						spr2.bottom		= btm;
						spr2.left		= left + fSprShift + add;
						spr2.right		= right + fSprShift + add;
					}
					else if (i==2)
					{
						spr2.top		= top;
						spr2.bottom		= btm;
						spr2.left		= left + fSprShift - add;
						spr2.right		= right + fSprShift - add;
					}
					else if (i==3)
					{
						spr2.top		= top + add;
						spr2.bottom		= btm + add;
						spr2.left		= left + fSprShift;
						spr2.right		= right + fSprShift;
					}
					else if (i==4)
					{
						spr2.top		= top - add;
						spr2.bottom		= btm - add;
						spr2.left		= left + fSprShift;
						spr2.right		= right + fSprShift;
					}

					spr2.top -= fpos;
					spr2.bottom -= fpos;

					spr2.oow		= sprOow + 0.1f;
					spr2.oowup		= sprOowup + 0.1f;
					spr2.spr		= img;
					spr2.rgba		= (alpha<<24) + (255<<16) + (255<<8) + 255;
					spr2.angle		= sprClip->angle+m_sprAngle;

					RenderSprite(spr2, palette, renderFlag);


				}
			}
		}

		if ( (m_BodyLight & BL_REFLECT_BODY) == BL_REFLECT_BODY )
		{
			if ( (m_BodyLight & BL_HIGHJUMP) != BL_HIGHJUMP && (m_BodyLight & BL_LANDJUMP) != BL_LANDJUMP )
			{
				RPSprite	spr2;
				float add;
				short alpha;

				m_BodySin2 ++;
				if (m_BodySin2>=200)
					m_BodySin2-=200;

				for (int i=1;i<5;i++)
				{
					add=(i*5)+(m_BodySin2*0.1f);
					if (add>=20)
						add-=20;
					alpha = 100 - add*5;

					add *= 400.0f / g_modeMgr.GetGameMode()->GetView()->GetDestDistance();

					spr2.top		= top - add;
					spr2.bottom		= btm + add;
					spr2.top -= fpos;
					spr2.bottom -= fpos;

					spr2.left		= left + fSprShift;
					spr2.right		= right + fSprShift;
					if (spr2.left > spr2.right)
					{
						spr2.left+=add;
						spr2.right-=add;
					}
					else
					{
						spr2.left-=add;
						spr2.right+=add;
					}

					spr2.oow		= sprOow - (0.000005f*add) - 0.00001f;
					spr2.oowup		= sprOowup - (0.000005f*add) - 0.00001f;
					spr2.spr		= img;
					spr2.rgba		= (alpha<<24) + (255<<16) + (255<<8) + 255;;
					spr2.angle		= sprClip->angle+m_sprAngle;

					if (renderFlag != RF_EFFECT)
					{
						RenderSprite(spr2, palette, renderFlag);

					}
				}

				spr.rgba		= (150<<24) + (255<<16) + (255<<8) + 255;;
			}
		}

		if ( (m_BodyLight & BL_QUAKE) == BL_QUAKE )
		{
			float ran[4];

			for (int i=0;i<4;i++)
			{
				ran[i] = random(11) - 5;
				ran[i] *= 400.0f / g_modeMgr.GetGameMode()->GetView()->GetDestDistance();
			}

			spr.top			= top + ran[0];
			spr.bottom		= btm + ran[1];
			spr.left		= left + fSprShift + ran[2];
			spr.right		= right + fSprShift + ran[3];

		}

		if ( (m_BodyLight & BL_HIGHJUMP) == BL_HIGHJUMP )
		{
			float add;
			short alpha;

			{
				add = m_BodyTime*20;
				alpha = 255-(m_BodyTime*20);
				if (alpha<0)
					alpha=0;

				add *= 400.0f / g_modeMgr.GetGameMode()->GetView()->GetDestDistance();

				spr.top			= top - add;
				spr.bottom		= btm - add;

				spr.rgba		= (alpha<<24) + (255<<16) + (255<<8) + 255;
			}
		}

		if ( (m_BodyLight & BL_LANDJUMP) == BL_LANDJUMP )
		{
			float add;
			short alpha;

			{
				add = ((25-m_BodyTime2)*20);
				add *= 400.0f / g_modeMgr.GetGameMode()->GetView()->GetDestDistance();
				if (add<0)
					add=0;
				alpha = (m_BodyTime2-10)*17;
				if (alpha>255)
					alpha=255;
				if (alpha<0)
					alpha=0;

				spr.top			= top - add;
				spr.bottom		= btm - add;

				spr.rgba		= (alpha<<24) + (255<<16) + (255<<8) + 255;
			}
		}

			if (fpos != 0)
		{
			spr.top		= spr.top - fpos;
			spr.bottom	= spr.bottom - fpos;
		}

		RenderSprite(spr, palette, renderFlag);




























			SetCharacterLight(spr, palette);

			if (DoubleAcc == 100)
			{
				DoubleAcc = 0;
			}
			else if (layer == Layer_Accessory3)
			{
				++p;
				if (m_accessory3 == 0x120 || m_accessory3 == 0x121)
					DoubleAcc = 1;
				else if (m_accessory3 == 0x124 || m_accessory3 == 0x125)
					DoubleAcc = 2;
				else
					--p;
			}
			else if (layer == Layer_Accessory2)
			{
				++p;
				if (m_accessory2 == 0x122 || m_accessory2 == 0x123)
					DoubleAcc = 3;
				else
					--p;
			}

		}
	if (isInsideScreen)
		SetRenderInfo(info, x, y);
	else
		SetTlvert(x, y);

	m_BeZero = 1;

}
#endif

void	CPc :: RenderBody2(matrix &vtm, BOOL isBlur)
{

	vector3d	tv;
	tv.MatrixMult(m_pos, vtm);

	float		x, y, oow;
	g_renderer->ProjectVertex(m_pos, vtm, &x, &y, &oow);


	float	pixelRatio = g_renderer->GetPixelRatio(oow);
	m_lastPixelRatio = pixelRatio / g_avgPixelRatio;
	float	fSprShift = m_sprShift * m_zoom * g_renderer->GetScreenXFactor() * m_lastPixelRatio;
	float	zHover = g_zHover * m_lastPixelRatio;

	float	lmRIntensity = 1.0f;
	float	lmGIntensity = 1.0f;
	float	lmBIntensity = 1.0f;
	SetLmIntensity(lmRIntensity, lmGIntensity, lmBIntensity);
	DWORD	m_A = (m_sprArgb&0xFF000000)>>24, m_R = (m_sprArgb&0x00FF0000)>>16, m_G = (m_sprArgb&0x0000FF00)>>8, m_B = m_sprArgb&0x000000FF;
	float	m_fA = float(m_A)/255.f, m_fR = float(m_R)/255.f, m_fG = float(m_G)/255.f, m_fB = float(m_B)/255.f;
	m_fR *= lmRIntensity; m_fG *= lmGIntensity; m_fB *= lmBIntensity;

	RENDER_INFO_RECT	info = { g_renderer->m_width, g_renderer->m_height, 0, 0, oow };
	CImfRes *			imfRes = (CImfRes *) g_resMgr().Get(m_imfName.c_str(), 0);

	BOOL	bodyAndAccessoryIsExchanged = 0;
	BOOL	headLayerPassed = FALSE;
	BOOL	isInsideScreen = FALSE;

	int			drawCount = 0;
	int		layer = 0;
	int		layerQueried = layer;

	if (strlen(GetActNameList(layer))==0 || strlen(GetSprNameList(layer))==0)
		return;

	if (!NeedWeaponLayer(m_curAction) && layer>=5)
		return;

	CActRes *	actRes = GetActResList(layer);
	CSprRes *	sprRes;
	sprRes = GetSprResList(layer);
	int			clipIndex = GetClipIndex(layer);

	if (m_curMotion >= actRes->GetMotionCount(m_curAction))
	{
		return;
	}

	CMotion *	mot = actRes->GetMotion(m_curAction, m_curMotion);

#ifdef	SECONDJOB_SKILL2
	if (layer == 6 && mot->m_eventId != -1) {
		std::string eventName = actRes->GetEventName(mot->m_eventId);
		if (eventName.find(".wav") != -1) {
			strcpy(m_hitWaveName, eventName.c_str());
		}
		m_isPlayHitWave = TRUE;
	}
#endif

	for(int clipCnt = 0; clipCnt < mot->numClips; clipCnt++)
	{
		CSprClip *	sprClip = mot->GetClip(clipCnt);
		if ((unsigned int)sprClip->sprIndex == 0xffffffff)
			continue;


		DWORD	A = sprClip->a, R = sprClip->r, G = sprClip->g, B = sprClip->b;
		float	fA = float(A)/255.f, fR = float(R)/255.f, fG = float(G)/255.f, fB = float(B)/255.f;
		fA *= m_fA; fR *= m_fR; fG *= m_fG; fB *= m_fB;
		DWORD	argb = (DWORD(fA*255.f)<<24) + (DWORD(fR*255.f)<<16) + (DWORD(fG*255.f)<< 8) + DWORD(fB*255.f);

		float	zoomX = 1.0f * m_zoom;
		float	zoomY = 1.0f * m_zoom;
		float	top, btm, left, right;

		SprImg *	img = sprRes->m_sprites[sprClip->clipType][sprClip->sprIndex];

		top = y + double(sprClip->y) * pixelRatio * sprClip->zoomy * zoomY;
		btm = y + double(sprClip->y+img->height*(img->isHalfH+1)) * pixelRatio * sprClip->zoomy * zoomY + 1;

		if (sprClip->flags & 1)
		{
			left = x + double(sprClip->x+img->width*(img->isHalfW+1)) * pixelRatio * sprClip->zoomx * zoomX;
			right = x + double(sprClip->x) * pixelRatio * sprClip->zoomx * zoomX + 1;

			if (top >= g_renderer->m_height || btm <= 0 || right >= g_renderer->m_width || left <= 0)
				continue;
		}
		else
		{
			left = x + double(sprClip->x) * pixelRatio * sprClip->zoomx * zoomX;
			right = x + double(sprClip->x+img->width*(img->isHalfW+1)) * pixelRatio * sprClip->zoomx * zoomX + 1;

			if (top >= g_renderer->m_height || btm <= 0 || left >= g_renderer->m_width || right <= 0)
				continue;
		}
		isInsideScreen = TRUE;

		float	sprOow;
		float	sprOowup;
		if (m_alwaysTopLayer)
		{
			sprOow =	1.0f/NEARPLANE + g_zOffset*float(0) - g_zOffset*20 + g_zOffset*m_offsetOow;
			sprOowup =	1.0f/NEARPLANE + g_zOffset*float(0) - g_zOffset*20 + g_zOffset*m_offsetOow;
		} else
		{
			if (m_isLieOnGround)
			{
				sprOow =	GetShadowOow(btm, y, tv.z)+g_zOffset*float(drawCount+3+m_offsetOow)+zHover;
				sprOowup =	GetShadowOow(top, y, tv.z)+g_zOffset*float(drawCount+3+m_offsetOow)+zHover;
			} else
			{
				sprOow =	GetSprOow(btm, y, tv.z) + g_zOffset*float(drawCount+3+m_offsetOow) + zHover;
				sprOowup =	GetSprOow(top, y, tv.z) + g_zOffset*float(drawCount+3+m_offsetOow) + zHover;
			}
		}
		++drawCount;

		if (m_isSingleColor && m_colorOfSingleColor != 0xffff0000)
		{
			argb = m_colorOfSingleColor;
		}


		RPSprite	spr;
		spr.top			= top;
		spr.bottom		= btm;
		spr.left		= left + fSprShift;
		spr.right		= right + fSprShift;
		spr.oow			= sprOow;
		spr.oowup		= sprOowup;
		spr.spr			= img;
		spr.rgba		= argb;
		spr.angle		= sprClip->angle+m_sprAngle;

		if(layer==0 || layer==1)
		{
			if (m_shouldAddPickInfo) info.Update(spr);
		}

		SYSTEMPALETTE *		palette;

		if (m_bodyPalette)
		{
			CPaletteRes *	palRes = (CPaletteRes *)g_resMgr().Get(m_bodyPaletteName.c_str(), 0);
			palette = palRes->m_pal;
		}
		else
		{
			palette = sprRes->m_pal;
		}

		int		ret;
		SendMsg(NULL, AM_IS_HAVE_EFFECT, int(&ret), 0, 0);

		RenderSprite(spr, palette, RF_SPRITE);


	}
	if (isInsideScreen)
		SetRenderInfo(info, x, y);
	else
		SetTlvert(x, y);
}

void CPc :: SetSprNames(const std::vector<std::string>& sprNames)
{
	for(int i=0; i<PC_LAYER_SIZE+4; ++i) SetSprNameList(i, sprNames[i].c_str());
}

const std::vector<std::string> CPc :: GetSprNames()
{
	std::vector<std::string> sprNames(PC_LAYER_SIZE+4);

	for(int i=0; i<PC_LAYER_SIZE+4; ++i)
		if(m_sprResList[i]) sprNames[i] = m_sprResList[i]->GetName();
		else sprNames[i] = "";

	return sprNames;
}

void CPc :: SetActNames(const std::vector<std::string>& actNames)
{
	for(int i=0; i<PC_LAYER_SIZE+4; ++i) SetActNameList(i, actNames[i].c_str());
}

const std::vector<std::string> CPc :: GetActNames()
{
	std::vector<std::string> actNames(PC_LAYER_SIZE+4);

	for(int i=0; i<PC_LAYER_SIZE+4; ++i)
		if(m_actResList[i]) actNames[i] = m_actResList[i]->GetName();
		else actNames[i] = "";

	return actNames;
}

void CPc :: SetSprNameList(int layer, const char* sprName)
{
	if(m_sprResList[layer]) m_sprResList[layer]->Unlock();

	if(IsNameYellow(m_gid)) {
		if (IsEventAID(m_gid)) {
			switch(layer)
			{
			case 0:
				if(m_sex)
					m_sprResList[0] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xb3\xb2\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xb3\xb2.Spr", 0);
				else
					m_sprResList[0] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xbf\xa9\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xbf\xa9.Spr", 0);
				break;
			case 5:
				if(m_sex)
					m_sprResList[5] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xb3\xb2_\xb0\xcb.Spr", 0);
				else
					m_sprResList[5] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xbf\xa9_\xb0\xcb.Spr", 0);
				break;
			case 6:
				m_sprResList[6] = NULL;
				break;
			default:
				m_sprResList[layer] = (CSprRes *)g_resMgr().Get(sprName, 0);
				break;
			}
		} else {
			switch(layer)
			{
			case 0:
				if(m_sex)
					m_sprResList[0] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xb3\xb2\\\xbf\xee\xbf\xb5\xc0\xda_\xb3\xb2.Spr", 0);
				else
					m_sprResList[0] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xbf\xa9\\\xbf\xee\xbf\xb5\xc0\xda_\xbf\xa9.Spr", 0);
				break;
			case 5:
				if(m_sex)
					m_sprResList[5] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda_\xb3\xb2_\xb0\xcb.Spr", 0);
				else
					m_sprResList[5] = (CSprRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda_\xbf\xa9_\xb0\xcb.Spr", 0);
				break;
			case 6:
				m_sprResList[6] = NULL;
				break;
			default:
				m_sprResList[layer] = (CSprRes *)g_resMgr().Get(sprName, 0);
				break;
			}
		}
	}
	else
	{
		m_sprResList[layer] = (CSprRes *)g_resMgr().Get(sprName, 0);
	}

	if(m_sprResList[layer]) m_sprResList[layer]->Lock();
}

const char* CPc :: GetSprNameList(int layer)
{
	if (m_sprResList[layer]) return ((CRes*)m_sprResList[layer])->GetName();
	return "";
}

CSprRes* CPc :: GetSprResList(int layer)
{
	return m_sprResList[layer];
}

void CPc :: SetActNameList(int layer, const char* actName)
{
	if(m_actResList[layer]) m_actResList[layer]->Unlock();

	if (IsNameYellow(m_gid))
	{
		if (IsEventAID(m_gid))
		{
			switch(layer)
			{

			case 0:
				if(m_sex)
					m_actResList[0] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xb3\xb2\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xb3\xb2.Act", 0);
				else
					m_actResList[0] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xbf\xa9\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xbf\xa9.Act", 0);
				break;
			case 5:
				if(m_sex)
					m_actResList[5] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xb3\xb2_\xb0\xcb.Act", 0);
				else
					m_actResList[5] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda" "2_\xbf\xa9_\xb0\xcb.Act", 0);
				break;
			case 6:
				m_actResList[6] = NULL;
				break;
			default:
				m_actResList[layer] = (CActRes *)g_resMgr().Get(actName, 0);
				break;
			}
		} else {
			switch(layer)
			{

			case 0:
				if(m_sex)
					m_actResList[0] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xb3\xb2\\\xbf\xee\xbf\xb5\xc0\xda_\xb3\xb2.Act", 0);
				else
					m_actResList[0] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\\xbf\xa9\\\xbf\xee\xbf\xb5\xc0\xda_\xbf\xa9.Act", 0);
				break;
			case 5:
				if(m_sex)
					m_actResList[5] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda_\xb3\xb2_\xb0\xcb.Act", 0);
				else
					m_actResList[5] = (CActRes *)g_resMgr().Get("\xc0\xce\xb0\xa3\xc1\xb7\\\xbf\xee\xbf\xb5\xc0\xda\\\xbf\xee\xbf\xb5\xc0\xda_\xbf\xa9_\xb0\xcb.Act", 0);
				break;
			case 6:
				m_actResList[6] = NULL;
				break;
			default:
				m_actResList[layer] = (CActRes *)g_resMgr().Get(actName, 0);
				break;
			}
		}
	}
	else
	{
		m_actResList[layer] = (CActRes *)g_resMgr().Get(actName, 0);
	}

	if(m_actResList[layer]) m_actResList[layer]->Lock();
}


const char* CPc :: GetActNameList(int layer)
{
	if (m_actResList[layer]) return ((CRes*)m_actResList[layer])->GetName();
	return "";
}

CActRes* CPc :: GetActResList(int layer)
{
	return m_actResList[layer];
}

CSprRes* CPc :: GetIllusionSprResList(void)
{
	char tmpName[256];
	int IllusionJob;

	IllusionJob = JT_NOVICE;

	if (GetJob() == JT_KNIGHT || GetJob() == JT_CRUSADER || GetJob() == JT_KNIGHT_H || GetJob() == JT_CRUSADER_H)
		IllusionJob = JT_SWORDMAN;
	else if (GetJob() == JT_WIZARD || GetJob() == JT_SAGE || GetJob() == JT_WIZARD_H || GetJob() == JT_SAGE_H)
		IllusionJob = JT_MAGICIAN;
	else if (GetJob() == JT_BLACKSMITH || GetJob() == JT_ALCHEMIST || GetJob() == JT_BLACKSMITH_H || GetJob() == JT_ALCHEMIST_H)
		IllusionJob = JT_MERCHANT;
	else if (GetJob() == JT_HUNTER || GetJob() == JT_BARD || GetJob() == JT_HUNTER_H || GetJob() == JT_BARD_H || GetJob() == JT_DANCER || GetJob() == JT_DANCER_H)
		IllusionJob = JT_ARCHER;
	else if (GetJob() == JT_PRIEST || GetJob() == JT_MONK || GetJob() == JT_PRIEST_H || GetJob() == JT_MONK_H)
		IllusionJob = JT_ACOLYTE;
	else if (GetJob() == JT_ASSASSIN || GetJob() == JT_ROGUE || GetJob() == JT_ASSASSIN_H || GetJob() == JT_ROGUE_H)
		IllusionJob = JT_THIEF;
	else if (GetJob() == JT_STAR || GetJob() == JT_LINKER)
		IllusionJob = 96;
	else if (GetJob() == JT_COLLECTOR || GetJob() == JT_DEATHKNIGHT)
		IllusionJob = 100;

	sprintf(tmpName, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s.spr", g_session.m_newPcSexNameTable[m_sex], g_session.m_newPcJobNameTable[IllusionJob], g_session.m_pcSexNameTable[m_sex]);
	return (CSprRes *)g_resMgr().Get(tmpName, 0);
}

CActRes* CPc :: GetIllusionActResList(void)
{
	char tmpName[256];
	int IllusionJob;

	IllusionJob = JT_NOVICE;

	if (GetJob() == JT_KNIGHT || GetJob() == JT_CRUSADER || GetJob() == JT_KNIGHT_H || GetJob() == JT_CRUSADER_H)
		IllusionJob = JT_SWORDMAN;
	else if (GetJob() == JT_WIZARD || GetJob() == JT_SAGE || GetJob() == JT_WIZARD_H || GetJob() == JT_SAGE_H)
		IllusionJob = JT_MAGICIAN;
	else if (GetJob() == JT_BLACKSMITH || GetJob() == JT_ALCHEMIST || GetJob() == JT_BLACKSMITH_H || GetJob() == JT_ALCHEMIST_H)
		IllusionJob = JT_MERCHANT;
	else if (GetJob() == JT_HUNTER || GetJob() == JT_BARD || GetJob() == JT_HUNTER_H || GetJob() == JT_BARD_H || GetJob() == JT_DANCER || GetJob() == JT_DANCER_H)
		IllusionJob = JT_ARCHER;
	else if (GetJob() == JT_PRIEST || GetJob() == JT_MONK || GetJob() == JT_PRIEST_H || GetJob() == JT_MONK_H)
		IllusionJob = JT_ACOLYTE;
	else if (GetJob() == JT_ASSASSIN || GetJob() == JT_ROGUE || GetJob() == JT_ASSASSIN_H || GetJob() == JT_ROGUE_H)
		IllusionJob = JT_THIEF;
	else if (GetJob() == JT_STAR || GetJob() == JT_LINKER)
		IllusionJob = 96;
	else if (GetJob() == JT_COLLECTOR || GetJob() == JT_DEATHKNIGHT)
		IllusionJob = 100;

	sprintf(tmpName, "\xc0\xce\xb0\xa3\xc1\xb7\\\xb8\xf6\xc5\xeb\\%s\\%s%s.act", g_session.m_newPcSexNameTable[m_sex], g_session.m_newPcJobNameTable[IllusionJob], g_session.m_pcSexNameTable[m_sex]);

	return (CActRes *)g_resMgr().Get(tmpName, 0);
}

void	CPc :: SetSprWeapon2(int weapon)
{
	m_weapon = weapon;

	char	tmpName[256];
	int		tmpJob = GetJob();
	if (tmpJob > JOBMINUS)
		tmpJob -= JOBMINUS;

	if (IsEffectStateMarried(m_effectState) || IsEffectStateSanta(m_effectState) || IsEffectStateSummer(m_effectState))
		return;

	if (GetJob() >= JT_GANGSI && GetJob() <= JT_COLLECTOR)
		return;

	if (!m_weapon) {
		if (GetJob() >= JT_TAEKWON && GetJob() <= JT_STAR2) {
			sprintf(tmpName, PATH_HUMAN"%s%s_\xb9\xdf\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex]);
			SetActNameList(Layer_Weapon2, tmpName);
			sprintf(tmpName, PATH_HUMAN"%s%s_\xb9\xdf\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex]);
			SetSprNameList(Layer_Weapon2, tmpName);
		} else {
			SetActNameList(Layer_Weapon2, "");
			SetSprNameList(Layer_Weapon2, "");
		}
		SetActNameList(Layer_Weapon1, "");
		SetSprNameList(Layer_Weapon1, "");
		return;
	}

	int left = m_weapon & 0xffff;
	int right = (m_weapon >> 16) & 0xffff;

	if (GetJob() >= JT_TAEKWON && GetJob() <= JT_STAR2) {
		SetActNameList(Layer_Weapon1, "");
		SetSprNameList(Layer_Weapon1, "");
		sprintf(tmpName, PATH_HUMAN"%s%s_\xb9\xdf\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex]);
		SetActNameList(Layer_Weapon2, tmpName);
		sprintf(tmpName, PATH_HUMAN"%s%s_\xb9\xdf\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex]);
		SetSprNameList(Layer_Weapon2, tmpName);
		return;
	}

	int type;
	if (g_session.GetWeaponType(right) > 0) {
		type = MakeWeaponType(left, right);

		if ((GetJob() == JT_ASSASSIN || GetJob() == JT_ASSASSIN_H || GetJob() == JT_ASSASSIN_B) &&
			(type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE || type == 0))
			type = WEAPONTYPE_SWORD;

		sprintf(tmpName, PATH_HUMAN"%s%s%s.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
		SetActNameList(Layer_Weapon1, tmpName);
		sprintf(tmpName, PATH_HUMAN"%s%s%s.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
		SetSprNameList(Layer_Weapon1, tmpName);

		switch (type) {
		case WEAPONTYPE_SHORTSWORD:
		case WEAPONTYPE_SWORD:
		case WEAPONTYPE_TWOHANDSWORD:
		case WEAPONTYPE_SPEAR:
		case WEAPONTYPE_TWOHANDSPEAR:
		case WEAPONTYPE_AXE:
		case WEAPONTYPE_TWOHANDAXE:
		case WEAPONTYPE_CATARRH:
		case WPCLASS_GUN_HANDGUN:
		case WPCLASS_GUN_RIFLE:
		case WEAPONTYPE_SHORTSWORD_SHORTSWORD:
		case WEAPONTYPE_SWORD_SWORD:
		case WEAPONTYPE_AXE_AXE:
		case WEAPONTYPE_SHORTSWORD_SWORD:
		case WEAPONTYPE_SHORTSWORD_AXE:
		case WEAPONTYPE_SWORD_AXE:
			sprintf(tmpName, PATH_HUMAN"%s%s%s_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
			SetActNameList(Layer_Weapon2, tmpName);
			sprintf(tmpName, PATH_HUMAN"%s%s%s_\xb0\xcb\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
			SetSprNameList(Layer_Weapon2, tmpName);
			break;
		default:
			SetActNameList(Layer_Weapon2, "");
			SetSprNameList(Layer_Weapon2, "");
			break;
		}
		return;
	}

	sprintf(tmpName, PATH_HUMAN"%s%s_%d.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
	if (!g_resMgr().IsExist(tmpName)) {
		type = g_session.GetWeaponType(left);
		if (type == -1 || type == 0 || left == 1599) {
			SetActNameList(Layer_Weapon1, "");
			SetSprNameList(Layer_Weapon1, "");
			SetActNameList(Layer_Weapon2, "");
			SetSprNameList(Layer_Weapon2, "");
		} else {
			if ((GetJob() == JT_THIEF || GetJob() == JT_THIEF_H || GetJob() == JT_THIEF_B) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_AXE || type == WEAPONTYPE_TWOHANDAXE))
				type = WEAPONTYPE_SWORD;
			else if ((GetJob() == JT_ASSASSIN || GetJob() == JT_ROGUE) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE))
				type = WEAPONTYPE_SWORD;
			else if ((GetJob() == JT_ASSASSIN_H || GetJob() == JT_ROGUE_H) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE))
				type = WEAPONTYPE_SWORD;
			else if ((GetJob() == JT_ASSASSIN_B || GetJob() == JT_ROGUE_B) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE))
				type = WEAPONTYPE_SWORD;

			if ((GetJob() == JT_WIZARD || GetJob() == JT_SAGE || GetJob() == JT_MAGICIAN) && type == WEAPONTYPE_TWOHANDSPEAR)
				type = WEAPONTYPE_ROD;
			if ((GetJob() == JT_WIZARD_H || GetJob() == JT_SAGE_H || GetJob() == JT_MAGICIAN_H) && type == WEAPONTYPE_TWOHANDSPEAR)
				type = WEAPONTYPE_ROD;
			if ((GetJob() == JT_WIZARD_B || GetJob() == JT_SAGE_B || GetJob() == JT_MAGICIAN_B) && type == WEAPONTYPE_TWOHANDSPEAR)
				type = WEAPONTYPE_ROD;

			if (GetJob() == JT_ROGUE && type == WEAPONTYPE_AXE)
				type = WEAPONTYPE_SHORTSWORD;
			if (GetJob() == JT_ROGUE_H && type == WEAPONTYPE_AXE)
				type = WEAPONTYPE_SHORTSWORD;
			if (GetJob() == JT_ROGUE_B && type == WEAPONTYPE_AXE)
				type = WEAPONTYPE_SHORTSWORD;

			if ((GetJob() == JT_PRIEST || GetJob() == JT_PRIEST_H || GetJob() == JT_PRIEST_B) && type == WEAPONTYPE_KNUKLE) {
				SetActNameList(Layer_Weapon1, "");
				SetSprNameList(Layer_Weapon1, "");
				SetActNameList(Layer_Weapon2, "");
				SetSprNameList(Layer_Weapon2, "");
				return;
			}

			sprintf(tmpName, PATH_HUMAN"%s%s%s.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
			SetActNameList(Layer_Weapon1, tmpName);
			sprintf(tmpName, PATH_HUMAN"%s%s%s.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
			SetSprNameList(Layer_Weapon1, tmpName);
		}

		switch (type) {
		case WEAPONTYPE_SHORTSWORD:
		case WEAPONTYPE_SWORD:
		case WEAPONTYPE_TWOHANDSWORD:
		case WEAPONTYPE_SPEAR:
		case WEAPONTYPE_TWOHANDSPEAR:
		case WEAPONTYPE_AXE:
		case WEAPONTYPE_TWOHANDAXE:
		case WEAPONTYPE_CATARRH:
		case WPCLASS_GUN_HANDGUN:
		case WPCLASS_GUN_RIFLE:
		case WEAPONTYPE_SHORTSWORD_SHORTSWORD:
		case WEAPONTYPE_SWORD_SWORD:
		case WEAPONTYPE_AXE_AXE:
		case WEAPONTYPE_SHORTSWORD_SWORD:
		case WEAPONTYPE_SHORTSWORD_AXE:
		case WEAPONTYPE_SWORD_AXE:
			sprintf(tmpName, PATH_HUMAN"%s%s_%d_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
			if (!g_resMgr().IsExist(tmpName)) {
				sprintf(tmpName, PATH_HUMAN"%s%s%s_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
				SetActNameList(Layer_Weapon2, tmpName);
				sprintf(tmpName, PATH_HUMAN"%s%s%s_\xb0\xcb\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
				SetSprNameList(Layer_Weapon2, tmpName);
			} else {
				sprintf(tmpName, PATH_HUMAN"%s%s_%d_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
				SetActNameList(Layer_Weapon2, tmpName);
				sprintf(tmpName, PATH_HUMAN"%s%s_%d_\xb0\xcb\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
				SetSprNameList(Layer_Weapon2, tmpName);
			}
			break;
		default:
			SetActNameList(Layer_Weapon2, "");
			SetSprNameList(Layer_Weapon2, "");
			break;
		}
		return;
	}

	{
		SetActNameList(Layer_Weapon1, tmpName);
		sprintf(tmpName, PATH_HUMAN"%s%s_%d.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
		SetSprNameList(Layer_Weapon1, tmpName);

		type = g_session.GetWeaponType(m_weapon);

		if ((GetJob() == JT_WIZARD || GetJob() == JT_SAGE || GetJob() == JT_MAGICIAN) && type == WEAPONTYPE_TWOHANDSPEAR)
			type = WEAPONTYPE_ROD;
		if ((GetJob() == JT_WIZARD_H || GetJob() == JT_SAGE_H || GetJob() == JT_MAGICIAN_H) && type == WEAPONTYPE_TWOHANDSPEAR)
			type = WEAPONTYPE_ROD;
		if ((GetJob() == JT_WIZARD_B || GetJob() == JT_SAGE_B || GetJob() == JT_MAGICIAN_B) && type == WEAPONTYPE_TWOHANDSPEAR)
			type = WEAPONTYPE_ROD;

		if ((GetJob() == JT_THIEF || GetJob() == JT_THIEF_H || GetJob() == JT_THIEF_B) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_AXE || type == WEAPONTYPE_TWOHANDAXE))
			type = WEAPONTYPE_SWORD;
		else if ((GetJob() == JT_ASSASSIN || GetJob() == JT_ROGUE) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE))
			type = WEAPONTYPE_SWORD;
		else if ((GetJob() == JT_ASSASSIN_H || GetJob() == JT_ROGUE_H) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE))
			type = WEAPONTYPE_SWORD;
		else if ((GetJob() == JT_ASSASSIN_B || GetJob() == JT_ROGUE_B) && (type == WEAPONTYPE_MACE || type == WEAPONTYPE_TWOHANDAXE))
			type = WEAPONTYPE_SWORD;

		if (GetJob() == JT_ROGUE && type == WEAPONTYPE_AXE)
			type = WEAPONTYPE_SHORTSWORD;
		if (GetJob() == JT_ROGUE_H && type == WEAPONTYPE_AXE)
			type = WEAPONTYPE_SHORTSWORD;
		if (GetJob() == JT_ROGUE_B && type == WEAPONTYPE_AXE)
			type = WEAPONTYPE_SHORTSWORD;

		switch (type) {
		case WEAPONTYPE_SHORTSWORD:
		case WEAPONTYPE_SWORD:
		case WEAPONTYPE_TWOHANDSWORD:
		case WEAPONTYPE_SPEAR:
		case WEAPONTYPE_TWOHANDSPEAR:
		case WEAPONTYPE_AXE:
		case WEAPONTYPE_TWOHANDAXE:
		case WEAPONTYPE_CATARRH:
		case WPCLASS_GUN_HANDGUN:
		case WPCLASS_GUN_RIFLE:
		case WEAPONTYPE_SHORTSWORD_SHORTSWORD:
		case WEAPONTYPE_SWORD_SWORD:
		case WEAPONTYPE_AXE_AXE:
		case WEAPONTYPE_SHORTSWORD_SWORD:
		case WEAPONTYPE_SHORTSWORD_AXE:
		case WEAPONTYPE_SWORD_AXE:
			sprintf(tmpName, PATH_HUMAN"%s%s_%d_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
			if (!g_resMgr().IsExist(tmpName)) {
				sprintf(tmpName, PATH_HUMAN"%s%s%s_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
				SetActNameList(Layer_Weapon2, tmpName);
				sprintf(tmpName, PATH_HUMAN"%s%s%s_\xb0\xcb\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], g_session.m_pcWeaponNameTable[type]);
				SetSprNameList(Layer_Weapon2, tmpName);
			} else {
				sprintf(tmpName, PATH_HUMAN"%s%s_%d_\xb0\xcb\xb1\xa4.act", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
				SetActNameList(Layer_Weapon2, tmpName);
				sprintf(tmpName, PATH_HUMAN"%s%s_%d_\xb0\xcb\xb1\xa4.spr", g_session.m_pcJobNameTable[tmpJob], g_session.m_pcSexNameTable[m_sex], left);
				SetSprNameList(Layer_Weapon2, tmpName);
			}
			break;
		default:
			SetActNameList(Layer_Weapon2, "");
			SetSprNameList(Layer_Weapon2, "");
			break;
		}
	}
}

int	CPc :: MakeWeaponType(int left, int right)
{
	int type = 0;
	if (!left && right)
	{
		left = right;
		right = 0;
	}

	if ((left >= 1100 && left < 1150) || (left >= 13400 && left < 13500))
	{
		type = 2;
		if (right >= 1100 && right < 1150)
			return 26;

		if (right >= 13400 && right < 13500)
			return 26;

		if (right >= 1300 && right < 1350)
			return 30;
	}
	else if (left >= 1200 && left < 1250)
	{
		type = 1;
		if (right >= 1200 && right < 1250)
			return 25;

		if (right >= 13000 && right < 13100)
			return 25;

		if (right >= 1100 && right < 1150)
			return 28;

		if (right >= 13400 && right < 13500)
			return 28;

		if (right >= 1300 && right < 1350)
			return 29;
	}
	else if (left >= 1300 && left < 1350)
	{
		type = 6;
		if (right >= 1300 && right < 1350)
			return 27;
	}
	else if (left >= 13000 && left < 13100)
	{
		type = 1;
		if (right >= 1200 && right < 1250)
			return 25;

		if (right >= 13000 && right < 13100)
			return 25;

		if (right >= 1100 && right < 1150)
			return 28;

		if (right >= 13400 && right < 13500)
			return 28;

		if (right >= 1300 && right < 1350)
			return 29;
	}

	return type;
}

CGrannyPc::CGrannyPc()
{
	int i, j;

	m_GrannyActorRes = NULL;
	m_Instance = NULL;
	m_isFirstProcess = TRUE;
	m_nUpdateAniFlag = 0;

	for (i = 0; i < 30; i++) {
		m_matVer[i] = NULL;
		m_rp[i] = NULL;
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 30; j++) {
			m_pTex[i][j] = NULL;
			m_pFaceArr[i][j] = NULL;
			m_nIndexNo[i][j] = 0;
		}
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 30; j++) {
			m_rpPart[i][j] = NULL;
			m_matVerPart[i][j] = NULL;
		}
	}

	for (i = 0; i < 10; i++) m_GrannyPartRes[i] = NULL;

	m_RenderAlpha = 0xff;
	m_nLastActAnimation = 0;
	m_nRenderType = 0;
	m_pCellTex = NULL;
	for (i = 0; i < 20; i++)
		m_Control[i] = NULL;

	OnInit();
}


CGrannyPc::~CGrannyPc()
{
	FreeRes();
	OnExit();
}

void CGrannyPc::FreeRes()
{
	int i;
	for (i = 0; i < 20; i++) {
		if (m_Control[i])
			m_Control[i] = NULL;
	}
	for (i = 0; i < 30; i++) {
		if (m_rp[i]) {
			delete m_rp[i];
			m_rp[i] = NULL;
			delete m_matVer[i];
			m_matVer[i] = NULL;
		}
	}
	for (int p = 0; p < 10; p++) {
		for (int j = 0; j < 30; j++) {
			if (m_rpPart[p][j]) {
				delete m_rpPart[p][j];
				m_rpPart[p][j] = NULL;
				delete m_matVerPart[p][j];
				m_matVerPart[p][j] = NULL;
			}
		}
	}
	m_pWorldPose = NULL;
}

void CGrannyPc::OnInit()
{
	m_gage = NULL;
	m_headPalette = 0;
	m_shield = 0;
	m_honor = 0;
	m_virtue = 0;
	m_gdid = 0;
	m_emblemVersion = 0;
	m_headDir = 0;
	m_renderWithoutLayer = FALSE;
	m_pk_rank = 0;
	m_pk_total = 0;
	m_isPc = TRUE;
	m_shadowTex = g_texMgr.CreateTexture(35UL, 35UL, PF_A4R4G4B4, NULL);
	m_shadowDotList.resize(4900);
}

void	CGrannyPc :: OnExit()
{
	if (m_gage) {
		g_windowMgr.PostQuit(m_gage);
		m_gage = NULL;
	}
	if (m_shadowTex) delete m_shadowTex;
}

void	CGrannyPc :: SetSprJob(int job)
{
	m_shadowZoom = g_session.GetShadowFactorWithJob(job);
}

void CGrannyPc::SetSprHead(int head) { m_head = head; }

void CGrannyPc::SetSprJobDye(int job) {}

void CGrannyPc::SetSprAccessory(int accessory) { m_accessory = accessory; }

void CGrannyPc::SetSprAccessory2(int accessory2) { m_accessory2 = accessory2; }

void CGrannyPc::SetSprAccessory3(int accessory3) { m_accessory3 = accessory3; }

void CGrannyPc::SetSprWeapon(int weapon) { m_weapon = weapon; }

void	CGrannyPc :: SetSprShield(int shield)
{
	GetJob();
}

void	CGrannyPc :: SetImfFileName()
{
	char	tmpName[256];
	m_imfName = g_session.GetImfName(GetJob(), m_head, m_sex, tmpName);
}

void	CGrannyPc :: SetHeadPaletteName()
{
	if (m_headPalette) {
		char	palName[64];
		m_headPaletteName = g_session.GetHeadPaletteName(m_head, GetJob(), m_sex, m_headPalette, palName);
	}
}

void	CGrannyPc :: SetSprAct(int job, int sex, int head, int weapon, int accessory, int accessory2, int accessory3, int shield, int headPal, int bodyPal)
{
	SetSex(sex);
	m_head = head;
	m_weapon = weapon;
	m_accessory = accessory;
	m_accessory2 = accessory2;
	m_accessory3 = accessory3;
	m_shield = shield;
	m_headPalette = headPal;
	m_bodyPalette = bodyPal;

	SetJob(job);
	SetSprHead(m_head);
	SetSprWeapon(m_weapon);
	SetSprAccessory(m_accessory);
	SetSprAccessory2(m_accessory2);
	SetSprAccessory3(m_accessory3);
	SetSprShield(m_shield);
	SetImfFileName();
	SetHeadPaletteName();

	if (m_GrannyActorRes)
		FreeRes();

	strcpy(m_strJobFn, "SWM@hd\xb1\xe2\xba\xbb_3.gr2");
	m_GrannyActorRes = (C3dGrannyModelRes *)g_resMgr().Get(m_strJobFn, false);
	m_Instance = m_GrannyActorRes->GetModelInstance();

	int i;
	for (i = 0; i < m_GrannyActorRes->GetMeshNo(); i++) {
		m_rp[i] = new RPMesh;
		m_rp[i]->AllocVerts(m_GrannyActorRes->GetMeshIndexCount(i));
		m_matVer[i] = (granny_pnt332_vertex *)operator new(m_GrannyActorRes->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
		memset(m_matVer[i], 0, m_GrannyActorRes->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
	}

	m_fLastAniCnt = GrannyGetSystemSeconds();
	m_GameClock = 0;
	m_actorType = 0;
	char *str;
	if ((str = strstr(m_strJobFn, "@hd")) != NULL) {
		str[2] = 'r';
		str[1] = 'h';
		char str2[260];
		strcpy(str2, str + 1);
		i = 0;
		while (str2[i] != '_')
			i++;
		str2[i + 1] = '9';
		str2[i + 2] = '9';
		str2[i + 3] = 0;
		strcat(str2, ".gr2");

		m_GrannyPartRes[1] = (C3dGrannyModelRes *)g_resMgr().Get(str2, false);
		for (i = 0; i < m_GrannyPartRes[1]->GetMeshNo(); i++) {
			m_rpPart[1][i] = new RPMesh;
			m_rpPart[1][i]->AllocVerts(m_GrannyPartRes[1]->GetMeshIndexCount(i));
			m_matVerPart[1][i] = (granny_pnt332_vertex *)operator new(m_GrannyPartRes[1]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
			memset(m_matVerPart[1][i], 0, m_GrannyPartRes[1]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
		}

		str[1] = 'b';
		str[2] = 'd';
		m_GrannyPartRes[2] = (C3dGrannyModelRes *)g_resMgr().Get(m_strJobFn, false);
		m_GrannyPartRes[2]->BindMeshToSkeleton(m_GrannyActorRes);
		for (i = 0; i < m_GrannyPartRes[2]->GetMeshNo(); i++) {
			m_rpPart[2][i] = new RPMesh;
			m_rpPart[2][i]->AllocVerts(m_GrannyPartRes[2]->GetMeshIndexCount(i));
			m_matVerPart[2][i] = (granny_pnt332_vertex *)operator new(m_GrannyPartRes[2]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
			memset(m_matVerPart[2][i], 0, m_GrannyPartRes[2]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
		}

		str[1] = 'p';
		str[2] = 't';
		m_GrannyPartRes[3] = (C3dGrannyModelRes *)g_resMgr().Get(m_strJobFn, false);
		m_GrannyPartRes[3]->BindMeshToSkeleton(m_GrannyActorRes);
		for (i = 0; i < m_GrannyPartRes[3]->GetMeshNo(); i++) {
			m_rpPart[3][i] = new RPMesh;
			m_rpPart[3][i]->AllocVerts(m_GrannyPartRes[3]->GetMeshIndexCount(i));
			m_matVerPart[3][i] = (granny_pnt332_vertex *)operator new(m_GrannyPartRes[3]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
			memset(m_matVerPart[3][i], 0, m_GrannyPartRes[3]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
		}

		str[1] = 'l';
		str[2] = 'e';
		m_GrannyPartRes[4] = (C3dGrannyModelRes *)g_resMgr().Get(m_strJobFn, false);
		m_GrannyPartRes[4]->BindMeshToSkeleton(m_GrannyActorRes);
		for (i = 0; i < m_GrannyPartRes[4]->GetMeshNo(); i++) {
			m_rpPart[4][i] = new RPMesh;
			m_rpPart[4][i]->AllocVerts(m_GrannyPartRes[4]->GetMeshIndexCount(i));
			m_matVerPart[4][i] = (granny_pnt332_vertex *)operator new(m_GrannyPartRes[4]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
			memset(m_matVerPart[4][i], 0, m_GrannyPartRes[4]->GetMeshIndexCount(i) * 3 * sizeof(granny_pnt332_vertex));
		}

		if (m_GrannyPartRes[1]) {
			m_GrannyActorRes->SetHairAndHeadMesh();
			m_hair.nPartMeshIdx = 1;
		}
	}
}

void CGrannyPc::SetSprAct2(int job, int sex, int head, int weapon, int accessory, int accessory2, int accessory3, int shield, int headPal, int bodyPal, int effectState)
{
	SetSprAct(job, sex, head, weapon, accessory, accessory2, accessory3, shield, headPal, bodyPal);
}

bool	CGrannyPc :: OnProcess()
{
	ProcessEffectState();
	ProcessWillBeAttacked();
	ProcessChatBalloon();
	ProcessGage();
	if (m_isSingleColor) {
		if (timeGetTime() > m_singleColorStartTick + 100) {
			m_colorOfSingleColor = 0xffff0000;
			m_isSingleColor = false;
		}
	}
	if (m_sprArgb != 0xffffffff)
		ModifyArgb(8, 8, 8, 8);
	bool result = ProcessState();
	ProcessSound();
	if (m_isPlayHitWave) {
		CGameMode* gameMode = g_modeMgr.GetGameMode();
		vector3d& pPos = gameMode->GetWorld()->m_player->m_pos;
		PlayWave(m_hitWaveName, m_pos.x - pPos.x, 0, m_pos.z - pPos.z);
		m_isPlayHitWave = FALSE;
	}
	if (m_roty > 360.0f) m_roty = (int)m_roty % 360;
	if (m_isFirstProcess == 1) {
		m_fCurRot = 180.0f - m_roty;
		m_isFirstProcess = false;
	} else {
		int roty = m_roty;
		int targetRot = 180.0f - roty;
		int rotDiff = m_fCurRot - targetRot;
		if (rotDiff > 360 || rotDiff < -360) rotDiff %= 360;
		if (rotDiff < 0) rotDiff += 360;
		if (rotDiff > 16) {
			if (rotDiff < 180)
				m_fCurRot -= 16.0f;
			else
				m_fCurRot += 16.0f;
		}
	}
	if (m_fCurRot > 360.0f) m_fCurRot = (int)m_fCurRot % 360;
	if (m_fCurRot < 0.0f) m_fCurRot += 360.0f;
	return result;
}

bool	CGrannyPc :: ProcessState()
{
	if (m_freezeEndTick) {
		if (timeGetTime() > m_freezeEndTick) {
			m_isMotionFreezed = false;
			m_freezeEndTick = 0;
		}
	}

	if (!ProcessVanish()) {
		return	false;
	}
	CGameMode *		gameMode = g_modeMgr.GetGameMode();
	sAssert(gameMode);
	switch (m_stateId) {
		case	8: {
			ProcessMotion();
			float stateCnt = (timeGetTime() - m_stateStartTick) / 24.0f;
			if (m_targetGid && int(stateCnt)%34==0) {
				CGameMode *		gameMode = g_modeMgr.GetGameMode();
				sAssert(gameMode);
				CGameActor *	tActor = gameMode->GetWorld()->GetGameActorByAID(m_targetGid);
				if (tActor) {
					CalcDir(tActor->m_pos.x, tActor->m_pos.z);
				}
			}
			break;
		}
		case	7:
		{
			ProcessMotion();
			if (m_isMotionFinished)
			SetState(ST_STAND);
			break;
		}
		case	2:
		{
			ProcessMotion();
			float stateCnt = (timeGetTime() - m_stateStartTick) / 24.0f;
			if (m_isMotionFinished)
				SetState(ST_STAND);
			break;

		}
		case	6:
			ProcessMotion();
			break;
		case	3:
#ifdef		SECONDJOB_SKILL2
			m_isForceState = false;
			m_isForceState2 = false;
			m_isForceState3 = false;
#endif
			m_isMotionFreezed = false;
			ProcessMotion();
			break;
		default:
			return	CGameActor::ProcessState();
			break;
	}
	return	true;
}

void	CGrannyPc :: SetState(int stateId)
{
	if (m_stateId == ST_DEAD || m_isTrickDead)
		return;

	if (m_stateId == ST_DAMAGE) {
		m_pos.x = m_damageDestX;
		m_pos.z = m_damageDestZ;
	}

	int prevStateId = m_stateId;
	switch (stateId) {
	case ST_BEGIN_SKILL:
		RegisterPos();
		SetAction(96, 6, MT_ONETIME);
		m_isMotionFinished = true;
		break;

	case ST_SIT:
		RegisterPos();
		SetAction(16, 4, MT_ONETIME);
		m_isMotionFinished = true;
		m_curMotion = m_headDir;
		break;

	case ST_MOVE:
		m_isCounter = FALSE;
		if (m_bodyState != BODYSTATE_STONECURSE && m_bodyState != BODYSTATE_FREEZING)
			m_isMotionFreezed = false;
		m_headDir = 0;
		CGameActor::SetState(ST_MOVE);
		break;

	case ST_STAND:
		CGameActor::SetState(ST_STAND);
		m_curMotion = m_headDir;
		if (prevStateId == ST_DAMAGE || prevStateId == ST_ATTACK || m_pkState || prevStateId == ST_BEGIN_SKILL)
			SetAction(32, 4, MT_LOOP);
		g_modeMgr.GetGameMode()->SetWaitUseItemAck(FALSE);
		break;

	case ST_SKILL:
		if (m_stateId == ST_SIT)
			return;

		if (GetJob() == JT_BARD || GetJob() == JT_DANCER || GetJob() == JT_CRUSADER) {
			SetAction(32, 6, MT_ONETIME);
		} else if (GetJob() == JT_BARD_H || GetJob() == JT_DANCER_H || GetJob() == JT_CRUSADER_H) {
			SetAction(32, 6, MT_ONETIME);
		} else if (GetJob() == JT_BARD_B || GetJob() == JT_DANCER_B || GetJob() == JT_CRUSADER_B) {
			SetAction(32, 6, MT_ONETIME);
		} else if (GetJob() == JT_MONK || GetJob() == JT_MONK_H || GetJob() == JT_MONK_B) {
			if (m_sex == 0) {
				SetAction(32, 6, MT_ONETIME);
			} else {
				SetAction(96, 6, MT_ONETIME);
				SetForceAnimSpeed(4);
				SetForceAnimation(96, 0, 0, 96, 0, TRUE, 400, FALSE);
			}
		} else {
			SetAction(96, 6, MT_ONETIME);
		}
		m_attackMotion = 6;
		break;

	case ST_ATTACK:
	{
		m_isCounter = FALSE;
		if (m_bodyState != BODYSTATE_STONECURSE && m_bodyState != BODYSTATE_FREEZING)
			m_isMotionFreezed = false;

		bool isSecondAttack = g_session.IsSecondAttack(GetJob(), m_sex, m_weapon);
		if (isSecondAttack)
			CGameActor::SetState(ST_ATTACK2);
		else
			CGameActor::SetState(ST_ATTACK);
		m_attackMotion = g_session.GetPCAttackMotion(GetJob(), m_sex, m_weapon, isSecondAttack);

		if (m_weapon == WEAPONTYPE_BOW) {
			g_modeMgr.GetGameMode()->MakeArrowEffect(m_pos, m_targetGid, m_attackMotion * m_motionSpeed, 0);
			m_attackMotion += 8 / m_motionSpeed;
		}
		break;
	}

	case ST_ATTACK2:
	{
		m_isCounter = FALSE;
		if (m_bodyState != BODYSTATE_STONECURSE && m_bodyState != BODYSTATE_FREEZING)
			m_isMotionFreezed = false;

		bool isSecondAttack = g_session.IsSecondAttack(GetJob(), m_sex, m_weapon);
		if (isSecondAttack)
			CGameActor::SetState(ST_ATTACK2);
		else
			CGameActor::SetState(ST_ATTACK);
		m_attackMotion = g_session.GetPCAttackMotion(GetJob(), m_sex, m_weapon, isSecondAttack);

		if (m_weapon == WEAPONTYPE_WHIP || m_weapon == WEAPONTYPE_INSTRUMENT) {
			g_modeMgr.GetGameMode()->MakeArrowEffect(m_pos, m_targetGid, m_attackMotion * m_motionSpeed, 0);
			m_attackMotion += 8 / m_motionSpeed;
		}
		break;
	}

	default:
		CGameActor::SetState(stateId);
		if (m_stateId == ST_DEAD)
			SetGrannyAction(MT_ONETIME);
		return;
	}

	m_curAction = m_baseAction + Get8Dir(m_roty);
	m_stateId = stateId;
	m_stateStartTick = timeGetTime();
}

void	CGrannyPc :: ProcessGage()
{
	if (m_gage) {
		m_gage->Move(m_lastTlvertX - 30, (int)((float)(g_renderer->m_height * 12 / 480) * m_lastPixelRatio + m_lastTlvertY));
	}
}

void	CGrannyPc :: SetModifyFactorOfmotionSpeed(int attackMT)
{
	if (m_weapon != 0xb) {
		if (attackMT > MAX_ATTACKMT)
			attackMT = MAX_ATTACKMT;
	}
	m_modifyFactorOfmotionSpeed = (float)attackMT / 432.0f;
}

void	CGrannyPc :: SetHonor(int honor)
{
	if (honor < 0)
		SendMsg(NULL, AM_EMOTION, 0x22, 0, 0);
	else {
		SendMsg(NULL, AM_DELETE_PROHIBITEFFECT, 0, 0, 0);
	}

	m_honor = honor;
}

void	CGrannyPc :: SetPropensityInfo(int honor, int virtue)
{
	SetHonor(honor);
	m_virtue = virtue;
}

void	CGrannyPc :: SendMsg(CGameObject *sender, int message, int arg1, int arg2, int arg3)
{
	CGameMode *		gameMode = g_modeMgr.GetGameMode();
	sAssert(gameMode);

	switch (message) {
		case	AM_SET_HEAD_DIR: {
			sAssert(arg1>=0 && arg1<=2);
			if (m_stateId == ST_STAND || m_stateId == ST_SIT) {
				m_curMotion = arg1;
			}
			break;
		}
		case	AM_REMOVEGAGE: {
			if (m_gage) {
				g_windowMgr.PostQuit(m_gage);
				m_gage = NULL;
			}
			break;
		}
		case	AM_UPDATEGAGE: {
			if (!m_gage) {
				m_gage = new UIPcGage();
				m_gage->Create(60, 5);
				g_windowMgr.AddWindowFront(m_gage);
			}
			m_gage->SetHp(arg1, arg2);
			m_gage->SetSp(0, 0);
			m_gage->Move(m_lastTlvertX - 30, g_renderer->m_height * 12 / 480 + m_lastTlvertY);
#ifdef _ARTICLE_BLOOD_DIS
			m_ActorTitle->SetHp(arg1, arg2);
#endif
			break;
		}
		case	AM_STANDENTRY:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;
			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);
			SetSprAct(
				s->job, s->sex, s->head,
				s->weapon,
				s->accessory,
				s->accessory2,
				s->accessory3,
				s->shield,
				s->headpalette, s->bodypalette);

			SetAttrState(s->bodyState, s->effectState, s->healthState);

			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					SetState(ST_STAND);
					break;
			}
			break;
			MakePartyEffect(s->GID);
		}
		case	AM_STANDENTRY2:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;
			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);
			SetSprAct(s->job, s->sex, s->head, s->weapon, s->accessory, s->accessory2, s->accessory3, s->shield, s->headpalette, s->bodypalette);
			SetAttrState(s->bodyState, s->effectState, s->healthState);
			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					SetState(ST_STAND);
					break;
			}
			break;
			MakePartyEffect(s->GID);
		}
		case	AM_MAKE_CHICKEN: {
			SetSprJob(GetJob());
			SetImfFileName();
			SetSprWeapon(GetWeapon());
			SetSprShield(m_shield);
			SetHeadPaletteName();
			SetBodyPaletteName();
		}
		break;
		case	AM_DELETE_CHICKEN: {
			SetSprJob(GetJob());
			SetImfFileName();
			SetSprWeapon(GetWeapon());
			SetSprShield(m_shield);
			SetHeadPaletteName();
			SetBodyPaletteName();
		}
		break;
		case	AM_INITSTANDENTRY:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			m_gid = s->GID;
			CGameMode *			gameMode = g_modeMgr.GetGameMode();
			sAssert(gameMode);

			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_speed = s->speed;
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;

			sAssert(m_headDir>=0 && m_headDir<=2);

			s->job = CGameActor::GetProperJob(s->job, s->effectState);

			SetSprAct(s->job, s->sex, s->head,
				s->weapon,
				s->accessory,
				s->accessory2,
				s->accessory3,
				s->shield,
				s->headpalette, s->bodypalette);

			SetAttrState(s->bodyState, s->effectState, s->healthState);
			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					sAssert(false);
					Trace("UNKNOWN PCSTATE");
					SetState(ST_STAND);
					break;
			}

			if (arg2 == 2 && !IsEffectStateSpecialHiding(m_effectState)) {
#ifdef SECONDJOB_SKILL2
				LaunchEffect(EF_ENTRY2);
#else
				LaunchEffect(EF_ENTRY);
#endif
			}
			MakePartyEffect(s->GID);
			break;
		}
		case	AM_INITSTANDENTRY2:
			{
			STANDENTRYSTRUCT *	s = (STANDENTRYSTRUCT *)arg1;
			m_gid = s->GID;
			CGameMode *			gameMode = g_modeMgr.GetGameMode();
			sAssert(gameMode);

			gameMode->GetClientCoor(s->x, s->y, m_pos.x, m_pos.z);
			CWorld *	world = gameMode->GetWorld();
			m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);
			m_roty = DirToDeg(s->dir);
			m_speed = s->speed;
			m_xSize = s->xSize;
			m_ySize = s->ySize;
			m_moveDestX = s->x;
			m_moveDestY = s->y;
			m_headDir = s->headDir;
			m_headType = s->head;

			s->accessory2;
			s->accessory3;

			s->job = CGameActor::GetProperJob(s->job, s->effectState);
			SetSprAct(s->job, s->sex, s->head, s->weapon, s->accessory, s->accessory2, s->accessory3, s->shield, s->headpalette, s->bodypalette);
			SetAttrState(s->bodyState, s->effectState, s->healthState);
			SetPKState(s->isPKModeON);
			SetGuildInfo(s->guildId, s->guildEmblemVersion);
			SetPropensityInfo(s->honor, s->virtue);
			switch (s->state) {
				case	PCSTATE_STAND:
					SetState(ST_STAND);
					break;
				case	PCSTATE_DEAD:
					SetState(ST_DEAD);
					break;
				case	PCSTATE_SIT:
					SetState(ST_SIT);
					break;
				default:
					sAssert(false);
					Trace("UNKNOWN PCSTATE");
					SetState(ST_STAND);
					break;
			}

			if (arg2 == 2 && !IsEffectStateSpecialHiding(m_effectState)) {
#ifdef SECONDJOB_SKILL2
				LaunchEffect(EF_ENTRY2);
#else
				LaunchEffect(EF_ENTRY);
#endif
			}
			MakePartyEffect(s->GID);
			break;
		}
		case	AM_INITMOVEENTRY: {
				MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
				m_gid = ms->GID;
				m_speed = ms->speed;
				m_moveStartTime = ms->moveStartTime;

				m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

				ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);

				SetSprAct(
					ms->job, ms->sex, ms->head,
					ms->weapon,
					ms->accessory,
					ms->accessory2,
					ms->accessory3,
					ms->shield,
					ms->headpalette, ms->bodypalette);

				SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
				SetPKState(ms->isPKModeON);
				SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
				SetPropensityInfo(ms->honor, ms->virtue);

				gameMode->GetClientCoor(ms->sx, ms->sy, m_pos.x, m_pos.z);
				m_pos.x += (ms->cellX-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				m_pos.z += (ms->cellY-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				CWorld *	world = gameMode->GetWorld();
				m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);

				bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);

				if (result) {
					m_moveDestX = ms->dx;
					m_moveDestY = ms->dy;
					SetState(ST_MOVE);
					m_pathStartCell = 0;
				} else
					SetState(ST_STAND);
				MakePartyEffect(ms->GID);
			}
			break;
		case	AM_INITMOVEENTRY2: {
				MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
				m_gid = ms->GID;
				m_speed = ms->speed;
				m_moveStartTime = ms->moveStartTime;
				m_effectState = ms->effectState;
				m_healthState = ms->healthState;
				m_bodyState = ms->bodyState;

				m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

				ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);
				SetSprAct(
					ms->job, ms->sex, ms->head,
					ms->weapon,
					ms->accessory,
					ms->accessory2,
					ms->accessory3,
					ms->shield,
					ms->headpalette, ms->bodypalette);

				SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
				SetPKState(ms->isPKModeON);
				SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
				SetPropensityInfo(ms->honor, ms->virtue);

				gameMode->GetClientCoor(ms->sx, ms->sy, m_pos.x, m_pos.z);
				m_pos.x += (ms->cellX-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				m_pos.z += (ms->cellY-8) * (float(gameMode->GetWorld()->m_attr->m_zoom)/16.0f);
				CWorld *	world = gameMode->GetWorld();
				m_pos.y = world->m_attr->GetHeight(m_pos.x, m_pos.z);

				bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);

				if (result)
				{
					m_moveDestX = ms->dx;
					m_moveDestY = ms->dy;
					SetState(ST_MOVE);
					m_pathStartCell = 0;
				} else
					SetState(ST_STAND);
				MakePartyEffect(ms->GID);
			}
			break;
		case	AM_MAKE_BLUR_ACTOR: {
			}
			break;
		case	AM_MOVEENTRY: {
			MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
			m_gid = ms->GID;
			m_speed = ms->speed;
			m_moveStartTime = ms->moveStartTime;

			m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

			ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);
			SetSprAct(ms->job, ms->sex, ms->head, ms->weapon, ms->accessory, ms->accessory2, ms->accessory3, ms->shield, ms->headpalette, ms->bodypalette);
			SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
			SetPKState(ms->isPKModeON);
			SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
			SetPropensityInfo(ms->honor, ms->virtue);

			bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);
			if (result) {
				m_moveDestX = ms->dx;
				m_moveDestY = ms->dy;
				SetState(ST_MOVE);
				m_pathStartCell = 0;
			}

			MakePartyEffect(ms->GID);
			break;
		}
		case	AM_MOVEENTRY2: {
			MOVEENTRYSTRUCT *	ms = (MOVEENTRYSTRUCT *)arg1;
			m_gid = ms->GID;
			m_speed = ms->speed;
			m_moveStartTime = ms->moveStartTime;

			m_headDir = HEADER_ANIM_FRAME_NUM*HEADERDIR_DEFAULT;

			ms->job = CGameActor::GetProperJob(ms->job, ms->effectState);
			SetSprAct(ms->job, ms->sex, ms->head, ms->weapon, ms->accessory, ms->accessory2, ms->accessory3, ms->shield, ms->headpalette, ms->bodypalette);
			SetAttrState(ms->bodyState, ms->effectState, ms->healthState);
			SetPKState(ms->isPKModeON);
			SetGuildInfo(ms->guildId, ms->guildEmblemVersion);
			SetPropensityInfo(ms->honor, ms->virtue);

			bool	result = FindPath(ms->sx, ms->sy, ms->cellX, ms->cellY, ms->dx, ms->dy, ms->speed);
			if (result)
			{
				m_moveDestX = ms->dx;
				m_moveDestY = ms->dy;
				SetState(ST_MOVE);
				m_pathStartCell = 0;
			}

			MakePartyEffect(ms->GID);
			break;
		}
		case	AM_CHANGE_RANK: {
			m_pk_rank = arg2;
			m_pk_total = arg1;
		}
		break;
		default:
			CGameActor::SendMsg(sender, message, arg1, arg2, arg3);
			break;
	}

}

void CGrannyPc::SetRank(int Total, int Rank) {}

void CGrannyPc::Render(matrix& vtm)
{
	if (!m_GrannyActorRes)
		return;

	GrannyLightInfo grannyLInfo;
	grannyLInfo.eyeVec = g_Weather.GetSunWorldLoc();
	grannyLInfo.nContrast = 0x80;
	grannyLInfo.lightB = grannyLInfo.lightG = grannyLInfo.lightR = 0xc8;

	if (!m_nUpdateAniFlag) {
		m_GrannyActorRes->SetHairInfo(&m_hair);
		RenderAndAni(vtm, &grannyLInfo, NULL, 0, NULL);
		for (int i = 1; i < 10; i++) {
			if (m_GrannyPartRes[i]) {
				if (i != 1)
					RenderAndAni(vtm, &grannyLInfo, m_GrannyPartRes[i], i, NULL);
				else
					RenderAndAni(vtm, &grannyLInfo, m_GrannyPartRes[i], i, m_GrannyPartRes[i]);
			}
		}

		vector3d lv(1800.0f, -1000.0f, 0.0f);
		RenderShadow(vtm, lv, false);
	} else {
		RenderBak(vtm, 0, NULL);
		if (m_GrannyPartRes[1]) m_GrannyPartRes[1]->SetHairInfo(&m_hair);
		for (int i = 1; i < 10; i++) {
			if (m_GrannyPartRes[i])
				RenderBak(vtm, i, m_GrannyPartRes[i]);
		}

		vector3d lv(1800.0f, -1000.0f, 0.0f);
		RenderShadow(vtm, lv, true);
	}

	if (++m_nUpdateAniFlag == 3) m_nUpdateAniFlag = 0;
}

void CGrannyPc::RenderAndAni(matrix &vtm, GrannyLightInfo *grannyLInfo, C3dGrannyModelRes *pModelRes, int nPartMeshIdx, C3dGrannyModelRes *pAniModelRes)
{
	RPMesh* rp;
	int pTex;
	granny_pnt332_vertex* srcVert;
	float oow;
	matrix revView;
	unsigned char isHairMesh;
	unsigned long vertOffs;
	int pFaceArr;
	RPMesh** rpArr;
	granny_world_pose* pWorldPose;
	float y;
	RENDER_INFO_RECT info;
	int i;
	float x;
	matrix ltm;
	vector3d vec;
	matrix poseMat;
	int nIndexNo;
	vector3d revEyeVec;

	if (!m_isVisible)
		return;
	pTex = 0;
	pFaceArr = 0;
	srcVert = NULL;
	if (m_gid == -1)
		return;

	m_nVertCol = (m_RenderAlpha << 24) + 0xffffff;

	(void)g_modeMgr.GetGameMode();

	pWorldPose = NULL;
	rpArr = m_rpPart[nPartMeshIdx];
	if (!pModelRes) {
		pModelRes = m_GrannyActorRes;
		m_pWorldPose = ProcessAni(NULL);
		rpArr = m_rp;
	} else {
		if (pAniModelRes)
			pWorldPose = ProcessAni(pAniModelRes);
		else
			pWorldPose = m_pWorldPose;
	}

	ltm.MakeXRotation(pi * 0.5f);
	ltm.AppendYRotation(float(m_fCurRot + 180.0f) * pi / 180.0f);
	ltm.AppendScale(0.3f, 0.3f, 0.3f);

	vec = m_pos;
	vec.x -= 2.0f;
	vec.z -= 2.0f;
	ltm.AppendTranslate(vec);

	g_renderer->ProjectVertex(m_pos, vtm, &x, &y, &oow);

	isHairMesh = 0;
	revView.MakeInverse(vtm);
	revEyeVec.MatrixMult(grannyLInfo->eyeVec, revView);
	revEyeVec.Normalize();

	info.left = g_renderer->m_width;
	info.top = g_renderer->m_height;
	info.right = 0;
	info.bottom = 0;
	info.oow = oow;

	for (i = 0; i < pModelRes->GetMeshNo(); i++) {
		matrix worldMat;
		int j;
		matrix final;
		if (nPartMeshIdx == 0)
			pModelRes->SetMatVer(m_matVer[i]);
		else
			pModelRes->SetMatVer(m_matVerPart[nPartMeshIdx][i]);

		srcVert = (granny_pnt332_vertex*)pModelRes->RenderMesh(i, &pTex, &pFaceArr, &nIndexNo, &poseMat, pWorldPose);

		if (!srcVert || !pTex)
			continue;

		if (nPartMeshIdx == m_hair.nPartMeshIdx) {
			worldMat.MultMatrix(m_hair.poseMat, ltm);
			isHairMesh = 1;
		} else {
			worldMat.MultMatrix(poseMat, ltm);
		}

		final.MultMatrix(worldMat, vtm);

		vertOffs = 0;
		rp = rpArr[i];
		unsigned short* indices = (unsigned short*)pFaceArr;
		for (j = 0; j < nIndexNo; j++) {
			for (int k = 0; k < 3; k++) {
				int vertNo = indices[vertOffs];
				tlvertex3d* verts = &rp->verts[vertOffs++];
				granny_pnt332_vertex* src = &srcVert[vertNo];
				vec.x = src->Position[0];
				vec.y = src->Position[1];
				vec.z = src->Position[2];
				if (isHairMesh == 1)
					if (i > 0)
						vec.MatrixMult(vec, poseMat);
				g_renderer->ProjectVertex(vec, final, verts);
				verts->tu = src->UV[0];
				verts->tv = src->UV[1];
				vector3d normal;
				normal.x = src->Normal[0];
				normal.y = src->Normal[1];
				normal.z = src->Normal[2];

				float dot = normal.DotProduct(revEyeVec);
				float light = 1.0f - dot;
				if (light > 1.0f)
					light = 1.0f;
				else if (light < 0.0f)
					light = 0.0f;

				m_nVertCol = (int)(light * grannyLInfo->nContrast + (255 - grannyLInfo->nContrast));
				int col = m_nVertCol;
				verts->color = (m_RenderAlpha << 24)
					+ (((grannyLInfo->lightR + col + 0x38) & 0xff) << 16)
					+ (((grannyLInfo->lightG + col + 0x38) & 0xff) << 8)
					+ ((grannyLInfo->lightB + col + 0x38) & 0xff);
			}
			if (nPartMeshIdx != 1)
				m_shadowDotList.push_back(vec);
		}

		rp->tex = (CTexture*)pTex;
		rp->numVerts = vertOffs;
		g_renderer->AddRP(rp, 0);
		m_pTex[nPartMeshIdx][i] = (CTexture*)pTex;
		m_pFaceArr[nPartMeshIdx][i] = (void*)pFaceArr;
		m_nIndexNo[nPartMeshIdx][i] = nIndexNo;
		m_matPose[i] = poseMat;
	}

	if (pModelRes != m_GrannyActorRes)
		return;

	CGameMode* gameMode = g_modeMgr.GetGameMode();
	CView* view = gameMode->GetView();
	if (m_GrannyActorRes->CalculateHitArea(info, vtm, ltm, *view->GetEeyeFromVector()))
		SetRenderInfo(info, x, y);
	else
		SetTlvert(x, y);
}

void CGrannyPc::RenderBak(matrix& vtm, int nPartMeshIdx, C3dGrannyModelRes* pModelRes)
{
	granny_pnt332_vertex* srcVert;
	float oow;
	unsigned char isHairMesh;
	float y;
	RENDER_INFO_RECT info;
	int i;
	float x;
	matrix ltm;
	vector3d vec;
	CView* pView;
	matrix worldMat;
	matrix final;

	if (!m_isVisible)
		return;
	if (m_gid == -1)
		return;

	m_nVertCol = (m_RenderAlpha << 24) + 0xffffff;

	(void)g_modeMgr.GetGameMode();
	pView = g_modeMgr.GetGameMode()->GetView();

	isHairMesh = 0;
	ltm.MakeXRotation(pi * 0.5f);
	ltm.AppendYRotation((m_fCurRot + 180.0f) * pi / 180.0f);
	ltm.AppendScale(0.3f, 0.3f, 0.3f);

	vec = m_pos;
	vec.x -= 2.0f;
	vec.z -= 2.0f;
	ltm.AppendTranslate(vec);

	g_renderer->ProjectVertex(m_pos, vtm, &x, &y, &oow);

	info.left = g_renderer->m_width;
	info.top = g_renderer->m_height;
	info.right = 0;
	info.bottom = 0;
	info.oow = oow;

	RPMesh** rpArr = m_rpPart[nPartMeshIdx];
	if (!pModelRes) {
		pModelRes = m_GrannyActorRes;
		rpArr = m_rp;
	}

	for (i = 0; i < pModelRes->GetMeshNo(); i++) {
		if (!(srcVert = (granny_pnt332_vertex*)pModelRes->GetRigidMesh(i))) {
			if (nPartMeshIdx == 0)
				srcVert = m_matVer[i];
			else
				srcVert = m_matVerPart[nPartMeshIdx][i];
		}
		if (!srcVert || !m_pTex[nPartMeshIdx][i])
			continue;

		if (nPartMeshIdx == m_hair.nPartMeshIdx) {
			worldMat.MultMatrix(m_hair.poseMat, ltm);
			isHairMesh = 1;
		} else {
			worldMat.MultMatrix(m_matPose[i], ltm);
		}

		final.MultMatrix(worldMat, vtm);

		unsigned short* indices = (unsigned short*)m_pFaceArr[nPartMeshIdx][i];
		RPMesh* rp = rpArr[i];
		int nIndexNo = m_nIndexNo[nPartMeshIdx][i];
		unsigned long vertOffs = 0;
		for (int j = 0; j < nIndexNo; j++) {
			for (int k = 0; k < 3; k++) {
				tlvertex3d* verts = &rp->verts[vertOffs];
				int vertNo = indices[vertOffs++];
				vec.x = srcVert[vertNo].Position[0];
				vec.y = srcVert[vertNo].Position[1];
				vec.z = srcVert[vertNo].Position[2];
				if (isHairMesh == 1)
					if (i > 0)
						vec.MatrixMult(vec, m_matPose[i]);
				g_renderer->ProjectVertex(vec, final, verts);
			}
		}

		rp->tex = m_pTex[nPartMeshIdx][i];
		rp->numVerts = m_nIndexNo[nPartMeshIdx][i] * 3;
		g_renderer->AddRP(rp, 0);
	}

	if (pModelRes != m_GrannyActorRes)
		return;

	bool hit = m_GrannyActorRes->CalculateHitArea(info, vtm, ltm, *pView->GetEeyeFromVector());
	if (hit)
		SetRenderInfo(info, x, y);
	else
		SetTlvert(x, y);
}

void CGrannyPc::RenderShadow(matrix& vtm, vector3d lv, bool isBakRender)
{
	CGameMode* gameMode = g_modeMgr.GetGameMode();
	float sinDeg = GetSin(int(m_fCurRot) + 90);
	float cosDeg = GetCos(int(m_fCurRot) + 90);
	int attrX;
	int attrY;
	int cellX;
	int cellY;
	int addx;
	int addy;
	vector3d vShadow;
	C3dGround* ground;
	float cos_r;
	float sin_r;
	vector2d uv[4];
	int i;
	int j;
	float v0;
	float v1;
	float u0;
	float u1;

	if (!isBakRender) {
		gameMode->GetServerCoor(m_pos.x, m_pos.z, attrX, attrY, cellX, cellY);

		float cx;
		float cy;
		gameMode->GetClientCoor(attrX, attrY, cx, cy);

		addx = int(m_pos.x - cx);
		addy = int(m_pos.z - cy);

		memset(tmpShadowDotBuf, 0, sizeof(tmpShadowDotBuf));
		std::vector<vector3d>::iterator it = m_shadowDotList.begin();
		while (it != m_shadowDotList.end()) {
			vShadow = *it;
			vShadow.x *= 0.35f;
			vShadow.y *= 0.35f;
			int dotX = int((vShadow.y * cosDeg) + (vShadow.x * sinDeg) + 17.0f);
			dotX += addx;
			int dotY = int((vShadow.x * cosDeg) - (vShadow.y * sinDeg) + 17.0f);
			dotY += addy;
			if (dotX >= 0 && dotX < 34 && dotY >= 0 && dotY < 34) {
				int idx = dotY * 35 + dotX;
				tmpShadowDotBuf[idx] = 0x5f000000;
				tmpShadowDotBuf[idx + 1] |= 0x2f000000;
				tmpShadowDotBuf[idx + 35] |= 0x2f000000;
			}
			++it;
		}

		std::vector<vector3d>& shadowDotList = m_shadowDotList;
		shadowDotList.erase(shadowDotList.begin(), shadowDotList.end());
		m_shadowTex->Update(0, 0, 35, 35, tmpShadowDotBuf, false, 0);
	}

	gameMode->GetServerCoor(m_pos.x, m_pos.z, attrX, attrY, cellX, cellY);

	ground = gameMode->GetWorld()->m_ground;
	attrX -= 2;
	attrY -= 2;

	sin_r = _sin(0.0f);
	cos_r = _cos(0.0f);

	for (i = 0; i < 5; ++i) {
		v0 = i - 2.5f;
		v1 = i + 1 - 2.5f;
		for (j = 0; j < 5; ++j) {
			u0 = j - 2.5f;
			u1 = j + 1 - 2.5f;

			uv[0].Set(v0 * sin_r + u0 * cos_r, v0 * cos_r - u0 * sin_r);
			uv[1].Set(v0 * sin_r + u1 * cos_r, v0 * cos_r - u1 * sin_r);
			uv[2].Set(v1 * sin_r + u0 * cos_r, v1 * cos_r - u0 * sin_r);
			uv[3].Set(v1 * sin_r + u1 * cos_r, v1 * cos_r - u1 * sin_r);

			uv[0].Set((uv[0].x + 2.5f) / 5.0f, (uv[0].y + 2.5f) / 5.0f);
			uv[1].Set((uv[1].x + 2.5f) / 5.0f, (uv[1].y + 2.5f) / 5.0f);
			uv[2].Set((uv[2].x + 2.5f) / 5.0f, (uv[2].y + 2.5f) / 5.0f);
			uv[3].Set((uv[3].x + 2.5f) / 5.0f, (uv[3].y + 2.5f) / 5.0f);

			ground->RenderGroundTile(vtm, attrX + j, attrY + i, 0x70ffffff, m_shadowTex, uv);
		}
	}
}

void CGrannyPc::SetGr2Names(const std::vector<std::string>& sprNames)
{
}

const std::vector<std::string> CGrannyPc::GetGr2Names()
{
	std::vector<std::string> ttt(PC_LAYER_SIZE+4);
	return ttt;
}

void CGrannyPc::SetActNames(const std::vector<std::string>& actNames)
{
}

const std::vector<std::string> CGrannyPc::GetActNames()
{
	std::vector<std::string> ttt(PC_LAYER_SIZE+4);
	return ttt;
}

granny_world_pose* CGrannyPc::ProcessAni(C3dGrannyModelRes* pRes)
{
	if (!pRes) pRes = m_GrannyActorRes;
	m_fAniCnt = GrannyGetSystemSeconds();
	float dt = GrannyGetSecondsElapsed(&m_fLastAniCnt, &m_fAniCnt);
	m_fLastAniCnt = m_fAniCnt;
	m_GameClock += dt + dt;
	if (pRes != m_GrannyActorRes) {
		pRes->ActAnimation(m_nLastActAnimation, pRes->GetModelInstance(), NULL, 0);
		return pRes->UpdateAni(m_GameClock, pRes->GetModelInstance());
	}
	if (m_nLastActAnimation == 2) {
		if (m_Control[2] && GrannyGetControlDurationLeft(m_Control[2]) <= 0.0) {
			m_nLastActAnimation = 0;
			m_Control[m_nLastActAnimation] = pRes->ActAnimation(m_nLastActAnimation, pRes->GetModelInstance(), m_Control[m_nLastActAnimation], 0);
		}
	}
	return pRes->UpdateAni(m_GameClock * 0.5f, m_Instance);
}

void CGrannyPc::ProcessMotion()
{
	if (m_stateId == 2) {
		if (GrannyGetControlDurationLeft(m_Control[2]) >= 0.1f) {
			m_isMotionFinished = 0;
			return;
		}
	}
	m_isMotionFinished = 1;
}

void CGrannyPc::SetAction(int action, int speed, int type)
{
	if (!m_GrannyActorRes)
		return;

	m_stateId = 0;
	switch (action) {
	case 48:
		m_stateId = 4;
		break;
	case 80:
	case 88:
		m_stateId = 2;
		break;
	case 64:
		m_stateId = 3;
		break;
	case 0:
		m_stateId = 0;
		break;
	case 8:
		m_stateId = 1;
		break;
	case 24:
		m_stateId = 5;
		break;
	case 32:
		m_stateId = 11;
		break;
	case 40:
		m_stateId = 12;
		break;
	case 16:
		m_stateId = 6;
	default:
		break;
	}
	SetGrannyAction(type);
}

void CGrannyPc::SetGrannyAction(int type)
{
	m_nLastActAnimation = m_stateId;
	for (int i = 0; i < 20; i++) {
		m_GrannyActorRes->DeActAnimation(m_Control[i]);
		m_Control[i] = NULL;
	}
	m_GameClock = 0;
	if (type == 1 && m_stateId != 6) {
		m_Control[m_nLastActAnimation] = m_GrannyActorRes->ActAnimation(
			m_nLastActAnimation, m_Instance, m_Control[m_nLastActAnimation], 1);
	} else {
		m_Control[m_nLastActAnimation] = m_GrannyActorRes->ActAnimation(
			m_nLastActAnimation, m_Instance, m_Control[m_nLastActAnimation], 0);
	}
}
