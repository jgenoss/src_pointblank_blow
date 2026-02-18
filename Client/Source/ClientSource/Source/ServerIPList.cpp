#include "pch.h"
#include "ServerIPList.h"
#include "SimpleMemObject.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( CServerIPList);//, i3ElementBase);

CServerIPList::CServerIPList()
{
	memset( m_ServerCompare, 0, sizeof( m_ServerCompare ) );
	memset( m_strUseServerSet, 0, sizeof( m_strUseServerSet ) );
}

CServerIPList::~CServerIPList()
{	
}

void CServerIPList::InitIPString(void)
{
	for (size_t i = 0; i < m_listIPString.size(); i++)
	{
		m_listIPString[i].clear();
	}
}

bool CServerIPList::Load(void)
{
	i3IniParser Parser;
	i3FileStream file;

	if( file.Open( "config/lwsi_En.sif", STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3TRACE( "Could not open Server Configuration file.\n");
		return false;
	}

	const UINT32 size = file.GetSize();
	char * pCh = new char[size];
	MEMDUMP_NEW( pCh, sizeof(char) * size);
	CSimpleMemObject<char> buff ( pCh, true );
	i3MemStream strm;
	bool				bNation = false;

	file.Read( buff, size);
	BitRotateDecript( buff, size, 7);


	strm.Open( buff, STREAM_READ | STREAM_SHAREREAD, size);

	if (false == Parser.Open(&strm))
	{
		return false;
	}

	// Default
	if (false == Parser.ReadSection("Default"))
	{
		return false;
	}

	i3::safe_string_copy(m_strUseServerSet, Parser.GetStringValue("UseServerSet"), MAX_STRING_COUNT * 2 );

	// ServerSet
	if (false == Parser.ReadSection(m_strUseServerSet))
	{
		return false;
	}

	UINT32 nServerCount = 0;		
	if (false == Parser.GetValue("ServerCount", nServerCount, &nServerCount)
		|| nServerCount < 1)
	{
		return false;
	}

	UINT32 nServerDefault = 0;
	if (true == Parser.GetValue("ServerDefault", nServerDefault, &nServerDefault))
	{
		bNation = true;
	}

	
	if (g_pEnvSet->GetDefaultNation() == -1)
	{
		SetCurrentIPIdx(nServerDefault);
	}
	else if (g_pEnvSet->GetDefaultNation() + 1 > (INT32)nServerCount)
	{
		SetCurrentIPIdx(0);
	}
	else
	{
		m_listCurrentIP = g_pEnvSet->GetDefaultNation();
	}
	
	// Clear IP List
	m_listIPString.resize(nServerCount);
	InitIPString();

	char strLabel[MAX_STRING_COUNT * 2];		// 512
	char strServerIp[MAX_STRING_COUNT * 2];		// 512
	for(UINT32 i = 0; i < nServerCount; i++)
	{
		sprintf_s(strLabel, "ServerIp%02d", i);
		if ( Parser.GetStringValue(strLabel) == nullptr)
			return false;

		i3::safe_string_copy(strServerIp, Parser.GetStringValue(strLabel), MAX_STRING_COUNT * 2 );

		if (bNation == true)
		{
			INT32 itemp = 0;
			sprintf_s(strLabel, "ServerIdx%02d", i);
			if (false == Parser.GetValue(strLabel, itemp, &itemp))
			{
				return false;
			}
			// Add IP List
			m_listIPString[itemp] = strServerIp;
		}
		else
		{
			m_listIPString[0] = strServerIp;
		}
	}
	
	I3ASSERT(m_listIPString[m_listCurrentIP] != "");
	return true;
}

INT32 CServerIPList::GetCount(void)
{
	return (INT32)m_listIPString.size();
}

const char* CServerIPList::GetIP(INT32 idxIP)
{
	if (-1 < idxIP && idxIP < (INT32)m_listIPString.size())
		return m_listIPString[idxIP].c_str();
	else
		return "";
}

const char* CServerIPList::GetCurrentIP()
{
	I3ASSERT(-1 < m_listCurrentIP);
	I3ASSERT(m_listCurrentIP < (INT32)m_listIPString.size());

	return m_listIPString[m_listCurrentIP].c_str();
}

void CServerIPList::_RemoveInvalidIP(void)
{
	INT32 idx = 0;
	while( idx < (INT32) m_listIPString.size() )
	{
		const char* strIP = m_listIPString[idx].c_str();
		
		if( m_listIPString[idx].length() < 7 || INADDR_NONE == ::inet_addr(strIP))
		{
			i3::vu::erase_index(m_listIPString, idx);
		}
		else
		{
			idx++;
		}
	}
}
