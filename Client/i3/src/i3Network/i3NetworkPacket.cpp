#include "i3NetworkDef.h"
#include "i3NetworkPacket.h"

I3_CLASS_INSTANCE(i3NetworkPacket);

i3NetworkPacket::i3NetworkPacket()	
{
	Clear(); 
	return; 
}

i3NetworkPacket::i3NetworkPacket( PROTOCOL ProtocolID )
{
	Clear(); 
	SetProtocol( ProtocolID ); 	
	return;
}

i3NetworkPacket::~i3NetworkPacket()
{
	return; 
}

void i3NetworkPacket::ReadEmptyData(INT32 size)
{
	if( 0 == size )return;
	if( m_pReadPosition + size > m_pEndOfDataField )return;
	m_pReadPosition	+= size;

	return; 
}

void i3NetworkPacket::ReadData( void* buffer, INT32 size)
{
	if( 0 == size )
	{
		//I3TRACE("[i3NetworkPacket::ReadData] Size Zero\n");
		return;
	}

	if( m_pReadPosition + size > m_pEndOfDataField )
	{
		I3TRACE("[i3NetworkPacket::ReadData] Error\n");
		return;
	}

	i3mem::Copy( buffer, m_pReadPosition, size );
	m_pReadPosition	+= size;

	return; 
}

void i3NetworkPacket::WriteData( const void* buffer, UINT16 size)
{
	if( 0 == size )	return;

	if( m_pWritePosition + size > m_pEndOfDataField )
	{
		I3TRACE("[i3NetworkPacket::WriteData] Write Failed \n"); 
		return;
	}

	i3mem::Copy( m_pWritePosition, buffer, size );
	m_pWritePosition	+= size;
	m_Size				+= size;

	*m_PacketHeader.pDataSize = (UINT16) m_Size; 

	return; 
}  

void i3NetworkPacket::CopyToBuffer(const char * buffer, INT32 Size, INT32 StartPos)
{
	if(Size > PACKETBUFFERSIZE )return; 

	Clear();	
	i3mem::Copy( m_pPacketBuffer, &buffer[StartPos], Size); 
	m_Size = Size;

	return; 
}

#ifdef __COMPRESS_RLE__
//특수 문자 설정
// 0x1F
#define SPECIAL_CHARACTER	0x1F
#define	MAX_RLE_SIZE		200
bool i3NetworkPacket::CompressRLE(void)
{
	UINT16 iPacketSize = *m_PacketHeader.pDataSize; 
	char * pDataPointer = (char *)&m_pPacketBuffer[PACKETHEADERSIZE]; 

	char	bchar = pDataPointer[0];
	UINT8	iEqualCount		= 0; 
	UINT16	iNewPacketSize	= 0;

	char	pNewData[PACKETDATASIZE * 2];

	//시작 캐릭이 특수 문자이다. 
	if( pDataPointer[0] == SPECIAL_CHARACTER )
	{
		iEqualCount++; 
	}

	for(UINT16 i = 1; i < iPacketSize; i++ )
	{
		//1. 동일한 문자이다. 
		if(bchar ==	pDataPointer[i])
		{
			iEqualCount++; 

			if(iEqualCount > MAX_RLE_SIZE)
			{
				//카운트의 최대값을 넘었다. 
				//SET DATA
				pNewData[iNewPacketSize + 0] = SPECIAL_CHARACTER; 
				pNewData[iNewPacketSize + 1] = bchar; 
				pNewData[iNewPacketSize + 2] = iEqualCount ; 
				iNewPacketSize  = iNewPacketSize + 3;   

				//RESET
				bchar		= pDataPointer[i]; 
				iEqualCount = 0; 
			}

			continue; 
		}

		//2. 이전까지 동일한 문자를 받다가 끝났을때 
		if( iEqualCount > 0 && bchar != pDataPointer[i] )
		{
			//SET DATA
			pNewData[iNewPacketSize + 0] = SPECIAL_CHARACTER; 
			pNewData[iNewPacketSize + 1] = bchar; 
			pNewData[iNewPacketSize + 2] = iEqualCount ; 
			iNewPacketSize  = iNewPacketSize + 3;   

			//RESET
			bchar		= pDataPointer[i]; 
			iEqualCount = 0; 
			continue; 
		}

		//3. 특수 문자일때 
		if( pDataPointer[i] == SPECIAL_CHARACTER )
		{
			iEqualCount++; 
			bchar = pDataPointer[i]; 
			continue; 
		}

		//압축할 수 없는 데이터
		bchar = pDataPointer[i]; 
		pNewData[iNewPacketSize] = pDataPointer[i];
		iNewPacketSize++;
	}

	//완료까지 중복된 문자이면 
	if( iEqualCount > 0 )
	{
		//SET DATA
		pNewData[iNewPacketSize + 0] = SPECIAL_CHARACTER; 
		pNewData[iNewPacketSize + 1] = bchar; 
		pNewData[iNewPacketSize + 2] = iEqualCount ; 
		iNewPacketSize  = iNewPacketSize + 3;   
	}

	//압축된 데이터가 기존 데이터 보다 크다면 변경할 필요없다. 
	if( iNewPacketSize >= iPacketSize)		return false;

	//새로운 패킷에 넣기
	i3mem::Copy( &m_pPacketBuffer[PACKETHEADERSIZE], pNewData, iNewPacketSize); 
	* m_PacketHeader.pDataSize = iNewPacketSize; 

	return true;
}

