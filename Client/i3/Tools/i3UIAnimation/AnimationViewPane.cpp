#include "stdafx.h"
#include "AnimationViewPane.h"
#include "MainFrm.h"
#include "GlobalInstance.h"
IMPLEMENT_DYNAMIC(CAnimationViewPane, CPaneBase)

BEGIN_MESSAGE_MAP(CAnimationViewPane, CPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

CAnimationViewPane::CAnimationViewPane()
{
	m_pTexture = NULL;
	m_MemDC = NULL;
	m_pAnimaton = NULL;
	m_pItem = NULL;
	m_nAnimationIndex = 0;
}


CAnimationViewPane::~CAnimationViewPane()
{
	if (m_MemDC != NULL)
	{
		if (m_hBitmap != NULL)
		{
			SelectObject(m_MemDC, m_hOldBitmap);

			DeleteObject(m_hBitmap);

			m_hBitmap = NULL;
		}

		::DeleteDC(m_MemDC);
		m_MemDC = NULL;
	}

	I3_SAFE_RELEASE(m_pTexture);
	I3_SAFE_RELEASE(m_pAnimaton);
	I3_SAFE_DELETE(m_pItem);
}
int CAnimationViewPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	AdjustLayout();

	return 0;
}


void CAnimationViewPane::OnSize(UINT nType, int cx, int cy)
{
	CPaneBase::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CAnimationViewPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
}

void CAnimationViewPane::OnPaint()
{
	CPaintDC dc(this);
	RECT rt;
	::GetClientRect(m_hWnd, &rt);

	::FillRect(dc.m_hDC, &rt, (HBRUSH)COLOR_HIGHLIGHT);
}

void CAnimationViewPane::OnDraw(HDC hdc)
{
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	RECT rt;
	::GetClientRect(m_hWnd, &rt);

	::FillRect(hdc, &rt, (HBRUSH)COLOR_HIGHLIGHT);

	HFONT hFont, hOldFont;
	hFont = CreateFont(12, 0, 0, 0, FW_HEAVY, 0, 0, 0, HANGUL_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림체");
	hOldFont = (HFONT)SelectObject(hdc, hFont);

	SetTextColor(hdc, RGB(255, 255, 0));
	SetBkMode(hdc, TRANSPARENT);

	i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager(i3Texture::static_meta());

	i3Texture* tex = NULL;
	if (pTexManager && m_pAnimaton)
	{
		tex = (i3Texture *)pTexManager->GetResource(m_pAnimaton->GetUVRes(m_nAnimationIndex).c_str(),
			i3ResourceFile::GetWorkingFolder(), 
			m_pAnimaton->GetUVRes(m_nAnimationIndex).c_str());

		TextOut(hdc, 0, 0, m_pAnimaton->GetName(), strlen(m_pAnimaton->GetName()));
	}
	else if (m_pItem)
	{
		tex = (i3Texture *)pTexManager->GetResource(m_pItem->_ResName,
			i3ResourceFile::GetWorkingFolder(),
			m_pItem->_ResName);
	}

	if (tex)
	{
		I3_REF_CHANGE(m_pTexture, tex);

		BITMAPINFOHEADER fi;
		PVOID pRaster;
		BITMAPINFO ih;

		//
		INT32		Height = -(INT32)m_pTexture->GetHeight();					//이미지가 거꾸로 들어와있는상태라height 는 - 
		UINT16				DestBitCount = 32;

		i3mem::FillZero(&fi, sizeof(BITMAPINFOHEADER));
		fi.biSize = sizeof(BITMAPINFOHEADER);
		fi.biBitCount = DestBitCount;

		fi.biHeight = Height;
		fi.biWidth = m_pTexture->GetWidth();
		fi.biPlanes = 1;
		fi.biCompression = BI_RGB;

		memcpy(&ih.bmiHeader, &fi, sizeof(BITMAPINFOHEADER));

		if (m_MemDC != NULL)
		{
			SelectObject(m_MemDC, m_hOldBitmap);

			DeleteObject(m_hBitmap);

			m_hBitmap = NULL;

			::DeleteDC(m_MemDC);
			m_MemDC = NULL;
		}

		m_hBitmap = CreateDIBSection(NULL, &ih, DIB_RGB_COLORS, &pRaster, NULL, 0);


		if (m_MemDC == NULL)
		{
			m_MemDC = CreateCompatibleDC(hdc);
			m_hOldBitmap = (HBITMAP) ::SelectObject(m_MemDC, m_hBitmap);
		}

		char * src = m_pTexture->Lock(0);
		char * pDest = (char*)pRaster;

		i3Image_Convert(m_pTexture->GetFormat(), src, m_pTexture->GetWidth(), m_pTexture->GetHeight(), 1, m_pTexture->GetClut(0),
			I3G_IMAGE_FORMAT_BGRA_8888, pDest, 4, NULL);

		{
			UINT8 * pPixel = (UINT8 *)pRaster;
			UINT32 i, j, back;

			for (i = 0; i < m_pTexture->GetWidth(); i++)
			{
				for (j = 0; j < m_pTexture->GetHeight(); j++)
				{
					pPixel[0] = (UINT8)((((UINT32)pPixel[0] * pPixel[3]) + ((255 - pPixel[3]))) >> 8);
					pPixel[1] = (UINT8)((((UINT32)pPixel[1] * pPixel[3]) + ((255 - pPixel[3]))) >> 8);
					pPixel[2] = (UINT8)((((UINT32)pPixel[2] * pPixel[3]) + ((255 - pPixel[3]))) >> 8);

					pPixel += 4;
				}
			}
		}

		m_pTexture->Unlock(0);
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////

		if (m_pAnimaton)
		{
			HBITMAP hBmp;
			HDC dcTemp = ::CreateCompatibleDC(m_MemDC);
			hBmp = ::CreateCompatibleBitmap(m_MemDC, m_pAnimaton->GetUVPos(m_nAnimationIndex).right,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).bottom);

			INT32 posX = rt.right / 2 - m_pAnimaton->GetUVPos(m_nAnimationIndex).right / 2;
			INT32 posY = rt.bottom / 2 - m_pAnimaton->GetUVPos(m_nAnimationIndex).bottom / 2;

			HBITMAP hOld = (HBITMAP) ::SelectObject(dcTemp, hBmp);
			int nOldStretchMode = SetStretchBltMode(dcTemp, COLORONCOLOR);
			::StretchBlt(hdc,
				posX,
				posY,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).right,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).bottom,
				m_MemDC,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).left,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).top,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).right,
				m_pAnimaton->GetUVPos(m_nAnimationIndex).bottom,
				SRCCOPY);

			SetStretchBltMode(dcTemp, nOldStretchMode);

			SelectObject(dcTemp, hOld);
			DeleteObject(hBmp);
			DeleteDC(dcTemp);
		}
		else if (m_pItem)
		{
			HBITMAP hBmp;
			HDC dcTemp = ::CreateCompatibleDC(m_MemDC);
			hBmp = ::CreateCompatibleBitmap(m_MemDC, m_pItem->_Rect.right,
				m_pItem->_Rect.bottom);

			INT32 posX = rt.right / 2 - m_pItem->_Rect.right / 2;
			INT32 posY = rt.bottom / 2 - m_pItem->_Rect.bottom / 2;

			HBITMAP hOld = (HBITMAP) ::SelectObject(dcTemp, hBmp);
			int nOldStretchMode = SetStretchBltMode(dcTemp, COLORONCOLOR);
			::StretchBlt(hdc,
				posX,
				posY,
				m_pItem->_Rect.right,
				m_pItem->_Rect.bottom,
				m_MemDC,
				m_pItem->_Rect.left,
				m_pItem->_Rect.top,
				m_pItem->_Rect.right,
				m_pItem->_Rect.bottom,
				SRCCOPY);

			SetStretchBltMode(dcTemp, nOldStretchMode);

			SelectObject(dcTemp, hOld);
			DeleteObject(hBmp);
			DeleteDC(dcTemp);
		}

		
	}

	SelectObject(hdc, hOldFont);
	DeleteObject(hFont);
}

