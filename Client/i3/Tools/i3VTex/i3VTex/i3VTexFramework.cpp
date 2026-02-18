#include "stdafx.h"
#include "i3VTexFramework.h"

#define		TEST_EDITBOX

I3_CLASS_INSTANCE( i3VTexFramework, i3ViewerFramework);

i3VTexFramework::i3VTexFramework(void)
{
	m_pText = NULL;
	m_pEdit = NULL;
}

i3VTexFramework::~i3VTexFramework(void)
{
}

void i3VTexFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	{
		i3RenderContext * pCtx = GetViewer()->GetRenderContext();
		COLOR col;
	
		i3Color::Set( &col, (UINT8) 128, 128, 128, 255);
		pCtx->SetClearColor( &col);
	}

#if !defined( TEST_EDITBOX)
	{
		i3UITextManager * pTextSys = getUIRenderer()->getTextManager();

		m_pFont = pTextSys->GetFont( "±¼¸²", 16);

		m_pText = pTextSys->CreateText(0);
		m_pText->SetFont( m_pFont);
		m_pText->SetView( 10, 10, 400, 200);
		m_pText->setAutoLineFeed( true);
		m_pText->SetShadowType( I3UI_TEXT_SHADOW_THIN);
	}

	#if 0
		m_pText->SetText( L"0123456789ABCDEF 0123456789ABCDEF 0123456789ABCDEF 0123456789ABCDEF 0123456789ABCDEF 0123456789ABCDEF 0123456789ABCDEF 0123456789ABCDEF");
	#else
		{
			i3FileStream file;

			if( file.Open( "text.txt", STREAM_READ) == TRUE)
			{
				WCHAR16 * pBuf;
				INT32 sz = file.GetSize();

				pBuf = (WCHAR16 *) i3MemAlloc( sz + 2);

				file.Read( pBuf, sz);

				pBuf[ (sz >> 1)] = 0;

				m_pText->SetText( pBuf, true);
			}

		}
	#endif
#else
	/*
	{
		i3VirtualTexture * pVTex = i3VirtualTexture::NewObject();
		pVTex->Create( "D:\\test.i3VTex", I3G_IMAGE_FORMAT_BGRA_8888);

		pVTex->PrepareForRendering( 2048, 2048);

		i3ResourceManager * pResMng = GetResourceManager();
		
		i3Texture * pTexture = pResMng->LoadTexture( "res/SelectImage.i3i");
		I3ASSERT( pTexture != NULL);

		INT32 offX, offY;
		pVTex->PlaceTexture( pTexture, &offX, &offY);
		pVTex->Flush();

		getUIRenderer()->SetTexture( pVTex);
		
		i3UITemplate_EditBox * pEditTemp = i3UITemplate_EditBox::NewObject();
		pEditTemp->Create();

		INT32 i;
		for( i = 0;i < pEditTemp->getShapeCount(); ++i)
		{
			i3UIShape * pShape = pEditTemp->getShape(i);
			pShape->setPosX( 0.0f);
			pShape->setPosY( 0.0f);
			pShape->setWidth(15.0f);
			pShape->setHeight(15.0f);
			
			pShape->setCoordU( 0.0f);
			pShape->setCoordV( 0.0f);
			pShape->setCoordW( 128.0f);
			pShape->setCoordH( 128.0f);
			//pShape->SetTexture( pTexture, 0.0f, 0.0f, 128.0f, 128.0f);
		}
		
		m_pEdit = i3UIEditBox::NewObject();
		//m_pEdit->SetControlShapeTemplate( pEditTemp);
		m_pEdit->SetTemplate( pEditTemp);
		m_pEdit->SetMaxTextCount( MAX_PATH);
		m_pEdit->SetReadOnly( false);
		//m_pEdit->SetTextAutoWrap( true);
	}
	*/
#endif

}

void i3VTexFramework::OnKeyDown( UINT32 nKey)
{
	INT32 idx;

	switch( nKey)
	{
		case VK_RETURN :
			#if defined( TEST_EDITBOX)
				SetFocusLock( m_pEdit);
			#else
				m_pText->BeginEdit();
			#endif		
			break;

		case VK_ESCAPE :
			#if defined( TEST_EDITBOX)
				ReleaseFocusLock();
			#else
				m_pText->EndEdit();
			#endif
			break;

		case VK_LEFT :
			if( m_pText->isEditing())
			{
				idx = m_pText->getCursorPos();
				if( idx > 0)
					idx--;

				m_pText->SetCursorPos( idx);
			}
			else
			{
				INT32 x = m_pText->getShowX();
				INT32 y = m_pText->getShowY();

				x -= 3;

				if( x < 0)
					x = 0;

				m_pText->SetShowPos( x, y);
			}
			break;

		case VK_RIGHT :
			if( m_pText->isEditing())
			{
				idx = m_pText->getCursorPos();
				idx++;

				m_pText->SetCursorPos( idx);
			}
			else
			{
				INT32 x = m_pText->getShowX();
				INT32 y = m_pText->getShowY();

				x += 3;

				if( x >= m_pText->getExtentWidth())
					x = m_pText->getExtentWidth();

				m_pText->SetShowPos( x, y);
			}
			break;
	}
}

void i3VTexFramework::OnKeyUp( UINT32 nKey)
{
}
