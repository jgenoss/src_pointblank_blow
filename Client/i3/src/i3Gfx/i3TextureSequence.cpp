#include "i3GfxType.h"
#include "i3TextureSequence.h"

I3_CLASS_INSTANCE( i3TextureSequence);

i3TextureSequence::~i3TextureSequence(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pTimeTable); 

	I3_SAFE_RELEASE( m_pTexture);
}

void i3TextureSequence::SetUseStaticTime( bool bFlag)
{
	if( bFlag)
	{
		m_Style |= I3G_TSEQ_STYLE_STATICTIME;

		if((m_pTimeTable == nullptr) && (m_Count > 0))
		{
			//m_TimeTableIdx	= i3MemAlloc( sizeof(REAL32) * m_Count);
			m_pTimeTable = (REAL32 *)i3MemAlloc( sizeof(REAL32) * m_Count);
		}
	}
	else
	{
		m_Style &= ~I3G_TSEQ_STYLE_STATICTIME;

		/*
		if( m_pTimeTable != nullptr)
		{
			i3mem::Free( m_TimeTableIdx );
			m_pTimeTable = nullptr;
		}
		*/
		I3MEM_SAFE_FREE_POINTER( m_pTimeTable ); 
	}

}

void i3TextureSequence::SetCount( UINT32 c)
{
	if( m_Count != c)
	{
		if( m_pTimeTable == nullptr)
		{
			//m_TimeTableIdx = i3MemAlloc( sizeof(REAL32) * c);
			//m_pTimeTable = (REAL32 *) i3mem::Lock( m_TimeTableIdx );
			m_pTimeTable = (REAL32 *)i3MemAlloc( sizeof(REAL32) * c);
		}
		else
		{
			//m_TimeTableIdx = i3MemReAlloc( m_TimeTableIdx, sizeof(REAL32) * c);
			//m_pTimeTable = (REAL32 *) i3mem::Lock( m_TimeTableIdx );
			m_pTimeTable = (REAL32 *) i3MemReAlloc( m_pTimeTable, sizeof(REAL32) * c ); 
		}
	}

	m_Count = c;
}

void i3TextureSequence::SetTexture( i3Texture * pTex)
{
	I3_REF_CHANGE( m_pTexture, pTex);

	CalcParams();
}

void i3TextureSequence::CalcParams(void)
{
	if( m_pTexture == nullptr)
		return;

	if((m_pTexture->GetWidth() == 0) || (m_pTexture->GetHeight() == 0))
		return;

	if( m_Interval == 0.0f)
		return;

	if( m_Width == 0)
		return;

	m_Duration = i3Math::Mul( m_Interval, (REAL32)m_Count);

	// 계산을 빠르게 하기 위한 값들을 미리 구해둔다.
	m_RInterval = i3Math::Reciprocal( m_Interval);

	m_LineCount = (INT32) (m_pTexture->GetWidth() / m_Width);
	m_LineTime = i3Math::Mul( (REAL32)m_LineCount, m_Interval);

	if( m_LineTime != 0.0f)
		m_RLineTime = i3Math::Reciprocal( m_LineTime);
	else
		m_RLineTime = 0.0f;

	m_WidthFactor = i3Math::div( m_Width, (REAL32)m_pTexture->GetWidth());
	m_HeightFactor = i3Math::div( m_Height, (REAL32)m_pTexture->GetHeight());
}

void i3TextureSequence::GetUV( REAL32 tm, VEC2D * pUV1, VEC2D * pUV2)
{
	UINT32 x, y;
	UINT32 idx;
	REAL32 u, v;

	if( m_pTexture == nullptr)
		return;

	if( IsRepeat())
		tm = i3Math::fmod( tm, m_Duration);

	if( IsRandomMode())
	{
		idx = i3Math::Rand() % m_Count;
	}
	else
	{
		if( GetUseStaticTime())
		{
			// 고정 간격 시간이기 때문에 계산으로 x, y를 결정한다.
			idx = (UINT32) i3Math::Mul( tm, m_RInterval);

			if( idx >= m_Count)
				idx = m_Count - 1;
		}
		else
		{
			// 임의 시간 간격이기 때문에 Time Table를 참조해 x, y를 결정한다.
			//
			// Time Table은 Sorting되어 있기 때문에 Binary Search 한다.
			INT32 start, end;

			start = 0;
			end = m_Count - 1;

			do
			{
				idx = (start + end) >> 1;
				
				if( m_pTimeTable[idx] > tm)
					end = idx - 1;
				else
					start = idx + 1;
			} while( start < end);
		}
	}

	idx += m_StartIndex;
	
	y = idx / m_LineCount;
	v = i3Math::Mul((REAL32)y, m_HeightFactor);

	x = idx % m_LineCount;
	u = i3Math::Mul((REAL32)x, m_WidthFactor);
	
	i3Vector::Set( pUV1, u, v);
	i3Vector::Set( pUV2, u + m_WidthFactor, v + m_HeightFactor);
}

