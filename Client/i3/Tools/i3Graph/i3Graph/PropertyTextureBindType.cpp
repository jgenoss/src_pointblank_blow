#include "stdafx.h"
#include "PropertyTextureBindType.h"

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_IMAGE_FORMAT Property 
CPropertyTextureBindType::CPropertyTextureBindType()
{
	m_Style |= I3_LIST; 
}

CPropertyTextureBindType::~CPropertyTextureBindType()
{

}

void CPropertyTextureBindType::Set(const char * pszValueStr)
{
	if( i3String::Compare((char *)pszValueStr,		"Multi-Texture 1") == 0)					m_value = I3G_TEXTURE_BIND_DIFFUSE1;
	else if(i3String::Compare((char *)pszValueStr,	"Multi-Texture 2") == 0)					m_value = I3G_TEXTURE_BIND_DIFFUSE2;
	else if(i3String::Compare((char *)pszValueStr,	"Multi-Texture 3") == 0)					m_value = I3G_TEXTURE_BIND_DIFFUSE3;
	else if(i3String::Compare((char *)pszValueStr,	"Shadow Map") == 0)							m_value = I3G_TEXTURE_BIND_SHADOW;
	else if(i3String::Compare((char *)pszValueStr,	"Specular Map") == 0)						m_value = I3G_TEXTURE_BIND_SPECULAR;
	else if(i3String::Compare((char *)pszValueStr,	"Reflect Map") == 0)						m_value = I3G_TEXTURE_BIND_REFLECT;
	else if(i3String::Compare((char *)pszValueStr,	"Normal Map") == 0)							m_value = I3G_TEXTURE_BIND_NORMAL;			
	else if(i3String::Compare((char *)pszValueStr,	"PRT Map") == 0)							m_value = I3G_TEXTURE_BIND_PRT;
	else if(i3String::Compare((char *)pszValueStr,	"Light Map") == 0)							m_value = I3G_TEXTURE_BIND_LIGHT;
	else																						m_value = I3G_TEXTURE_BIND_DIFFUSE;

}

void CPropertyTextureBindType::Get(char * pszValueStr)
{
	switch(m_value)
	{
		case I3G_TEXTURE_BIND_DIFFUSE1				: strcpy( pszValueStr, "Multi-Texture 1");		break;
		case I3G_TEXTURE_BIND_DIFFUSE2				: strcpy( pszValueStr, "Multi-Texture 2");		break;
		case I3G_TEXTURE_BIND_DIFFUSE3				: strcpy( pszValueStr, "Multi-Texture 3");		break;
		case I3G_TEXTURE_BIND_SHADOW				: strcpy( pszValueStr, "Shadow Map");			break;
		case I3G_TEXTURE_BIND_LIGHT					: strcpy( pszValueStr, "Light Map");			break;
		case I3G_TEXTURE_BIND_SPECULAR				: strcpy( pszValueStr, "Specular Map");			break;
		case I3G_TEXTURE_BIND_REFLECT				: strcpy( pszValueStr, "Reflect Map");		break;
		case I3G_TEXTURE_BIND_NORMAL				: strcpy( pszValueStr, "Normal Map");			break;
		case I3G_TEXTURE_BIND_PRT					: strcpy( pszValueStr, "PRT Map");				break;
		default 									: strcpy( pszValueStr, "Diffuse Map");			break;
	}
}

void CPropertyTextureBindType::GetValueList(i3StringList * pList)
{
	pList->Add("Diffuse Map");					
	pList->Add("Multi-Texture 1");					
	pList->Add("Multi-Texture 2");				
	pList->Add("Multi-Texture 3");				
	pList->Add("Shadow Map");					
	pList->Add("Specular Map");				
	pList->Add("Reflect Map");			
	pList->Add("Normal Map");				
	pList->Add("PRT Map");
}

void CPropertyTextureBindType::OnChangeList(void)
{
	//πÃ¡§
}
