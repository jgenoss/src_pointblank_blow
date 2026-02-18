#include "i3SceneDef.h"
#include "i3Base.h"
#include "i3LightAttr.h"

I3_CLASS_INSTANCE( i3LightAttr);

i3LightAttr::i3LightAttr(void)
{
	SetID( I3_ATTRID_LIGHT);
	m_pLight = i3Light::new_object();
}

i3LightAttr::~i3LightAttr(void)
{
	I3_SAFE_RELEASE( m_pLight);
}

void i3LightAttr::SetLightObject( i3Light * pLight)
{
	I3_REF_CHANGE( m_pLight, pLight);
}

void i3LightAttr::Apply( i3RenderContext * pContext)
{
	//pContext->AddLight( m_pLight);
}

void i3LightAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3LightAttr * pDest = (i3LightAttr *) pObj;

	i3RenderAttr::CopyTo( pObj, method);

	if( method == I3_COPY_REF)
	{
		pDest->SetLightObject( m_pLight);
	}
	else
	{
		m_pLight->CopyTo( pDest->m_pLight, method);
	}
}

UINT32 i3LightAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc;

	Rc = m_pLight->OnSave( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LightAttr::OnSave()", "Could not write light instance.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3LightAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3LightAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc;

	Rc = m_pLight->OnLoad( pResFile);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LightAttr::OnLoad()", "Could not read light instance.");
		return STREAM_ERR;
	}

	return Rc;
}

bool i3LightAttr::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return m_pLight->OnSaveXML( pFile, pXML);
}

bool i3LightAttr::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return m_pLight->OnLoadXML( pFile, pXML);
}

#if defined( I3_DEBUG)
void i3LightAttr::Dump(void)
{
	I3TRACE( "i3LightAttr : D(%.2f, %.2f, %.2f, %.2f), S(%.2f, %.2f, %.2f, %.2f), Direction (%.2f, %.2f, %.2f)\n",
		i3Color::GetR( getDiffuse()),
		i3Color::GetG( getDiffuse()),
		i3Color::GetB( getDiffuse()),
		i3Color::GetA( getDiffuse()),

		i3Color::GetR( getSpecular()),
		i3Color::GetG( getSpecular()),
		i3Color::GetB( getSpecular()),
		i3Color::GetA( getSpecular()),

		i3Vector::GetX( m_pLight->GetWorldSpaceDirection()),
		i3Vector::GetY( m_pLight->GetWorldSpaceDirection()),
		i3Vector::GetZ( m_pLight->GetWorldSpaceDirection()));
}

#endif
