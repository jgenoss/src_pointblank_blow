#include "i3SceneDef.h"
#include "i3TextureMatrixAttr.h"

I3_CLASS_INSTANCE( i3TextureMatrixAttr);

i3TextureMatrixAttr::i3TextureMatrixAttr(void)
{
	SetID( I3_ATTRID_TEXTURE_MATRIX);
}

void i3TextureMatrixAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	i3TextureMatrixAttr * pDest = static_cast<i3TextureMatrixAttr*>( pObj);

	pDest->SetBindType( GetBindType());
	pDest->SetMatrix( GetMatrix());
	pDest->SetProjectiveState( IsProjective());
	pDest->SetOutputElementCount( GetOutputElementCount());
	pDest->SetUVAnimTypeU( GetUVAnimTypeU());
	pDest->SetUVAnimTypeV( GetUVAnimTypeV());

	pDest->SetUB1( GetUB1());
	pDest->SetUB2( GetUB2());
	pDest->SetVB1( GetVB1());
	pDest->SetVB2( GetVB2());

}

void i3TextureMatrixAttr::Apply( i3RenderContext * pContext)
{
	REAL32 tm = pContext->GetDeltaSec();

	ApplyUVAnim( tm);

	pContext->SetTextureMatrix( GetBindType(), &m_Matrix, GetOutputElementCount(), IsProjective());

	
}

void i3TextureMatrixAttr::ApplyUVAnim( REAL32 tm)
{
	
	REAL32 fUB = 0.0f;
	REAL32 fVB = 0.0f;

	//U
	accTmU += tm;
	
	if( m_eUVAnimU == EUVANIM_NONE)
	{
		fUB = 0.0f;
	}
	else if( m_eUVAnimU == EUVANIM_LINEAR)
	{
		fUB = accTmU * m_fUB1 + m_fUB2;
		
		if( accTmU > 100.0f && i3Math::abs( fUB - (INT32)fUB) < 0.01f )	//reset
		{
			accTmU = 0.0f;
		}
	}
	else if( m_eUVAnimU == EUVANIM_SINUSOIDAL)
	{
		fUB = m_fUB2 * i3Math::sin( m_fUB1 * accTmU);

		if( accTmU * m_fUB1 >= I3_PI * 50.0f)	//reset
		{
			accTmU = 0.0f;
		}
	}

	//V
	accTmV += tm;

	if( m_eUVAnimV == EUVANIM_NONE)
	{
		fVB = 0.0f;
	}
	else if( m_eUVAnimV == EUVANIM_LINEAR)
	{
		fVB = accTmU * m_fVB1 + m_fVB2;

		if( accTmV > 100.0f && i3Math::abs( fVB - (INT32)fVB) < 0.01f )	//reset
		{
			accTmV = 0.0f;
		}
	}
	else if( m_eUVAnimV == EUVANIM_SINUSOIDAL)
	{
		fVB = m_fVB2 * i3Math::sin( m_fVB1 * accTmV);

		if( accTmV * m_fVB1 >= I3_PI * 50.0f)	//reset
		{
			accTmV = 0.0f;
		}
	}

	m_Matrix.m20 = fUB;
	m_Matrix.m21 = fVB;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED TEXTURE_MATRIX_INFO
	{
		UINT8			m_Bind = 0;
		UINT8			m_bProjective = 0;
		UINT8			m_Reserved[2] = { 0 };
		INT32			m_OutputElementCount = 0;
		i3::pack::MATRIX	m_Matrix;
	};


	struct PACKED TEXTURE_MATRIX_INFO_NEW
	{
		UINT8			m_ID[4] = { 'T', 'E', 'X', 'M' };

		UINT8			m_Bind = 0;
		UINT8			m_bProjective = 0;
		UINT8			m_Reserved[2] = { 0 };
		INT32			m_OutputElementCount = 0;
		i3::pack::MATRIX	m_Matrix;

		//UV Animation
		INT8			m_nUVAnimU = 0;
		INT8			m_nUVAnimV = 0;
		REAL32			m_fUB1 = 0.0f;
		REAL32			m_fUB2 = 0.0f;
		REAL32			m_fVB1 = 0.0f;
		REAL32			m_fVB2 = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextureMatrixAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	
	pack::TEXTURE_MATRIX_INFO_NEW Info;	//
	
	Info.m_Bind = (UINT8) GetBindType();
	Info.m_bProjective = (UINT8) IsProjective();
	Info.m_OutputElementCount = GetOutputElementCount();

	Info.m_nUVAnimU = (INT8)m_eUVAnimU;			//
	Info.m_nUVAnimV = (INT8)m_eUVAnimV;			//
	Info.m_fUB1 = m_fUB1;			//
	Info.m_fUB2 = m_fUB2;			//
	Info.m_fVB1 = m_fVB1;			//
	Info.m_fVB2 = m_fVB2;			//
	
	i3Matrix::Copy( & Info.m_Matrix, GetMatrix());

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureMatrixAttr::OnSave()", "Could not write Texture Matrix Information.");
	}
	
	return Rc;
}

UINT32 i3TextureMatrixAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3TextureMatrixAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_MATRIX_INFO_NEW Info;

	UINT8 id[4];

	Rc = pStream->Read( id, 4);
	I3_CHKIO( Rc);

	pStream->SetPosition( -4, STREAM_CURRENT);

	if( memcmp( id, "TEXM", 4) != 0)		//Old Version
	{
		pack::TEXTURE_MATRIX_INFO oldInfo;

		Rc = pStream->Read( &oldInfo, sizeof( oldInfo));
		I3_CHKIO( Rc);

		Info.m_Bind = oldInfo.m_Bind;
		Info.m_bProjective = oldInfo.m_bProjective;
		Info.m_Matrix = oldInfo.m_Matrix;
		Info.m_OutputElementCount = oldInfo.m_OutputElementCount;
		
	}
	else		//New Version
	{
		Rc = pStream->Read( &Info, sizeof( Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TextureMatrixAttr::OnLoad()", "Could not read Texture Matrix Information.");
		}

		m_fUB1 = Info.m_fUB1;	//
		m_fUB2 = Info.m_fUB2;	//
		m_fVB1 = Info.m_fVB1;	//
		m_fVB2 = Info.m_fVB2;	//

		switch( Info.m_nUVAnimU)
		{
		case 0:		m_eUVAnimU = EUVANIM_NONE;			break;
		case 1:		m_eUVAnimU = EUVANIM_LINEAR;		break;
		case 2:		m_eUVAnimU = EUVANIM_SINUSOIDAL;	break;
		default:	I3ASSERT_0;						break;
		}

		switch( Info.m_nUVAnimV)
		{
		case 0:		m_eUVAnimV = EUVANIM_NONE;			break;
		case 1:		m_eUVAnimV = EUVANIM_LINEAR;		break;
		case 2:		m_eUVAnimV = EUVANIM_SINUSOIDAL;	break;
		default:	I3ASSERT_0;						break;
		}
		
	}

	SetBindType( (I3G_TEXTURE_BIND) Info.m_Bind);
	SetProjectiveState( Info.m_bProjective != 0);
	SetOutputElementCount( Info.m_OutputElementCount);
	i3Matrix::Copy( GetMatrix(), & Info.m_Matrix);
	
	return Rc;
}

#if defined( I3_DEBUG)
void i3TextureMatrixAttr::Dump(void)
{
	I3TRACE( "%s :\n", meta()->class_name());

	i3Matrix::Dump( &m_Matrix);
}
#endif
