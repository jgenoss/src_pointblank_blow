#if !defined( __WARN_H)
#define __WARN_H

class Warn : public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( Warn);
protected:
	bool		m_bEnable;
	void *		m_pUserData;
	bool		m_bResult;

protected:

	bool		_Rec_CheckTrace( i3Node * pNode, i3StringList * pMsgList);
	void		_CheckTrace( i3Node * pNode, i3StringList * pMsgList, void * pUserData = NULL);

	bool		_Rec_SolveTrace( i3Node * pNode, i3StringList * pMsgList);
	void		_SolveTrace( i3Node * pNode, i3StringList * pMsgList, void * pUserData = NULL);

	virtual bool	_OnCheckNode( i3Node * pNode, i3StringList * pMsgList, void * pUserData = NULL);
	virtual bool	_OnSolveNode( i3Node * pNode, i3StringList * pMsgList, void * pUserData = NULL);
	
public:
	Warn(void);
	virtual ~Warn(void);

	bool		isEnable(void)					{ return m_bEnable; }
	void		setEnable( bool bFlag)			{ m_bEnable = bFlag; }

	virtual const char *	GetTitle(void) = 0;
	virtual bool			IsInterested( char * pszName, char * pszExt) = 0;
	virtual bool			Check( char * pszName, char * pszExt, i3ElementBase * pObj, i3StringList * pMsgList) = 0;
	virtual i3ElementBase *	Solve( i3ElementBase * pObj, i3StringList * pMsgList) = 0;
};

#endif
