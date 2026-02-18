#include "stdafx.h"
#include "i3FMODGeometry.h"
#include "i3SoundContextFMOD.h"
#include "i3SoundGlobal.h"

I3_CLASS_INSTANCE( i3FMODSoundGeometry);

i3FMODSoundGeometry::i3FMODSoundGeometry()
{
	m_listGeometry.reserve( 32);
}

i3FMODSoundGeometry::~i3FMODSoundGeometry()
{
	ReleaseGeometryGroup();
}

void i3FMODSoundGeometry::Create( i3SoundContext * pCtx)
{
	m_pSndCtx = (i3SoundContextFMOD*)pCtx;

	ReleaseGeometryGroup();
}

bool i3FMODSoundGeometry::AddGeometry( I3SND_FMOD_POLYGON * pPoly)
{
	FMOD::System * pFMODSystem = m_pSndCtx->getFMODSystem();

	FMOD::Geometry * pNew;

	INT32 nVCnt = pPoly->_nVertexCount % 3;
	INT32 nTriCnt = (INT32)(pPoly->_nVertexCount / 3);
	if( nVCnt != 0)
	{
		nVCnt = pPoly->_nVertexCount  - nVCnt;
		nTriCnt = nTriCnt - 1;
	}
	else
	{
		nVCnt = pPoly->_nVertexCount;
	}
	
	FMOD_RESULT rv = pFMODSystem->createGeometry( nTriCnt, nVCnt, &pNew);
	ERRCHECK( rv);

	INT32 idx = 0;

	for( INT32 i = 0; i < nVCnt;)
	{
		FMOD_VECTOR * pVec = (FMOD_VECTOR*)pPoly->_pvVertexArray + i;

		pNew->addPolygon( pPoly->_rDirectOcclusion, pPoly->_rReverbOcclusion, (pPoly->_bDoubleSided > 0)? true : false, 3, pVec, &idx);

		i +=3;
	}

	m_listGeometry.push_back( pNew);

	return true;
}

UINT32 i3FMODSoundGeometry::LoadGeometryGroup( const char * pszFileName)
{
	UINT32 Result, Rc;
	i3FileStream File;

	if( File.Open( pszFileName, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Couldn't Open File: %s", pszFileName);
		return false;
	}

	INT32	nGeomCnt = 0;

	Result = File.Read( &nGeomCnt, sizeof( INT32));
	I3_CHKIO( Result);

	Rc	= File.Read( &m_rMapScale, sizeof( REAL32));
	I3_CHKIO( Result);
	Result += Rc;

	I3ASSERT( nGeomCnt < 1000000);

	for( INT32 i = 0; i< nGeomCnt; ++i)
	{
		I3SND_FMOD_POLYGON Poly;

		Rc = File.Read( &Poly, sizeof( I3SND_FMOD_POLYGON));
		I3_CHKIO( Result);
		Result += Rc;

		//아..이거 맘에 무쟈게 안든다...
		I3_BOUNDCHK( Poly._nVertexCount, 1000000);
		Poly._pvVertexArray = (i3::pack::VEC3D*)i3MemAlloc( Poly._nVertexCount * sizeof( i3::pack::VEC3D));
		I3ASSERT( Poly._pvVertexArray != nullptr);

		Rc = File.Read( Poly._pvVertexArray, Poly._nVertexCount * sizeof( i3::pack::VEC3D));
		I3_CHKIO( Rc);
		Result += Rc;

		AddGeometry( &Poly);

		I3MEM_SAFE_FREE( Poly._pvVertexArray);
	}

	FMOD::System * pFMODSystem = m_pSndCtx->getFMODSystem();

	pFMODSystem->setGeometrySettings( m_rMapScale);

	I3TRACE("SCALE: %f, cnt: %d\n", m_rMapScale, nGeomCnt);

	return Result;
}

bool i3FMODSoundGeometry::ReleaseGeometryGroup( void)
{
	for( size_t i = 0; i< m_listGeometry.size(); ++i)
	{
		FMOD::Geometry * pGeom = m_listGeometry[i];

		pGeom->release();
	}

	m_listGeometry.clear();

	return true;
}