void CAnimationViewPane::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//Invalidate(FALSE);
	if (m_pAnimaton)
	{
		if (m_nAnimationIndex == m_pAnimaton->GetSpriteCount())
			m_nAnimationIndex = 0;
	}

	CRect rect;
	::GetClientRect(m_hWnd, &rect);

	// 메모리 DC 선언
	CDC* pDC = GetDC();
	
	if (pDC)
	{
 		CDC memDC;
  		CBitmap *pOldBitmap, bitmap;
  
  		// 화면 DC와 호환되는 메모리 DC 객체를 생성
   		memDC.CreateCompatibleDC(pDC);
   
   		// 마찬가지로 화면 DC와 호환되는 Bitmap 생성
   		bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
   
   		pOldBitmap = memDC.SelectObject(&bitmap);
 
 		OnDraw(memDC);
 
 		// 메모리 DC를 화면 DC에 고속 복사
   		pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
   
   		memDC.SelectObject(pOldBitmap);
   		memDC.DeleteDC();
   		bitmap.DeleteObject();
		ReleaseDC(pDC);
	}

	m_nAnimationIndex++;

	CPaneBase::OnTimer(nIDEvent);
}

void CAnimationViewPane::CopyToAni(i3UIAnimation2DSprite* pData)
{
	if (m_pAnimaton)
	{
		I3_SAFE_RELEASE(m_pAnimaton);
	}

	m_pAnimaton = i3UIAnimation2DSprite::new_object();
	pData->CopyTo(m_pAnimaton, I3_COPY_INSTANCE);

	REAL32 fTime = m_pAnimaton->GetLifeTime() / m_pAnimaton->GetSpriteCount();
	fTime = fTime * 1000;
	SetTimer(1, fTime, NULL);

	m_nAnimationIndex = 0;
}

void CAnimationViewPane::SelectFrame(UVITEM* pItem)
{
	if (m_pAnimaton)
	{
		I3_SAFE_RELEASE(m_pAnimaton);
	}

	if (m_pItem)
	{
		I3_SAFE_DELETE(m_pItem);
	}

	m_pItem = new I3_UVINFO;
	m_pItem->_Rect.left = pItem->_pControl->getPos()->x;
	m_pItem->_Rect.top = pItem->_pControl->getPos()->y;
	m_pItem->_Rect.right = pItem->_pControl->getWidth();
	m_pItem->_Rect.bottom = pItem->_pControl->getHeight();

	sprintf_s(m_pItem->_ResName, "%s", pItem->_pTexture->GetName());
	

	SetTimer(1, 100, NULL);
}