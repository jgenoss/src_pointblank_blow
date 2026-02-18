#include "i3MathType.h"
#include "i3KeyframeTable.h"
#include "i3MathDefine.h"

I3_CLASS_INSTANCE( i3KeyframeTable);

i3KeyframeTable::~i3KeyframeTable(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pTable); 
	I3MEM_SAFE_FREE_POINTER(m_pTime); 
}

void i3KeyframeTable::Create( INT32 count, bool bFixedInterval)
{
	if( m_pTable == nullptr)		m_pTable = (REAL32 *) i3MemAlloc( count * sizeof( REAL32));
	else						m_pTable = (REAL32 *) i3MemReAlloc(m_pTable,  count * sizeof( REAL32));	
	I3ASSERT( m_pTable != nullptr);

	m_Count = count;
	m_fCount = (REAL32) m_Count;
	m_fStep = i3Math::div( 1.0f, m_fCount);

	if( bFixedInterval == false)
	{
		if( m_pTime == nullptr)		m_pTime = (REAL32 *) i3MemAlloc( count * sizeof( REAL32));
		else						m_pTime = (REAL32 *) i3MemReAlloc( m_pTime, count * sizeof(REAL32));
		I3ASSERT( m_pTime != nullptr );
	}
}

UINT32 i3KeyframeTable::_GetTimeIndex( REAL32 t)
{
	register INT32 si, ei, mi;

	si = 0;
	ei = m_Count - 1;

	if( t >= m_pTime[ei] )
	{
		return ei;
	}

	do
	{
		mi = (si + ei) >> 1;

		if((ei - si) <= 1)
		{
			break;
		}
		else if( m_pTime[mi] < t)
		{
			si = mi;
		}
		else if( m_pTime[mi] > t)
		{
			ei = mi;
		}
		else
		{
			si = mi;
			break;
		}
	} while( (ei - si) > 1);

	return si;
}

REAL32 i3KeyframeTable::GetKeyframe( REAL32 t, INTERPOLATION mode)
{
	INT32 idx;
	REAL32 s;

	if( m_pTime != nullptr)
	{
		idx = _GetTimeIndex( t);
		
	}
	else
	{
		idx = getIndex( t);
	}

	switch( mode)
	{
		default :		return m_pTable[idx];

		case LERP :	

			if( idx < (m_Count - 1))
			{
				if( m_pTime != nullptr)
					s = i3Math::div((t - m_pTime[idx]), (m_pTime[idx + 1] - m_pTime[idx]));
				else
					s = i3Math::Mul( (t - i3Math::Mul((REAL32)idx, m_fStep)), m_fCount);

				return m_pTable[idx] + i3Math::Mul((m_pTable[idx + 1] - m_pTable[idx]), s);
			}

			return m_pTable[idx];
	}
}

void i3KeyframeTable::SetKeyframe( REAL32 t, REAL32 val)
{
	INT32 idx;

	idx  = getIndex( t);

	I3ASSERT( idx < m_Count);

	m_pTable[idx] = val;
}

void i3KeyframeTable::SetKeyframeByIndex( INT32 idx, REAL32 val)
{
	m_pTable[idx] = val;
}

void i3KeyframeTable::InsertKeyframe( REAL32 t, REAL32 val)
{
	INT32 idx, MoveSize;

	if( IsFixedInterval())
	{
		idx = (INT32)i3Math::Mul(t, (REAL32)m_Count);
	}
	else
	{
		idx = _GetTimeIndex( t);
	}

	idx++;

	MoveSize = ( m_Count - idx) * sizeof(REAL32);

	// 만약 Count 보다 많다면 개수를 재조정한다.
	Create( m_Count + 1, IsFixedInterval());
	
	if( MoveSize > 0)
	{
		memmove( &(m_pTable[idx+1]), &(m_pTable[idx]), MoveSize);

		if( IsFixedInterval() == false)
			memmove( &(m_pTime[idx+1]), &(m_pTime[idx]), MoveSize);
	}

	m_pTable[idx] = val;

	if( IsFixedInterval() == false)
		m_pTime[idx] = t;
}

void i3KeyframeTable::RemoveKeyframe( INT32 Idx)
{
	INT32 MoveLength;

	if( m_Count == 0)
		return;

	MoveLength = m_Count - Idx - 1;
	if( MoveLength > 0)
	{
		memcpy( &m_pTable[Idx], &m_pTable[Idx+1], MoveLength * sizeof(REAL32));

		if( IsFixedInterval() == false)
			memcpy( &m_pTime[Idx], &m_pTime[Idx+1], MoveLength * sizeof(REAL32));
	}

	m_Count--;
}

