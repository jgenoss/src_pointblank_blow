#include "pch.h"
#include "LinkDB.h"

I3_CLASS_INSTANCE( CLinkDB, i3Thread );

CLinkDB::CLinkDB()
{
	m_pDB = NULL;
}

CLinkDB::~CLinkDB()
{
	OnDestroy();
}

BOOL CLinkDB::OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut, char* strFilePath )
{
	i3String::Format( m_strUdlFilePath, SERVER_STRING_COUNT, "File Name=%s\\%s;", g_pCCurrentPath, strFilePath );

	m_pDB = CADODatabase::NewObject();
	if( NULL == m_pDB )											return FALSE;
	if( FALSE == m_pDB->Connect( m_strUdlFilePath ) )			return FALSE;

	return CLinkBase::OnCreate( pRingLinkIn, pRingLinkOut );
}

void CLinkDB::OnDestroy()
{
	I3_SAFE_RELEASE( m_pDB );
}

void CLinkDB::ReconnectDB( char* strQuery )
{
	WriteLog( m_i32LogThreadIdx, "[DBAS] Query Fail : %s", strQuery );

	if( m_pDB->IsConnected() )
	{
		WriteLog( m_i32LogThreadIdx, "[Link] DataBase Connected" );
	}
	else
	{
		WriteLog( m_i32LogThreadIdx, "[Link] DataBase DisConnected" );
	}

	m_pDB->Close();
	if( m_pDB->Connect( m_strUdlFilePath ) )
	{
		WriteLog( m_i32LogThreadIdx, "[Link] ReConnect Success" );
	}
	else
	{
		WriteLog( m_i32LogThreadIdx, "[Link] ReConnect Fail" );
	}
}