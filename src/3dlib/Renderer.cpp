#include "common.h"
#include "../framework/Locale.h"

#include "../base/Res.h"
#include "../resource/Bitmap.h"
#include "../Mode.h"
#include "Device.h"
#include "D3dutil.h"
#include "Renderer.h"
#include "DrawUtil.h"
#include "../ijl.h"
#include "../GameMode.h"
#include "../Session.h"
#include "../GameActor.h"

#include <algorithm>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>

extern "C" int __cdecl sprintf(char *, const char *, ...);
extern "C" void *__cdecl malloc(unsigned int);
extern "C" void __cdecl free(void *);
extern "C" int __cdecl rand(void);


CTexMgr g_texMgr;
CDynamicVB g_vb;
CDynamicVB g_vblm;
extern CModeMgr g_modeMgr;
extern int g_texLenDivFactor;
extern int g_isLightMap;
extern bool g_isSupportLMTex;
extern DWORD EXPIRETIME;
extern HRESULT D3DUtil_SetProjectionMatrix(D3DMATRIX& mat, FLOAT fFOV, FLOAT fAspect,
										   FLOAT fNearPlane, FLOAT fFarPlane);
extern int g_se_go;
extern char g_se_type;
extern short g_se_count;
extern char g_AgeRate;
extern int g_AgeAlpha;
extern ServiceType g_serviceType;

CRenderer* g_renderer;
int g_se_go = 0;
char g_se_type = 0;
short g_se_count = 0;
char g_AgeRate = 0;
int g_AgeAlpha = 0;





int cacheSizeTable[16][2] = {
	{ 32,  32  }, { 32,  64  }, { 32,  128 }, { 32,  256 },
	{ 64,  32  }, { 64,  64  }, { 64,  128 }, { 64,  256 },
	{ 128, 32  }, { 128, 64  }, { 128, 128 }, { 128, 256 },
	{ 256, 32  }, { 256, 64  }, { 256, 128 }, { 256, 256 },
};
float g_gradient = 0.16f;
float g_slope = 0.16f;
float g_shadowSlope = 0.16f;
extern float g_avgPixelRatio;




typedef std::map<char const *, CTexture *, CharPrtLess, std::allocator<CTexture *> >::iterator
		texture_map_iter;
typedef std::map<char const *, CTexture *, CharPrtLess, std::allocator<CTexture *> >::value_type
		texture_map_value;




typedef std::vector<RPFace *>::iterator                          rp_tex_map_iter;
typedef std::vector<RPRaw  *>::iterator                          rp_raw_map_iter;
typedef std::vector<RPLmFace *>::iterator                        rp_lmtex_map_iter;
typedef std::vector<std::pair<float, RPFace *> >::iterator       rp_rp_map_iter;
typedef std::pair<float, RPFace *>                               rp_float_pair;

FogPara g_fogPara;

inline bool CRenderer::RPFaceTexPrtLess::operator()(const RPFace *left, const RPFace *right) const
{
	return left->tex < right->tex;
}

inline bool CRenderer::RPRawTexPrtLess::operator()(const RPRaw *left, const RPRaw *right) const
{
	return left->tex < right->tex;
}




void	RPRaw :: DrawPri(IDirect3DDevice7* device)
{
	if (numVerts) {
		D3DMATRIX d3dWorld(world.v11, world.v12, world.v13, 0,
						   world.v21, world.v22, world.v23, 0,
						   world.v31, world.v32, world.v33, 0,
						   world.v41, world.v42, world.v43, 1);

		device->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dWorld);
		device->DrawPrimitiveVB(D3DPT_TRIANGLELIST, vb->GetInterface(), vertStart, numVerts, 0);
	}
}
void	RPRaw :: DrawPriIndexed(IDirect3DDevice7* device)
{
	if (numIndices) {
		D3DMATRIX d3dWorld(world.v11, world.v12, world.v13, 0,
						   world.v21, world.v22, world.v23, 0,
						   world.v31, world.v32, world.v33, 0,
						   world.v41, world.v42, world.v43, 1);

		device->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dWorld);
		device->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST, vb->GetInterface(), 0, numVerts, indices, numIndices, 0);
	}
}
RPFace :: RPFace(tlvertex3d* vert, int numVert)
:	verts(vert)
,	numVerts(numVert)
,	numIndices(0)
,	primType(D3DPT_TRIANGLELIST)
#ifdef _DX9
,	mRPF_VB(0)
,	mPRF_IB(0)
#endif
{
}

void	RPFace :: SetGeomInfo(int vertId, const tlvertex3d &src)
{
	sAssert(vertId >=0 && vertId < numVerts);
	verts[vertId].x = src.x;
	verts[vertId].y = src.y;
	verts[vertId].z = src.z;
	verts[vertId].oow = src.oow;
}

void	RPFace :: SetGeomInfo(int vertId, const vector4d &src)
{
	sAssert(vertId >=0 && vertId < numVerts);
	verts[vertId].x = src.x;
	verts[vertId].y = src.y;
	verts[vertId].z = src.z;
	verts[vertId].oow = src.oow;
}

void	RPFace :: InitSpecular()
{
	for(int i=0; i<numVerts; ++i)
	{
		verts[i].specular = GetSpecularFromOow(verts[i].oow);
	}
}

void	RPFace :: DrawPri(IDirect3DDevice7* device)
{
	if (numVerts)
	{
		if (numIndices)
		{
			device->DrawIndexedPrimitive(primType, 0x1C4, verts, numVerts, indices, numIndices, 0);
		}
		else
		{
			device->DrawPrimitive(primType, 0x1C4, verts, numVerts, 0);
		}
	}
}


RPQuadFace :: RPQuadFace()
:	RPFace(m_verts, 4)
{
	primType = D3DPT_TRIANGLESTRIP;
}

RPQuadFace :: RPQuadFace(const RPQuadFace& rp)
:	RPFace(m_verts, 4)
{
	primType = D3DPT_TRIANGLESTRIP;

	memcpy(m_verts, rp.m_verts, sizeof(m_verts));
}

const RPQuadFace& RPQuadFace :: operator=(const RPQuadFace& rp)
{
	memcpy(m_verts, rp.m_verts, sizeof(m_verts));

	return *this;
}

void	RPLmFace :: InitSpecular()
{
	for(int i=0; i<numVerts; ++i)
	{
		lmverts[i].vert.specular = GetSpecularFromOow(lmverts[i].vert.oow);
	}
}

void	RPLmFace :: DrawPri(IDirect3DDevice7* device)
{
	if (numVerts)
	{
		if (numIndices)
		{
			device->DrawIndexedPrimitive(primType, 0x2C4, lmverts, numVerts, indices, numIndices, 0);
		}
		else
		{
			device->DrawPrimitive(primType, 0x2C4, lmverts, numVerts, 0);
		}
	}
}

RPLmQuadFace :: RPLmQuadFace()
{
	lmverts		= m_lmverts;
	numVerts	= 4;
	indices		= NULL;
	numIndices	= 0;
	primType	= D3DPT_TRIANGLESTRIP;
}

RPLmQuadFace :: RPLmQuadFace(const RPLmQuadFace& rp)
{
	lmverts		= m_lmverts;
	numVerts	= 4;
	indices		= NULL;
	numIndices	= 0;
	primType	= D3DPT_TRIANGLESTRIP;

	memcpy(m_lmverts, rp.m_lmverts, sizeof(m_lmverts));
}

const RPLmQuadFace& RPLmQuadFace :: operator=(const RPLmQuadFace& rp)
{
	memcpy(m_lmverts, rp.m_lmverts, sizeof(m_lmverts));
	return * this;
}

RPMesh :: RPMesh( void )
:	RPFace(NULL, 0)
{
}

RPMesh :: ~RPMesh( void )
{
	if( verts ) {
		delete verts;
		verts = NULL;
	}
}

void RPMesh :: AllocVerts( int numVerts )
{
	verts = new tlvertex3d[numVerts*3];
}

CTexture CTexMgr::s_dummy_texture(1, 1, PF_A1R5G5B5, NULL);


CTexMgr :: CTexMgr()
{
}

CTexMgr :: ~CTexMgr()
{
	DestroyAllTexture();
}