void i3KeyframeTable::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);
	i3KeyframeTable * pTable;

	I3ASSERT( i3::same_of<i3KeyframeTable*>(pDest));

	pTable = (i3KeyframeTable *) pDest;

	pTable->m_fCount = m_fCount;
	pTable->m_fStep = m_fStep;
	pTable->m_Count = m_Count;

	if( method == I3_COPY_REF)
	{
		pTable->m_pTable = m_pTable;
		pTable->m_pTime = m_pTime;
	}
	else
	{
		pTable->Create( GetCount(), IsFixedInterval());

		for( INT32 i = 0; i < m_Count; i++)
		{
			pTable->GetTable()[i] = m_pTable[i];

			if( IsFixedInterval() == false)
				pTable->SetTime( i, m_pTime[i]);
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED KEYFRAME_TABLE
	{
		INT32		m_Count = 0;
		INT8		m_bIsTime = 0;
		char		m_Reserved[7] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3KeyframeTable::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::KEYFRAME_TABLE Info;
	
	Info.m_Count = m_Count;
	Info.m_bIsTime = ! IsFixedInterval();

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3KeyframeTable::OnSave()", "Could not write keyframe table info.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Table Data
	#if defined( I3_FIXED_POINT)
		INT32 i;

		for( i = 0; i < m_Count; i++)
		{
			REAL32 val = __PT( m_pTable[i]);

			Rc = pStream->Write( &val, sizeof(REAL32));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3KeyframeTable::OnSave()", "Could not write keyframe data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		if( IsFixedInterval() == false)
		{
			for( i = 0; i < m_Count; i++)
			{
				REAL32 val = __PT( m_pTime[i]);

				Rc = pStream->Write( &val, sizeof(REAL32));
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3KeyframeTable::OnSave()", "Could not write time data.");
					return STREAM_ERR;
				}
				Result += Rc;
			}
		}

	#else
		if( m_Count > 0)
		{
			Rc = pStream->Write( m_pTable, sizeof(REAL32) * m_Count);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3KeyframeTable::OnSave()", "Could not write keyframe data.");
				return STREAM_ERR;
			}
			Result += Rc;

			if( IsFixedInterval() == false)
			{
				Rc = pStream->Write( m_pTime, sizeof(REAL32) * m_Count);
				if( Rc == STREAM_ERR)
				{
					i3Log( "i3KeyframeTable::OnSave()", "Could not write time data.");
					return STREAM_ERR;
				}
				Result += Rc;
			}
		}
	#endif

	return Result;
}

UINT32 i3KeyframeTable::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::KEYFRAME_TABLE Info;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3KeyframeTable::OnLoad()", "Could not read keyframe table info.");
		return STREAM_ERR;
	}
	Result += Rc;

	Create( Info.m_Count, ! Info.m_bIsTime);

	// Table Data
	if( GetCount() > 0)
	{
		Rc = pStream->Read( m_pTable, sizeof(REAL32) * GetCount());
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3KeyframeTable::OnLoad()", "Could not read keyframe data.");
			return STREAM_ERR;
		}
		Result += Rc;

		if( Info.m_bIsTime)
		{
			Rc = pStream->Read( m_pTime, sizeof(REAL32) * GetCount());
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3KeyframeTable::OnLoad()", "Could not read time data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}

		#if defined( I3_FIXED_POINT)
		for( INT32 i = 0; i < m_Count; i++)
		{
			m_pTable[i] = __RT( *((REAL32 *) & m_pTable[i]));
			m_pTime[i] = __RT( *((REAL32 *) & m_pTime[i]));
		}
		#endif
	}

	return Result;
}

bool i3KeyframeTable::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("Count", m_Count);
	pXML->addAttr("FixedInterval", UINT32(!IsFixedInterval()));

	char szTemp[128];
#if defined( I3_FIXED_POINT)
	INT32 i;

	for (i = 0; i < m_Count; i++)
	{
		i3::sprintf(szTemp, "Table%d", i);
		pXML->addAttr(szTemp, __PT(m_pTable[i]));
	}

	if (IsFixedInterval() == false)
	{
		for (i = 0; i < m_Count; i++)
		{
			i3::sprintf(szTemp, "Time%d", i);
			pXML->addAttr(szTemp, __PT(m_pTime[i]));
		}
	}

#else
	if (m_Count > 0)
	{
		for (INT32 i = 0; i < m_Count; i++)
		{
			i3::sprintf(szTemp, "Table%d", i);
			pXML->addAttr(szTemp, m_pTable[i]);
		}

		if (IsFixedInterval() == false)
		{
			for (INT32 i = 0; i < m_Count; i++)
			{
				i3::sprintf(szTemp, "Time%d", i);
				pXML->addAttr(szTemp, m_pTime[i]);
			}
		}
	}
#endif
	return true;
}

bool i3KeyframeTable::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	UINT32 uTemp;
	char szTemp[128];
	pXML->getAttr("Count", &m_Count);
	pXML->getAttr("FixedInterval", &uTemp);

	Create(m_Count, !uTemp);

	if (GetCount() > 0)
	{
		for (INT32 i = 0; i < m_Count; i++)
		{
			i3::sprintf(szTemp, "Table%d", i);
			pXML->getAttr(szTemp, &m_pTable[i]);
		}
		
		if (uTemp)
		{
			for (INT32 i = 0; i < m_Count; i++)
			{
				i3::sprintf(szTemp, "Time%d", i);
				pXML->getAttr(szTemp, &m_pTime[i]);
			}
		}
	}

	return true;
}