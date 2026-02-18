#include "i3FrameworkPCH.h"
#include "i3UIImageBoxEx.h"

#include "i3UIManager.h"
#include "i3UIRenderer.h"

#include "i3Framework.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/atoi.h"

I3_CLASS_INSTANCE( i3UIImageBoxEx);

i3UIImageBoxEx::i3UIImageBoxEx()
{
	m_nControlState = 0;

#if defined( I3_DEBUG)
	ResetPathCount();
#endif

	addStyle( I3UI_STYLE_NO_VT);

}

i3UIImageBoxEx::~i3UIImageBoxEx()
{
	
	I3_SAFE_NODE_RELEASE( m_pSprite);

	for( size_t i = 0; i < m_ShapeList.size(); i++)
	{
		SHAPETEXINFO * pInfo = m_ShapeList[ i];

		I3_SAFE_RELEASE( pInfo->_pShape);
		I3_SAFE_RELEASE( pInfo->_pTex);

		I3MEM_SAFE_FREE( pInfo);
	}

#if defined( I3_DEBUG)
	for(size_t i = 0; i < m_ImagePathList.size(); ++i)
	{
		char * pszPath = m_ImagePathList[i];

		I3MEM_SAFE_FREE( pszPath);
	}
	m_ImagePathList.clear();
#endif
}

void i3UIImageBoxEx::_CalculateSizeByTexSize( REAL32 width, REAL32 height)
{
	VEC2D * pSize = getSize();

	width = MAX( getX( pSize), width);
	height = MAX( getY( pSize), height);

	setSize( width, height);
}

INT32 i3UIImageBoxEx::_FindShapeByPath( const char * pszPath)
{
	

	for(size_t i = 0; i < m_ShapeList.size(); i++)
	{
		SHAPETEXINFO * pInfo = m_ShapeList[ i];

		if( pInfo->_pTex != nullptr)
		{
			if( i3::generic_is_iequal( pInfo->_pTex->GetNameString(), pszPath) )
				return i;
		}
	}

	return -1;
}

void i3UIImageBoxEx::prepareSprites()
{	
	/*if( m_hSprites != nullptr &&
		(m_pSprite != nullptr && m_pSprite->GetCount() == (INT32)m_ShapeList.size()) )
		return;*/

	if( m_ShapeList.size() == 0)
		return;

	m_nShapeCount = 1;

	if( m_pSprite == nullptr || (m_pSprite->GetCount() != (INT32)m_ShapeList.size()))
	{
		I3_SAFE_RELEASE( m_pSprite);
		m_pSprite = i3Sprite2D::new_object();
		
		if( m_ShapeList.size() > 0)
		{
			#if USE_VERTEX_BANK
				m_pSprite->Create( (UINT32)m_ShapeList.size(), true, true, true);
			#else
				m_pSprite->Create( (UINT32)m_ShapeList.size(), true, true, false);
			#endif

			m_pSprite->setCoordType( I3_SPRITE2D_COORD_PIXEL);
		}
	}

	if( m_hSprites == nullptr)
	{
		m_hSprites = (INT32*)i3MemAlloc( sizeof( INT32));
		*m_hSprites = -1;
	}

	if( *m_hSprites != -1)
	{
		g_pUIRenderer->getSprite( *m_hSprites)->setNode( nullptr);
		g_pUIRenderer->FreeSprite( *m_hSprites);
	}

	*m_hSprites = g_pUIRenderer->AllocSprite( I3UI_SPRITE_STYLE_NODE, this);
	I3ASSERT( *m_hSprites >= 0);
	g_pUIRenderer->getSprite( *m_hSprites)->setNode( m_pSprite);

	m_bResetSprites = false;

	return;
}

void i3UIImageBoxEx::OnUpdate( REAL32 rDeltaSeconds)
{
	UpdateRenderObjects();

	i3GameNode::OnUpdate( rDeltaSeconds);
}

