#if !defined( __I3_GAMENODE_TRAVERSAL_H)
#define __I3_GAMENODE_TRAVERSAL_H

#include "i3GameNode.h"

class I3_EXPORT_FRAMEWORK i3GameNodeTraversal : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GameNodeTraversal, i3ElementBase);
public:
	typedef bool	(* PROC)( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData);

protected:
	void *		m_pUserData = nullptr;
	PROC 		m_pUserProc = nullptr;

	INT32		m_Depth = 0;
	INT32		m_NodeCount = 0;

	i3GameNode *	m_pRoot = nullptr;

public:
	void *			getUserData(void)				{ return m_pUserData; }
	void			setUserData( void * pData)		{ m_pUserData = pData; }

	PROC 			getUserProc(void)				{ return m_pUserProc; }
	void			setUserProc( PROC  pProc)		{ m_pUserProc = pProc; }

	INT32			getDepth(void)					{ return m_Depth; }
	INT32			getNodeCount(void)				{ return m_NodeCount; }
	i3GameNode *	getRoot(void)					{ return m_pRoot; }

	void		Traverse( i3GameNode * pRoot);


protected:
	bool		_Rec_Traverse(i3GameNode * pNode);
};

#endif
