#include "pch.h"
#include "PathExporter.h"

#define I3LEVEL_PATHPOINT_VER2

CPathExporter::CPathExporter(void)
{
}

CPathExporter::~CPathExporter(void)
{
	_RemoveAllPoints();
	_RemoveAllLinks();
}

INT32 CPathExporter::_FindPoint( VEC3D * pPt, REAL32 fRadius)
{
	
	PATHPOINT_INFO * pInfo;
	VEC3D diff;

	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		pInfo = m_PointList[i];

		i3Vector::Sub( &diff, &pInfo->m_Point, pPt);

		if( i3Vector::Length( &diff) < 1.f)		//(fRadius + pInfo->m_fRadius))
		{
			return i;
		}
	}

	return -1;
}

INT32 CPathExporter::_AddPoint( VEC3D * pPt, REAL32 fRadius, REAL32 fTheta )
{
	PATHPOINT_INFO * pInfo;
	INT32 idx;

	idx = _FindPoint( pPt, fRadius);
	if( idx != -1)
	{
		pInfo = m_PointList[ idx];

		pInfo->m_fRadius = MAX( pInfo->m_fRadius, fRadius);
		pInfo->m_fTheta	= MAX( pInfo->m_fTheta, fTheta );
		pInfo->m_UsedCount++;

		return idx;
	}

	pInfo = new PATHPOINT_INFO;

	i3Vector::Copy( &pInfo->m_Point, pPt);
	pInfo->m_fRadius = fRadius;
	pInfo->m_fTheta	= fTheta;
	pInfo->m_UsedCount = 1;

	m_PointList.push_back( pInfo);

	return (INT32)m_PointList.size() - 1;
}

void CPathExporter::_RemoveAllPoints(void)
{
	
	PATHPOINT_INFO * pInfo;

	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		pInfo = m_PointList[i];

		delete pInfo;
	}

	m_PointList.clear();
}

void CPathExporter::_RemoveAllLinks(void)
{

	PATHLINK_INFO * pInfo;

	for(size_t i = 0; i < m_LinkList.size(); i++)
	{
		pInfo = m_LinkList[i];

		delete pInfo;
	}

	m_LinkList.clear();
}

void CPathExporter::_BuildPointList(void)
{
}

void CPathExporter::_BuildLinkList(void)
{

}

//void CPathExporter::_FindLinkByPoint(  INT32 linkIndex, INT32 endPtIndex, List * pList)
//{
//
//}

bool CPathExporter::ExportCode( const char * pszPath )
{
	
	BOOL Rv;
	char szTemp[4096];

	i3FileStream stream;

	Rv = stream.Create( pszPath, STREAM_WRITE);
	if( Rv == FALSE)
	{
		return false;
	}

	/////////////////////////////////////////////////////////////////////
	// Points의 Export
	stream.Printf( szTemp, "// Point Array\n");
	stream.Printf( szTemp, "NavigationPoint g_PointTable[%d] =\n", (INT32)m_PointList.size());
	stream.Printf( szTemp, "{\n");

	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		PATHPOINT_INFO * pInfo;

		pInfo = m_PointList[i];

		stream.Printf( szTemp, "\t{\t{	%ff, %ff, %ff	},	%ff\t},	// %d\n",	
			i3Vector::GetX( &pInfo->m_Point),
			i3Vector::GetY( &pInfo->m_Point),
			i3Vector::GetZ( &pInfo->m_Point),
			pInfo->m_fRadius,
			i);
	}

	stream.Printf( szTemp, "};\n");

	stream.Printf( szTemp, "\n\n// Link Table (각 Link들의 Point Index Array)\n");

	/////////////////////////////////////////////////////////////////////
	// Path Points의 Export
	for(size_t i = 0; i < m_LinkList.size(); i++)
	{
		PATHLINK_INFO * pLink = m_LinkList[i];

		stream.Printf( szTemp, "UINT32	g_Link%d_Points[] = {\t", i);

		for(size_t j = 0; j < pLink->m_PointList.size(); j++)
		{
			if( j > 0)
				stream.Printf( szTemp, ", ");

			stream.Printf( szTemp, "%d", (INT32) pLink->m_PointList[j]);
		}

		stream.Printf( szTemp, " }; \n");
	}

	stream.Printf( szTemp, "\n\n// Link Table 정보\n");
	/////////////////////////////////////////////////////////////////////
	// Path Link의 Export
	for(size_t i = 0; i < m_LinkList.size(); i++)
	{
		PATHLINK_INFO * pLink = m_LinkList[i];

		if( pLink->m_StartLinkList.size() > 0)
		{
			stream.Printf( szTemp, "UINT32	g_Link%d_StartLinks[] = {\t", i);

			for(size_t j = 0; j < pLink->m_StartLinkList.size(); j++)
			{
				if( j > 0)
					stream.Printf( szTemp, ", ");

				stream.Printf( szTemp, "%d", (INT32) pLink->m_StartLinkList[j]);
			}

			stream.Printf( szTemp, " }; \n");
		}

		if( pLink->m_EndLinkList.size() > 0)
		{
			stream.Printf( szTemp, "UINT32	g_Link%d_EndLinks[] = {\t", i);

			for(size_t j = 0; j < pLink->m_EndLinkList.size(); j++)
			{
				if( j > 0)
					stream.Printf( szTemp, ", ");

				stream.Printf( szTemp, "%d", (INT32) pLink->m_EndLinkList[j]);
			}

			stream.Printf( szTemp, " }; \n");
		}
	}

	stream.Printf( szTemp, "\n\n");

	/////////////////////////////////////////////////////////////////////
	// Path의 Information...
	stream.Printf( szTemp, "NavigationPath	g_PathTable[] =\n");
	stream.Printf( szTemp, "{\n");

	for(size_t i = 0; i < m_LinkList.size(); i++)
	{
		PATHLINK_INFO * pLink = m_LinkList[i];
		char szStartLink[64], szEndLink[64];

		if( pLink->m_StartLinkList.size() > 0)
			sprintf( szStartLink, "g_Link%d_StartLinks", i);
		else
			sprintf( szStartLink, "NULL");

		if( pLink->m_EndLinkList.size() > 0)
			sprintf( szEndLink, "g_Link%d_EndLinks", i);
		else
			sprintf( szEndLink, "NULL");

		stream.Printf( szTemp, "	{ %d,	g_Link%d_Points,	%d,	%s,		%d, %s	},		// Path %d\n",
				(INT32)pLink->m_PointList.size(),
				i,
				(INT32)pLink->m_StartLinkList.size(),
				szStartLink,
				(INT32)pLink->m_EndLinkList.size(),
				szEndLink,
				i);
	}
	stream.Printf( szTemp, "};\n");

	stream.Close();
	
	return true;
}