void i3UIImageBoxEx::OnVisibleScene( void)
{
	i3UIControl::OnVisibleScene();

	if( m_pSprite != nullptr)
	{
		m_pSprite->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
}

void i3UIImageBoxEx::OnInvisibleScene( void)
{
	i3UIControl::OnInvisibleScene();

	if( m_pSprite != nullptr)
	{
		m_pSprite->AddFlag( I3_NODEFLAG_DISABLE);
	}
}

void i3UIImageBoxEx::UpdateRenderObjects( void)
{
	INT32 nShapeCount;

	VEC3D	Position;

	bool bEnable = isEnable() && isReadyToShow() && isVisible();

	if( m_pSprite !=nullptr && bEnable && m_CurrentImage != -1 && m_ShapeList.size() > 0)
	{
		m_pSprite->RemoveFlag( I3_NODEFLAG_DISABLE);

		nShapeCount = (INT32)m_ShapeList.size();
		
		// 이미지 설정
		SHAPETEXINFO * pInfo = m_ShapeList[ m_CurrentImage];
		i3UIShape * pShape	= pInfo->_pShape;
		i3Texture * pTex = (i3Texture*) pInfo->_pTex;
		
		// Texture의  UV값을 설정합니다.
		REAL32 offX2, offY2;
		REAL32 baseX, baseY;

		baseX = (REAL32) pShape->getTexCoordOffset()->x;
		baseY = (REAL32) pShape->getTexCoordOffset()->y;

		if( !m_bUseCustomTexCoord)
		{
			REAL32 u1 = baseX + pShape->getCoordU();
			REAL32 v1 = baseY + pShape->getCoordV();
			REAL32 u2 = u1 + pShape->getCoordW();
			REAL32 v2 = v1 + pShape->getCoordH();

			m_pSprite->SetTexture( pTex );
			m_pSprite->SetTextureFilter( I3G_TEXTURE_FILTER_LINEAR, I3G_TEXTURE_FILTER_LINEAR);
			m_pSprite->SetTextureCoord( m_CurrentImage, u1, v1, u2, v2);
			
			//m_pSprite->SetFlip( i, bFlipX, bFlipY );
		}

		offX2 = pShape->getTexCoordOffset()->x + pShape->getCoordW();
		offY2 = pShape->getTexCoordOffset()->y + pShape->getCoordH();
		
		m_pSprite->SetColor( m_CurrentImage, &m_Color );

		CalcAlignedPos( pShape->getRect(), &Position);
		m_pSprite->SetCenter( m_CurrentImage, getX( &Position) + pShape->getWidth() *  0.5f, getY( &Position) + pShape->getHeight() * 0.5f,  0.0f);
		m_pSprite->SetSize( m_CurrentImage, pShape->getWidth(), pShape->getHeight());
		m_pSprite->SetEnable( m_CurrentImage, true);
	}
	else
	{
		if( m_pSprite != nullptr)
			m_pSprite->AddFlag( I3_NODEFLAG_DISABLE);
	}
}

void i3UIImageBoxEx::EnableCtrl( bool bTrue, bool bRecursive)
{
	i3UIImageBox::EnableCtrl( bTrue, bRecursive);

//	m_nControlState = 0;
}

INT32 i3UIImageBoxEx::AddTexture( i3Texture * pTex, REAL32 w, REAL32 h)
{
	SHAPETEXINFO * pInfo = (SHAPETEXINFO*) i3MemAlloc( sizeof( SHAPETEXINFO));

	pInfo->_pShape = i3UIShape::new_object();
	pInfo->_pTex = pTex;
	I3_MUST_ADDREF( pInfo->_pTex);

	i3::pack::RECT rect;

	rect.left = 0;
	rect.top = 0;
	if( w == 0)
		rect.right = static_cast<REAL32>(pTex->GetWidth());
	else
		rect.right = w;
	if( h == 0)
		rect.bottom = static_cast<REAL32>(pTex->GetHeight());
	else
		rect.bottom = h;

	_CalculateSizeByTexSize( rect.right, rect.bottom);

	pInfo->_pShape->setRect( &rect);

	pInfo->_pShape->SetTextureCoord( &rect);
	pInfo->_pShape->setTexcoordOffset( 0, 0);
	
	m_ShapeList.push_back( pInfo);

	return (INT32)m_ShapeList.size() - 1;
}

// 아래 함수는 파셜텍스쳐를 위해 급추가된것임.11.05.23.수빈
INT32	i3UIImageBoxEx::AddTexture( i3Texture * pTex, INT32 x, INT32 y, INT32 w, INT32 h)
{
	SHAPETEXINFO * pInfo = (SHAPETEXINFO*) i3MemAlloc( sizeof( SHAPETEXINFO));

	pInfo->_pShape = i3UIShape::new_object();
	pInfo->_pTex = pTex;
	I3_MUST_ADDREF( pInfo->_pTex);

	i3::pack::RECT rect;
	rect.left = 0;
	rect.right = static_cast<REAL32>(w);	
	rect.top = 0;
	rect.bottom =  static_cast<REAL32>(h);

	_CalculateSizeByTexSize( rect.right, rect.bottom);

	i3::pack::RECT rect_coord;		// 이부분은 확실치 않은데.만약 0~1좌표계라면 수정하면 됨. 11.05.23.수빈..
	rect_coord.left = static_cast<REAL32>(x);	
	rect_coord.right = static_cast<REAL32>(w); 
	rect_coord.top = 	static_cast<REAL32>(y);	
	rect_coord.bottom = static_cast<REAL32>(h);

	pInfo->_pShape->setRect( &rect);
	pInfo->_pShape->SetTextureCoord( &rect_coord);
	pInfo->_pShape->setTexcoordOffset( 0, 0);

	m_ShapeList.push_back( pInfo);

	return (INT32)m_ShapeList.size() - 1;
}

INT32	i3UIImageBoxEx::AddTextureMovie( i3Texture * pTex, i3::pack::RECT r1, i3::pack::RECT r2)
{
	SHAPETEXINFO * pInfo = (SHAPETEXINFO*) i3MemAlloc( sizeof( SHAPETEXINFO));

	pInfo->_pShape = i3UIShape::new_object();
	pInfo->_pTex = pTex;
	I3_MUST_ADDREF( pInfo->_pTex);

	i3::pack::RECT rect;
	rect.left = r1.left;
	rect.right = r1.right;
	rect.top = r1.top;
	rect.bottom =  r1.bottom;

	_CalculateSizeByTexSize( rect.right, rect.bottom);

	i3::pack::RECT rect_coord;		// 이부분은 확실치 않은데.만약 0~1좌표계라면 수정하면 됨. 11.05.23.수빈..
	rect_coord.left = r2.left;	
	rect_coord.right = r2.right; 
	rect_coord.top = 	r2.top;	
	rect_coord.bottom = r2.bottom;

	pInfo->_pShape->setRect( &rect);
	pInfo->_pShape->SetTextureCoord( &rect_coord);
	pInfo->_pShape->setTexcoordOffset( 0, 0);

	m_ShapeList.push_back( pInfo);
	return (INT32)m_ShapeList.size() - 1;
}


void i3UIImageBoxEx::SetCurrentImage( INT32 idx)
{
	if( m_CurrentImage != idx)
	{
		if( m_pSprite && m_CurrentImage != -1)
			m_pSprite->SetEnable( m_CurrentImage, false);

		if( (INT32)m_ShapeList.size() > idx && idx != -1)
		{
			m_CurrentImage = idx;

			if( m_pSprite)
				m_pSprite->SetEnable( m_CurrentImage, true);
		}
	}
}

INT32 i3UIImageBoxEx::FindShape( i3Texture * pTex, INT32 x, INT32 y, INT32 w, INT32 h)
{
	SHAPETEXINFO * pInfo;

	for(size_t i = 0; i < m_ShapeList.size(); i++)
	{
		pInfo = m_ShapeList[ i];

		if( pInfo->_pTex == pTex)
		{
			if( pInfo->_pShape != nullptr)
			{
				if( pInfo->_pShape->getCoordU() != x)
					continue;
				if( pInfo->_pShape->getCoordV() != y)
					continue;
				if( pInfo->_pShape->getCoordW() != w)
					continue;
				if( pInfo->_pShape->getCoordH() != h)
					continue;
				
				return (INT32)i;
			}
		}
	}

	return -1;

}

void i3UIImageBoxEx::DeleteShape( INT32 idx)
{
	I3_BOUNDCHK( idx, (INT32)m_ShapeList.size());

	SHAPETEXINFO * pInfo = m_ShapeList[ idx];

	I3_SAFE_RELEASE( pInfo->_pShape);
	I3_SAFE_RELEASE( pInfo->_pTex);

	I3MEM_SAFE_FREE( pInfo);

	m_ShapeList.erase( m_ShapeList.begin() + idx);
}

void i3UIImageBoxEx::DeleteShapeAll()
{
	for(size_t i = 0; i < m_ShapeList.size(); i++)
	{
		SHAPETEXINFO * pInfo = m_ShapeList[ i];

		I3_SAFE_RELEASE( pInfo->_pShape);
		I3_SAFE_RELEASE( pInfo->_pTex);

		I3MEM_SAFE_FREE( pInfo);
	}

	m_ShapeList.clear();
	m_CurrentImage = -1;
}

void i3UIImageBoxEx::SetShapeSize( REAL32 w, REAL32 h)
{
	for(size_t i = 0; i < m_ShapeList.size(); i++)
	{
		SHAPETEXINFO * pInfo = m_ShapeList[ i];
		if( pInfo->_pShape !=nullptr)
		{
			pInfo->_pShape->setWidth( w);
			pInfo->_pShape->setHeight( h);
			m_pSprite->SetSize( i, w, h);
		}
	}

	setSize( w, h);
}

void i3UIImageBoxEx::SetRotationRad( REAL32 val)
{
	for(size_t i = 0; i < m_ShapeList.size(); i++)
	{
		SHAPETEXINFO * pInfo = m_ShapeList[ i];
		if( pInfo->_pShape !=nullptr)
		{
			pInfo->_pShape->setRotation( val);
			m_pSprite->SetRotation( i, val);
		}
	}
}

void i3UIImageBoxEx::SetTextureCoord( REAL32 u1, REAL32 v1, REAL32 u2, REAL32 v2)
{
	i3::pack::RECT rect;
	rect.left = u1;	rect.top = v1;
	rect.right = u2;	rect.bottom = v2;

	for(size_t i = 0; i < m_ShapeList.size(); i++)
	{
		SHAPETEXINFO * pInfo = m_ShapeList[ i];
		if( pInfo->_pShape !=nullptr)
		{
			pInfo->_pShape->SetTextureCoord(&rect);
		}
	}
}



void i3UIImageBoxEx::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3UIImageBox::OnBuildObjectList( List);

	/*if( m_pSprite != nullptr)
		pList->Add( m_pSprite);*/
}