bool i3NetworkPacket::DecompressRLE(void)
{
	char	pNewData[PACKETDATASIZE];
	UINT16	iNewPacketSize	= 0;

	UINT16 iPacketSize = *m_PacketHeader.pDataSize; 
	char * pDataPointer = (char *)&m_pPacketBuffer[PACKETHEADERSIZE]; 

	iNewPacketSize = 0; 

	UINT8 iCount; 
	char cChara;
	for(UINT16 i = 0; i < iPacketSize; i++ )
	{
		if( pDataPointer[i] == SPECIAL_CHARACTER )
		{
			i3mem::Copy(&cChara, &pDataPointer[i+1], sizeof(char)); 
			i3mem::Copy(&iCount, &pDataPointer[i+2], sizeof(UINT8)); 
			i3mem::Fill(&pNewData[iNewPacketSize], cChara, iCount); 

			i = i + 2; 
			iNewPacketSize = iNewPacketSize + iCount;

			continue; 
		}

		pNewData[iNewPacketSize] = pDataPointer[i];
		iNewPacketSize++;
	}

	//새로운 패킷에 넣기
	i3mem::Copy( &m_pPacketBuffer[PACKETHEADERSIZE], pNewData, iNewPacketSize); 
	* m_PacketHeader.pDataSize = iNewPacketSize; 

	return true;
}

bool i3NetworkPacket::Compress(UINT8 iType)
{
	if (*m_PacketHeader.pCompressType != COMPRESS_TYPE_NONE)return false;

	bool iRv = true;

	switch(iType)
	{
	case COMPRESS_TYPE_RLE	: iRv = CompressRLE(); break; 
	case COMPRESS_TYPE_NONE	: 
	default					: 
		break; 
	}

	if(iRv == trueTRUE)*m_PacketHeader.pCompressType = iType;

	return iRv; 
}

bool i3NetworkPacket::Decompress(void)
{
	if (*m_PacketHeader.pCompressType == COMPRESS_TYPE_NONE)return false;

	UINT8	iType = *m_PacketHeader.pCompressType;
	bool	iRv = true

	switch(iType)
	{
	case COMPRESS_TYPE_RLE	: iRv = DecompressRLE(); break; 
	case COMPRESS_TYPE_NONE	: 
	default					: 
		break; 
	}
	return true;
}
#endif

bool i3NetworkPacket::Encript(UINT32 c)
{
#ifdef __COMPRESS_RLE__
	if (*m_PacketHeader.pEncriptType == 1)return false;
#endif

	UINT16 Size = *m_PacketHeader.pDataSize; 
	BitRotateEncript( (UINT8*)&m_pPacketBuffer[PACKETENCRIPTSIZE], (Size + PACKETENCRIPTSIZE), c);
	*m_PacketHeader.pDataSize = Size | 0x8000 ; 

#ifdef __COMPRESS_RLE__
	*m_PacketHeader.pEncriptType = 1; 
#endif
	return true;
}

bool i3NetworkPacket::Decript(UINT32 c)
{
#ifdef __COMPRESS_RLE__
	if (*m_PacketHeader.pEncriptType == 0)return false;
#endif

	UINT16 Size = *m_PacketHeader.pDataSize & 0x7FFF; 
	BitRotateDecript( (UINT8*)&m_pPacketBuffer[PACKETENCRIPTSIZE], (Size + PACKETENCRIPTSIZE), c);
#ifdef __COMPRESS_RLE__
	*m_PacketHeader.pEncriptType = 0; 
#endif
	return true;
}

void i3NetworkPacket::Clear(void)
{
	i3mem::FillZero( m_pPacketBuffer, PACKETHEADERSIZE );
#ifdef __COMPRESS_RLE__
	m_PacketHeader.pCompressType= ( UINT8 * )&m_pPacketBuffer[0];	//  pCompressType	size = 1
	m_PacketHeader.pEncriptType	= ( UINT8 * )&m_pPacketBuffer[1];	//  pEncriptType	size = 1
	m_PacketHeader.pDataSize	= ( UINT16* )&m_pPacketBuffer[2];	//  packetSize		size = 2
	m_PacketHeader.pProtocolID	= ( UINT16* )&m_pPacketBuffer[4];	//  protocolID		size = 2
#else
	m_PacketHeader.pDataSize	= ( UINT16* )&m_pPacketBuffer[0];	//  packetSize		size = 2
	m_PacketHeader.pProtocolID	= ( UINT16* )&m_pPacketBuffer[2];	//  protocolID		size = 2
#endif
	
	m_pDataField				= &m_pPacketBuffer[PACKETHEADERSIZE];
	m_pReadPosition				= m_pWritePosition = m_pDataField;
	m_pEndOfDataField			= &m_pPacketBuffer[PACKETBUFFERSIZE];
	m_Size						= 0;
	return; 
}
