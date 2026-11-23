#include "common.h"
#include "Texture.h"
#include "Device.h"
#include "D3dutil.h"
#include "Renderer.h"
#include "../resource/Sprite.h"

#include <windows.h>
#include <ddraw.h>

float CTexture::m_uOffset = 0.5f;
float CTexture::m_vOffset = 0.5f;

extern CRenderer *g_renderer;
extern DWORD g_ddsdFlagMipmap;
extern DWORD g_ddsCapsMipmap;
DDPIXELFORMAT g_texPixelFormat[PF_LAST];


static HRESULT WINAPI TextureSearchCallback(DDPIXELFORMAT* pddpf, VOID* param);


CSurface :: CSurface(DWORD w, DWORD h)
:	m_pddsSurface(NULL)
{
	Create(w, h);
}

CSurface :: CSurface(DWORD w, DWORD h, IDirectDrawSurface7 * surface)
:	m_w(w), m_h(h), m_pddsSurface(surface)
{
}

CSurface :: ~CSurface()
{
	SAFE_RELEASE(m_pddsSurface);
}

void	CSurface :: Update(int x, int y, int width, int height, DWORD *image, bool blackkey, int lPitch)
{
	if(!m_pddsSurface) return;
	if(width == 0) return;
	if(height == 0) return;

	if(lPitch == 0) lPitch = width;

	RECT rtDest = { x, y, x+width, y+height };
	DWORD argb;
	DDSURFACEDESC2 ddsd = { sizeof(DDSURFACEDESC2), 0 };
	if(FAILED(m_pddsSurface->Lock(&rtDest, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL)))
		return;

	DWORD maskKey = ((0 >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR())
				  + ((255 >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR())
				  + ((255 >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR());

	if(ddsd.ddpfPixelFormat.dwRGBBitCount == 16) {
		WORD* surface;
		surface = (WORD*)ddsd.lpSurface;
		for(int i=0; i<height; ++i) {
			for(int j=0; j<width; ++j) {
				argb = image[j];
				DWORD color = ((((argb >> 8) & 0xff) >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR())
							+ ((((argb >> 16) & 0xff) >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR())
							+ (((int)(argb & 0xff) >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR());
				if((WORD)color != (WORD)maskKey)
					surface[j] = (WORD)color;
			}
			surface = (WORD*)((BYTE*)surface + ddsd.lPitch);
			image += lPitch;
		}
	} else {
		DWORD* surface = (DWORD*)ddsd.lpSurface;
		for(int i=0; i<height; ++i) {
			for(int j=0; j<width; ++j) {
				DWORD color = image[j];
				if((color & 0xffffff) != 0xff00ff)
					surface[j] = color;
			}
			surface = (DWORD*)((BYTE*)surface + ddsd.lPitch);
			image += lPitch;
		}
	}

	m_pddsSurface->Unlock(NULL);
}

void	CSurface :: ClearSurface(RECT* rect, DWORD color)
{
	if (!m_pddsSurface) return;

	DDBLTFX bltfx;
	bltfx.dwSize = sizeof(DDBLTFX);

	bltfx.dwFillColor = (((((color >> 8) & 0xff) >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR()) + ((((color >> 16) & 0xff) >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR()) + (((int)(color & 0xff) >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR())) & 0xffff;

	m_pddsSurface->Blt(rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
}

void	CSurface :: DrawSurface(int x, int y, int width, int height, DWORD color)
{
	int rwidth = g_3dDevice.GetWidth();
	RECT r;
	r.right  = width;
	r.left   = 0;
	r.top    = 0;
	r.bottom = height;

	if (x + width > rwidth)
		r.right = rwidth - x;
	int rheight = g_3dDevice.GetHeight();
	if (y + height > rheight)
		r.bottom = rheight - y;

	if (x < 0) { r.left = -x; x = 0; }
	if (y < 0) { r.top  = -y; y = 0; }

	if (r.right > r.left && r.bottom > r.top)
		g_3dDevice.GetRenderSurface()->BltFast(x, y, m_pddsSurface, &r, DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
}

void	CSurface :: DrawSurfaceStretch(int x, int y, int width, int height)
{
	RECT dest = { 0, 0, width, height };
	g_3dDevice.GetRenderSurface()->Blt(&dest, m_pddsSurface, NULL, DDBLT_WAIT, NULL );
}

bool CSurface::Create(DWORD w, DWORD h)
{
	DDSURFACEDESC2 ddsd;

	m_w = w + ((-(int)(w & 3)) & 3);
	m_h = h + ((-(int)(h & 3)) & 3);

	SAFE_RELEASE(m_pddsSurface);

	ZeroMemory(&ddsd.dwHeight, sizeof(ddsd) - 8);
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
	ddsd.dwWidth = m_w;
	ddsd.dwHeight = m_h;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.ddpfPixelFormat = g_3dDevice.GetFrameBufferDesc()->ddpfPixelFormat;
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue =
		ddsd.ddpfPixelFormat.dwBBitMask | ddsd.ddpfPixelFormat.dwRBitMask;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue =
		ddsd.ddpfPixelFormat.dwBBitMask | ddsd.ddpfPixelFormat.dwRBitMask;

	if (FAILED(g_3dDevice.GetDirectDraw()->CreateSurface(&ddsd, &m_pddsSurface, NULL)))
		return false;
	return true;
}

CSurfaceWallpaper :: CSurfaceWallpaper(DWORD w, DWORD h)
:	CSurface(w, h)
{
}

void	CSurfaceWallpaper :: Update(int x, int y, int width, int height, DWORD *imgData, bool blackkey, int lPitch)
{
	int w = width;
	int h = height;
	CSurface	imageSurface(w, h);
	imageSurface.Update(0, 0, w, h, imgData, false, 0);
	m_pddsSurface->Blt(NULL, imageSurface.GetDDSurface(), NULL, DDBLT_WAIT, NULL);
}

CTexture :: CTexture(DWORD w, DWORD h, PixelFormat pf)
:	m_lockCnt(0), m_timeStamp(0)
{
	m_texName[0] = 0;

	if( pf==PF_BUMP ) CreateBump(w, h);
	else {
		Create(w, h, pf);
		SetUVAdjust(w, h);
	}
}

CTexture :: CTexture(DWORD w, DWORD h, PixelFormat pf, IDirectDrawSurface7 * surface)
:	m_pf(pf), m_lockCnt(0), m_timeStamp(0)
{
	m_w = w;
	m_h = h;

	if( pf==PF_BUMP ) CreateBump(w, h, surface);
	else {
		m_pddsSurface = surface;
	}
}

void	CTexture :: ClearSurface(RECT* rect, DWORD color)
{
	if (!m_pddsSurface) return;

	DDBLTFX bltfx;
	bltfx.dwSize = sizeof(DDBLTFX);

	bltfx.dwFillColor = (((((color >> 8) & 0xff) >> g_3dDevice.GetGShiftL()) << g_3dDevice.GetGShiftR()) + ((((color >> 16) & 0xff) >> g_3dDevice.GetBShiftL()) << g_3dDevice.GetBShiftR()) + (((int)(color & 0xff) >> g_3dDevice.GetRShiftL()) << g_3dDevice.GetRShiftR())) & 0xffff;

	m_pddsSurface->Blt(rect, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &bltfx);
}

long	CTexture :: Lock()
{
	UpdateStamp();
	return ++m_lockCnt;
}

long	CTexture :: Unlock()
{
	return --m_lockCnt;
}

void	CTexture :: SetUVAdjust(DWORD width, DWORD height)
{
	while(width > m_w) width = width/2;
	while(height > m_h) height = height/2;

	m_updateWidth = width;
	m_updateHeight = height;
}

void CTexture::UpdateMipmap(tagRECT &src)
{
	DDSCAPS2 ddsCaps;
	ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
	IDirectDrawSurface7 *pddsSurface = m_pddsSurface;
	ddsCaps.dwCaps2 = 0;
	RECT dest = src;
	ZeroMemory(&ddsCaps.dwCaps3, sizeof(DWORD) * 2);

	while (pddsSurface->GetAttachedSurface(&ddsCaps, &pddsSurface) >= 0)
	{
		LONG top = dest.top >> 1;
		LONG right = dest.right >> 1;
		LONG bottom = dest.bottom >> 1;
		LONG left = dest.left >> 1;
		dest.top = top;
		dest.bottom = bottom;
		dest.left = left;
		dest.right = right;

		if (dest.left != dest.right && dest.top != dest.bottom)
		{
			if (pddsSurface->Blt(&dest, m_pddsSurface, &src, DDBLT_ASYNC, (DDBLTFX *)0) < 0)
				pddsSurface->Blt(&dest, m_pddsSurface, &src, DDBLT_WAIT, (DDBLTFX *)0);
		}
		pddsSurface->Release();
	}
}

void	CTexture :: Update(int x, int y, int width, int height, DWORD *image, bool blackkey, int lPitch)
{
	SetUVAdjust(width, height);
	m_blackkey = blackkey;
	if(!m_pddsSurface) return;
	if(image == NULL) return;

	if(lPitch == 0) lPitch = width;

	DDSURFACEDESC2 ddsd = { sizeof(DDSURFACEDESC2), 0 };
	if(FAILED(m_pddsSurface->GetSurfaceDesc(&ddsd)))
		return;

	int iter_x, iter_y;
	for(iter_x = 1; width / iter_x > ddsd.dwWidth; ++iter_x);
	for(iter_y = 1; height / iter_y > ddsd.dwHeight; ++iter_y);

	x = x / iter_x;
	y = y / iter_y;
	width = width / iter_x;
	height = height / iter_y;

	if(width == 0 || height == 0) return;

	RECT rtDest = { x, y, x+width, y+height };

	if(FAILED(m_pddsSurface->Lock(&rtDest, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL))) return;

	DWORD color;
	WORD* surface;
	DWORD* surface32;
	int i, j, imageIter;
	switch(m_pf)
	{
	case PF_A1R5G5B5:
		surface = (WORD*)ddsd.lpSurface;
		for(i = 0; i < height; ++i)
		{
			for(j = 0, imageIter = 0; j < width; ++j, imageIter += iter_x)
			{
				color = image[imageIter];
				color = ((color >> 6) & 0x03e0) +
						((color >> 3) & 0x001f) +
						((color >> 16) & 0x8000) +
						((color >> 9) & 0x7c00);

				if(color == 0xfc1f)
					surface[j] = 0;
				else if(m_blackkey && (color & 0x7fff) == 0)
					surface[j] = 0;
				else
					surface[j] = (WORD)color;
			}
			surface = (WORD*)((BYTE*)surface + ddsd.lPitch);
			image += lPitch * iter_y;
		}
		break;
	case PF_A4R4G4B4:
		surface = (WORD*)ddsd.lpSurface;
		for(i = 0; i < height; ++i)
		{
			for(j = 0, imageIter = 0; j < width; ++j, imageIter += iter_x)
			{
				color = ((image[imageIter] >> 16) & 0xf000) +
						((image[imageIter] >> 12) & 0x0f00) +
						((image[imageIter] >> 8) & 0x00f0) +
						((image[imageIter] >> 4) & 0x000f);

				if((color & 0x0fff) == 0x0f0f)
					surface[j] = 0;
				else if(m_blackkey && (color & 0x0fff) == 0)
					surface[j] = 0;
				else
					surface[j] = (WORD)color;
			}
			surface = (WORD*)((BYTE*)surface + ddsd.lPitch);
			image += lPitch * iter_y;
		}
		break;
	case PF_R5G6B5:
		surface = (WORD*)ddsd.lpSurface;
		for(i = 0; i < height; ++i)
		{
			for(j = 0, imageIter = 0; j < width; ++j, imageIter += iter_x)
			{
				color = image[imageIter];
				color = ((color >> 5) & 0x07e0) +
						((color >> 3) & 0x001f) +
						((color >> 8) & 0xf800);

				if(!m_blackkey)
					surface[j] = (WORD)color;
				else if(color != 0xf81f)
					surface[j] = (WORD)color;
			}
			surface = (WORD*)((BYTE*)surface + ddsd.lPitch);
			image += lPitch * iter_y;
		}
		break;
	case PF_A8R8G8B8:
		surface32 = (DWORD*)ddsd.lpSurface;
		for(i = 0; i < height; ++i)
		{
			for(j = 0, imageIter = 0; j < width; ++j, imageIter += iter_x)
			{
				if(m_blackkey && image[imageIter] == 0)
					surface32[j] = 0;
				else
					surface32[j] = image[imageIter];
			}
			surface32 = (DWORD*)((BYTE*)surface32 + ddsd.lPitch);
			image += lPitch * iter_y;
		}
		break;
	}

	m_pddsSurface->Unlock(NULL);

	RECT srcRect = { 0, 0, m_w, m_h };
	UpdateMipmap(srcRect);
}

void	CTexture :: UpdateSprite(int x, int y, int width, int height, SprImg &img, SYSTEMPALETTE* pal)
{
	if(!m_pddsSurface)
		return;

	DDSURFACEDESC2 ddsd = { sizeof(DDSURFACEDESC2), 0 };
	if(FAILED(m_pddsSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
		return;

	int keyCnt = 0;
	BYTE* src = img.m_8bitImage;
	WORD* surface = (WORD*)((BYTE*)ddsd.lpSurface + ddsd.lPitch * y) + x;
	int i;
	for(i = 0; i < img.height; ++i)
	{
		int j;
		for(j = 0; j < img.width; ++j)
		{
			if(keyCnt)
			{
				surface[j] = 0;
				--keyCnt;
			}
			else
			{
				if(*src)
				{
					surface[j] = (WORD)pal[*src];
				}
				else
				{
					surface[j] = 0;
					keyCnt = *(++src) - 1;
				}
				++src;
			}
		}

		memset(surface+j, 0, (width-j)*sizeof(WORD));

		surface = (WORD*)((BYTE*)surface + ddsd.lPitch);
	}

	for(; i < height; ++i)
	{
		memset(surface, 0, width*sizeof(WORD));

		surface = (WORD*)((BYTE*)surface + ddsd.lPitch);
	}

	m_pddsSurface->Unlock(NULL);

	RECT srcRect = { x, y, x+width, y+height };
	UpdateMipmap(srcRect);
}

void	CTexture :: DrawSurface(int x, int y, int w, int h, DWORD color)
{
	vector2d xy[2] = { vector2d(x,y), vector2d(x+w,y+h) };
	vector2d uv[2] = { vector2d(m_uOffset/m_w, m_vOffset/m_h), vector2d((w + m_uOffset)/m_w, (h + m_vOffset)/m_h) };

	RPQuadFace& rpFace = g_renderer->BorrowQuadRP();

	rpFace.tex = this;

	tlvertex3d*	tlvert = rpFace.m_verts;

	tlvert[0] = tlvertex3d(xy[0].x,	xy[0].y, 0.00001f, 0.99999f, color, 0xffffffff, uv[0].x, uv[0].y);
	tlvert[1] = tlvertex3d(xy[1].x, xy[0].y, 0.00001f, 0.99999f, color, 0xffffffff, uv[1].x, uv[0].y);
	tlvert[2] = tlvertex3d(xy[0].x,	xy[1].y, 0.00001f, 0.99999f, color, 0xffffffff, uv[0].x, uv[1].y);
	tlvert[3] = tlvertex3d(xy[1].x,	xy[1].y, 0.00001f, 0.99999f, color, 0xffffffff, uv[1].x, uv[1].y);

	g_renderer->AddRP(&rpFace, RF_UI);
}

void	CTexture :: DrawSurfaceStretch(int x, int y, int w, int h)
{
	vector2d xy[2] = { vector2d(x,y), vector2d(x+w,y+h) };
	vector2d uv[2] = { vector2d(m_uOffset/m_w, m_vOffset/m_h), vector2d((m_updateWidth + m_uOffset)/m_w, (m_updateHeight + m_vOffset)/m_h) };

	RPQuadFace& rpFace = g_renderer->BorrowQuadRP();

	rpFace.tex = this;

	tlvertex3d*	tlvert = rpFace.m_verts;

	tlvert[0] = tlvertex3d(xy[0].x,	xy[0].y, 0.00001f, 0.99999f, 0xffffffff, 0xffffffff, uv[0].x, uv[0].y);
	tlvert[1] = tlvertex3d(xy[1].x, xy[0].y, 0.00001f, 0.99999f, 0xffffffff, 0xffffffff, uv[1].x, uv[0].y);
	tlvert[2] = tlvertex3d(xy[0].x,	xy[1].y, 0.00001f, 0.99999f, 0xffffffff, 0xffffffff, uv[0].x, uv[1].y);
	tlvert[3] = tlvertex3d(xy[1].x,	xy[1].y, 0.00001f, 0.99999f, 0xffffffff, 0xffffffff, uv[1].x, uv[1].y);

	g_renderer->AddRP(&rpFace, RF_UI);
}

bool	CTexture :: Create(DWORD w, DWORD h, PixelFormat pf)
{
	m_w = w;
	m_h = h;
	m_pf = pf;
	g_3dDevice.AdjustTextureSize(&m_w, &m_h);

	SAFE_RELEASE(m_pddsSurface);

	if (pf != PF_A1R5G5B5 && pf != PF_A4R4G4B4 && pf != PF_A8R8G8B8 && pf != PF_R5G6B5)
		ErrorMsg("Create Texture::Unsupported pixelformat");

	if (g_texPixelFormat[pf].dwSize != sizeof(DDPIXELFORMAT))
		g_3dDevice.GetD3DDevice()->EnumTextureFormats(TextureSearchCallback, &pf);

	if (g_texPixelFormat[pf].dwSize != sizeof(DDPIXELFORMAT))
		ErrorMsg("This video system cannot run GFC"
);

	DDSURFACEDESC2 ddsd = { sizeof(DDSURFACEDESC2), 0 };
	ddsd.dwFlags = g_ddsdFlagMipmap | DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH |
		DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE |
		((pf == PF_R5G6B5) ? DDSD_CKSRCBLT : 0);
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.ddsCaps.dwCaps = g_ddsCapsMipmap | DDSCAPS_TEXTURE;
	ddsd.dwWidth = m_w;
	ddsd.dwHeight = m_h;
	ddsd.dwTextureStage = 0;
	ddsd.ddpfPixelFormat = g_texPixelFormat[pf];
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue =
		ddsd.ddpfPixelFormat.dwBBitMask | ddsd.ddpfPixelFormat.dwRBitMask;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue =
		ddsd.ddpfPixelFormat.dwBBitMask | ddsd.ddpfPixelFormat.dwRBitMask;

	ddsd.dwMipMapCount = 1;
	while ((1 << ddsd.dwMipMapCount) < m_w &&
		   (1 << ddsd.dwMipMapCount) < m_h)
		++ddsd.dwMipMapCount;

	if (FAILED(g_3dDevice.GetDirectDraw()->CreateSurface(&ddsd, &m_pddsSurface, NULL)))
		return false;
	return true;
}


static HRESULT WINAPI TextureSearchCallback(DDPIXELFORMAT* pddpf, VOID* param)
{
	if (!pddpf || !param)
		return DDENUMRET_OK;

	PixelFormat pf = *(PixelFormat*)param;
	switch (pf) {
	case PF_A1R5G5B5:
		if (pddpf->dwFlags & DDPF_ALPHAPIXELS)
			if (pddpf->dwRGBBitCount == 16)
				if (pddpf->dwRGBAlphaBitMask == 0x8000) {
					g_texPixelFormat[PF_A1R5G5B5] = *pddpf;
					return DDENUMRET_CANCEL;
				}
		break;

	case PF_A4R4G4B4:
		if (pddpf->dwFlags & DDPF_ALPHAPIXELS)
			if (pddpf->dwRGBBitCount == 16)
				if (pddpf->dwRGBAlphaBitMask == 0xf000) {
					g_texPixelFormat[PF_A4R4G4B4] = *pddpf;
					return DDENUMRET_CANCEL;
				}
		break;

	case PF_R5G6B5:
		if (!(pddpf->dwFlags & DDPF_ALPHAPIXELS))
			if (pddpf->dwRGBBitCount == 16)
				if (pddpf->dwRBitMask == 0xf800) {
					g_texPixelFormat[PF_R5G6B5] = *pddpf;
					return DDENUMRET_CANCEL;
				}
		break;

	case PF_A8R8G8B8:
		if (pddpf->dwFlags & DDPF_ALPHAPIXELS)
			if (pddpf->dwRGBBitCount == 32)
				if (pddpf->dwRGBAlphaBitMask == 0xff000000) {
					g_texPixelFormat[PF_A8R8G8B8] = *pddpf;
					return DDENUMRET_CANCEL;
				}
		break;
	}

	return DDENUMRET_OK;
}

bool	CTexture :: CreateBump(DWORD w, DWORD h)
{
	if (!(g_3dDevice.GetDeviceDesc().dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
		MessageBox(NULL, "D3DTEXOPCAPS_BUMPENVMAP", "notsuppot", 0);

	if (!(g_3dDevice.GetDeviceDesc().dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE))
		MessageBox(NULL, "D3DTEXOPCAPS_BUMPENVMAPLUMINANCE", "notsuppot", 0);

	m_w = w;
	m_h = h;
	m_pf = PF_BUMP;
	g_3dDevice.AdjustTextureSize(&m_w, &m_h);
	SAFE_RELEASE(m_pddsSurface);

	IDirectDrawSurface7* pddsBumpMap;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	ddsd.ddsCaps.dwCaps3 = 0;
	ddsd.ddsCaps.dwCaps4 = 0;
	ddsd.dwWidth = m_w;
	ddsd.dwHeight = m_h;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwTextureStage = 0;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
	ddsd.ddpfPixelFormat.dwBumpBitCount = 16;
	ddsd.ddpfPixelFormat.dwBumpDuBitMask = 0x00ff;
	ddsd.ddpfPixelFormat.dwBumpDvBitMask = 0xff00;
	ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask = 0;

	if (FAILED(g_3dDevice.GetDirectDraw()->CreateSurface(&ddsd, &pddsBumpMap, NULL)))
		return false;

	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	while( pddsBumpMap->Lock( NULL, &ddsd, 0, 0 ) == DDERR_WASSTILLDRAWING );

	CHAR* pDst = (CHAR*)ddsd.lpSurface;

	for( DWORD y=0; y<ddsd.dwHeight; y++ )
	{
		for( DWORD x=0; x<ddsd.dwWidth; x++ )
		{
			FLOAT fy = (y/(FLOAT)ddsd.dwHeight) - 0.5f;

			FLOAT r = _sqrt( fy*fy );
			CHAR iDu = (CHAR)( expf( -r*5.0f )*(32*_cos( 300.0f*r )) );
			CHAR iDv = (CHAR)( expf( -r*5.0f )*(32*_sin( 300.0f*r )) );

			iDu += (CHAR)( 16*_cos( 150.0f*fy ) );
			iDv += (CHAR)( 16*_sin( 150.0f*fy ) );

			iDu += (CHAR)( 7*_cos( 140.0f*(-fy) ) );
			iDv += (CHAR)( 7*_sin( 140.0f*(-fy) ) );

			pDst[2*x+0] = iDu;
			pDst[2*x+1] = iDv;
		}
		pDst += ddsd.lPitch;
	}

	pddsBumpMap->Unlock(NULL);
	m_pddsSurface = pddsBumpMap;
	strcpy(m_texName, "WATERBUMPMAP");

	return true;
}

bool	CTexture :: CreateBump(DWORD w, DWORD h, IDirectDrawSurface7 * pSurface)
{
	if (!(g_3dDevice.GetDeviceDesc().dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
		MessageBox(NULL, "D3DTEXOPCAPS_BUMPENVMAP", "notsuppot", 0);

	if (!(g_3dDevice.GetDeviceDesc().dwTextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE))
		MessageBox(NULL, "D3DTEXOPCAPS_BUMPENVMAPLUMINANCE", "notsuppot", 0);

	m_w = w;
	m_h = h;
	m_pf = PF_BUMP;
	g_3dDevice.AdjustTextureSize(&m_w, &m_h);
	SAFE_RELEASE(m_pddsSurface);

	if (pSurface == NULL)
		return false;

	DDSURFACEDESC2 ddsd;
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	pSurface->GetSurfaceDesc(&ddsd);

	IDirectDrawSurface7* pddsBumpMap;
	HRESULT hr;
	DWORD y = 0;
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.ddsCaps.dwCaps3 = 0;
	ddsd.ddsCaps.dwCaps4 = 0;
	ddsd.dwTextureStage = 0;
	ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
	ddsd.ddpfPixelFormat.dwBumpBitCount = 16;
	ddsd.ddpfPixelFormat.dwBumpDuBitMask = 0x00ff;
	ddsd.ddpfPixelFormat.dwBumpDvBitMask = 0xff00;
	ddsd.ddpfPixelFormat.dwBumpLuminanceBitMask = 0;

	if (FAILED(hr = g_3dDevice.GetDirectDraw()->CreateSurface(&ddsd, &pddsBumpMap, NULL)))
		return false;

	while( pSurface->Lock( NULL, &ddsd, 0, 0 ) == DDERR_WASSTILLDRAWING );

	DWORD lSrcPitch = ddsd.lPitch;
	BYTE* pSrc = (BYTE*)ddsd.lpSurface;

	while( pddsBumpMap->Lock( NULL, &ddsd, 0, 0 ) == DDERR_WASSTILLDRAWING );

	DWORD lDstPitch = ddsd.lPitch;
	BYTE* pDst = (BYTE*)ddsd.lpSurface;

	for (y = 0; y < ddsd.dwHeight; ++y)
	{
		BYTE* pDstT  = pDst;
		BYTE* pSrcB0 = (BYTE*)pSrc;
		BYTE* pSrcB1 = ( pSrcB0 + lSrcPitch );
		BYTE* pSrcB2 = ( pSrcB0 - lSrcPitch );

		if( y == ddsd.dwHeight-1 )
			pSrcB1 = pSrcB0;
		if( y == 0 )
			pSrcB2 = pSrcB0;

		for( DWORD x=0; x<ddsd.dwWidth; x++ )
		{
			LONG v00 = *(pSrcB0+0);
			LONG v01 = *(pSrcB0+4);
			LONG vM1 = *(pSrcB0-4);
			LONG v10 = *(pSrcB1+0);
			LONG v1M = *(pSrcB2+0);

			LONG iDu = (vM1-v01);
			LONG iDv = (v1M-v10);

			if( (v00 < vM1) && (v00 < v01) )
			{
				iDu = vM1-v00;
				if( iDu < v00-v01 ) iDu = v00-v01;
			}

			*pDstT++ = (BYTE)iDu;
			*pDstT++ = (BYTE)iDv;

			pSrcB0+=4;
			pSrcB1+=4;
			pSrcB2+=4;
		}
		pSrc += lSrcPitch;
		pDst += lDstPitch;
	}

	pddsBumpMap->Unlock(NULL);
	pSurface->Unlock(NULL);
	m_pddsSurface = pddsBumpMap;
	strcpy(m_texName, "WATERBUMPMAP");

	return true;
}

bool	CTexture :: CopyTexture( CTexture *srcTex, int sx, int sy, int sw, int sh, int dx, int dy, int dw, int dh )
{
	IDirectDrawSurface7 * srcSurface = srcTex->m_pddsSurface;
	IDirectDrawSurface7 * pddsSurface = m_pddsSurface;
	RECT srcRect = { sx, sy, sx + sw, sy + sh };
	RECT dstRect = { dx, dy, dx + dw, dy + dh };

	if(pddsSurface->Blt(&dstRect, srcSurface, &srcRect, DDBLT_WAIT, NULL) == DD_OK) {
		RECT rect = { 0, 0, m_w, m_h };
		UpdateMipmap(rect);
		return true;
	}
	return false;
}
