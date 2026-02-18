#if !defined( __I3_TDK_UTIL_COMMON_H)
#define __I3_TDK_UTIL_COMMON_H

namespace i3TDK
{
	I3_EXPORT_TDK	INT32			getTerrainTypeCount(void);
	I3_EXPORT_TDK	const TCHAR *	getTerrainTypeName( I3_TERRAIN_TYPE idx);
	I3_EXPORT_TDK	I3_TERRAIN_TYPE	getTerrainTypeValue( const TCHAR* pszName);

	I3_EXPORT_TDK	void			GetNodeFlagString( i3Node * pNode, TCHAR* outStr, INT32 outBuffSize);
};

#endif
