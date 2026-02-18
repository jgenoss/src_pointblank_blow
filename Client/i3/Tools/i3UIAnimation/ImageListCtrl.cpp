#include "stdafx.h"
#include "ImageListCtrl.h"
#include "i3TDKUtilGDI.h"
#include "PropertyPane.h"
#include "GlobalInstance.h"

IMPLEMENT_DYNAMIC(CImageListCtrl, CListCtrl)
BEGIN_MESSAGE_MAP(CImageListCtrl, CListCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()

static int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
// Sort the items by passing in the comparison function. 
int CALLBACK CompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	UNREFERENCED_PARAMETER(lParamSort);

	return (int)(lParam2 - lParam1);
}

CImageListCtrl::CImageListCtrl()
{
	m_MemDC = NULL;
	m_pTexture = NULL;
	m_bImageSet = false;
}


CImageListCtrl::~CImageListCtrl()
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
}
void CImageListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, POINT ImageSize )
{
	CListCtrl::Create(dwStyle, rect, pParentWnd, nID);

	m_ImageSize = ImageSize;
	m_ImageList.Create(ImageSize.x, ImageSize.y, ILC_COLOR32, 32, 0);
	SetImageList(&m_ImageList, LVSIL_NORMAL);
}

int CImageListCtrl::InsertItem(UVITEM* pUVItem, LVITEM* pControlItem)
{
	if (pUVItem->_pTexture == NULL) return 0;

	INT32 nIndex = AddImage(pUVItem);

	nIndex -= 1;

	char chTemp[MAX_PATH];
	sprintf_s(chTemp, "%s_%d", pControlItem->pszText, nIndex);
	pControlItem->pszText = chTemp;
	pControlItem->iImage = nIndex;
	pControlItem->iItem = nIndex;

	return CListCtrl::InsertItem(pControlItem);
}


BOOL CImageListCtrl::DeleteAllItems()
{
	while(m_ImageList.GetImageCount() > 0)
	{
		m_ImageList.Remove(0);
	}
	
	return CListCtrl::DeleteAllItems();
}


INT32 CImageListCtrl::AddImage(UVITEM* pUVItem)
{
	HBITMAP hBmp = CreateImage(pUVItem);

	CBitmap cBmp;
	cBmp.Attach(hBmp);

	m_ImageList.Add(&cBmp, RGB(0, 0, 0));

	return m_ImageList.GetImageCount();
}

HBITMAP CImageListCtrl::CreateImage(UVITEM* pUVItem)
{
	I3_REF_CHANGE(m_pTexture, pUVItem->_pTexture);

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
		HDC dc;

		dc = ::GetDC(m_hWnd);

		m_MemDC = CreateCompatibleDC(dc);
		m_hOldBitmap = (HBITMAP) ::SelectObject(m_MemDC, m_hBitmap);

		::ReleaseDC(m_hWnd, dc);
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
 				back = ((((i >> 3) ^ (j >> 3)) & 0x01) << 6) + 128;
 
 				pPixel[0] = (UINT8)((((UINT32)pPixel[0] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
 				pPixel[1] = (UINT8)((((UINT32)pPixel[1] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
 				pPixel[2] = (UINT8)((((UINT32)pPixel[2] * pPixel[3]) + ((255 - pPixel[3]) * back)) >> 8);
 
 				pPixel += 4;
 			}
 		}
	}

	m_pTexture->Unlock(0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	HBITMAP hBmp;

	HDC dcTemp = ::CreateCompatibleDC(m_MemDC);

	hBmp = ::CreateCompatibleBitmap(m_MemDC, m_ImageSize.x, m_ImageSize.y);

	HBITMAP hOld = (HBITMAP) ::SelectObject(dcTemp, hBmp);
	int nOldStretchMode = SetStretchBltMode(dcTemp, COLORONCOLOR);
	::StretchBlt(dcTemp,
		0,
		0,
		m_ImageSize.x,
		m_ImageSize.y,
		m_MemDC,
		(INT32)pUVItem->_pControl->getPos()->x,
		(INT32)pUVItem->_pControl->getPos()->y,
		(INT32)pUVItem->_pControl->getSize()->x,
		(INT32)pUVItem->_pControl->getSize()->y,
		SRCCOPY);

	SetStretchBltMode(dcTemp, nOldStretchMode);

	::SelectObject(dcTemp, hOld);
	::DeleteDC(dcTemp);

	return hBmp;
}

void CImageListCtrl::SetImageSize(POINT pt)
{
	if (m_bImageSet == false)
	{
		m_ImageSize = pt;
		m_ImageList.Create(m_ImageSize.x, m_ImageSize.y, ILC_COLOR32, 32, 0);
		SetImageList(&m_ImageList, LVSIL_NORMAL);
		m_bImageSet = true;
	}
}

void CImageListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SortItems(CompareProc, 0);
}

void CImageListCtrl::UpdateImage(UVITEM* pUVItem, INT32 nIndex)
{
	HBITMAP hBmp = CreateImage(pUVItem);

	CBitmap cBmp;
	cBmp.Attach(hBmp);

	m_ImageList.Replace(nIndex, &cBmp, RGB(0, 0, 0));
	RedrawItems(0, m_ImageList.GetImageCount());
}


