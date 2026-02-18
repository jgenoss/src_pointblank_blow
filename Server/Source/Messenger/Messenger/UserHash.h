#ifndef _UserHash_h
#define _UserHash_h

class CUser;

class CUserHash: public i3ElementBase
{
	I3_CLASS_DEFINE(CUserHash);
public:
	CUserHash();
	virtual ~CUserHash();

	BOOL	Create(void);
	void	Insert( CUser* pnode );
	BOOL	Delete( UINT32 ui32userId );
	BOOL	Retrieve( UINT32 ui32userId,CUser** ppnode );

private:
	i3BinList** m_hashTableList;	
};

#endif
