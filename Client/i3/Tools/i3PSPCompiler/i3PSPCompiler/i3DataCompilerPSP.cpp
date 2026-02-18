#include "stdafx.h"
#include "i3DataCompilerPSP.h"

I3_CLASS_INSTANCE( i3DataCompilerPSP, i3ElementBase);

i3DataCompilerPSP::i3DataCompilerPSP(void)
{
	m_pScene = NULL;
	m_pTexture = NULL;

	m_Option =	I3PSP_CA_FORCE_16BITS_IMAGE | 
				I3PSP_CA_SWIZZLED_IMAGE | 
				I3PSP_CA_NATIVE_VERTEX_FORMAT | 
				I3PSP_CA_COMPILE_STATIC_GEOMETRY | 
				I3PSP_CA_OPTIMIZE_INDEX_ARRAY |
				I3PSP_CA_ROTATE_AXIS |
				I3PSP_CA_ADD_CLIPPING_BOUND;

	m_ImageCount = 0;
	m_VertexArrayCount = 0;
	m_IndexArrayCount = 0;
	m_OrgImageSize = 0;
	m_NewImageSize = 0;
	m_OrgVertexArraySize = 0;
	m_NewVertexArraySize = 0;
	m_OrgIndexArraySize = 0;
	m_NewIndexArraySize = 0;

	m_TargetPlatform = I3VF_PLATFORM_PSP;

	m_pUserLogProc = NULL;
	m_pUserData = NULL;

	m_szFileName[0] = 0;
}

i3DataCompilerPSP::~i3DataCompilerPSP(void)
{
	I3_SAFE_RELEASE( m_pScene);
}

void i3DataCompilerPSP::SetSceneGraph( i3Node * pNode)
{
	if( pNode != NULL)
	{
		pNode->AddRef();
	}

	I3_SAFE_RELEASE( m_pScene);

	m_pScene = pNode;

	if( m_pScene != NULL)
	{
		RetreiveStatistics();
	}
}

void i3DataCompilerPSP::SetTexture( i3Texture * pTex)
{
	if( pTex != NULL)
		pTex->AddRef();

	I3_SAFE_RELEASE( m_pTexture);

	m_pTexture = pTex;
}

void i3DataCompilerPSP::RetreiveStatistics(void)
{
}

static char tempstr[4096];

void i3DataCompilerPSP::Log( const char * pszText, ...)
{
	va_list marker;

	va_start( marker, pszText);

	vsprintf( tempstr, pszText, marker);

	if( m_pUserLogProc != NULL)
	{
		m_pUserLogProc( m_szFileName, tempstr, m_pUserData);
	}
}

BOOL i3DataCompilerPSP::Load( const char * pszFile)
{
	char szExt[MAX_PATH];

	i3String::SplitFileExt( pszFile, szExt, sizeof(szExt) - 1);

	if( i3String::Compare( szExt, "I3S") == 0)
	{
		i3SceneFile file;
		i3Node * pNode;

		if( file.Load( pszFile) == STREAM_ERR)
		{
			Log( "Could not load %s file", pszFile);
			return FALSE;
		}

		strcpy( m_szFileName, pszFile);

		pNode = file.GetSceneGraph();

		SetSceneGraph( pNode);
		SetTexture( NULL);

		pNode->Release();
	}
	else if( i3String::Compare( szExt, "I3I") == 0)
	{
		i3ImageFile file;
		i3Texture * pTex;

		pTex = file.Load( pszFile);
		if( pTex == NULL)
		{
			Log( "Could not load %s file.", pszFile);
			return FALSE;
		}

		strcpy( m_szFileName, pszFile);

		SetTexture( pTex);
		SetSceneGraph( NULL);

		pTex->Release();
	}

	return TRUE;
}

BOOL i3DataCompilerPSP::Save( const char * pszFile)
{
	if( m_pScene != NULL)
	{
		i3SceneFile file;
		i3List List;

		List.Add( m_pScene);

		//file.SetObjects( &List);
		file.SetSceneGraph( m_pScene);

		if( file.Save( pszFile) == STREAM_ERR)
		{
			Log( "Could not save %s file.", pszFile);
			return FALSE;
		}
	}
	else if( m_pTexture != NULL)
	{
		i3ImageFile file;

		if( file.SaveI3I( pszFile, m_pTexture) == FALSE)
		{
			Log( "Could not save %s file.", pszFile);
			return FALSE;
		}
	}

	return TRUE;
}