#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

namespace pack
{
	struct ALIGN4 UI_IMAGEBOXEX
	{
		UINT8	m_ID[4] = { 'U', 'I', 'X', '1' };
		OBJREF	m_Sprite = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif


UINT32 i3UIImageBoxEx::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::UI_IMAGEBOXEX data;

	Rc = i3UIImageBox::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;
	
	//if( m_pSprite != nullptr)
	//	data.m_Sprite = pResFile->GetObjectPersistID( m_pSprite);
	//else
		data.m_Sprite = 0;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;


	return Result;
}

UINT32 i3UIImageBoxEx::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::UI_IMAGEBOXEX data;

	Rc = i3UIImageBox::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	/*if( data.m_Sprite != 0)
	{
		i3Sprite2D * pSprite = (i3Sprite2D*) pResFile->FindObjectByID( data.m_Sprite);
		I3_REF_CHANGE( m_pSprite, pSprite);
	}*/

	return Result;
}

bool i3UIImageBoxEx::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3UIImageBox::OnSaveXML( pFile, pXML) == false)
		return false;

	/*if( m_pSprite != nullptr)
	{
		OBJREF ref = pFile->GetObjectPersistID( m_pSprite);
		pXML->addAttr( "Sprite2D", (INT32) ref);
	}*/


	return true;
}

