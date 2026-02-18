#if !defined( __MEM_INFO_H)
#define __MEM_INFO_H

class MemInfo
{
public:
	char		m_szPath[MAX_PATH];
	INT32		m_Line;
	INT32		m_Count;
	INT32		m_Size;

	UINT32		m_CRC;

	void		CalcCRC(void);

	static		void		DeleteAllInfo(void);
	static		INT32		Find( UINT32 crc);
	static		INT32		GetCount(void);
	static		MemInfo *	GetInfo( INT32 idx);
	static		void		AddInfo( MemInfo * pInfo);

	//sort
	static	void		SortByName();
	static	void		SortByCount();
	static	void		SortBySize();
};

#endif