CTexture *	CTexMgr :: CreateTexture(DWORD w, DWORD h, PixelFormat pf, IDirectDrawSurface7* surface)
{
	if (surface)
		return new CTexture(w, h, pf, surface);
	else
		return new CTexture(w, h, pf);
}

CTexture *	CTexMgr :: CreateTexture(int w, int h, DWORD *imgData, PixelFormat pf, bool blackkey)
{
	CTexture* tex = CreateTexture(w/g_texLenDivFactor, h/g_texLenDivFactor, pf, NULL);
	if (tex) tex->Update(0, 0, w, h, imgData, blackkey, 0);
	return tex;
}

void	CTexMgr :: ReCreateTexture(CTexture * tex, int w, int h, DWORD *imgData)
{
	if (tex) {
		tex->Create(w/g_texLenDivFactor, h/g_texLenDivFactor, tex->m_pf);
		tex->Update(0, 0, w, h, imgData, 0, 0);
	}
}




CTexture *	CTexMgr :: GetTexture(const char *fName, bool blackkey)
{
	CTexture *	tex;

	tex = FindTexture(fName);

	if (tex)
	{
		tex->UpdateStamp();
		return	tex;
	}

	tex = LoadTexture(fName, blackkey);

	if (tex == NULL)
		return GetDummyTexture();

	strcpy(tex->m_texName, fName);
	tex->UpdateStamp();



	tex = tex; m_texTable.begin();
	m_texTable.insert(texture_map_value(tex->m_texName, tex));

	return tex;
}

void	CTexMgr :: DestroyTexture(CTexture *t)
{
	if(t->m_lockCnt)
	{
		return;
	}

	if(t == GetDummyTexture())
	{
		return;
	}

	texture_map_iter i = m_texTable.find(t->m_texName);

	if(i == m_texTable.end())
		return;

	delete i->second;
	m_texTable.erase(i);
}

void	CTexMgr :: DestroyAllTexture()
{
	texture_map_iter i = m_texTable.begin();

	while (i != m_texTable.end())
	{
		delete i->second;

		++i;
	}

	m_texTable.clear();
}

void	CTexMgr :: ReloadAllTexture()
{
	texture_map_iter i = m_texTable.begin();

	while (i != m_texTable.end())
	{

		CBitmapRes *	bmpRes = (CBitmapRes*)g_resMgr().Get(i->first, false);

		if(bmpRes) {
			ReCreateTexture(i->second, bmpRes->m_width, bmpRes->m_height, bmpRes->m_data);
			g_resMgr().Unload(bmpRes);
		}

		++i;
	}
}

void	CTexMgr :: UnloadRarelyUsedTexture()
{
	DWORD	expireTime = timeGetTime() - EXPIRETIME;

	texture_map_iter i = m_texTable.begin();

	while (i != m_texTable.end())
	{
		CTexture *t = i->second;

		if ((t->m_lockCnt == 0)
			&& t->m_timeStamp < expireTime)
		{
			delete t;
			i = m_texTable.erase(i);
		}
		else
		{
			++i ;
		}
	}

	HeapCompact(GetProcessHeap(), 0);
	if (g_3dDevice.GetDirect3D())
		g_3dDevice.GetDirect3D()->EvictManagedTextures();

}

CTexture *	CTexMgr :: FindTexture(const char* fName)
{
	texture_map_iter i = m_texTable.find(fName);

	if(i == m_texTable.end())
	{
		return 0 ;
	}

	return	i->second;
}

CTexture *	CTexMgr :: LoadTexture(const char *fName, bool blackkey)
{
	CTexture *		tmp = NULL;
	CBitmapRes *	bmpRes = (CBitmapRes*)g_resMgr().Get(fName, false);

	if(bmpRes)
	{
		tmp = g_texMgr.CreateTexture(bmpRes->m_width, bmpRes->m_height, bmpRes->m_data, bmpRes->m_isAlpha ? PF_A4R4G4B4 : PF_A1R5G5B5, blackkey);
		g_resMgr().Unload(bmpRes);
	}
	else
	{
		DWORD dwColor = 0;
		tmp = g_texMgr.CreateTexture(1, 1, &dwColor, PF_A1R5G5B5, blackkey);
	}

	return tmp;
}

void	CTexMgr :: AddTexture(CTexture* tex)
{
	sAssert(tex != NULL);
	sAssert(tex != GetDummyTexture());

	m_texTable.insert(std::map<char const *, CTexture *, CharPrtLess, std::allocator<CTexture *> >::value_type(tex->m_texName, tex));
}

CRenderer :: CRenderer()
{
	m_curFrame = 0;
	StartFPSCount();
	m_oldTexture = NULL;
	m_oldLmapTexture = NULL;
	m_isFoggy = FALSE;
	m_isVertexFog = FALSE;

	m_guardBandLeft = 0;
	m_guardBandRight = 0;
	m_guardBandTop = 0;
	m_guardBandBottom = 0;
	m_isShowInfo = FALSE;

	CreateUnusedCacheSurfaces();

	m_rpNullFaceList.resize(1024, RPFace(NULL, 0));
	m_rpNullFaceListIter = m_rpNullFaceList.begin();

	m_rpQuadFaceList.resize(512);
	m_rpQuadFaceListIter = m_rpQuadFaceList.begin();

	m_rpLmQuadFaceList.resize(4096);
	m_rpLmQuadFaceListIter = m_rpLmQuadFaceList.begin();

	m_rpFaceList.reserve(1000);
	m_rpLMGroundList.reserve(1000);
	m_rpLMLightList.reserve(1000);
	m_rpAlphaDepthList.reserve(1000);
	m_rpAlphaList.reserve(1000);
	m_rpAlphaNoDepthList.reserve(1000);
	m_rpEmissiveDepthList.reserve(1000);
	m_rpEmissiveList.reserve(1000);
	m_rpEmissiveNoDepthList.reserve(1000);
	m_rpRawList.reserve(1000);
	m_rpRawAlphaList.reserve(1000);
	m_rpRawEmissiveList.reserve(1000);
	m_rpAlphaOPList.reserve(1000);
	m_rpLmList.reserve(1000);
	m_rpBumpFaceList.reserve(1000);
	m_vertBuffer.reserve(10240);
}

CRenderer :: ~CRenderer()
{
	ReleaseCacheSurfaces();
	ReleaseUnusedCacheSurfaces();
}

void	CRenderer :: SetSize(int cx, int cy)
{
	m_width = cx;
	m_height = cy;
	m_halfWidth = cx/2;
	m_halfHeight = cy/2;
	m_aspectRatio = float(cy)/float(cx);
	m_hpc = float(m_halfWidth) / tan(DegToRad(HFOV / 2));
	m_vpc = float(m_halfHeight) / tan(DegToRad(VFOV / 2));
	m_hratio = tan(DegToRad(90 - HFOV / 2));
	m_vratio = tan(DegToRad(90 - VFOV / 2));
	m_hpc *= m_aspectRatio;
	m_hratio *= m_aspectRatio;
	m_xoffset = cx/2;
	m_yoffset = cy/2;
	m_screenXFactor = float(m_width) / 640.0f;
	m_screenYFactor = float(m_height) / 480.0f;
	g_avgPixelRatio = 1.34f * float(m_height) / 768.0f;
	g_slope = g_gradient / m_screenYFactor;
	g_shadowSlope = g_gradient / m_screenYFactor;
}

void	CRenderer :: SetPixelFormat(PixelFormat pf)
{
	m_pf = pf;
}

void	CRenderer :: Clear(int clearScreen)
{
	if (clearScreen)
		g_3dDevice.Clear(0xFF000000);
	else
		g_3dDevice.ClearZBuffer();
}

void	CRenderer :: ClearBackground()
{
	g_3dDevice.Clear(m_nClearColor);
}

bool	CRenderer :: DrawScene()
{
	m_dwScreenWidth  = g_3dDevice.GetWidth();
	m_dwScreenHeight = g_3dDevice.GetHeight();
	if (SUCCEEDED(m_device->BeginScene()))
	{
		FlushRenderList();
		DestroyAllRPList();
		m_device->EndScene();
		return true;
	}
	DestroyAllRPList();
	return false;
}

