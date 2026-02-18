#include "stdafx.h"
#include "i3GICShapeRenderTexture.h"
#include "i3GICDiagramContext.h"
#include "i3GICFileProperty.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeRenderTexture);

static TCHAR BASED_CODE s_RenderTextureFileFilter[] = _T( "i3Engine Image Files(*.i3Tex)|*.i3Tex|All Files(*.*)|*.*||");

i3GICShapeRenderTexture::i3GICShapeRenderTexture()
{
	setStyle( I3GIC_STYLE_TEXTURE | I3GIC_STYLE_NOFILL);
}

i3GICShapeRenderTexture::~i3GICShapeRenderTexture()
{
	I3_SAFE_RELEASE( m_pTexture);

	_DeleteImage();
	_DeleteGDIOldBitmap();
}

void i3GICShapeRenderTexture::_CreateImage( void)
{
	if( m_pTexture != nullptr)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//Create Bitmap
		BITMAPINFOHEADER fi;
		PVOID pRaster;
		BITMAPINFO ih;

		INT32	WidthofImg  = m_pTexture->GetWidth();
		INT32	HeightofImg = m_pTexture->GetHeight();

		//
		INT32		Height = - HeightofImg;
		I3G_IMAGE_FORMAT	DestFormat	 = I3G_IMAGE_FORMAT_BGRA_8888;  

		::ZeroMemory(&fi,sizeof(BITMAPINFOHEADER));
		fi.biSize		= sizeof(BITMAPINFOHEADER);
		fi.biBitCount	= 32;

		fi.biHeight		= Height;
		fi.biWidth		= WidthofImg;
		fi.biPlanes		= 1;
		fi.biCompression= BI_RGB;

		memcpy(&ih.bmiHeader,&fi,sizeof(BITMAPINFOHEADER));

		m_hBitmap = CreateDIBSection(nullptr,&ih,DIB_RGB_COLORS,&pRaster,nullptr,0);
		
		char * src		= m_pTexture->Lock( 0);
		char * pDest	= (char*)pRaster;

		i3Image_Convert( m_pTexture->GetFormat(), src, WidthofImg , HeightofImg , 1, m_pTexture->GetClut(0),
						 DestFormat, pDest, 4, nullptr);

		m_pTexture->Unlock( 0);

		_DeleteGDIOldBitmap();
		_CreateGDIOldBitmap();
	}
}

void i3GICShapeRenderTexture::_DeleteImage( void)
{
	if(m_hdcMem	!= nullptr)
	{
		if(m_hBitmap != nullptr)
		{	
			::SelectObject(m_hdcMem , m_hOldBitmap);
			
			::DeleteObject(m_hBitmap);

			m_hBitmap = nullptr;
		}

		::DeleteDC(m_hdcMem);

		m_hdcMem = nullptr;
	}
}

void i3GICShapeRenderTexture::_CreateGDIOldBitmap( void)
{
	HDC dc = ::GetDC( nullptr);

	m_hdcMem = ::CreateCompatibleDC( dc);
	m_hOldBitmap = (HBITMAP) ::SelectObject( m_hdcMem, m_hBitmap);

	::ReleaseDC( nullptr, dc);
}

void i3GICShapeRenderTexture::_DeleteGDIOldBitmap( void)
{
	if( m_hOldBitmap != nullptr)
	{
		::DeleteObject( m_hOldBitmap);
		m_hOldBitmap = nullptr;
	}
}


void i3GICShapeRenderTexture::SetTexture( i3Texture * pTex)
{
	I3_REF_CHANGE( m_pTexture, pTex);

	if( isBindDiagram())
	{
		_DeleteImage();
		_CreateImage();
	}
}

/*virtual*/ void i3GICShapeRenderTexture::OnDraw( i3GICDiagramContext * pCtx)
{
	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		if( (m_hBitmap != nullptr) && (m_hdcMem != nullptr))
		{
			RECT rect, imageRect;

			rect.left = (INT32)getPosX();
			rect.top = (INT32)getPosY();
			rect.right = (INT32)getWidth();
			rect.bottom = (INT32)getHeight();

			imageRect.left = 0;
			imageRect.top = 0;
			imageRect.right = m_pTexture->GetWidth();
			imageRect.bottom = m_pTexture->GetHeight();
			
			pCtx->drawBitmap( m_hdcMem, &rect, &imageRect);
		}
	}

	i3GICShapeRect::OnDraw( pCtx);
}

/*virtual*/ void i3GICShapeRenderTexture::OnBindDiagram( bool bBind)
{
	i3GICShapeRect::OnBindDiagram( bBind);

	if( bBind)	_CreateImage();
	else		_DeleteImage();
}

/*virtual*/ void i3GICShapeRenderTexture::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTexture != nullptr)
		List.push_back(m_pTexture);
	
	i3GICShapeRect::OnBuildObjectList( List);
}

/*virtual*/ void i3GICShapeRenderTexture::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShapeRect::CopyTo( pDest, method);

	i3GICShapeRenderTexture * pDestShape = (i3GICShapeRenderTexture*) pDest;

	// texture는 공유한다.
	pDestShape->SetTexture( getTexture());
}

/*virtual*/ UINT32 i3GICShapeRenderTexture::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeRect::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	OBJREF ref = pResFile->GetObjectPersistID( m_pTexture);
	Rc = pStream->Write( &ref, sizeof( ref));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ UINT32 i3GICShapeRenderTexture::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeRect::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	OBJREF ref;

	Rc = pStream->Read( &ref, sizeof( ref));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Texture * pTex = (i3Texture*) pResFile->FindObjectByID( ref);
	SetTexture( pTex);


	return Result;
}

/*virtual*/ bool i3GICShapeRenderTexture::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeRect::OnSaveXML( pFile, pXML) == false)
		return false;

	if( m_pTexture != nullptr)
	{
		pXML->addAttr( "Texture", m_pTexture->GetName());
	}

	return true;
}

/*virtual*/ bool i3GICShapeRenderTexture::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeRect::OnLoadXML( pFile, pXML) == false)
		return false;

	char szPath[MAX_PATH];

	i3::safe_string_copy( szPath, pXML->getAttr( "Texture"), sizeof( szPath));

	if( i3::generic_string_size( szPath) > 0)
	{
		i3ImageFile file;
		i3Texture * pTex = file.Load( szPath);

		SetTexture( pTex);

		if (pTex)
			pTex->SetName( szPath);

		I3_SAFE_RELEASE( pTex);
	}

	return true;
}


/*virtual*/ void i3GICShapeRenderTexture::OnProperty(CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeRect::OnProperty( pCtrl);

	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(_T("Texture"));

	//  Path;
	i3GICFileProperty * pPropPath;
	if( m_pTexture != nullptr)
	{
		CString szPath( m_pTexture->GetName());
		pPropPath = new i3GICFileProperty( _T("Path"), TRUE, szPath, _T("i3Tex"), 0, s_RenderTextureFileFilter);
	}
	else
	{
		pPropPath = new i3GICFileProperty( _T("Path"), TRUE, _T(""), _T("i3Tex"), 0, s_RenderTextureFileFilter);
	}

	pPropPath->setShape( this);
	pPropPath->setUpdateType( GIC_PROPERTY_UPDATE_PATH_TEXTURE);

	pProperty->AddSubItem( pPropPath);

	pCtrl->AddProperty( pProperty);
}

#endif	// I3_DEBUG
