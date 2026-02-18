#include "stdafx.h"
#include "i3PerfConfig.h"

I3_CLASS_INSTANCE( i3PerfConfig, i3NamedElement);

i3PerfConfig::i3PerfConfig(void)
{
	INT32 i;

	m_szPath[0] = 0;

	m_szLastI3S[0] = 0;

	m_Width = 800;
	m_Height = 600;

	m_bShadow = FALSE;
	
	for( i = 0; i < MAX_CAM_RECORD; i++)
	{
		i3Matrix::Identity( &m_CamPos[i]);
	}
}

i3PerfConfig::~i3PerfConfig(void)
{
}

bool i3PerfConfig::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 i;
	char szName[256];

	pXML->addAttr( "LastI3S", m_szLastI3S);
	pXML->addAttr( "Width",		m_Width);
	pXML->addAttr( "Height",	m_Height);
	
	for( i = 0; i < MAX_CAM_RECORD; i++)
	{
		sprintf( szName, "CamRecord%d", i);

		i3Matrix::SetToXML( pXML, szName, & m_CamPos[i]);
	}

	pXML->addAttr( "Shadow",	m_bShadow);

	return true;
}

bool i3PerfConfig::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( pXML->getAttr( "LastI3S") != NULL)
	{
		i3String::Copy( m_szLastI3S, pXML->getAttr( "LastI3S"), MAX_PATH );
	}
	else
	{
		m_szLastI3S[0] = 0;
	}

	pXML->getAttr( "Width", &m_Width);
	pXML->getAttr( "Height", &m_Height);

	INT32 i;
	char szName[256];

	for( i = 0; i < MAX_CAM_RECORD; i++)
	{
		sprintf( szName, "CamRecord%d", i);

		i3Matrix::GetFromXML( pXML, szName, &m_CamPos[i]);
	}

	pXML->getAttr( "Shadow", &m_bShadow);

	return true;
}

i3PerfConfig *		i3PerfConfig::Load( const char * pszName)
{
	i3XMLFile file;
	i3PerfConfig * pConfig = NULL;	

	if( file.Load( pszName) == false)
	{
		pConfig = i3PerfConfig::NewObject();
		i3String::Copy( pConfig->m_szPath, pszName, MAX_PATH );
		return pConfig;
	}

	pConfig = (i3PerfConfig *) file.getKeyObject();

	pConfig->AddRef();

	i3String::Copy( pConfig->m_szPath, pszName, MAX_PATH );

	return pConfig;
}

void i3PerfConfig::Save(void)
{
	i3PerfConfig::Save( m_szPath, this);
}

bool i3PerfConfig::Save( const char * pszName, i3PerfConfig * pConfig)
{
	i3XMLFile file;

	I3ASSERT( pConfig != NULL);

	file.setKeyObject( pConfig);

	file.Save( pszName);

	i3String::Copy( pConfig->m_szPath, pszName, MAX_PATH );

	return true;
}

