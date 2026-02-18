#pragma once

#include "Avatar.h"

class ConvertBlendIndex
{
private:
	i3Skeleton *	m_pFrameSkel;	// ¥Î«• Skeleton

protected:
	void	_ConvertFullPath( char * pDest, const char * pszPath);
	void	_BuildBoneMap( i3LOD * pLOD);
	void	_ConvertBlend( i3VertexArray * pVA, INT32 * boneMap, UINT32 boneMapCnt);
	
public:
	ConvertBlendIndex();

	void	LoadFrameSkel( const char * pszFileName, INT32 lod);
	void	ConvertSG( const char * pszFrame, const char * pszFileName);

};