int	CRenderer :: Lock()
{
	DDSURFACEDESC2 desc;
	desc.dwSize = sizeof(desc);
	desc.dwFlags = 0;
	memset(&desc.dwHeight, 0, sizeof(desc) - 8);
	if (FAILED(g_3dDevice.GetRenderSurface()->Lock(NULL, &desc, DDLOCK_WAIT, NULL))) return 0;
	m_lpSurfacePtr = desc.lpSurface;
	m_bRGBBitCount = desc.ddpfPixelFormat.dwRGBBitCount;
	m_lPitch = desc.lPitch;
	return 1;
}


void	CRenderer :: Unlock()
{
	g_3dDevice.GetRenderSurface()->Unlock(NULL);
}

void	CRenderer :: ReleaseCacheSurfaces()
{
	for (int i = 0; i < 16; i++) {
		std::list<CacheSurface>::iterator iter = m_cacheSurfaces[i].begin(), end = m_cacheSurfaces[i].end();

		while (iter != end) {
			if (!iter->tex->m_lockCnt)
				delete iter->tex;
			else
				m_unusedCacheSurfaces.push_back(iter->tex);
			++iter;
		}
		m_cacheSurfaces[i].clear();
	}
}










void	CRenderer :: CreateUnusedCacheSurfaces()
{
	for (int i = 50; i > 0; --i) {
		CTexture* tex = g_texMgr.CreateTexture(256UL, 256UL, PF_A1R5G5B5, NULL);
		m_unusedCacheSurfaces.push_back(tex);
		(*m_unusedCacheSurfaces.rbegin())->Lock();
	}
}

void	CRenderer :: ReleaseUnusedCacheSurfaces()
{
	std::list<CTexture *>::iterator iter = m_unusedCacheSurfaces.begin();

	while (iter != m_unusedCacheSurfaces.end()) {
		(*iter)->Unlock();
		delete (*iter);
		++iter;
	}
	m_unusedCacheSurfaces.clear();
}

void	CRenderer :: UnloadRarelyUsedCaches()
{
	DWORD expireTime = timeGetTime() - EXPIRETIME;

	for (int i = 0; i < 16; ++i) {
		std::list<CacheSurface> &cacheSurfaces = m_cacheSurfaces[i];
		std::list<CacheSurface>::iterator end = cacheSurfaces.end();
		std::list<CacheSurface>::iterator iter = cacheSurfaces.begin();
		while (iter != end) {
			if (iter->lastTime < expireTime) {
				if (!iter->tex->m_lockCnt) {
					delete iter->tex;
				} else {
					m_unusedCacheSurfaces.push_back(iter->tex);
				}
				iter = cacheSurfaces.erase(iter);
			} else {
				++iter;
			}
		}
	}
}

CTexture *	CRenderer :: AddSpriteIndex(SprImg &img, unsigned long* const pal, CacheInfo *info)
{
	int sector;
	int offset;

	if (img.width <= 32)
		sector = 0;
	else if (img.width <= 64)
		sector = 4;
	else if (img.width <= 128)
		sector = 8;
	else
		sector = 12;

	if (img.height <= 32)
		offset = 0;
	else if (img.height <= 64)
		offset = 1;
	else if (img.height <= 128)
		offset = 2;
	else
		offset = 3;

	sector += offset;

	unsigned long expireTime = timeGetTime() - (EXPIRETIME >> 2);
	std::list<CacheSurface>::iterator surfaceIt;
	CacheInfo *cacheInfo = NULL;

START:
	cacheInfo = NULL;

	for (surfaceIt = m_cacheSurfaces[sector].begin(); surfaceIt != m_cacheSurfaces[sector].end(); ++surfaceIt)
	{
		cacheInfo = surfaceIt->info.begin();

		for (; cacheInfo != surfaceIt->info.end(); ++cacheInfo)
		{
			if (cacheInfo->lastTime < expireTime) {
				cacheInfo->id = (unsigned long)&img;
				cacheInfo->palID = (unsigned long)pal;
				if (g_serviceType != ServiceGermany) {
					cacheInfo->tu1 = float(img.width + cacheInfo->x) / 256.0f;
					cacheInfo->tv1 = float(img.height + cacheInfo->y) / 256.0f;
				} else {
					if (img.width > 64)
						cacheInfo->tu1 = float(img.width + cacheInfo->x) / 256.0f - 0.005f;
					else
						cacheInfo->tu1 = float(img.width + cacheInfo->x) / 256.0f;

					if (img.height > 64)
						cacheInfo->tv1 = float(img.height + cacheInfo->y) / 256.0f - 0.005f;
					else
						cacheInfo->tv1 = float(img.height + cacheInfo->y) / 256.0f;
				}

				surfaceIt->tex->UpdateSprite(cacheInfo->x, cacheInfo->y, surfaceIt->texWidth,
					surfaceIt->texHeight, img, pal);
				cacheInfo->lastTime = timeGetTime();
				surfaceIt->lastTime = cacheInfo->lastTime;
				info->tu = cacheInfo->tu;
				info->tv = cacheInfo->tv;
				info->tu1 = cacheInfo->tu1;
				info->tv1 = cacheInfo->tv1;
				return surfaceIt->tex;
			}
		}
	}

	{
		CacheSurface newCache;
		m_cacheSurfaces[sector].push_back(newCache);
	}
	surfaceIt = --m_cacheSurfaces[sector].end();
	CacheSurface& surface = *surfaceIt;




	if (0) m_unusedCacheSurfaces.push_back(new CTexture(256, 256, PF_A1R5G5B5));
	if (m_unusedCacheSurfaces.size() == 0) {
		surface.tex = new CTexture(256, 256, PF_A1R5G5B5);
	} else {
		surface.tex = *m_unusedCacheSurfaces.rbegin();
		m_unusedCacheSurfaces.pop_back();
	}

	surfaceIt->tex->ClearSurface(NULL, 0);
	surface.texWidth = cacheSizeTable[sector][0];
	surface.texHeight = cacheSizeTable[sector][1];
	int w = 256 / surface.texWidth;
	int h = 256 / surface.texHeight;
	surface.w = w;
	surface.h = h;
	surface.info.resize(w * h);


	for (int y = 0; y < surface.h; y++) {
		for (int x = 0; x < surface.w; x++) {
			int index = y * surface.w + x;
			surface.info[index].x = surface.texWidth * x;
			surface.info[index].y = surface.texHeight * y;
			surface.info[index].tu = float(surface.info[index].x) / 256.0f;
			surface.info[index].tv = float(surface.info[index].y) / 256.0f;
		}
	}

	goto START;
}


CTexture *	CRenderer :: GetSpriteIndex(SprImg &img, unsigned long palID, CacheInfo *info)
{
	CRenderer *renderer = this;
	int sector;
	int offset;

	if (img.width <= 32)
		sector = 0;
	else if (img.width <= 64)
		sector = 4;
	else if (img.width <= 128)
		sector = 8;
	else
		sector = 12;

	if (img.height <= 32)
		offset = 0;
	else if (img.height <= 64)
		offset = 1;
	else if (img.height <= 128)
		offset = 2;
	else
		offset = 3;

	sector += offset;

	for (std::list<CacheSurface>::iterator iter = renderer->m_cacheSurfaces[sector].begin();
		iter != renderer->m_cacheSurfaces[sector].end(); ++iter)
	{
		for (std::vector<CacheInfo>::iterator infoIter = iter->info.begin();
			infoIter != iter->info.end(); ++infoIter)
		{
			if (infoIter->id == (unsigned long)&img && infoIter->palID == palID)
			{
				unsigned long tick = timeGetTime();
				infoIter->lastTime = tick;
				iter->lastTime = tick;
				info->tu = infoIter->tu;
				info->tv = infoIter->tv;
				info->tu1 = infoIter->tu1;
				info->tv1 = infoIter->tv1;
				return iter->tex;
			}
		}
	}

	return NULL;
}