void i3TextureSequence::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3TextureSequence * pTexSeq = (i3TextureSequence *) pDest;

	I3ASSERT( i3::same_of<i3TextureSequence*>(pDest));

	pTexSeq->SetCount( GetCount());
	pTexSeq->SetUseStaticTime( GetUseStaticTime());
	pTexSeq->SetRepeat( IsRepeat());
	pTexSeq->SetRandomMode( IsRandomMode());
	pTexSeq->SetFixedFrameMode( IsFixedFrameMode());
	pTexSeq->SetFrameSize( GetFrameWidth(), GetFrameHeight());
	pTexSeq->SetStaticInterval( GetStaticInterval());
	pTexSeq->SetDuration( GetDuration());
	pTexSeq->SetStartIndex( GetStartIndex());
	pTexSeq->SetTexture( GetTexture());

	memcpy( pTexSeq->m_pTimeTable, m_pTimeTable, sizeof(REAL32) * m_Count);
}

void i3TextureSequence::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pTexture != nullptr)
	{
		m_pTexture->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct TEXTURE_SEQ_INFO
	{
		INT8	m_ID[4] = { 'T', 'S', 'Q', '1' };
		UINT32	m_Style = 0;
		REAL32	m_Width = 0.0f;
		REAL32	m_Height = 0.0f;
		REAL32	m_Interval = 0.0f;
		UINT32	m_Count = 0;
		UINT32	m_StartIndex = 0;
		OBJREF	m_pTexture = 0;
		INT8	m_Reserved[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TextureSequence::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_SEQ_INFO Info;

	Info.m_Style = m_Style;
	Info.m_Width = m_Width;
	Info.m_Height = m_Height;
	Info.m_Interval = m_Interval;
	Info.m_Count = m_Count;
	Info.m_StartIndex = m_StartIndex;
	Info.m_pTexture = pResFile->GetObjectPersistID( (i3PersistantElement *) m_pTexture);

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureSequence::OnSave()", "Could not write texture sequence info.");
		return STREAM_ERR;
	}
	Result += Rc;
	
	if((m_Count > 0) && (m_pTimeTable != nullptr))
	{
		Rc = pStream->Write( m_pTimeTable, sizeof(REAL32) * m_Count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TextureSequence::OnSave()", "Could not write time table.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TextureSequence::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::TEXTURE_SEQ_INFO Info;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TextureSequence::OnLoad()", "Could not read texture sequence info.");
		return STREAM_ERR;
	}
	Result += Rc;

	m_Style = Info.m_Style;

	SetUseStaticTime( GetUseStaticTime());
	SetCount( Info.m_Count);
	SetStartIndex( Info.m_StartIndex);
	SetFrameSize( Info.m_Width, Info.m_Height);
	SetStaticInterval( Info.m_Interval);

	if( Info.m_pTexture != 0)
	{
		i3Texture * pTex = (i3Texture *) pResFile->FindObjectByID( Info.m_pTexture);

		SetTexture( pTex);
	}

	if( m_Count > 0)
	{
		Rc = pStream->Read( m_pTimeTable, sizeof(REAL32) * m_Count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TextureSequence::OnLoad()", "Could not read time table data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	CalcParams();

	return Result;
}

bool i3TextureSequence::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("Style", UINT32(m_Style));
	pXML->addAttr("Width", m_Width);
	pXML->addAttr("Height", m_Height);
	pXML->addAttr("Interval", m_Interval);
	pXML->addAttr("Count", m_Count);
	pXML->addAttr("StartIndex", m_StartIndex);
	pXML->addAttr("Texture", OBJREF(pFile->GetObjectPersistID((i3PersistantElement *)m_pTexture)));

	if((m_Count > 0) && (m_pTimeTable != nullptr))
	{
		char szTemp[128];
		for (UINT32 i = 0; i < m_Count; i++)
		{
			i3::sprintf(szTemp, "TimeTable%d", i);
			pXML->addAttr(szTemp, m_pTimeTable[i]);
		}
	}
	return true;
}

bool i3TextureSequence::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	UINT32 uTemp;

	pXML->getAttr("Style", &uTemp);
	m_Style = uTemp;

	SetUseStaticTime(GetUseStaticTime());

	pXML->getAttr("Count", &uTemp);
	SetCount(uTemp);

	pXML->getAttr("StartIndex", &uTemp);
	SetStartIndex(uTemp);

	REAL32 fWidth, fHeight;
	pXML->getAttr("Width", &fWidth);
	pXML->getAttr("Height", &fHeight);
	SetFrameSize(fWidth, fHeight);

	REAL32 fInterval;
	pXML->getAttr("Interval", &fInterval);
	SetStaticInterval(fInterval);

	OBJREF objref;
	pXML->getAttr("Texture", &objref);
	if (objref != 0)
	{
		i3Texture * pTex = (i3Texture *)pFile->FindObjectByID(objref);

		SetTexture(pTex);
	}

	if (m_Count > 0)
	{
		char szTemp[128];
		for (UINT32 i = 0; i < m_Count; i++)
		{
			i3::sprintf(szTemp, "TimeTable%d", i);
			pXML->getAttr(szTemp, &m_pTimeTable[i]);
		}
	}

	CalcParams();
	return true;
}