#ifndef	__ARTP_COMMON_H__
#define	__ARTP_COMMON_H__

#include <time.h>
#include <windows.h>

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

#define ARCODE_DC	_T("CM001001")
#define ARCODE_CF	_T("CM001002")
#define ARCODE_ACE	_T("CM001003")
#define ARCODE_PT	_T("CM001004")
#define ARCODE_ST	_T("CM001005")
#define ARCODE_EC2	_T("CM001006")
#define ARCODE_LK	_T("CM001007")
#define ARCODE_AT	_T("CM001008")	// kr:Call of Chaos
#define ARCODE_HIS	_T("CM001011")	
#define ARCODE_EC3	_T("CM001012")	
#define ARCODE_MD	_T("CM001013")
#define ARCODE_PB	_T("CM001014")

#include <PshPack1.h>

/*
* packet의 header 역할을 하는 클래스
* packet의 실제 사이즈와, packet version 정보를 가지고 있다.
*/
struct artpInfo
{
	USHORT		m_pktSize			;	// sizeof all packet,
};

struct artpHeader : public artpInfo
{
	enum {
		txt_type = 0,
		bin_type,
	};

	BYTE		m_hdrProtocolType	;	// 1:binary, 0:text
	USHORT		m_hdrProtoSetNo		;
	USHORT		m_hdrPacketIndex	;
	USHORT		m_hdrPacketSubIndex	;
	BYTE		m_hdrIsEncoded		;
	__int64		m_hdrTime			;		

	static bool makeARTPHeader( OUT artpHeader& hdr_, IN USHORT pktIdx_, IN USHORT pktSubIdx_, BYTE isEncoded_ = 0, IN BYTE type_ = 1 )
	{		
		hdr_.m_hdrProtocolType	= type_ ;
		hdr_.m_hdrProtoSetNo	= 0 ; // 0 is default, this var reserved for later(future)
		hdr_.m_hdrPacketIndex	= pktIdx_ ;
		hdr_.m_hdrPacketSubIndex = pktSubIdx_ ;
		hdr_.m_hdrIsEncoded		= isEncoded_ ;
		hdr_.m_hdrTime			= time(0);

		return true;
	}
};

typedef artpHeader ARTP_HEADER;

#include <PopPack.h>

#define DEFAULT_PACKET_SIZE		4096

#endif // __ARTP_COMMON_H__

