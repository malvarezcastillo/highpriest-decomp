#include "common.h"
#include "ImfRes.h"
#include "../base/File.h"






typedef std::vector<ImfData> IMFARRAY;




#ifndef MAX_IMF_LAYER
#define MAX_IMF_LAYER 15
#endif
#ifndef MAX_ACTION_
#define MAX_ACTION_ 104
#endif


#ifndef SPRLAYER_VER
#define SPRLAYER_VER 1.01000
#endif






static CImfRes CImfResInstance(Exemplar(), "imf", "imf\\");
CImfRes::CImfRes()
: CRes()
{
	int a, b;
	for (a = 0; a < MAX_IMF_LAYER; a++) {
		m_maxAction[a] = 0;
		for (b = 0; b < MAX_ACTION_; b++) {
			m_maxMotion[a][b] = 0;
		}
	}
}

void CImfRes::Create()
{

}

bool CImfRes::Load(const char *fName)
{
	int a, b, c, numAction, numMotion, MaxLayer, Buf, Sum;
	float Ver;
	CFile fp;
	if(!fp.Open(fName, CFile::modeRead))
	{
		Trace("Hey, file %s seems missing.", fName);
		return false;
	}
	fp.Read(&Ver, sizeof(float));
	if(Ver < (float)SPRLAYER_VER) {
		Trace("Failed to read %s! Old format. Re-save it in the new version, then load it.", fName);
		return false;
	}
	fp.Read(&Sum, sizeof(int));
	fp.Read(&MaxLayer, sizeof(int));

	for(a = 0; a <= MaxLayer; a++) {
		IMFARRAY ImfArBuf;
		fp.Read(&numAction, sizeof(int));
		m_maxAction[a] = numAction;
		for(b = 0; b < numAction; b++) {
			ImfData ImfBuf;
			fp.Read(&numMotion, sizeof(int));
			m_maxMotion[a][b] = numMotion;
			for(c = 0; c < numMotion; c++) {
				fp.Read(&Buf, sizeof(int));
				ImfBuf.m_Priority.push_back(Buf);
				fp.Read(&Buf, sizeof(int));
				ImfBuf.m_cx.push_back(Buf);
				fp.Read(&Buf, sizeof(int));
				ImfBuf.m_cy.push_back(Buf);
			}
			ImfArBuf.push_back(ImfBuf);
		}
		m_ImfData.push_back(ImfArBuf);
	}
































	fp.Close();
	return true;
}

int CImfRes::CheckSum(int MaxLayer)
{
	int Sum, a ,b ,c, d, Multiply;
	int *buf, buf2;
	buf = new int[MaxLayer];

	if(MaxLayer < 4)
		Multiply = 2;
	else Multiply = 1;
	Sum = 0;
	for(a = 0; a <= MaxLayer; a++) {
		buf[a] = 0;
		buf2 = 0;
		for(b = 0; b < m_ImfData[a].size(); b++) {
			for(c = 0; c < m_ImfData[a][b].m_Priority.size(); c++) {
				buf[a] += m_ImfData[a][b].m_Priority[c];
				if(m_ImfData[a][b].m_cx[c] != 0)
					buf2 += m_ImfData[a][b].m_cx[c] * buf[a];
				if(m_ImfData[a][b].m_cy[c] != 0)
					buf2 += m_ImfData[a][b].m_cy[c] * buf[a];
			}
		}
		for(d = MaxLayer * Multiply; d >= 0; d--)
			buf[a] *= 10;
		buf[a] += buf2;
		Sum += buf[a];
	}
	return Sum;
}

int CImfRes::GetNumLayer()
{
	return m_ImfData.size();
}

int CImfRes::GetNumAction(int NumLayer)
{
	return m_ImfData[NumLayer].size();
}

int CImfRes::GetNumMotion(int NumLayer, int NumAction)
{
	return m_ImfData[NumLayer][NumAction].m_cx.size();
}

int CImfRes::GetLongestMotion(int NumLayer)
{
	int a, MaxMotion = 0, NowSize;
	for(a = 0; a < GetNumAction(NumLayer); a++) {
		NowSize = m_ImfData[NumLayer][a].m_cx.size();
		if(MaxMotion < NowSize)
			MaxMotion = NowSize;
	}
	return MaxMotion;
}

int CImfRes::GetLayer(int Priority, int Act, int Mot)
{
	if ( Priority >= MAX_IMF_LAYER )
		return -1;

	if (Act >= m_maxAction[Priority])
	{
		return -1;
	} else if (Mot >= m_maxMotion[Priority][Act])
	{
		return -1;
	}
	int Pr;
	for(int a = 0; a < GetNumLayer(); a++) {
		Pr = m_ImfData[a][Act].m_Priority[Mot];
#ifdef _DEBUG
		if (!(Pr == 0 || Pr == 1)) {
			int s1 = m_ImfData.size();
			int s2 = m_ImfData[a].size();
			int s3 = m_ImfData[a][Act].m_Priority.size();
			sAssert(false);
		}
#endif

		if(Pr == Priority)
			return a;
	}
	sAssert(false);
	return -1;
}

int CImfRes::GetPriority(int Layer, int Act, int Mot)
{
	return m_ImfData[Layer][Act].m_Priority[Mot];
}

POINT CImfRes::GetPoint(int Layer, int Act, int Mot)
{
	POINT point;
	point.x = m_ImfData[Layer][Act].m_cx[Mot];
	point.y = m_ImfData[Layer][Act].m_cy[Mot];
	return point;
}
