#include "common.h"
#include <windows.h>
#include "../base/File.h"
#include "../3dlib/Renderer.h"
#include "Attr.h"

#include "../base/Util.h"

extern BOOL  CheckRayTriIntersect(const ray3d& ray, const vector3d& v0, const vector3d& v1, const vector3d& v2, float* u, float* v, float* t);









static C3dAttr C3dAttrInstance(Exemplar(), "gat", "");
CAttrCell :: CAttrCell()
{
}

C3dAttr :: C3dAttr()
	: CRes()
{
	m_width = 0;
	m_height = 0;
	m_zoom = 5;
}

void	C3dAttr :: Create(int w, int h)
{
	Reset();
	m_cells.resize(w*h);
	m_width = w;
	m_height = h;
}

void	C3dAttr :: Reset()
{
	m_width = 0;
	m_height = 0;
}

bool	C3dAttr :: Load(const char *fName)
{
	int		w, h;

	CFile	fp;

	fp.Open(fName, CFile::modeRead);
	char	magicId[] = "GRAT";
	char	magicBuf[5];

	fp.Read(magicBuf, 4);
	magicBuf[4]=0;
	if (strcmp(magicBuf, magicId)) {
		ErrorMsg("This GAT file is an old version without Block Attr support. Fix it in a newer RPGSTUDIO");
		return	false;
	} else {


		BYTE	verMajor;
		BYTE	verMinor;
		fp.Read(&verMajor, sizeof(BYTE));
		fp.Read(&verMinor, sizeof(BYTE));
		if (verMajor != 1 || verMinor > 2) {
			ErrorMsg("C3dAttr::Unsupported Version");
			return	TRUE;
		}
		fp.Read(&w, sizeof(int));
		fp.Read(&h, sizeof(int));
		Create(w, h);
		fp.Read(&m_cells[0], sizeof(CAttrCell)*w*h);
	}
	fp.Close();
	return	TRUE;
}

int		C3dAttr :: GetAttr(float x, float z)
{
	float	zoom = m_zoom;

	int	MapCenterX = (x+(m_width)*zoom/2) / zoom;
	int	MapCenterY = (z+(m_height)*zoom/2) / zoom;


	if (MapCenterX<0 || MapCenterY<0 || MapCenterX>=m_width || MapCenterY>=m_height) {
		ErrorMsg((int)x);
		ErrorMsg((int)z);
		return	1;
	}
	return	GetCell(MapCenterX, MapCenterY)->flag;
}

void	C3dAttr :: GetWorldPos(float attrX, float attrY, vector2d & pos)
{
	pos.x = m_zoom*(attrX-m_width/2);
	pos.y = m_zoom*(attrY-m_height/2);
}

float	C3dAttr :: GetHeight(float x, float z)
{
	float	zoom = m_zoom;

	int	MapCenterX = (x+(m_width)*zoom/2) / zoom;
	int	MapCenterY = (z+(m_height)*zoom/2) / zoom;


	CAttrCell * cell = GetCell(MapCenterX, MapCenterY);
	if (cell==NULL)
		return 0;

	float	ofsX = (x+(m_width)*zoom/2) - MapCenterX*zoom;
	float	ofsY = (z+(m_height)*zoom/2) - MapCenterY*zoom;
	float	leftY, rightY;

	leftY = cell->h1 + ((cell->h3-cell->h1)*ofsY)/zoom;
	rightY = cell->h2 + ((cell->h4-cell->h2)*ofsY)/zoom;

	return	leftY + ((rightY-leftY)*ofsX)/zoom;
}

float	C3dAttr :: RayTest(const ray3d &ray, int x, int y)
{
	float	u, v;
	float	nearest = infinity;
	float	dist;
	CAttrCell * cell = GetCell(x, y);
	if (cell == NULL)
		return 0;




	vector3d	vec[4];
	vec[0].Set(float(x-m_width/2)*m_zoom, cell->h1, float(y-m_height/2)*m_zoom);
	vec[1].Set(float(x+1-m_width/2)*m_zoom, cell->h2, float(y-m_height/2)*m_zoom);
	vec[2].Set(float(x-m_width/2)*m_zoom, cell->h3, float(y+1-m_height/2)*m_zoom);
	vec[3].Set(float(x+1-m_width/2)*m_zoom, cell->h4, float(y+1-m_height/2)*m_zoom);

	if (CheckRayTriIntersect(ray, vec[0], vec[1], vec[2], &u, &v, &dist)) {
		if (dist < nearest) {
			nearest = dist;
		}
	}

	if (CheckRayTriIntersect(ray, vec[1], vec[3], vec[2], &u, &v, &dist)) {
		if (dist < nearest) {
			nearest = dist;
		}
	}
	return	nearest;
}

void	C3dAttr :: GetHeightMinMax(const RECT &area, float &topHeight, float &bottomHeight)
{
	sAssert(area.left>=0 && area.left < m_width);
	sAssert(area.right>=0 && area.right < m_width);
	sAssert(area.top>=0 && area.top < m_height);
	sAssert(area.bottom>=0 && area.bottom < m_height);
	sAssert(area.right>area.left && area.bottom>area.top);

	topHeight = infinity;
	bottomHeight = -infinity;

	for (int i=area.left; i<=area.right; i++) {
		for (int j=area.top; j<=area.bottom; j++) {
			CAttrCell *cell = GetCell(i, j);
			if (cell) {
				if (bottomHeight < cell->h1)
					bottomHeight = cell->h1;
				if (bottomHeight < cell->h2)
					bottomHeight = cell->h2;
				if (bottomHeight < cell->h3)
					bottomHeight = cell->h3;
				if (bottomHeight < cell->h4)
					bottomHeight = cell->h4;
				if (topHeight > cell->h1)
					topHeight = cell->h1;
				if (topHeight > cell->h2)
					topHeight = cell->h2;
				if (topHeight > cell->h3)
					topHeight = cell->h3;
				if (topHeight > cell->h4)
					topHeight = cell->h4;
			}
		}
	}
}
