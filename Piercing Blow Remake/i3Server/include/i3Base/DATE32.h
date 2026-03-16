#ifndef __DATE32_H__
#define __DATE32_H__

#include <time.h>
#include <ATLComTime.h>

#define SMALLDATE_STRING_SIZE		20
#define DATE32_YEAR_INIT			2000
#define DATE32_YEAR_MAX				(DATE32_YEAR_INIT + 63)		// 2063
#define DATE32_INIT					((0 << 26) | (1 << 22) | (1 << 17))	// 2000-01-01 00:00:00
#define SUMMER_TIME_FOLLOW_SYSTEM	-1			// A value less than zero to have the C run-time library code compute whether standard time or daylight saving time is in effect.

enum I3DATETYPE
{
	I3DATETYPE_YEAR		= 0xFC000000,
	I3DATETYPE_MONTH	= 0x3C00000,
	I3DATETYPE_DAY		= 0x3E0000,
	I3DATETYPE_HOUR		= 0x1F000,
	I3DATETYPE_MINUTE	= 0xFC0,
	I3DATETYPE_SECOND	= 0x3F,
};

enum I3DATE_ADD_TYPE
{
	I3DATE_ADD_TYPE_SECOND,
	I3DATE_ADD_TYPE_MINUTE,
	I3DATE_ADD_TYPE_HOUR,
	I3DATE_ADD_TYPE_DAY,
	I3DATE_ADD_TYPE_MONTH,
	I3DATE_ADD_TYPE_YEAR,
};

class I3_EXPORT_BASE DATE32
{
	// 32 ��Ʈ�� ����մϴ�.
	// 111111 1111 11111 11111 111111 111111
	// (�⵵) (��)  (��)  (��)   (��)   (��)
	UINT32					m_ui32Date;

public:
	DATE32();
	DATE32(UINT32 ui32Date);
	~DATE32();

	INT32					GetYear()	{ return ((m_ui32Date & I3DATETYPE_YEAR) >> 26) + DATE32_YEAR_INIT; }
	INT32					GetMonth()	{ return (m_ui32Date & I3DATETYPE_MONTH) >> 22; }
	INT32					GetDay()	{ return (m_ui32Date & I3DATETYPE_DAY) >> 17; }
	INT32					GetHour()	{ return (m_ui32Date & I3DATETYPE_HOUR) >> 12; }
	INT32					GetMinute() { return (m_ui32Date & I3DATETYPE_MINUTE) >> 6; }
	INT32					GetSecond() { return (m_ui32Date & I3DATETYPE_SECOND); }
	UINT32					GetDate()	{ return m_ui32Date; }

	inline void				GetSmallDate(char* strDate, INT32 i32Length);
	inline void				GetSmallDate(wchar_t* wstrDate, INT32 i32Length);
	inline UINT32			GetDateTimeBin(INT32 i32DateType = (I3DATETYPE_YEAR | I3DATETYPE_MONTH | I3DATETYPE_DAY | I3DATETYPE_HOUR | I3DATETYPE_MINUTE | I3DATETYPE_SECOND));
	inline UINT32			GetDateTimeYYYYMMDD();
	inline UINT32			GetDateTimeYYMMDDHHMI();
	inline UINT32			GetDateTimeMMDDHHMISS();

	void					Reset() { m_ui32Date = 0; }
	void					CheckDATE32();						   // DATE32 ���� ����ؼ� Ʋ�� ��� �ʱ�ȭ 2017591296 => 2000-01-01 00:00:00 
	void					AddTime(I3DATE_ADD_TYPE eAddType, INT32 i32Time);
	INT32					DiffTime(DATE32 dt32Date);

	inline DATE32&			operator=(const SYSTEMTIME& pTime);
	inline DATE32&			operator=(const tm& pTime);
	inline DATE32&			operator=(const DATE& pTime);
	inline DATE32&			operator=(const UINT32 pTime);
	inline DATE32&			operator=(const DATE32& pTime);

	friend inline BOOL		operator==(const DATE32& dt32Date1, const DATE32& dt32Date2)
	{
		return (dt32Date1.m_ui32Date == dt32Date2.m_ui32Date);
	}
	friend inline BOOL		operator!=(const DATE32& dt32Date1, const DATE32& dt32Date2)
	{
		return (dt32Date1.m_ui32Date != dt32Date2.m_ui32Date);
	}
	friend inline BOOL		operator<(const DATE32& dt32Date1, const DATE32& dt32Date2)
	{
		return (dt32Date1.m_ui32Date < dt32Date2.m_ui32Date);
	}
	friend inline BOOL		operator<=(const DATE32& dt32Date1, const DATE32& dt32Date2)
	{
		return (dt32Date1.m_ui32Date <= dt32Date2.m_ui32Date);
	}
	friend inline BOOL		operator>(const DATE32& dt32Date1, const DATE32& dt32Date2)
	{
		return (dt32Date1.m_ui32Date > dt32Date2.m_ui32Date);
	}
	friend inline BOOL		operator>=(const DATE32& dt32Date1, const DATE32& dt32Date2)
	{
		return (dt32Date1.m_ui32Date >= dt32Date2.m_ui32Date);
	}
};

#endif