bool i3UIImageBoxEx::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3UIImageBox::OnLoadXML( pFile, pXML) == false)
		return false;

	OBJREF ref;
	pXML->getAttr( "Sprite2D", (INT32*) &ref);

	/*if( ref != 0)
	{
		i3Sprite2D * pSprite = (i3Sprite2D*) pFile->FindObjectByID( ref);
		I3_REF_CHANGE( m_pSprite, pSprite);
	}*/

	return true;
}


// PropertyGrid 관련..
#if defined( I3_DEBUG)
void i3UIImageBoxEx::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	INT32 i;
	i3UIImageBox::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UIImageBoxEx");
		
		AddMFCProperty_Int32( List, &m_ImageCount, "Image Count");

		char szPath[ MAX_PATH];
		for( i = 0; i < m_ImageCount; i++)
		{
			i3::snprintf( szPath, MAX_PATH, "Path %d", i+1);
			AddMFCProperty_File( List, m_ImagePathList[ i], MAX_PATH, szPath); 
		}
		
	AddMFCProperty_EndGroup( List);
}

void i3UIImageBoxEx::OnTDKPropertyChanged( const char* szname)
{	
	i3UIImageBox::OnTDKPropertyChanged( szname);

	if( i3::generic_is_iequal( szname, "Image Count") )
	{
		ResetPathCount();
	}
	else if( i3::contain_string( szname, "Path") >= 0)
	{
		INT32 num = i3::atoi( &szname[ 5]);
		if( num != -1 && num < (INT32)m_ImagePathList.size())
		{
			char * pszPath = (char*) m_ImagePathList[ num];

			if( i3::generic_string_size( pszPath) > 0)
			{
				INT32 idx = _FindShapeByPath( pszPath);
				if( idx == -1)
				{
					if( i3UI::getViewer() != nullptr)
					{
						if( i3UI::getViewer()->GetFramework() != nullptr)
						{
							i3Texture * pNewTex = (i3Texture*) i3UI::getViewer()->GetFramework()->LoadResource( pszPath, 0);

							AddTexture( pNewTex);
						}
					}
				}
			}
			else
			{
				DeleteShape( num);
			}
		}
	}
}

