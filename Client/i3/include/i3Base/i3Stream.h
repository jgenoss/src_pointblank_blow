// i3Stream.h
#ifndef __I3STREAM_H
#define __I3STREAM_H

#include "i3ElementBase.h"
#include "itl/range/detail/enable_if_iterator.h"
#include "itl/range/value_type.h"
#include "itl/range/generic_push_back.h"

#define STREAM_ERR				0xFFFFFFFF

#define STREAM_BEGIN			1
#define STREAM_CURRENT			2
#define STREAM_END				3

#define STREAM_READ			0x00000001
#define STREAM_WRITE		0x00000002
#define STREAM_SHAREREAD	0x00000004
#define STREAM_SHAREWRITE	0x00000008
#define	STREAM_ASYNC		0x00000010

class i3PackFile;

class I3_EXPORT_BASE i3Stream : public i3ElementBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3Stream, i3ElementBase);

public:

	enum STREAMCODE {
		STREAM_NOERROR,									// No Error
		STREAM_NOTINITIALIZED,							// Not initialized
		STREAM_NOTENOUGHSPACE,							// Not enough space
		STREAM_IOERROR,									// Some I/O error
		STREAM_TIMEOUT,									// Timeout
		STREAM_EOS,										// End Of Stream
		STREAM_ACCESSVIOLATION							// Access Violation
	};

protected:
	UINT32 m_Size = 0;
	UINT32 m_RecordSize = 0;
	STREAMCODE m_LastCode = STREAM_NOTINITIALIZED;
	INT32 m_Access = 0;

public:
	UINT32			GetSize(void) const					{ return m_Size; } 
	STREAMCODE		GetLastError(void) const			{ return m_LastCode; }
	INT32			GetAccessMode(void) const			{ return m_Access; }


	void	BeginRecord(void)							{ m_RecordSize = 0; }
	UINT32	GetRecordSize(void) const					{ return m_RecordSize; }

	virtual bool  Close(void) = 0;
	virtual bool  IsOpened() const = 0;

	virtual UINT32 Read( void *Buf, INT32 Length) = 0;
	virtual UINT32 CopyTo( i3Stream *Stream, UINT32 Length);
	virtual UINT32 Write( const void *Buf, INT32 Length) = 0;
	virtual UINT32 CopyFrom( i3Stream *Stream, UINT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType = STREAM_BEGIN) = 0;
	virtual UINT32 GetPosition(void) const = 0;

	// ReadLine/WriteLine의 가상키워드를 제거...
	// 개인적 판단으로 볼때, 현재 구현의 ReadLine의 리턴값은 부정확한 계산때문에 쓰면 안된다... 
	// 하지만, 이상태로 리턴값이 쓰인 곳이 많기 때문에 함수 수정은 하지 않고 일단 방치...
	// 2014.08.04.수빈
	UINT32 ReadLine( char * pStr, INT32 BufLength = -1 );
	UINT32 ReadLineW( WCHAR16 * pStr, INT32 BufLength = -1 );
	

	// container로 받을수 있는 버전.. 리턴값은 글자수같은게 아니고..그냥 읽은 바이트수 혹은 STREAM_ERR이다..
	template<class Seq> typename i3::disable_if_iterator<Seq,
	UINT32>::type ReadLine(Seq& out);
		
	// WriteLine의 구현은 비일관성 문제가 있다.... ('\n'이나 '\r\n' 고려가 안되있고, line이 아닌데도 이 함수를 
	//												써서 구현된 코드가 엔진과 게임소스에 있다.)
	// 2014.08.04.수빈
	UINT32 WriteLine( const char * pStr);
	UINT32 WriteLineW( const WCHAR16 * pStr);

	virtual UINT32 Printf( char *szBuf, const char *szFmt, ... );
};


namespace detail_i3Stream
{
	template<std::size_t>
	struct has_bom_by_typesize;

	template<>
	struct has_bom_by_typesize<1>
	{
		template<class Ch> __forceinline static bool call(Ch v) { return false; }
	};
	template<>
	struct has_bom_by_typesize<2>
	{
		template<class Ch> __forceinline static bool call(Ch v) 
		{ 
			return v == 0xfeff || v == 0xfffe; 
		}
	};
}


template<class Seq> typename i3::disable_if_iterator<Seq,
UINT32>::type i3Stream::ReadLine(Seq& out)
{
	out.clear();
	UINT32 rc;
	typedef typename i3::range_value<Seq>::type	Ch;
	Ch val;
	
	UINT32 oldPos = this->GetPosition();

	for ( ; ; )
	{
		rc = this->Read( &val, sizeof( Ch ));
		if( rc == STREAM_ERR)
		{
			switch( GetLastError())
			{
			case STREAM_EOS :	break;
			default:			return STREAM_ERR;	
			}
			break;
		}
			
		if ( detail_i3Stream::has_bom_by_typesize<sizeof(Ch)>::call( val ) )
			continue;

		if( val == Ch('\r') )
			continue;
		if( val == Ch('\n') )
			break;
		
		i3::generic_push_back(out, val);

	}
	
	return this->GetPosition() - oldPos;
	
}




#endif
