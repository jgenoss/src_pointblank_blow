#include "i3FrameworkPCH.h"
#include "i3RespawnObj.h"


I3_CLASS_INSTANCE( i3RespawnObj);


REAL32 i3RespawnObj::GetTheta(void)
{
	MATRIX * pMtx = GetMatrix();

	VEC3D	vAt;

	i3Vector::Normalize( &vAt, i3Matrix::GetAt( pMtx));

	return atan2f( getX( &vAt), getZ( &vAt));
}

void	i3RespawnObj::CopyTo( i3ElementBase *pDest, I3_COPY_METHOD method)
{
	i3GameObj::CopyTo( pDest, method);

	i3RespawnObj * pPoint = (i3RespawnObj *)pDest;

	pPoint->setRespawnStyle( m_nRespawnStyle);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct RESPAWN_OBJ
	{
		UINT8	m_ID[4] = { 'R', 'S', 'P', '1' };
		UINT32	m_nStyle = 0;
		UINT32	pad[6] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3RespawnObj::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;
	pack::RESPAWN_OBJ data;

	Result = i3GameObj::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_nStyle	= getRespawnStyle();

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3RespawnObj::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 nResult, Rc;
	pack::RESPAWN_OBJ data;

	nResult = i3GameObj::OnLoad( pResFile);
	I3_CHKIO( nResult);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	nResult += Rc;

	setRespawnStyle( data.m_nStyle);

	return nResult;
}
