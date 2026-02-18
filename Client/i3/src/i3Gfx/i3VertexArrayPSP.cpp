#include "i3GfxType.h"
#include "../i3Math/i3Color.h"

#ifdef I3G_PSP
#include "i3GfxUtilPSP.h"
#include "i3VertexArrayPSP.h"
#include "i3VertexArrayPSP.h"
#include "i3VRAMManager.h"

//#if !defined( I3_TOOL)
//I3_CONCRETE_CLASS_INSTANCE( i3VertexArrayPSP, i3VertexArray );
//#else
//I3_CLASS_INSTANCE( i3VertexArrayPSP, i3VertexArray );
//#endif
I3_CLASS_INSTANCE( i3VertexArrayPSP);


i3VertexArrayPSP::i3VertexArrayPSP(void)
{
}

i3VertexArrayPSP::~i3VertexArrayPSP(void)
{
	if( m_pBuffer != nullptr)
	{
		i3VRAMManager::Free( m_BufferIdx ); 
		//i3mem::Free( m_BufferIdx );
		m_pBuffer = nullptr;
	}
}

UINT32 i3VertexArrayPSP::CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count)
{
	I3_PERSIST_VERTEX_OFFSET Info;
	INT32 i;

	Info.m_Format = * pFormat;

	CalcPersistantBufferSizePSP( &Info, 1);

	m_Stride = Info.m_Stride;
	m_OffsetPos = Info.m_OffsetPos;
	m_OffsetNormal = Info.m_OffsetNormal;
	m_OffsetColor = Info.m_OffsetColor;
	m_OffsetWeight = Info.m_OffsetWeight;

	for( i = 0; i < pFormat->GetTextureCoordCount(); i++)
	{
		m_OffsetTexCoord[i] = Info.m_OffsetTexCoord[i];
	}

	m_OffsetBlendIndex = Info.m_OffsetBlendIndex;

	return m_Stride * Count;
}


bool i3VertexArrayPSP::Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage)
{
	UINT32 BuffSz;

#if 0
	// Weight 값을 확인하기 위해 잠시 추가했던 Test용 Code.
	pFormat->SetHasColor(true);
#endif

	m_Format = * pFormat;
	m_PersistFormat = * pFormat;
    m_Count = count;
	m_Usage = usage;
 
	// PSP의 경우 통과모드(IsTransformed)를 사용할경우, 드로잉,텍셀좌표계의 형태가 정해져 있으므로 이를 변경합니다.
	if( m_Format.IsTransformed() && m_Format.GetTextureCoordCount()>=1)
	{		
		m_Format.SetTextureCoordType(I3VF_TYPE_FIXED16_0);
		m_Format.SetPositionType(I3VF_TYPE_FIXED16_0);
	}

	BuffSz = CalcBufferSize( &m_Format, count);
	m_VertexFormatCode = i3PSPUT_GetNativeVertexFormat( &m_Format);
	
#if 0
	m_BufferIdx = i3MemAlloc( BuffSz);
	m_pBuffer	= (char *) i3mem::Lock( m_BufferIdx ); 
#else
	m_BufferIdx = i3VRAMManager::Alloc( BuffSz , 64);
	m_pBuffer	= (char *) i3VRAMManager::Lock( m_BufferIdx ); 
#endif

	I3ASSERT( m_pBuffer != nullptr);

	return true;
};


bool i3VertexArrayPSP::Lock( I3G_LOCKMETHOD lock, UINT32 StartIdx, UINT32 EndIdx)
{
	if( m_bLocked)
		return false;

	I3ASSERT( m_pBuffer);

	m_LockStartIdx = 0;		// Bug 아닙니다. i3VertexArray에서 m_pLockedBuf를 사용하는 방식에 따른 조치입니다.

	m_pLockedBuf = (INT8 *) m_pBuffer;

	m_bLocked = true;

	return true;
}

void i3VertexArrayPSP::Unlock(void)
{
	if( ! IsLocked())
	{
		return;
	}

	I3ASSERT( m_pBuffer);
	I3ASSERT( m_pLockedBuf);

	m_bLocked = false;
}

/*
static char s_szTypeName[][32] =
{
	"NA",
	"R32",
	"F32_16",
	"F16_8",
	"F16_12",
	"F16_15",
	"F8_4",
	"F8_7",
	"C8888",
	"C1555",
	"C565",
	"C4444",
	"F16",
	"F8",
	"UF16_15",
	"UF8_7",
};

void DumpTypes( i3VertexFormat * pFmt)
{
	char conv[128], temp[64];

	conv[0] = 0;

	if( pFmt->GetHasPosition())
	{
		sprintf( temp, "POS:%s", s_szTypeName[ pFmt->GetPositionType()]);
		strncat( conv, temp, sizeof(conv)-1);
	}

	if( pFmt->GetHasNormal())
	{
		sprintf( temp, "NML:%s", s_szTypeName[ pFmt->GetNormalType()]);
		strncat( conv, temp, sizeof(conv)-1);
	}

	if( pFmt->GetHasColor())
	{
		sprintf( temp, "COL:%s", s_szTypeName[ pFmt->GetColorType()]);
		strncat( conv, temp, sizeof(conv)-1);
	}

	if( pFmt->GetTextureCoordSetCount() > 0)
	{
		sprintf( temp, "TMX:%s", s_szTypeName[ pFmt->GetTextureCoordType()]);
		strncat( conv, temp, sizeof(conv)-1);
	}

	I3TRACE( "%s\n", conv);
}
*/

void i3VertexArrayPSP::MakeRuntimeFormat( I3_PERSIST_VERTEX_OFFSET * pInfo)
{
	if( pInfo->m_Format.GetPlatform() == I3VF_PLATFORM_PSP)
	{
		// Persist Data에 이미 올바른 PSP용 Vertex Array Data로 만들어져
		// 있기 때문에, Persist Data를 복사한다.
		UINT32 sz;

		sz = CalcBufferSize( &m_Format, GetCount());

		memcpy( m_pBuffer, m_pPersistData, sz);
	}
	else
	{
		i3VertexArray::MakeRuntimeFormat( pInfo);
	}

	// Persist Data를 제거한다.
	//I3MEM_SAFE_FREE( m_pPersistData);
	if(m_pPersistData != nullptr)
	{
		i3mem::Free( m_PersistDataIdx ); 
		m_pPersistData = nullptr; 		
	}
}

#endif
