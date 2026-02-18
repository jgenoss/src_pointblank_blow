#include "StdAfx.h"
#include ".\bitmapimage.h"

CBitmapImage::CBitmapImage(void)
{
	I3ASSERT( sizeof( I3COLOR ) == sizeof( COLORREF ) );
	m_pBuffer = NULL;
	m_pGridBuffer = NULL;
}

CBitmapImage::~CBitmapImage(void)
{
	Destroy();
	DestroyGrid();
}

void CBitmapImage::DestroyGrid()
{
	m_GridImage.Destroy();
	I3_SAFE_DELETE_ARRAY( m_pGridBuffer );
}

void _SetPixel( COLORREF *pBuffer, UINT32 Color, INT32 sx, INT32 sy, INT32 w, INT32 h, INT32 nPitch )
{
	COLORREF *pDestLine = &pBuffer[ sx + ( sy * nPitch ) ];
	INT32 x, y;
	for( y = 0; y < h; y++ )
	{
		COLORREF *pDestPixel = pDestLine;

		for( x = 0; x <  w; x++ )
		{
            *pDestPixel = Color;
			pDestPixel++;
		}

		pDestLine += nPitch;
	}
}

BOOL CBitmapImage::CreateGrid( INT32 nWidth, INT32 nHeight, INT32 nGridSize, COLORREF FrontColor, COLORREF BackColor )
{
	DestroyGrid();

	if( ! m_GridImage.Create( nWidth, nHeight, 32, 0 ) )
	{
		return FALSE;
	}

	m_pGridBuffer = new COLORREF[ nWidth * nHeight ];

	I3ASSERT( ( nWidth % nGridSize ) == 0 );

	INT32 x, y;
	INT32 nCol = nWidth / nGridSize;
	INT32 nRow = nHeight / nGridSize;

	for( y = 0; y < nRow; y++ )
	{
		for( x = 0; x < nCol; x++ )
		{
			BOOL bFront = ( ( x + y ) % 2 ) == 0;
			COLORREF Color = bFront ? FrontColor : BackColor;

			_SetPixel( m_pGridBuffer, Color, x * nGridSize, y * nGridSize, nGridSize, nGridSize, nWidth );
		}
	}

	for( y = 0; y < nHeight; y++ )
	{
		for( x = 0; x < nWidth; x++ )
		{
			m_GridImage.SetPixel( x, y, m_pGridBuffer[ y * nWidth + x ] );
		}
	}

	return TRUE;
}

void CBitmapImage::Destroy()
{
	m_ImageRGB.Destroy();
	m_ImageAlpha.Destroy();
	m_ImageAll.Destroy();

	I3_SAFE_DELETE_ARRAY( m_pBuffer );
}