void i3UIImageBoxEx::ResetPathCount( void)
{
	INT32 i;
	INT32 addCount = m_ImageCount-m_PreImageCount;

	if( addCount > 0)
	{
		for( i = 0; i < addCount; i++)
		{
			char * pszPath = (char*) i3MemAlloc( sizeof( char) * MAX_PATH);
			m_ImagePathList.push_back( pszPath);
		}
	}
	else if( addCount < 0)
	{
		for( i = m_PreImageCount-1; i >= m_ImageCount; i--)
		{
			char * pszPath = m_ImagePathList[ i];

			INT32 idx = _FindShapeByPath( pszPath);
			I3ASSERT( idx != -1);


			I3MEM_SAFE_FREE( pszPath);

			m_ImagePathList.erase( m_ImagePathList.begin() + i);
		}
	}

	m_PreImageCount = m_ImageCount;
}

#endif


extern "C" {
	int i3UIImageBoxEx_Callback( LuaState * L)
	{
		i3UIImageBoxEx * pCtrl = (i3UIImageBoxEx*) i3Lua::GetUserDataArg( L, 1);
		if( pCtrl != nullptr)
		{
			char szData[ 64];

			I3UIIMAGEBOX_PROC proc = (I3UIIMAGEBOX_PROC) i3Lua::GetUserDataArg( L, 2);

			if( proc != nullptr)
			{
				if( i3Lua::GetStringArg( L, 3, szData, sizeof( szData)))
				{
					proc( pCtrl, szData);
				}
			}
		}

		return 0;
	}
}

LuaFuncReg i3UIImageBoxEx_Glue[] = {
	{ "uiImageBoxEx_Callback",		i3UIImageBoxEx_Callback },		///<< ctrl, proc, szData
	{nullptr,							nullptr}
};

void i3UIImageBoxEx::RegisterLuaFunction( LuaState * pState)
{
	INT32 i;

	for( i = 0; i3UIImageBoxEx_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIImageBoxEx_Glue[i].name, i3UIImageBoxEx_Glue[i].func);
	}
}