#pragma pack(push, 4 )
namespace pack
{
	struct PATHPOINTHEAD
	{
#if defined( I3_LEVEL_PATH_VER2 )
		UINT8	m_ID[4] = { 'L', 'V', 'P', 'X' };
#else
		UINT8	m_ID[4] = { 'L', 'V', 'P', 'T' };
#endif
		UINT32	m_nPointCount = 0;
		UINT32	m_nLinkCount = 0;
	};

	struct PATHLINKINFO
	{
		UINT8	m_nSLinkCount = 0;
		UINT8	m_nELinkCount = 0;

		UINT32	m_pSLinks[16] = { 0 };
		UINT32	m_pELinks[16] = { 0 };

	} ;
}

#pragma pack(pop)

bool CPathExporter::ExportPersist( const char * pszPath )
{

	BOOL Rv;
	i3FileStream stream;

	i3::pack::VEC3D vec;

	pack::PATHPOINTHEAD head;
	pack::PATHLINKINFO	link;

	Rv = stream.Create( pszPath, STREAM_WRITE );
	if( Rv == FALSE )
	{
		return false;
	}

	// Header
	head.m_nPointCount		= m_PointList.size();
	head.m_nLinkCount		= m_LinkList.size();

	stream.Write( &head, sizeof( head ) );

	/////////////////////////////////////////////////////////////////////
	// Points의 Export
	for(size_t i = 0; i < m_PointList.size(); i++)
	{
		PATHPOINT_INFO * pInfo;

		pInfo = m_PointList[i];

		vec.x		= i3Vector::GetX( &pInfo->m_Point );
		vec.y		= i3Vector::GetY( &pInfo->m_Point );
		vec.z		= i3Vector::GetZ( &pInfo->m_Point );

		stream.Write( &vec, sizeof( vec ) );
		stream.Write( &pInfo->m_fRadius, sizeof( REAL32 ) );
		stream.Write( &pInfo->m_fTheta, sizeof( REAL32 ) );
	}

	// Path Points의 Export
	size_t	nPoint;
	UINT32	pIdx[ 64 ];
	for(size_t i = 0; i < m_LinkList.size(); i++)
	{
		PATHLINK_INFO * pLink = m_LinkList[i];

		nPoint = pLink->m_PointList.size();

		memset( pIdx, 0, sizeof( pIdx ) );

		for(size_t j = 0; j < nPoint; j++)
		{
			pIdx[ j ] = pLink->m_PointList[j];
		}

		stream.Write( &nPoint, sizeof( nPoint ) );
		stream.Write( pIdx, sizeof( UINT32 ) * nPoint );
	}

	// Path Link의 Export
	for(size_t i = 0; i < m_LinkList.size(); i++)
	{
		PATHLINK_INFO * pLink = m_LinkList[i];

		if( pLink->m_StartLinkList.size() > 0)
		{
			link.m_nSLinkCount	= ( UINT8 ) pLink->m_StartLinkList.size();
			memset( link.m_pSLinks, 0, sizeof( link.m_pSLinks ) );

			for(size_t j = 0; j < pLink->m_StartLinkList.size(); j++)
			{
				link.m_pSLinks[ j ] = pLink->m_StartLinkList[j];
			}
		}
		else
			link.m_nSLinkCount	= 0;

		if( pLink->m_EndLinkList.size() > 0)
		{
			link.m_nELinkCount	= ( UINT8 ) pLink->m_EndLinkList.size();
			memset( link.m_pELinks, 0, sizeof( link.m_pELinks ) );

			for(size_t j = 0; j < pLink->m_EndLinkList.size(); j++)
			{
				link.m_pELinks[ j ] = pLink->m_EndLinkList[j];
			}
		}
		else
			link.m_nELinkCount = 0;

		stream.Write( &link.m_nSLinkCount, sizeof( link.m_nSLinkCount ) );
		stream.Write( &link.m_nELinkCount, sizeof( link.m_nELinkCount ) );

		if( link.m_nSLinkCount > 0 )
			stream.Write( link.m_pSLinks, sizeof( UINT32 ) * link.m_nSLinkCount );
		if( link.m_nELinkCount > 0 )
			stream.Write( link.m_pELinks, sizeof( UINT32 ) * link.m_nELinkCount );
	}

	stream.Close();

	return true;
}

bool CPathExporter::Export( const char * pszPath)
{
	_BuildPointList();

	_BuildLinkList();

	return ExportPersist( pszPath );
}
