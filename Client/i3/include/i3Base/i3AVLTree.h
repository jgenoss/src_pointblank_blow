#if !defined( __I3_AVL_TREE_H)
#define __I3_AVL_TREE_H

#include "i3ElementBase.h"
#include "i3Debug.h"

template <class T>
class i3AVLNode 
{
public:
	i3AVLNode *		m_pParent;
	i3AVLNode *		m_pLeft;
	i3AVLNode *		m_pRight;

	INT32			m_Depth;
	INT32			m_Balance;

	T				m_Data;

	void			reset(void)
	{
		m_pParent = nullptr;
		m_pLeft = nullptr;
		m_pRight = nullptr;
		m_Balance = 0;
		m_Depth = 1;
	}

	bool			isLeaf(void)
	{
		return (m_pLeft == nullptr) && (m_pRight == nullptr);
	}
};

template <class T>
class i3AVLTree
{
public:
	typedef		bool	(*TRAVERSAL)( i3AVLNode<T> * pNode, INT32 level, void * pUserData);
	typedef		void	(*DUMPPROC)( const T * pData);

	struct FIND_LINEAR_INFO
	{
		T *				m_pData;
		i3AVLNode<T> *	m_pFindNode;
	} ;

protected:
	i3AVLNode<T> *	m_pRoot = nullptr;
	TRAVERSAL		m_pTraversalProc = nullptr;
	void *			m_pUserData = nullptr;

#if defined( I3_DEBUG)
	DUMPPROC		m_pDumpProc = nullptr;
	bool			m_bDump = false;
#endif

protected:
	INT32		Insert( i3AVLNode<T> * pNode, const T * pData, i3AVLNode<T> ** ppAddNode);

	INT32		calcDepth( i3AVLNode<T> * pNode);
	INT32		getLeftDepth( i3AVLNode<T> * pNode);
	INT32		getRightDepth( i3AVLNode<T> * pNode);

	i3AVLNode<T> *	AddLeft( i3AVLNode<T> * pNode, const T * pData);
	i3AVLNode<T> *	AddRight( i3AVLNode<T> * pNode, const T * pData);

	void		SetLeft( i3AVLNode<T> * pNode, i3AVLNode<T> * pChild);
	void		SetRight( i3AVLNode<T> * pNode, i3AVLNode<T> * pChild);

	void		RotateLeft( i3AVLNode<T> * pNode);
	void		RotateRight( i3AVLNode<T> * pNode);

	void		UpdateBalance( i3AVLNode<T> * pNode);

	i3AVLNode<T> *	GetSmallestNode( i3AVLNode<T> * pNode);
	i3AVLNode<T> *	GetLargestNode( i3AVLNode<T> * pNode);
	void		UnlinkFromParent( i3AVLNode<T> * pNode);

	void				Retrace( i3AVLNode<T> * pNode);

	bool		_Rec_Traversal( i3AVLNode<T> * pNode, INT32 level);

#if defined( I3_DEBUG)
	static		bool	_DumpProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData);
	static		bool	_CheckOrderProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData);
#endif

	static		bool	_FindLinearProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData);
	static		bool	_CountProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData);
	static		bool	_GetAllProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData);

public:
	i3AVLTree(void) {}
	virtual ~i3AVLTree(void) {}

	i3AVLNode<T> *		Add( const T & pData);
	void				Remove( const T & data);
	void				Delete( i3AVLNode<T> * pNode);

	i3AVLNode<T> *		LookUp( const T & data);
	virtual T *			Find( const T & data);
	virtual T *			FindLinear( const T & data);
	INT32				GetCount(void);
	void				GetAll( i3::vector<T*>& List);

	void				Traversal( TRAVERSAL pProc, void * pUserData);

#if defined( I3_DEBUG)
	void				Dump( const char * pszStr = nullptr);
	void				DumpNode( i3AVLNode<T> * pNode, TRAVERSAL pDumpProc);	

	void				SetDumpProc( DUMPPROC pProc)		{ m_pDumpProc = pProc; }
	void				SetDumpState( bool bState)			{ m_bDump = bState; }

	INT32				CheckBalanceFactor( i3AVLNode<T> * pNode);
	void				CheckOrder(void);
	void				CheckLeftValue( i3AVLNode<T> * pNode, T * pData);
	void				CheckRightValue( i3AVLNode<T> * pNode, T * pData);
	void				CheckValue(void);
	void				CheckIntegrity(void);
#endif
};

#include "i3AVLTree.hpp"

#endif