void	CRenderer :: DestroyAllRPList()
{
	m_rpFaceList.clear();

	m_rpLMGroundList.clear();
	m_rpLMLightList.clear();

	m_rpAlphaDepthList.clear();
	m_rpAlphaList.clear();
	m_rpAlphaNoDepthList.clear();

	m_rpEmissiveDepthList.clear();
	m_rpEmissiveList.clear();
	m_rpEmissiveNoDepthList.clear();

	m_rpRawList.clear();
	m_rpRawAlphaList.clear();
	m_rpRawEmissiveList.clear();

	m_rpAlphaOPList.clear();
	m_rpLmList.clear();
	m_rpBumpFaceList.clear();

	m_rpNullFaceListIter = m_rpNullFaceList.begin();
	m_rpQuadFaceListIter = m_rpQuadFaceList.begin();
	m_rpLmQuadFaceListIter = m_rpLmQuadFaceList.begin();

	m_vertBuffer.clear();
}



void	CRenderer :: SetLookAt(vector3d& from, vector3d& at, vector3d& up)
{
}

void	CRenderer :: SetLight(vector3d& lightDir, vector3d& diffuseCol, vector3d& ambientCol)
{
	D3DLIGHT7 light;
	memset(&light, 0, sizeof(light));
	light.dltType = D3DLIGHT_DIRECTIONAL;
	light.dvDirection.x = -lightDir.x;
	light.dvDirection.y = -lightDir.y;
	light.dvDirection.z = -lightDir.z;
	light.dcvDiffuse.a = 255;
	light.dcvDiffuse.r = diffuseCol.x;
	light.dcvDiffuse.g = diffuseCol.y;
	light.dcvDiffuse.b = diffuseCol.z;
	light.dcvAmbient.a = 255;
	light.dcvAmbient.r = ambientCol.x;
	light.dcvAmbient.g = ambientCol.y;
	light.dcvAmbient.b = ambientCol.z;
	m_device->SetLight(0, &light);
	m_device->LightEnable(0, TRUE);
}

void CRenderer::FlushRenderList()
{
	m_device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_LINEAR);
	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	m_device->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0xcf);
	m_device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
	m_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
	m_device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
	m_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);

	FlushFaceList();
	FlushRawList();

	m_device->SetRenderState(D3DRENDERSTATE_AMBIENT, 0xffffffff);
	m_device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);

	if (g_isLightMap) {
		if (g_isSupportLMTex) {
			m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATEALPHA_ADDCOLOR);
			FlushLmList();
			m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		} else {
			FlushLMGroundList();

			m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
			m_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			m_device->SetRenderState(D3DRENDERSTATE_ALPHAREF, TRUE);
			m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
			m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA);

			FlushLMLightList();

			m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			m_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			m_device->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0xcf);
		}
	} else {
		FlushLMGroundList();
	}

	m_device->SetRenderState(D3DRENDERSTATE_AMBIENT, 0);
	m_device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
	m_device->SetRenderState(D3DRENDERSTATE_ALPHAREF, TRUE);
	m_device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

	FlushAlphaDepthList();

	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

	FlushEmissiveDepthList();

	m_device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

	FlushAlphaList();
	FlushAlphaRawList();

	m_device->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

	FlushEmissiveList();
	FlushEffectList();

	m_device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

	FlushAlphaNoDepthList();

	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

	FlushEmissiveNoDepthList();

	m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_device->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);
}

