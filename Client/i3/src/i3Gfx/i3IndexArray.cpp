#include "i3GfxType.h"
#include "i3Base.h"
#include "i3IndexArray.h"
#include "i3GfxUtil.h"

I3_CLASS_INSTANCE( i3IndexArray);

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED INDEXBUFFER_INFO_OLD
	{
		UINT32	m_Count = 0;
		UINT32	m_PersistUsage = 0;
	};

	struct PACKED INDEXBUFFER_INFO
	{
		INT8	m_ID[4] = { 'I', 'I', 'A', '2' };

		UINT32	m_Count = 0;
		UINT32	m_PersistUsage = 0;
		INT32	m_Mode = 0;
		UINT32	m_State = 0;

		UINT32	pad[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32		i3IndexArray::OnSave( i3ResourceFile * pFile)
{
	UINT32 Rc, Result = 0;
	pack::INDEXBUFFER_INFO Info;
	UINT32 unitSz;
	
	Info.m_Count = GetCount();
	Info.m_PersistUsage = i3Gfx::Convert( GetUsageFlag());
	Info.m_Mode = GetDataType();
	Info.m_State = m_State;

	Rc = pFile->Write( &Info, sizeof(pack::INDEXBUFFER_INFO));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3IndexArray::OnSave()", "Could not write index buffer informaion.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Index Data.
	Lock();

	if( m_DataType == I3G_IDX_16BITS)
		unitSz = sizeof( UINT16);
	else
		unitSz = sizeof( UINT32);

	Rc = pFile->Write( GetData(), unitSz * GetCount());
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3IndexArray::OnSave()", "Could not write index data.");
		return STREAM_ERR;
	}
	Result += Rc;

	Unlock();

	return Result;
}

UINT32		i3IndexArray::OnLoad( i3ResourceFile * pFile)
{
	UINT32 Rc, Result = 0;
	pack::INDEXBUFFER_INFO Info;
	UINT32 unitSz;
	INT8	id[4];

	setRestorePoint( pFile->GetStream());

	Rc = pFile->Read( id, 4);
	I3_CHKIO( Rc);
	Result += Rc;

	pFile->GetStream()->SetPosition( -4, STREAM_CURRENT);

	if( memcmp( id, "IIA2", 4) != 0)
	{
		// Old Version
		pack::INDEXBUFFER_INFO_OLD oldInfo;

		Rc = pFile->Read( &oldInfo, sizeof(oldInfo));
		I3_CHKIO( Rc);
		Result += Rc;

		Info.m_Count			= oldInfo.m_Count;
		Info.m_PersistUsage		= oldInfo.m_PersistUsage;
		Info.m_Mode				= I3G_IDX_16BITS;
		Info.m_State			= 0;
	}
	else
	{
		Rc = pFile->Read( &Info, sizeof(Info));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	SetDataType( (I3G_IDX_TYPE) Info.m_Mode);
	m_State = Info.m_State;

	setThreadMode( pFile->InThread());
	if( pFile->InThread())
		this->setRestoreType( I3G_RESTORE_MEM);

	Create( Info.m_Count, i3Gfx::Convert( (I3G_PERSIST_USAGE) Info.m_PersistUsage), m_DataType);

	// Index data;
	I3ASSERT( Lock() == true);

	if( m_DataType == I3G_IDX_16BITS)
		unitSz = sizeof( UINT16);
	else
		unitSz = sizeof( UINT32);

	Rc = pFile->Read( GetData(), unitSz * GetCount());
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3IndexArray::OnLoad()", "Could not read index data.");
		return STREAM_ERR;
	}
	Result += Rc;

	Unlock();

	setThreadMode( false);

	return Result;
}