BOOL CBitmapImage::Create( i3Texture *pTexture, INT32 nLevel )
{
	Destroy();

	I3G_IMAGE_FORMAT	DestFormat	 = I3G_IMAGE_FORMAT_BGRA_8888;  
	UINT16				DestBitCount = 32;

	m_Width		= pTexture->GetWidth()	>> nLevel;
	m_Height	= pTexture->GetHeight()	>> nLevel;

	CreateGrid( m_Width, m_Height, 8, RGB( 255, 255, 255 ), RGB( 200,  200, 200 ) );

	// 알파 채널 생성 버그 있음. -_-;
	//if( ! m_ImageRGB.Create( m_Width, m_Height, 32, CImage::createAlphaChannel | BI_RGB ) )
	//if( ! m_ImageRGB.Create( m_Width, m_Height, 32,  CImage::createAlphaChannel ) )
	if( ! m_ImageRGB.Create( m_Width, m_Height, 32 ) )
	{
		//AfxMessageBox( "Failed to create CBitmapImage", MB_OK );
		return FALSE;
	}

	if( ! m_ImageAlpha.Create( m_Width, m_Height, 32 ) )
	{
		//AfxMessageBox( "Failed to create CBitmapImage", MB_OK );
		return FALSE;
	}

	if( ! m_ImageAll.Create( m_Width, m_Height, 32 ) )
	{
		//AfxMessageBox( "Failed to create CBitmapImage", MB_OK );
		return FALSE;
	}

	// i3Texture에서 RGBA 색상 추출해서 버퍼에 저장하기
	{
		m_pBuffer			= new COLORREF[ m_Width * m_Height ];

		char * pSrcBuf		= pTexture->Lock( nLevel );

		i3Image_Convert(
			pTexture->GetFormat(),
			pSrcBuf,
			m_Width, m_Height,
			1,
			pTexture->GetClut( nLevel ),
			DestFormat,
			(char*)m_pBuffer,
			4,
			NULL
			);

		pTexture->Unlock( nLevel );
	}

	INT32 y, x;
	I3COLOR *pColor;

	// RGB 채널 만들기
	pColor = (I3COLOR*)m_pBuffer;
	for( y = 0; y < m_Height; y++ )
	{
		for( x = 0; x < m_Width; x ++ )
		{
			UINT8 a = i3Color::GetA( pColor );
			UINT8 r = i3Color::GetB( pColor );	// -_-;
			UINT8 g = i3Color::GetG( pColor );
			UINT8 b = i3Color::GetR( pColor );	// -_-;

			// SetPixel 함수는 버그 있음.
			m_ImageRGB.SetPixelRGB( x, y, r, g, b );

			pColor++;
		}
	}

	// Alpha 채널 만들기
	pColor = (I3COLOR*)m_pBuffer;
	for( y = 0; y < m_Height; y++ )
	{
		for( x = 0; x < m_Width; x ++ )
		{
			UINT8 a = i3Color::GetA( pColor );
			//I3ASSERT( a == 0 );
			m_ImageAlpha.SetPixelRGB( x, y, a, a, a );
			pColor++;
		}
	}

	// 그리드 이미지와 알파 블랜딩한 이미지 만들기
	pColor = (I3COLOR*)m_pBuffer;
	I3COLOR *pGrid = (I3COLOR*)m_pGridBuffer;
	for( y = 0; y < m_Height; y++ )
	{
		for( x = 0; x < m_Width; x ++ )
		{
			UINT8 sa = i3Color::GetA( pColor );
			UINT8 sr = i3Color::GetB( pColor );	// -_-;
			UINT8 sg = i3Color::GetG( pColor );
			UINT8 sb = i3Color::GetR( pColor );	// -_-;

			UINT8 gr = i3Color::GetB( pGrid );	// -_-;
			UINT8 gg = i3Color::GetG( pGrid );
			UINT8 gb = i3Color::GetR( pGrid );	// -_-;

			UINT8 r  = ( ( sr * sa ) + ( gr * ( 255 - sa ) ) ) / 255;
			UINT8 g  = ( ( sg * sa ) + ( gg * ( 255 - sa ) ) ) / 255;
			UINT8 b  = ( ( sb * sa ) + ( gb * ( 255 - sa ) ) ) / 255;

			m_ImageAll.SetPixelRGB( x, y, r, g, b );

			pColor++;
			pGrid++;
		}
	}

	return TRUE;
}

void CBitmapImage::Draw( HDC hDC, DRAW_MODE nDrawMode, BOOL bFlipX )
{
	if( m_pBuffer == NULL )
	{
		if( ! m_GridImage.IsNull() )
		{
			m_GridImage.BitBlt( hDC, 0, 0, SRCCOPY );
		}
	}
	else
	{
		switch( nDrawMode )
		{
		case DRAW_MODE_ALL:
			{
				m_ImageAll.BitBlt( hDC, 0, 0, SRCCOPY );
			}
			break;
		case DRAW_MODE_RGB:
			{
				m_ImageRGB.BitBlt( hDC, 0, 0, SRCCOPY );
			}
			break;
		case DRAW_MODE_ALPHA:
			{
				m_ImageAlpha.BitBlt( hDC, 0, 0, SRCCOPY );
			}
			break;
		}
	}
}
