#include "i3SceneDef.h"
#include "i3MaterialAttr.h"

I3_CLASS_INSTANCE( i3MaterialAttr);

i3MaterialAttr::i3MaterialAttr(void)
{
	SetID( I3_ATTRID_MATERIAL);
}

void i3MaterialAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetMaterial( &m_Diffuse, &m_Specular, &m_Emissive, m_Shininess, m_ValidFlag);
}

void i3MaterialAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3MaterialAttr * pDest = (i3MaterialAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	pDest->SetDiffuse( GetDiffuse());
	pDest->SetSpecular( GetSpecular());
	pDest->SetEmissive( GetEmissive());
	pDest->SetShininess( GetShininess());
	pDest->m_ValidFlag = m_ValidFlag;
}

bool i3MaterialAttr::IsSame( i3RenderAttr * pAttr)
{
	i3MaterialAttr * pAttr2 = (i3MaterialAttr *) pAttr;

	return	i3Color::IsSame( &m_Diffuse, pAttr2->GetDiffuse()) &&
			i3Color::IsSame( &m_Specular, pAttr2->GetSpecular()) &&
			i3Color::IsSame( &m_Emissive, pAttr2->GetEmissive()) &&
			(m_Shininess == pAttr2->m_Shininess) && 
			(m_ValidFlag == pAttr2->m_ValidFlag);
}

UINT32 i3MaterialAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3::pack::COLORREAL pvec;

	// Ambient
	{
		pvec.r = 0.0f;
		pvec.g = 0.0f;
		pvec.b = 0.0f;
		pvec.a = 0.0f;

		Rc = pStream->Write( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnSave()", "Could not write Ambient color value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Diffuse
	{
		pvec.r = i3Color::GetR( &m_Diffuse);
		pvec.g = i3Color::GetG( &m_Diffuse);
		pvec.b = i3Color::GetB( &m_Diffuse);
		pvec.a = i3Color::GetA( &m_Diffuse);

		Rc = pStream->Write( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnSave()", "Could not write Diffuse color value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Specular
	{
		pvec.r = i3Color::GetR( &m_Specular);
		pvec.g = i3Color::GetG( &m_Specular);
		pvec.b = i3Color::GetB( &m_Specular);
		pvec.a = i3Color::GetA( &m_Specular);

		Rc = pStream->Write( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnSave()", "Could not write Specular color value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Emissive
	{
		pvec.r = i3Color::GetR( &m_Emissive);
		pvec.g = i3Color::GetG( &m_Emissive);
		pvec.b = i3Color::GetB( &m_Emissive);
		pvec.a = i3Color::GetA( &m_Emissive);

		Rc = pStream->Write( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnSave()", "Could not write Emissive color value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Shininess
	{
		Rc = pStream->Write( &m_Shininess, sizeof(REAL32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnSave()", "Could not write Shininess value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3MaterialAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3MaterialAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	i3::pack::COLORREAL pvec;

	// Ambient
	{
		Rc = pStream->Read( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnLoad()", "Could not read Ambient color value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Diffuse
	{
		Rc = pStream->Read( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnLoad()", "Could not read Diffuse color value.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Color::Set( &m_Diffuse, pvec.r, pvec.g, pvec.b, pvec.a);
	}
	
	// Specular
	{
		Rc = pStream->Read( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnLoad()", "Could not read Specular color value.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Color::Set( &m_Specular, pvec.r, pvec.g, pvec.b, pvec.a);
	}

	// Emissive
	{
		Rc = pStream->Read( &pvec, sizeof(i3::pack::COLORREAL));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnLoad()", "Could not read Emissive color value.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Color::Set( &m_Emissive, pvec.r, pvec.g, pvec.b, pvec.a);
	}

	// Shininess
	{
		Rc = pStream->Read( &m_Shininess, sizeof(REAL32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MaterialAttr::OnLoad()", "Could not read shininess value.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

#if defined( I3_DEBUG)
void i3MaterialAttr::Dump(void)
{
	I3TRACE( "i3MaterialAttr : D(%.2f, %.2f, %.2f, %.2f), S(%.2f, %.2f, %.2f, %.2f), E(%.2f, %.2f, %.2f, %.2f)\n",
		i3Color::GetR( &m_Diffuse),
		i3Color::GetG( &m_Diffuse),
		i3Color::GetB( &m_Diffuse),
		i3Color::GetA( &m_Diffuse),

		i3Color::GetR( &m_Specular),
		i3Color::GetG( &m_Specular),
		i3Color::GetB( &m_Specular),
		i3Color::GetA( &m_Specular),

		i3Color::GetR( &m_Emissive),
		i3Color::GetG( &m_Emissive),
		i3Color::GetB( &m_Emissive),
		i3Color::GetA( &m_Emissive));
}
#endif