i3Texture *	 i3DataCompilerPSP::CompileDither16Bits( i3Texture * pTex)
{
	i3Texture * pNewTex = NULL;
	INT32 level;
	UINT32 w, h;
	char * pSrc, * pDest;
	BOOL Rv;
	
	if( pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_PSP)
		return pTex;

	if( pTex->GetBitsPerPixel() <= 16)
		return pTex;

	if( ((pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM) != 0) &&
		((pTex->GetFormat() & I3G_IMAGE_FORMAT_MASK_PLATFORM) != I3G_IMAGE_FORMAT_MASK_PSP))
	{
		Log( "Could not convert image from other native formats rather than PSP.");
		return pTex;
	}

	w = pTex->GetWidth();
	h = pTex->GetHeight();

	pNewTex = i3Texture::NewObjectRef();

	Rv = pNewTex->Create(	w, h, pTex->GetLevelCount(), I3G_IMAGE_FORMAT_ABGR_4444, pTex->GetUsageFlag());
	if( Rv == FALSE)
	{
		Log( "Could not create 16 bits image.");
		return pTex;
	}

	for( level = 0; level < (INT32) pTex->GetLevelCount(); level++)
	{
		pSrc = pTex->Lock( level);
		pDest = pNewTex->Lock( level);

		i3Image_Convert( pTex->GetFormat(), pSrc, w, h, 1, pTex->GetClut( level),
							pNewTex->GetFormat(), pDest, 1, NULL);

		pTex->Unlock( level);
		pNewTex->Unlock( level);

		w = w >> 1;
		h = h >> 1;
	}

	return pNewTex;
}

void i3DataCompilerPSP::CompileDither16Bits( i3List * pList)
{
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex, * pNewTex;
	INT32 i;

	for( i = 0; i < pList->GetCount(); i++)
	{
		pTexBind = (i3TextureBindAttr *) pList->Items[i];

		pTex = pTexBind->GetTexture();

		pNewTex = CompileDither16Bits( pTex);

		if( pNewTex != pTex)
		{
			pTexBind->SetTexture( pNewTex);
		}
	}
}

void i3DataCompilerPSP::CompileIndexArray( i3List * pList)
{
	// I3PSP_CA_OPTIMIZE_INDEX_ARRAY
}

void i3DataCompilerPSP::Rec_FindStaticNode( i3Node * pNode)
{
	if( pNode->IsFlag(I3_NODEFLAG_STATIC))
	{
		//m_pTracer->Trace( 

	}
}

void i3DataCompilerPSP::CompileStaticGeometry(void)
{
	// I3PSP_CA_COMPILE_STATIC_GEOMETRY
	i3OptBuildPSPCommandList opt;

	if( IsOption( I3PSP_CA_ROTATE_AXIS))
		opt.SetRotateAxis( TRUE);
	else
		opt.SetRotateAxis( FALSE);

	opt.Trace( m_pScene);
}

void i3DataCompilerPSP::Compile(void)
{
	if( m_pScene != NULL)
	{
		//////////////////////////////////////////////////////////////////////////////
		// Image Optimizations
		{
			i3List BindList;

			i3Scene::FindAttrByExactType( m_pScene, i3TextureBindAttr::GetClassMeta(), &BindList, true);

			if( IsOption( I3PSP_CA_FORCE_16BITS_IMAGE))
			{
				CompileDither16Bits( &BindList);
			}

			if( IsOption( I3PSP_CA_SWIZZLED_IMAGE))
			{
				i3OptPSPSwizzledImage * pOpt;

				pOpt = i3OptPSPSwizzledImage::NewObject();

				pOpt->Trace( m_pScene);
				//CompileSwizzledImage( &BindList);

				pOpt->Release();
			}
		} 

		//////////////////////////////////////////////////////////////////////////////
		// Vertex Array Optimizations
		{
			if( IsOption( I3PSP_CA_NATIVE_VERTEX_FORMAT))
			{
				i3OptConvertVertexFormat opt;

				opt.SetPositionConvertEnable( GetPositionConvertEnable());
				opt.SetPositionType( GetPositionType());

				opt.SetNormalConvertEnable( GetNormalConvertEnable());
				opt.SetNormalType( GetNormalType());

				opt.SetColorConvertEnable( GetColorConvertEnable());
				opt.SetColorType( GetColorType());

				opt.SetWeightConvertEnable( GetWeightConvertEnable());
				opt.SetWeightType( GetWeightType());

				opt.SetTextureCoordConvertEnable( GetTextureCoordConvertEnable());
				opt.SetTextureCoordType( GetTextureCoordType());

				opt.SetTargetPlatform( m_TargetPlatform);

				opt.Trace( m_pScene);
				//CompileNativeVertexFormat( &GeoList);
			}
		}

		//////////////////////////////////////////////////////////////////////////////
		// Compile Static Geometry
		if( IsOption( I3PSP_CA_COMPILE_STATIC_GEOMETRY))
			CompileStaticGeometry();
	}
	else if( m_pTexture != NULL)
	{
		i3Texture * pTex;

		if( IsOption( I3PSP_CA_FORCE_16BITS_IMAGE))
		{
			pTex = CompileDither16Bits( m_pTexture);
			
			if( pTex != m_pTexture)
			{
				SetTexture( pTex);
			}
		}

		if( IsOption( I3PSP_CA_SWIZZLED_IMAGE))
		{
			i3OptPSPSwizzledImage * pOpt;

			pOpt = i3OptPSPSwizzledImage::NewObject();

			pTex = pOpt->MakeSwizzledImage( m_pTexture);
			
			if( pTex != m_pTexture)
			{
				SetTexture( pTex);
			}

			pOpt->Release();
		}
	}
}