void	CRenderer :: FlushFaceList()
{
	std::sort(m_rpFaceList.begin(), m_rpFaceList.end(), RPFaceTexPrtLess());

	rp_tex_map_iter i_rp_begin = m_rpFaceList.begin();
	for(; i_rp_begin != m_rpFaceList.end(); ++i_rp_begin)
	{
		RPFace *rp = *i_rp_begin;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}

	SetLmapTexture(NULL);
}
void	CRenderer :: FlushRawList()
{
	std::sort(m_rpRawList.begin(), m_rpRawList.end(), RPRawTexPrtLess());

	rp_raw_map_iter i_rp_begin = m_rpRawList.begin();
	for(; i_rp_begin != m_rpRawList.end(); ++i_rp_begin)
	{
		RPRaw *rp = *i_rp_begin;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushAlphaRawList()
{
	std::sort(m_rpRawAlphaList.begin(), m_rpRawAlphaList.end(), RPRawTexPrtLess());

	rp_raw_map_iter i_rp_begin = m_rpRawAlphaList.begin();
	for(; i_rp_begin != m_rpRawAlphaList.end(); ++i_rp_begin)
	{
		RPRaw *rp = *i_rp_begin;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushEmissiveRawList()
{
	std::sort(m_rpRawEmissiveList.begin(), m_rpRawEmissiveList.end(), RPRawTexPrtLess());

	rp_raw_map_iter i_rp_begin = m_rpRawEmissiveList.begin();
	for(; i_rp_begin != m_rpRawEmissiveList.end(); ++i_rp_begin)
	{
		RPRaw *rp = *i_rp_begin;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}
void	CRenderer :: FlushLMGroundList()
{
	std::sort(m_rpLMGroundList.begin(), m_rpLMGroundList.end(), RPFaceTexPrtLess());

	rp_tex_map_iter i_rp_begin = m_rpLMGroundList.begin();
	for(; i_rp_begin != m_rpLMGroundList.end(); ++i_rp_begin)
	{
		RPFace *rp = *i_rp_begin;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushLMLightList()
{
	std::sort(m_rpLMLightList.begin(), m_rpLMLightList.end(), RPFaceTexPrtLess());

	rp_tex_map_iter i_rp_begin = m_rpLMLightList.begin();
	for(; i_rp_begin != m_rpLMLightList.end(); ++i_rp_begin)
	{
		RPFace *rp = *i_rp_begin;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}
void	CRenderer :: FlushAlphaNoDepthList()
{
	rp_rp_map_iter i = m_rpAlphaNoDepthList.begin();
	for (; i != m_rpAlphaNoDepthList.end(); ++i)
	{
		RPFace *rp = i->second;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}
void	CRenderer :: FlushAlphaDepthList()
{
	std::sort(m_rpAlphaDepthList.begin(), m_rpAlphaDepthList.end());


	rp_rp_map_iter i = m_rpAlphaDepthList.begin();
	for(; i != m_rpAlphaDepthList.end(); ++i)
	{
		RPFace *rp = i->second;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushEmissiveDepthList()
{
	std::sort(m_rpEmissiveDepthList.begin(), m_rpEmissiveDepthList.end());

	rp_rp_map_iter i = m_rpEmissiveDepthList.begin();
	for(; i != m_rpEmissiveDepthList.end(); ++i)
	{
		RPFace *rp = i->second;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushAlphaList()
{
	std::sort(m_rpAlphaList.begin(), m_rpAlphaList.end());

	rp_rp_map_iter i = m_rpAlphaList.begin();
	for(; i != m_rpAlphaList.end(); ++i)
	{
		RPFace *rp = i->second;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushEmissiveList()
{
	std::sort(m_rpEmissiveList.begin(), m_rpEmissiveList.end());

	rp_rp_map_iter i = m_rpEmissiveList.begin();
	for(; i != m_rpEmissiveList.end(); ++i)
	{
		RPFace *rp = i->second;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}

void	CRenderer :: FlushEmissiveNoDepthList()
{
	std::sort(m_rpEmissiveNoDepthList.begin(), m_rpEmissiveNoDepthList.end());

	rp_rp_map_iter i = m_rpEmissiveNoDepthList.begin();
	for(; i != m_rpEmissiveNoDepthList.end(); ++i)
	{
		RPFace *rp = i->second;
		SetTexture(rp->tex);
		rp->DrawPri(m_device);
	}
}
void	CRenderer :: FlushEffectList()
{
	rp_tex_map_iter i = m_rpAlphaOPList.begin();
	for (; i != m_rpAlphaOPList.end(); ++i)
	{
		RPFace* rp = *i;
		SetTexture(rp->tex);
		SetMultiTextureMode(rp->mtPreset);
		m_device->SetRenderState(D3DRENDERSTATE_SRCBLEND, rp->srcAlphaMode);
		m_device->SetRenderState(D3DRENDERSTATE_DESTBLEND, rp->destAlphaMode);
		rp->DrawPri(m_device);
	}
}
void	CRenderer :: FlushBumpList()        { }
void	CRenderer :: FlushLmList()
{
	std::sort(m_rpLmList.begin(), m_rpLmList.end(), RPLmFaceTexPrtLess());

	rp_lmtex_map_iter i = m_rpLmList.begin();
	for(; i != m_rpLmList.end(); ++i)
	{
		RPLmFace *rp = *i;
		SetTexture(rp->tex);
		SetLmapTexture(rp->tex2);
		rp->DrawPri(m_device);
	}
}


void	CRenderer :: Flip(bool vertSync)
{
	++m_curFrame;
	++m_fpsFrameCount;
	if (Lock()) Unlock();
	g_3dDevice.ShowFrame(vertSync);
}


int GetCacheSize(int i)
{
	switch(i) {
	case 0:	return 32;
	case 1:	return 64;
	case 2:	return 128;
	case 3:	return 256;
	default: return -1;
	}

}

void	CRenderer :: StartFPSCount()
{
	m_fpsFrameCount = 0;
	m_fpsStartTick = timeGetTime();
}

void	CRenderer :: ShowInfo()
{
	char buf[128];
	DWORD timeDelta = timeGetTime() - m_fpsStartTick;
	float FPS = (1000.0f * m_fpsFrameCount) / timeDelta;

	if (timeDelta > 1000 && !(m_fpsFrameCount & 1)) {
		m_fpsFrameCount /= 2;
		m_fpsStartTick += timeDelta / 2;
	}

	sprintf(buf, "Total : %d - %d", 0, 0);
	TextOutScreen(0, 0, buf, 0, 12, 0);

	sprintf(buf, "Unused : %d - %d", m_unusedCacheSurfaces.size(), m_unusedCacheSurfaces.size() << 17);
	TextOutScreen(0, 16, buf, 0, 12, 0);

	sprintf(buf, "%.01f fps", FPS);
	TextOutScreen(m_width - 40, m_height - 14, buf, 0xffff, 12, 0);
}

int	CRenderer :: TextOutScreen(int x, int y, const char *text,
								   unsigned long colorRef, int fontHeight,
								   char fontPrint)
{
	g_3dDevice.GetRenderSurface()->AddRef();
	CDCSurface surface(g_3dDevice.GetWidth(), g_3dDevice.GetHeight(), g_3dDevice.GetRenderSurface());
	DrawDC dc(&surface);
	dc.SetFont(0, fontHeight, false);

	if (!fontPrint) {
		dc.SetTextColor(colorRef);
		dc.TextOutA(x, y, text, strlen(text));
	} else if (fontPrint == 1) {
		dc.SetTextColor(RGB(50, 50, 50));
		dc.TextOutA(x + 1, y + 1, text, strlen(text));
		dc.SetTextColor(colorRef);
		dc.TextOutA(x, y, text, strlen(text));
	}

	return 0;
}

tagSIZE * CRenderer::GetTextSize(char const *text, int textLen, int fontType, int fontHeight, bool bold)
{
	if (!text) return 0;

	if (!textLen) {
		textLen = strlen(text);
		if (!textLen)
			return 0;
	}

	tagSIZE t_size;

	g_3dDevice.GetRenderSurface()->AddRef();
	CDCSurface surface(g_3dDevice.GetWidth(), g_3dDevice.GetHeight(), g_3dDevice.GetRenderSurface());
	DrawDC dc(&surface);
	dc.SetFont(fontType, fontHeight, bold);
	dc.GetTextExtentPoint32A(text, textLen, &t_size);
	return &t_size;
}

void	CRenderer :: FogSwitch(int sw)
{
	if (g_3dDevice.GetDeviceDesc().dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX)
		m_isVertexFog = TRUE;
	else
		m_isVertexFog = FALSE;

	m_isFoggy = sw;
	m_device->SetRenderState(D3DRENDERSTATE_FOGENABLE, sw);
	m_fogChanged = TRUE;
	g_fogPara.useRange = TRUE;
	g_fogPara.mode = D3DFOG_LINEAR;
}

void	CRenderer :: DoFog()
{
	if (!m_fogChanged)
		return;

	float	fogstart, fogend;

	if (m_isFoggy) {
		D3DMATRIX matWorld, matProj;

		fogstart = ((FARPLANE - NEARPLANE) * g_fogPara.start) + NEARPLANE;
		fogend = ((FARPLANE - NEARPLANE) * g_fogPara.end) + NEARPLANE;
		int vertexFog = (g_3dDevice.GetDeviceDesc().dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX) != 0;

		D3DUtil_SetIdentityMatrix(matWorld);

		D3DUtil_SetProjectionMatrix(matProj, VFOV * (3.141592f / 180.0f), m_aspectRatio, NEARPLANE, FARPLANE);
		m_device->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
		m_device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);
		m_device->SetTransform(D3DTRANSFORMSTATE_VIEW, &matWorld);
		m_device->SetRenderState(D3DRENDERSTATE_FOGCOLOR, g_fogPara.color);

		if ((D3DFOGMODE)g_fogPara.mode == D3DFOG_LINEAR) {
			if (vertexFog) {
				m_device->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD *)&fogstart);
				m_device->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD *)&fogend);
				m_device->SetRenderState(D3DRENDERSTATE_FOGDENSITY, *(DWORD *)&g_fogPara.density);
				m_device->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
				m_device->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, g_fogPara.mode);
			} else {
				m_device->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
			}
		} else {
			if (vertexFog) {
				m_device->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);
				m_device->SetRenderState(D3DRENDERSTATE_FOGVERTEXMODE, g_fogPara.mode);
			} else {
				ErrorMsg("Your graphic accelerator does not support fog effect.");
			}
			m_device->SetRenderState(D3DRENDERSTATE_FOGDENSITY, *(DWORD *)&g_fogPara.density);
		}



		m_fogChanged = m_fogChanged;
		if (vertexFog && g_fogPara.useRange)
			m_device->SetRenderState(D3DRENDERSTATE_RANGEFOGENABLE, TRUE);
	}

	m_fogChanged = FALSE;
}



void	CRenderer :: SetMultiTextureMode(int nMode)
{

	switch (nMode) {


		case 0:
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			m_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
			break;

	case 1:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		break;


	case 2:
		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		break;

	case 3:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADD );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		break;

	case 4:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		break;

	case 5:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 6:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 7:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_ALPHAREPLICATE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 8:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 9:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 10:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 11:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 12:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE | D3DTA_COMPLEMENT );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADD );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 13:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_ADD );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 14:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;

	case 15:

		{
			m_device->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

			m_device->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED );
			m_device->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_device->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
		break;
	}
}

void	CRenderer :: BuildRay(int sx, int sy, const vector3d &from, const matrix &invViewMatrix, ray3d *ray)
{
	float		xv, yv;
	vector3d	dirView;
	matrix		ivtm = invViewMatrix;

	xv = -1+(float(sx)/m_halfWidth);
	yv =  1-(float(sy)/m_halfHeight);
	ray->org = from;
	ivtm.v41 = ivtm.v42 = ivtm.v43 = 0;
	dirView.Set(xv*m_halfWidth/m_hpc, -yv*m_halfHeight/m_vpc, 1);
	ray->dir.MatrixMult(dirView, ivtm);
	ray->dir.Normalize();
}

int CRenderer::SaveBmp(char const *fName, CBitmapRes *bitmap)
{
	if (!Lock())
		return 0;

	FILE *file = fopen(fName, "wb");
	if (!file)
		return 0;

	BITMAPFILEHEADER FileHdr;
	FileHdr.bfType = 0x4d42;
	FileHdr.bfReserved1 = 0;
	FileHdr.bfReserved2 = 0;
	FileHdr.bfSize = (m_width * m_height + 18) * 3;
	FileHdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	fwrite(&FileHdr, sizeof(BITMAPFILEHEADER), 1, file);

	BITMAPINFO InfoHdr;
	InfoHdr.bmiHeader.biPlanes = 1;
	InfoHdr.bmiHeader.biXPelsPerMeter = 0;
	InfoHdr.bmiHeader.biYPelsPerMeter = 0;
	InfoHdr.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	InfoHdr.bmiHeader.biCompression = 0;
	InfoHdr.bmiHeader.biSizeImage = 0;
	InfoHdr.bmiHeader.biClrImportant = 0;
	InfoHdr.bmiHeader.biClrUsed = 0;
	InfoHdr.bmiHeader.biBitCount = 24;
	InfoHdr.bmiHeader.biWidth = m_width;
	InfoHdr.bmiHeader.biHeight = m_height;
	fwrite(&InfoHdr, sizeof(BITMAPINFOHEADER), 1, file);

	unsigned long rMask = g_3dDevice.GetRBitMask();
	unsigned long gMask = g_3dDevice.GetGBitMask();
	unsigned long bMask = g_3dDevice.GetBBitMask();
	int rShiftR = g_3dDevice.GetRShiftR();
	int gShiftR = g_3dDevice.GetGShiftR();
	int bShiftR = g_3dDevice.GetBShiftR();
	int rShiftL = g_3dDevice.GetRShiftL();
	int gShiftL = g_3dDevice.GetGShiftL();
	int bShiftL = g_3dDevice.GetBShiftL();
	unsigned short transColor = (unsigned short)(((255 >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR())
							  + ((0 >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR())
							  + ((255 >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR()));
	unsigned char tempBuf[0x2a30];
	unsigned long argb;
	int i;

	for (i = m_height - 1; i >= 0; i--) {
		unsigned char *src = (unsigned char *)m_lpSurfacePtr + m_lPitch * i;
		if (m_bRGBBitCount == 16) {
			for (int j = 0; j < m_width; j++) {
				if (j >= m_width - bitmap->m_width && i >= m_height - bitmap->m_height) {
					int y = bitmap->m_height - m_height + i;
					int x = bitmap->m_width - m_width + j;
					if (x >= 0 && x < bitmap->m_width && y >= 0 && y < bitmap->m_height) {
						argb = bitmap->m_data[x + y * bitmap->m_width];
						unsigned long r = (argb >> 16) & 0xff, g = (argb >> 8) & 0xff, b = argb & 0xff;
						if (transColor != g_3dDevice.EncodePixel(r, g, b)) {
							tempBuf[j * 3 + 0] = (unsigned char)b;
							tempBuf[j * 3 + 1] = (unsigned char)g;
							tempBuf[j * 3 + 2] = (unsigned char)r;
							continue;
						}
					}
				}

				unsigned long color = *(unsigned short *)&src[j * 2];
				tempBuf[j * 3 + 0] = (unsigned char)(((color & bMask) >> bShiftR) << bShiftL);
				tempBuf[j * 3 + 1] = (unsigned char)(((color & gMask) >> gShiftR) << gShiftL);
				tempBuf[j * 3 + 2] = (unsigned char)(((color & rMask) >> rShiftR) << rShiftL);
			}
		} else {
			for (int j = 0; j < m_width; j++) {
				if (j >= m_width - bitmap->m_width && i >= m_height - bitmap->m_height) {
					int y = bitmap->m_height - m_height + i;
					int x = bitmap->m_width - m_width + j;
					if (x >= 0 && x < bitmap->m_width && y >= 0 && y < bitmap->m_height) {
						argb = bitmap->m_data[x + y * bitmap->m_width];
						if ((argb & 0xffffff) != 0xff00ff) {
							memcpy(&tempBuf[j * 3], &argb, 3);
							continue;
						}
					}
				}

				memcpy(&tempBuf[j * 3], &src[j * 4], 3);
			}
		}
		fwrite(tempBuf, m_width * 3, 1, file);
	}

	g_3dDevice.GetRenderSurface()->Unlock(NULL);
	fclose(file);
	return 1;
}

void	CRenderer :: ScreenEffect()
{
	{
		char msg[256];
		sprintf(msg, "Hello! This is Kakarot!");
	}

	if (!g_session.m_isEffectOn)
		return;

	if (g_se_go > 0)
	{
		if (Lock())
		{
			BYTE *saver;
			saver = (BYTE *) malloc(m_lPitch);

			if (g_se_type == 2)
			{
				short tempSin;
				g_se_count += 7;
				if (g_se_count >= 360)
					g_se_count -= 360;

				tempSin = g_se_count;

				for (int i = m_height - 1; i >= 0; --i)
				{
					{
						BYTE *destPtr = ((BYTE *) m_lpSurfacePtr) + i * m_lPitch;
						int add;

						add = g_sinTable[tempSin] * m_width * 0.01f;
						if (i == 0 && add < 0)
							add = 0;

						if (add != 0)
						{
							if (m_bRGBBitCount == 16)
								add *= 2;
							else
								add *= 4;

							memcpy(saver, destPtr, m_lPitch);

							if (add < 0)
								memcpy(destPtr, saver - add, m_lPitch + add);
							else
								memcpy(destPtr + add, saver, m_lPitch - add);
						}
					}

					tempSin += 2;
					if (tempSin >= 360)
						tempSin -= 360;
				}
			}
			else
			{
				if (g_se_count < 2)
					g_se_count = 2;

				for (int i = m_height - 1; i >= 0; --i)
				{
					BYTE *destPtr = ((BYTE *) m_lpSurfacePtr) + i * m_lPitch;
					BYTE ir;

					if (g_se_count <= 10)
						ir = g_se_count;
					else
						ir = 20 - g_se_count;

					if (i == m_height - 1)
						memcpy(saver, destPtr, m_lPitch);

					if (i % ir == 0)
					{
						memcpy(saver, destPtr, m_lPitch);
					}
					else
					{
						int add;
						add = (rand() % 8) * 16;

						if (g_se_type == 0)
							memcpy(destPtr, saver, m_lPitch);
						else if (g_se_type == 1)
							memcpy(destPtr + add, saver, m_lPitch - add);
					}
				}

				g_se_count += 2;
				if (g_se_count < 2)
					g_se_count = 2;
				if (g_se_count > 16)
				{
					g_se_count = 2;
					g_se_go--;
					if (g_se_go < 0)
						g_se_go = 0;
				}
			}

			free(saver);
			Unlock();
		}
	}
}
#include "rag_budget_fill.h"
void	CRenderer :: DrawBattleFieldIcon()
{
	RAG_BUDGET_FILL(10)
	RECT rect;
	float oow = 1.0f / NEARPLANE - 0.0001f;
	int m_h15;
	int m_w113;
	int RGBA;
	int x;
	int y;
	int eachNum[3];
	int number;
	int numberCnt;
	int yspace;
	int i;
	char numDigit;

	if (g_session.m_camp_A < 0 || g_session.m_camp_B < 0)
		return;

	yspace = 26;
	RGBA = (205 << 24) + (255 << 16) + (255 << 8) + 255;
	m_h15 = 48;
	m_w113 = 48;

	rect.left = m_width - 232 - m_w113;
	rect.right = m_width - 232;
	rect.top = yspace;
	rect.bottom = m_h15 + yspace;

	((CMode *) g_modeMgr.GetGameMode())->DrawTexture(&rect, 0, oow, RGBA,
		g_texMgr.GetTexture("effect\\\xbb\xe7\xc0\xda(A).bmp", false));

	rect.left = m_width - 232 - m_w113;
	rect.right = m_width - 232;
	rect.top = m_h15 + 10 + yspace;
	rect.bottom = (m_h15 * 2) + 10 + yspace;

	((CMode *) g_modeMgr.GetGameMode())->DrawTexture(&rect, 0, oow, RGBA,
		g_texMgr.GetTexture("effect\\\xb5\xb6\xbc\xf6\xb8\xae(B).bmp", false));

	if (g_session.m_camp_A > 9 || g_session.m_camp_B > 9)
		numDigit = 2;
	else
		numDigit = 1;

	CActRes *rank_act = (CActRes *) g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\RankFont.act", false);
	CSprRes *rank_spr = (CSprRes *) g_resMgr().Get("\xc0\xcc\xc6\xd1\xc6\xae\\RankFont.spr", false);

	number = g_session.m_camp_A;
	if (number < 0) number = 0;

	numberCnt = 1;
	if (number > 9) numberCnt = 2;

	eachNum[0] = 0;
	eachNum[1] = 0;
	eachNum[0] = number % 10;
	number /= 10;
	eachNum[1] = number % 10;
	number /= 10;

	x = m_width - 150;
	y = 24 + yspace;
	for (i = 0; i < numberCnt; i++)
	{
		if (numDigit > 1)
			((CMode *) g_modeMgr.GetGameMode())->DrawSprite(x + (-i * 32) - 25, y, rank_act, rank_spr, eachNum[i], 0, 1.0f, 0.0f, 0xffffffff);
		else
			((CMode *) g_modeMgr.GetGameMode())->DrawSprite(x + (-i * 32) - 25 - 32, y, rank_act, rank_spr, eachNum[i], 0, 1.0f, 0.0f, 0xffffffff);
	}

	number = g_session.m_camp_B;
	if (number < 0) number = 0;

	numberCnt = 1;
	if (number > 9) numberCnt = 2;

	eachNum[0] = 0;
	eachNum[1] = 0;
	eachNum[0] = number % 10;
	number /= 10;
	eachNum[1] = number % 10;
	number /= 10;

	x = m_width - 150;
	y = 82 + yspace;
	for (i = 0; i < numberCnt; i++)
	{
		if (numDigit > 1)
			((CMode *) g_modeMgr.GetGameMode())->DrawSprite(x + (-i * 32) - 25, y, rank_act, rank_spr, eachNum[i], 0, 1.0f, 0.0f, 0xffffffff);
		else
			((CMode *) g_modeMgr.GetGameMode())->DrawSprite(x + (-i * 32) - 25 - 32, y, rank_act, rank_spr, eachNum[i], 0, 1.0f, 0.0f, 0xffffffff);
	}
}
void	CRenderer :: DrawAgeRate()
{
	RAG_BUDGET_FILL(55)
	SYSTEMTIME sTime;
	GetLocalTime(&sTime);

	if (sTime.wMinute == 0 && sTime.wSecond == 0 && g_AgeRate == 0)
	{
		g_AgeRate = 1;
		g_AgeAlpha = 0;
	}
	else if (sTime.wMinute == 0 && sTime.wSecond >= 10 && g_AgeRate == 1)
	{
		g_AgeRate = 2;
	}

	if (g_AgeRate >= 1)
	{
		RECT rect;
		float oow = 1.0f / NEARPLANE - 0.0001f;
		int m_h15 = m_height * 0.3f;
		int m_w113 = m_width * 0.23f;
		int RGBA;

		rect.bottom = m_h15;
		rect.left = m_width - 140 - m_w113;
		rect.right = m_width - 140;
		rect.top = 0;

		if (g_AgeRate == 1)
		{
			if (g_AgeAlpha < 220)
				g_AgeAlpha++;
		}
		else
		{
			if (g_AgeAlpha > 0)
				g_AgeAlpha--;
			else
				g_AgeRate = 0;
		}

		RGBA = (g_AgeAlpha << 24) + 0xffffff;

		((CMode *) g_modeMgr.GetGameMode())->DrawTexture(&rect, 0, oow, RGBA,
			g_texMgr.GetTexture("\xc0\xaf\xc0\xfa\xc0\xce\xc5\xcd\xc6\xe4\xc0\xcc\xbd\xba\\t_12\xbc\xbc.tga", false));
	}
}




int CRenderer::SaveJPG(char const *fName, CBitmapRes *bitmap)
{
	if (!Lock()) return 0;

	unsigned char *colorBuf = new unsigned char[m_width * m_height * 3];
	unsigned long rMask = g_3dDevice.GetRBitMask();
	unsigned long gMask = g_3dDevice.GetGBitMask();
	unsigned long bMask = g_3dDevice.GetBBitMask();
	int rShiftR = g_3dDevice.GetRShiftR();
	int gShiftR = g_3dDevice.GetGShiftR();
	int bShiftR = g_3dDevice.GetBShiftR();
	int rShiftL = g_3dDevice.GetRShiftL();
	int gShiftL = g_3dDevice.GetGShiftL();
	int bShiftL = g_3dDevice.GetBShiftL();
	unsigned long transColor = (((255 >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR()) + ((0 >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR()) + ((255 >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR())) & 0xffff;
	unsigned long argb;
	int i;


	for (i = m_height - 1; i >= 0; i--) {

		unsigned char *src = (unsigned char *)m_lpSurfacePtr + m_lPitch * i;
		unsigned char *dst = colorBuf + i * m_width * 3;

		if (m_bRGBBitCount == 16) {

			for (int j = 0; j < m_width; j++) {

				if (j >= m_width - bitmap->m_width && i >= m_height - bitmap->m_height) {
					int y = bitmap->m_height - m_height + i;
					int x = bitmap->m_width - m_width + j;
					if (x >= 0 && x < bitmap->m_width && y >= 0 && y < bitmap->m_height) {
						argb = bitmap->m_data[x + y * bitmap->m_width];


						int r = (argb >> 16) & 0xff, g = (argb >> 8) & 0xff, b = argb & 0xff;

						if (transColor != ((((r >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR()) + ((g >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR()) + ((b >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR())) & 0xffff)) {
							dst[j * 3 + 0] = b;
							dst[j * 3 + 1] = g;
							dst[j * 3 + 2] = r;
							continue;
						}
					}
				}


				unsigned long color = *(unsigned short *)&src[j * 2];


				dst[j * 3 + 0] = (unsigned char)(((color & bMask) >> bShiftR) << bShiftL);
				dst[j * 3 + 1] = (unsigned char)(((color & gMask) >> gShiftR) << gShiftL);
				dst[j * 3 + 2] = (unsigned char)(((color & rMask) >> rShiftR) << rShiftL);
			}
		} else {
			for (int j = 0; j < m_width; j++) {
				if (j >= m_width - bitmap->m_width && i >= m_height - bitmap->m_height) {
					int y = bitmap->m_height - m_height + i;
					int x = bitmap->m_width - m_width + j;
					if (x >= 0 && x < bitmap->m_width && y >= 0 && y < bitmap->m_height) {
						argb = bitmap->m_data[x + y * bitmap->m_width];
						if ((argb & 0xffffff) != 0xff00ff) {
							memcpy(&dst[j * 3], &argb, 3);
							continue;
						}
					}
				}


				memcpy(&dst[j * 3], &src[j * 4], 3);
			}
		}
	}

	g_3dDevice.GetRenderSurface()->Unlock(NULL);
	JPEG_CORE_PROPERTIES JPGimg;
	memset(&JPGimg, 0, sizeof(JPEG_CORE_PROPERTIES));
	if (ijlInit(&JPGimg) != IJL_OK)
		return 0;

	JPGimg.DIBWidth = m_width;
	JPGimg.DIBHeight = m_height;
	JPGimg.DIBBytes = colorBuf;
	JPGimg.DIBPadBytes = IJL_DIB_PAD_BYTES(m_width, 3);
	JPGimg.DIBChannels = 3;
	JPGimg.DIBColor = IJL_BGR;
	JPGimg.JPGFile = fName;
	JPGimg.JPGWidth = m_width;
	JPGimg.JPGHeight = m_height;


	if (ijlWrite(&JPGimg, IJL_JFILE_WRITEWHOLEIMAGE) != IJL_OK) {
		delete colorBuf;
		return 0;
	}

	delete colorBuf;
	return 1;
}


void	CRenderer :: AddRP(RPFace *rp, int renderFlag)
{
	if (m_isVertexFog) rp->InitSpecular();

	if (renderFlag & RF_ALPHA)
	{
		if (renderFlag & RF_ALPHAOP)
		{
			m_rpAlphaOPList.push_back(rp);
		}
		else if (renderFlag & RF_EMISSIVE)
		{
			if (renderFlag & RF_DEPTHWRITE)
				m_rpEmissiveDepthList.push_back(rp_float_pair(((rp->verts[0].oow - rp->verts[1].oow) / 2.0f) + rp->verts[1].oow, rp));
			else if (renderFlag & RF_NODEPTHCHECK)
				m_rpEmissiveNoDepthList.push_back(rp_float_pair(((rp->verts[0].oow - rp->verts[1].oow) / 2.0f) + rp->verts[1].oow, rp));
			else
				m_rpEmissiveList.push_back(rp_float_pair(((rp->verts[0].oow - rp->verts[1].oow) / 2.0f) + rp->verts[1].oow, rp));
		}
		else
		{
			if (renderFlag & RF_DEPTHWRITE)
				m_rpAlphaDepthList.push_back(rp_float_pair(((rp->verts[0].oow - rp->verts[1].oow) / 2.0f) + rp->verts[1].oow, rp));
			else if (renderFlag & RF_NODEPTHCHECK)
				m_rpAlphaNoDepthList.push_back(rp_float_pair(((rp->verts[0].oow - rp->verts[1].oow) / 2.0f) + rp->verts[1].oow, rp));
			else
				m_rpAlphaList.push_back(rp_float_pair(rp->verts[0].oow, rp));
		}
	}
	else
	{
		if(renderFlag & RF_GROUND)
			m_rpLMGroundList.push_back(rp);
		else if(renderFlag & RF_LIGHT)
			m_rpLMLightList.push_back(rp);
		else
			m_rpFaceList.push_back(rp);
	}
}

void	CRenderer :: AddRP(RPTriFace *rp, int renderFlag)
{
	RPFace& rpFace = BorrowNullRP();

	rpFace.verts			= rp->verts;
	rpFace.numVerts			= 3;
	rpFace.tex				= rp->tex;
	rpFace.mtPreset			= 0;
	rpFace.srcAlphaMode		= rp->srcAlphaMode;
	rpFace.destAlphaMode	= rp->destAlphaMode;

	AddRP(&rpFace, renderFlag);
}


void	CRenderer :: AddRawRP(RPRaw *rp, int renderFlag)
{
	if(renderFlag & RF_ALPHA)	{
		m_rpRawAlphaList.push_back(rp);
	}
	else
	if( renderFlag & RF_EMISSIVE ) {
		m_rpRawEmissiveList.push_back(rp);
	}
	else
		m_rpRawList.push_back(rp);
}

void		CRenderer :: AddLmRP(RPLmFace* rp, RfLmType renderFlag)
{
	if (m_isVertexFog) rp->InitSpecular();

	switch(renderFlag)
	{

	case RF_LM_LIGHTMAP:
		{
			m_rpLmList.push_back(rp);
		}
		break;

	case RF_LM_BUMPMAP:
		m_rpBumpFaceList.push_back(rp);
		break;
	}

}

RPFace&		CRenderer :: BorrowNullRP()
{
	if(m_rpNullFaceListIter == m_rpNullFaceList.end())
	{
		m_rpNullFaceList.push_back(RPFace(NULL, 0));

		m_rpNullFaceListIter = m_rpNullFaceList.end();
		return m_rpNullFaceList.back();
	}
	else
	{

		RPFace& rpFace = *m_rpNullFaceListIter;

		++m_rpNullFaceListIter;

		rpFace.numIndices = 0;
		return rpFace;
	}
}

RPQuadFace&	CRenderer :: BorrowQuadRP()
{
	if(m_rpQuadFaceListIter == m_rpQuadFaceList.end())
	{
		m_rpQuadFaceList.push_back(RPQuadFace());
		m_rpQuadFaceListIter = m_rpQuadFaceList.end();
		return m_rpQuadFaceList.back();
	}
	else
	{

		RPQuadFace& rpQuadFace = *m_rpQuadFaceListIter;

		++m_rpQuadFaceListIter;

		sAssert(rpQuadFace.verts == rpQuadFace.m_verts);
		return rpQuadFace;
	}
}

RPLmQuadFace&	CRenderer :: BorrowLmQuadRP()
{
	if(m_rpLmQuadFaceListIter == m_rpLmQuadFaceList.end()) {

		m_rpLmQuadFaceList.push_back(RPLmQuadFace());

		m_rpLmQuadFaceListIter = m_rpLmQuadFaceList.end();

		return m_rpLmQuadFaceList.back();
	} else {

		RPLmQuadFace& rp = *m_rpLmQuadFaceListIter;

		++m_rpLmQuadFaceListIter;

		return rp;
	}
}

tlvertex3d*	CRenderer :: BorrowVerts(UINT vertCount)
{
	if(m_vertBuffer.size() + vertCount > m_vertBuffer.capacity())
	{
		tlvertex3d* oldStart = m_vertBuffer.begin();
		tlvertex3d* oldEnd = m_vertBuffer.end();

		m_vertBuffer.reserve(m_vertBuffer.capacity() * 2);

		tlvertex3d* newStart = m_vertBuffer.begin();

		for(std::list<RPFace>::iterator i = m_rpNullFaceList.begin(); i != m_rpNullFaceListIter; ++i)
		{
			if(i->verts >= oldStart && i->verts < oldEnd)
			{
				i->verts = newStart + (i->verts - oldStart);
			}
		}
	}

	tlvertex3d* verts = m_vertBuffer.end();

	m_vertBuffer.resize(m_vertBuffer.size() + vertCount);

	return verts;
}

void	CRenderer :: BackupFrame()
{
	g_3dDevice.BackupFrame();
}

void	CRenderer :: RestoreFrame()
{
	g_3dDevice.RestoreFrame();
}

void	CRenderer :: SetTexture(CTexture *tex)
{
	if (m_oldTexture == tex)
	{
		return;
	}

	if (tex == NULL)
		m_device->SetTexture(0, NULL);
	else
		m_device->SetTexture(0, tex->m_pddsSurface);
	m_oldTexture = tex;
}

void	CRenderer :: SetLmapTexture(CTexture *tex)
{
	if (m_oldLmapTexture == tex)
		return;

	if (tex == NULL)
		m_device->SetTexture(1, NULL);
	else
		m_device->SetTexture(1, tex->m_pddsSurface);
	m_oldLmapTexture = tex;
}

void	CRenderer :: ProjectVertex(const vector3d &src, const matrix &vtm, float *x, float *y, float *oow)
{
	*oow = 1.0f/(src.x*vtm.v13 + src.y*vtm.v23 + src.z*vtm.v33 + vtm.v43);
	*x = (src.x*vtm.v11 + src.y*vtm.v21 + src.z*vtm.v31 + vtm.v41) * (*oow) * m_hpc + m_xoffset;
	*y = (src.x*vtm.v12 + src.y*vtm.v22 + src.z*vtm.v32 + vtm.v42) * (*oow) * m_vpc + m_yoffset;
}

void	CRenderer :: ProjectVertex(const vector3d &src, const matrix &vtm, tlvertex3d *vert)
{
	vert->oow = 1.0f/(src.x*vtm.v13 + src.y*vtm.v23 + src.z*vtm.v33 + vtm.v43);
	vert->x = (src.x*vtm.v11 + src.y*vtm.v21 + src.z*vtm.v31 + vtm.v41) * vert->oow * m_hpc + m_halfWidth;
	vert->y = (src.x*vtm.v12 + src.y*vtm.v22 + src.z*vtm.v32 + vtm.v42) * vert->oow * m_vpc + m_halfHeight;
	vert->z = FARPLANE/(FARPLANE-NEARPLANE)*(1/ vert->oow-NEARPLANE)* vert->oow;
}

float	CRenderer :: GetPixelRatio(float oow)
{
	return	m_hpc * oow / 7;
}

float	CRenderer :: GetScreenXFactor()
{
	return m_screenXFactor;
}

float	CRenderer :: GetScreenYFactor()
{
	return m_screenYFactor;
}

DWORD	GetSpecularFromOow(float oow)
{
	float factor = (((1/oow)-NEARPLANE)/(FARPLANE-NEARPLANE));
	if (g_fogPara.end-g_fogPara.start!=0)
		factor = ((factor-g_fogPara.start)/(g_fogPara.end-g_fogPara.start));
	if (factor<0) factor=0;
	if (factor>1) factor=1;
	return ((0x000000ff&((DWORD)((1 - factor) * 255)))<<24);
}

float	GetZFromOow(float oow)
{
	return	FARPLANE/(FARPLANE-NEARPLANE)*(1/oow-NEARPLANE)*oow;
}

float	GetOowFromZ(float z)
{
	return	(FARPLANE-(FARPLANE-NEARPLANE)*z)/(FARPLANE*NEARPLANE);
}
