
//#define		_AVL_TRACE

#if defined( _AVL_TRACE)
#define		_AVLT( a)			I3TRACE( a)
#else
#define		_AVLT( a)			__noop
#endif

#if defined( I3_DEBUG)
#define		CALL_DUMP( pNode)		if( m_bDump) { I3TRACE( "[%p] : ", (pNode)); if( m_pDumpProc != NULL) m_pDumpProc( &(pNode)->m_Data); I3TRACE( "\n"); }
#define		AVL_DUMP( msg)			if( m_bDump) { I3TRACE( msg); }
#else
#define		CALL_DUMP( pNode)		__noop
#define		AVL_DUMP( msg)			__noop
#endif

template <class T>
i3AVLTree<T>::i3AVLTree(void)
{
	m_pRoot = NULL;

	m_pTraversalProc = NULL;
	m_pUserData = NULL;

#if defined( I3_DEBUG)
	m_pDumpProc = NULL;
	m_bDump = false;
#endif
}

template <class T>
i3AVLTree<T>::~i3AVLTree(void)
{
}

template <class T>
INT32		i3AVLTree<T>::calcDepth( i3AVLNode<T> * pNode)
{
	if( pNode == NULL)
		return 0;

	return MAX( calcDepth( pNode->m_pLeft), calcDepth( pNode->m_pRight)) + 1;
}

template<class T>
INT32	i3AVLTree<T>::getLeftDepth( i3AVLNode<T> * pNode)
{
	if( pNode->m_pLeft != NULL)
		return pNode->m_pLeft->m_Depth;
	
	return 0;
}

template<class T>
INT32	i3AVLTree<T>::getRightDepth( i3AVLNode<T> * pNode)
{
	if( pNode->m_pRight != NULL)
		return pNode->m_pRight->m_Depth;

	return 0;
}

template <class T>
void	i3AVLTree<T>::SetLeft( i3AVLNode<T> * pNode, i3AVLNode<T> * pChild)
{
	I3ASSERT( pNode != pChild);
	pNode->m_pLeft = pChild;

	INT32 rDepth = getRightDepth( pNode);
	INT32 lDepth = 0;

	if( pChild != NULL)
	{
		pChild->m_pParent = pNode;
		lDepth = pChild->m_Depth;
	}

	pNode->m_Depth = max( rDepth, lDepth) + 1;
	pNode->m_Balance = rDepth - lDepth;
}

template <class T>
void	i3AVLTree<T>::SetRight( i3AVLNode<T> * pNode, i3AVLNode<T> * pChild)
{
	I3ASSERT( pNode != pChild);
	pNode->m_pRight = pChild;

	INT32 lDepth = getLeftDepth( pNode);
	INT32 rDepth = 0;

	if( pChild != NULL)
	{
		pChild->m_pParent = pNode;
		rDepth = pChild->m_Depth;
	}

	pNode->m_Depth = max( rDepth, lDepth) + 1;
	pNode->m_Balance = rDepth - lDepth;
}

template <class T>
i3AVLNode<T> *	i3AVLTree<T>::AddLeft( i3AVLNode<T> * pNode, const T * pData)
{
	I3ASSERT( pNode->m_pLeft == NULL);

	_AVLT( "AddLeft\n");

	i3AVLNode<T> * pNew = new i3AVLNode<T>;

	pNew->reset();
	pNew->m_Data = *pData;

	SetLeft( pNode, pNew);

	return pNew;
}

template <class T>
i3AVLNode<T> *	i3AVLTree<T>::AddRight( i3AVLNode<T> * pNode, const T * pData)
{
	I3ASSERT( pNode->m_pRight == NULL);

	_AVLT( "AddRight\n");

	i3AVLNode<T> * pNew = new i3AVLNode<T>;

	pNew->reset();
	pNew->m_Data = *pData;

	SetRight( pNode, pNew);

	return pNew;
}

template <class T>
void	i3AVLTree<T>::UpdateBalance( i3AVLNode<T> * pNode)
{
	INT32 lDepth =  getLeftDepth( pNode);
	INT32 rDepth =  getRightDepth( pNode);

	pNode->m_Depth = max( lDepth, rDepth) + 1;
	pNode->m_Balance = rDepth - lDepth;
}

template <class T>
void i3AVLTree<T>::RotateLeft( i3AVLNode<T> * pNode)
{
	I3ASSERT( pNode->m_pRight != NULL);

	_AVLT( "RotateLeft\n");

	i3AVLNode<T> * pNewRoot = pNode->m_pRight;
	i3AVLNode<T> * pParent = pNode->m_pParent;

	SetRight( pNode, pNewRoot->m_pLeft);
	SetLeft( pNewRoot, pNode);

	if( pParent != NULL)
	{
		if( pParent->m_pLeft == pNode)
			SetLeft( pParent, pNewRoot);
		else
			SetRight( pParent, pNewRoot);
	}
	else
	{
		m_pRoot = pNewRoot;
		pNewRoot->m_pParent = NULL;
	}
}

template <class T>
void i3AVLTree<T>::RotateRight( i3AVLNode<T> * pNode)
{
	I3ASSERT( pNode->m_pLeft != NULL);

	_AVLT( "RotateRight\n");

	i3AVLNode<T> * pNewRoot = pNode->m_pLeft;
	i3AVLNode<T> * pParent = pNode->m_pParent;

	SetLeft( pNode, pNewRoot->m_pRight);
	SetRight( pNewRoot, pNode);

	if( pParent != NULL)
	{
		if( pParent->m_pLeft == pNode)
			SetLeft( pParent, pNewRoot);
		else
			SetRight( pParent, pNewRoot);
	}
	else
	{
		m_pRoot = pNewRoot;
		pNewRoot->m_pParent = NULL;
	}
}


template <class T>
INT32		i3AVLTree<T>::Insert( i3AVLNode<T> * pNode, const T * pData, i3AVLNode<T> ** ppAddNode)
{
	i3AVLNode<T> * pChild;

	CALL_DUMP( pNode);

	if( pNode->m_Data == *pData)
	{
		// 동일한 값이다. 추가할 필요없음.
		*ppAddNode = NULL;

		AVL_DUMP( "Skip to add\n");
	}
	else if( pNode->m_Data > *pData)			// Left
	{
		if( pNode->m_pLeft == NULL)
		{
			AVL_DUMP( "Add to Left\n");
			*ppAddNode = AddLeft( pNode, pData);					// 여기에 추가.
		}
		else
		{
			AVL_DUMP( "Pass to Left\n");
			Insert( pNode->m_pLeft, pData, ppAddNode);	// Recursive add

			UpdateBalance( pNode);
		}

		if( pNode->m_Balance <= -2)
		{
			pChild = pNode->m_pLeft;

			if( pChild->m_Balance < 0)
			{
				// Left-Left case
				AVL_DUMP( "Left-Left rotation\n");
				RotateRight( pNode);
			}
			else
			{
				// Left-Right case
				AVL_DUMP( "Left-Right rotation\n");
				RotateLeft( pChild);
				RotateRight( pNode);
			}
		}
	}
	else										// Right
	{
		if( pNode->m_pRight == NULL)
		{
			AVL_DUMP( "Add to Right\n");
			*ppAddNode = AddRight( pNode, pData);					// 여기에 추가.
		}
		else
		{
			AVL_DUMP( "Pass to Right\n");
			Insert( pNode->m_pRight, pData, ppAddNode);		// Recursive add

			UpdateBalance( pNode);
		}

		if( pNode->m_Balance >= 2)
		{
			pChild = pNode->m_pRight;

			if( pChild->m_Balance > 0)
			{
				// Right-Right case
				AVL_DUMP( "Right-Right rotation\n");
				RotateLeft( pNode);
			}
			else
			{
				// Right-Left case
				AVL_DUMP( "Right-Left rotation\n");
				RotateRight( pChild);
				RotateLeft( pNode);
			}
		}
	}

	return pNode->m_Depth;
}

template <class T>
i3AVLNode<T> *		i3AVLTree<T>::Add( const T & data)
{
	i3AVLNode<T> * pNewNode;

	if( m_pRoot == NULL)
	{
		pNewNode = m_pRoot = new i3AVLNode<T>;

		m_pRoot->reset();
		m_pRoot->m_Data = data;
	}
	else
	{
		Insert( m_pRoot, & data, &pNewNode);
	}

	return pNewNode;
}

static bool s_bDump = false;

template <class T>
i3AVLNode<T> *	i3AVLTree<T>::LookUp( const T & data)
{
	i3AVLNode<T> * pTemp = m_pRoot;
	INT32 cnt = 0;

	while( pTemp != NULL)
	{
		if( s_bDump)
		{
			I3TRACE( "[%d] %p \n", cnt, pTemp);
		}

		if( pTemp->m_Data == data)
			return pTemp;
		else if( pTemp->m_Data > data)
			pTemp = pTemp->m_pLeft;
		else
			pTemp = pTemp->m_pRight;

		cnt++;
	}

	return NULL;
}

template <class T>
T *		i3AVLTree<T>::Find( const T & data)
{
	i3AVLNode<T> * pFind;

	pFind = LookUp( data);

	if( pFind == NULL)
		return NULL;

	return &pFind->m_Data;
}

template <class T>
bool	i3AVLTree<T>::_FindLinearProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData)
{
	FIND_LINEAR_INFO * pInfo = (FIND_LINEAR_INFO *) pUserData;

	if( pNode->m_Data == * pInfo->m_pData)
	{
		pInfo->m_pFindNode = pNode;
		return false;
	}

	return true;
}

template <class T>
T *		i3AVLTree<T>::FindLinear( const T & data)
{
	FIND_LINEAR_INFO info;

	info.m_pData = (T *) &data;
	info.m_pFindNode = NULL;

	Traversal( _FindLinearProc, &info);

	if( info.m_pFindNode == NULL)
		return NULL;

	return &info.m_pFindNode->m_Data;
}

template <class T>
void i3AVLTree<T>::Remove( const T & data)
{
	i3AVLNode<T> * pNode;

	pNode = LookUp( data);

	if( pNode == NULL)
		return;

	Delete( pNode);
}

template <class T>
i3AVLNode<T> *	i3AVLTree<T>::GetSmallestNode( i3AVLNode<T> * pNode)
{
	while( pNode->m_pLeft != NULL)
	{
		pNode = pNode->m_pLeft;
	}

	return pNode;
}

template <class T>
i3AVLNode<T> *	i3AVLTree<T>::GetLargestNode( i3AVLNode<T> * pNode)
{
	while( pNode->m_pRight != NULL)
	{
		pNode = pNode->m_pRight;
	}

	return pNode;
}

template <class T>
void	i3AVLTree<T>::UnlinkFromParent( i3AVLNode<T> * pNode)
{
	i3AVLNode<T> * pParent;

	// 이 함수는 일반적인 용도로 사용되는 것은 아니며, 반드시 Delete 함수 내에서 사용된다.
	// 그래서 pNode가 Left-Subtree의 Largest Node이거나,
	// 또는 Right-Subtree의 Smallest Node라는 가정이 있다.
	pParent = pNode->m_pParent;

	if( pParent != NULL)
	{
		if( pParent->m_pLeft == pNode)
		{
			I3ASSERT( pNode->m_pLeft == NULL);

			SetLeft( pParent, pNode->m_pRight);
		}
		else
		{
			I3ASSERT( pParent->m_pRight == pNode);
			I3ASSERT( pNode->m_pRight == NULL);

			SetRight( pParent, pNode->m_pLeft);
		}
	}
	else
	{
		__debugbreak();
	}

	// Subtree를 가지지 않도록 초기화.
	pNode->m_pParent = NULL;
	pNode->m_pLeft = NULL;
	pNode->m_pRight = NULL;
	pNode->m_Balance = 0;
	pNode->m_Depth = 1;
}

template <class T>
void i3AVLTree<T>::Delete( i3AVLNode<T> * pNode)
{
	i3AVLNode<T> * pParent, * pSubtree = NULL;
	i3AVLNode<T> * pRetraceStart = NULL;

	pParent = pNode->m_pParent;

	if( pNode->isLeaf())
	{
		pSubtree = NULL;
	}
	else
	{
		if( pNode->m_Balance <= 0)
		{
			I3ASSERT( pNode->m_pLeft != NULL);

			pSubtree = GetLargestNode( pNode->m_pLeft);
		}
		else
		{
			I3ASSERT( pNode->m_pRight != NULL);

			pSubtree = GetSmallestNode( pNode->m_pRight);
		}

		I3ASSERT( pSubtree != NULL);

		// Retrace가 시작되는 Node..
		pRetraceStart = pSubtree->m_pParent;

		if( pSubtree->m_pParent != pNode)
		{
			UnlinkFromParent( pSubtree);
		}

		if( pSubtree != pNode->m_pLeft)
			SetLeft( pSubtree, pNode->m_pLeft);

		if( pSubtree != pNode->m_pRight)
			SetRight( pSubtree, pNode->m_pRight);
	}

	// Parent에 새로운 Subtree를 설정.
	if( pParent != NULL)
	{
		if( pParent->m_pLeft == pNode)
		{
			SetLeft( pParent, pSubtree);
		}
		else
		{
			I3ASSERT( pParent->m_pRight == pNode);

			SetRight( pParent, pSubtree);
		}
	}
	else
	{
		m_pRoot = pSubtree;
	}

	delete pNode;

	if( pRetraceStart != NULL)
	{
		if( pRetraceStart != pNode)
			Retrace( pRetraceStart);
		else
			Retrace( pSubtree);
	}
}

template <class T>
void i3AVLTree<T>::Retrace( i3AVLNode<T> * pNode)
{
	i3AVLNode<T> * pChild;

	// Balance Refresh
	UpdateBalance( pNode);

	if( pNode->m_Balance <= -2)
	{
		pChild = pNode->m_pLeft;

		I3ASSERT( pChild != NULL);

		if( pChild->m_Balance < 0)
		{
			// Left-Left case
			//I3TRACE( "RotateRight\n");
			RotateRight( pNode);
		}
		else
		{
			// Left-Right case
			//I3TRACE( "RotateLeft\n");
			RotateLeft( pChild);
			//I3TRACE( "RotateRight\n");
			RotateRight( pNode);
		}
	}
	else if( pNode->m_Balance >= 2)
	{
		pChild = pNode->m_pRight;

		I3ASSERT( pChild != NULL);

		if( pChild->m_Balance > 0)
		{
			// Right-Right case
			RotateLeft( pNode);
			//I3TRACE( "RotateLeft\n");
		}
		else
		{
			// Right-Left case
			//I3TRACE( "RotateRight\n");
			RotateRight( pChild);
			//I3TRACE( "RotateLeft\n");
			RotateLeft( pNode);
		}
	}

	if( pNode->m_pParent != NULL)
	{
		Retrace( pNode->m_pParent);
	}
}

template <class T>
bool	i3AVLTree<T>::_Rec_Traversal( i3AVLNode<T> * pNode, INT32 level)
{
	if( pNode == NULL)
		return true;

	if( _Rec_Traversal( pNode->m_pLeft, level + 1) == false)
		return false;

	if( m_pTraversalProc( pNode, level, m_pUserData) == false)
		return false;

	if( _Rec_Traversal( pNode->m_pRight, level + 1) == false)
		return false;

	return true;
}

template <class T>
void i3AVLTree<T>::Traversal( TRAVERSAL pProc, void * pUserData)
{
	m_pTraversalProc = pProc;
	m_pUserData = pUserData;

	_Rec_Traversal( m_pRoot, 0);
}

template <class T>
bool	i3AVLTree<T>::_CountProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData)
{
	INT32 * pCount = (INT32 *) pUserData;

	(*pCount) ++;

	return true;
}

template <class T>
INT32 i3AVLTree<T>::GetCount(void)
{
	INT32 count = 0;

	Traversal( _CountProc, &count);

	return count;
}

template <class T>
bool	i3AVLTree<T>::_GetAllProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData)
{
	i3List *	pList = (i3List *) pUserData;

	pList->Add( &pNode->m_Data);

	return true;
}

template <class T>
void i3AVLTree<T>::GetAll( i3List * pList)
{
	Traversal( _GetAllProc, pList);
}

#if defined( I3_DEBUG)
template <class T>
bool	i3AVLTree<T>::_DumpProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData)
{
	INT32 i;

	I3ASSERT( level < 16);

	for( i = 0; i < level; i++)
		I3TRACE( "\t");

	I3TRACE( "B(%d) : %p-%p\n", pNode->m_Balance, pNode, pNode->m_pParent);

	return true;
}

template <class T>
void i3AVLTree<T>::Dump( const char * pszStr)
{
	I3TRACE( "--------- %s ----------\n", pszStr);

	Traversal( _DumpProc, NULL);
}

template <class T>
void i3AVLTree<T>::DumpNode( i3AVLNode<T> * pNode, TRAVERSAL pDumpProc)
{
	i3List list;

	while( pNode != NULL)
	{
		list.Add( pNode);

		pNode = pNode->m_pParent;
	}

	INT32 i;

	for( i = list.GetCount() - 1; i >= 0; i--)
	{
		pNode = (i3AVLNode<T> *) list.GetItem( i);

		pDumpProc( pNode, list.GetCount() - i - 1, NULL);
	}
}

template <class T>
INT32	i3AVLTree<T>::CheckBalanceFactor( i3AVLNode<T> * pNode)
{
	INT32 ldepth = 0, rdepth = 0;

	if( pNode->m_pLeft != NULL)
		ldepth = CheckBalanceFactor( pNode->m_pLeft);

	if( pNode->m_pRight != NULL)
		rdepth = CheckBalanceFactor( pNode->m_pRight);

	INT32 depth = max( ldepth, rdepth) + 1;
	INT32 balance = rdepth - ldepth;

	I3ASSERT( depth == pNode->m_Depth);
	I3ASSERT( balance == pNode->m_Balance);

	return depth;
}

template <class T>
bool	i3AVLTree<T>::_CheckOrderProc( i3AVLNode<T> * pNode, INT32 level, void * pUserData)
{
	i3AVLNode<T> ** ppPrevNode = (i3AVLNode<T> **) pUserData;
	i3AVLNode<T> * pChild;

	if( pNode->m_pLeft != NULL)
	{
		pChild = pNode->m_pLeft;

		I3ASSERT( pChild->m_Data < pNode->m_Data);
	}

	if( pNode->m_pRight != NULL)
	{
		pChild = pNode->m_pRight;

		I3ASSERT( pChild->m_Data > pNode->m_Data);
	}
		

	if( *ppPrevNode != NULL)
	{
		I3ASSERT( (*ppPrevNode)->m_Data < pNode->m_Data);
	}

	*ppPrevNode = pNode;

	return true;
}

template <class T>
void	i3AVLTree<T>::CheckOrder(void)
{
	i3AVLNode<T> * pPrev = NULL;

	Traversal( _CheckOrderProc, &pPrev);
}

template <class T>
void i3AVLTree<T>::CheckLeftValue( i3AVLNode<T> * pNode, T * pData)
{
	I3ASSERT( pNode->m_Data < *pData);
	
	if( pNode->m_pLeft != NULL)
	{
		CheckLeftValue( pNode->m_pLeft, pData);

		CheckLeftValue( pNode->m_pLeft, &pNode->m_Data);
	}

	if( pNode->m_pRight != NULL)
	{
		CheckLeftValue( pNode->m_pRight, pData);

		CheckRightValue( pNode->m_pRight, &pNode->m_Data);
	}
}

template <class T>
void i3AVLTree<T>::CheckRightValue( i3AVLNode<T> * pNode, T * pData)
{
	I3ASSERT( pNode->m_Data > *pData);
	
	if( pNode->m_pLeft != NULL)
	{
		CheckRightValue( pNode->m_pLeft, pData);

		CheckLeftValue( pNode->m_pLeft, &pNode->m_Data);
	}

	if( pNode->m_pRight != NULL)
	{
		CheckRightValue( pNode->m_pRight, pData);

		CheckRightValue( pNode->m_pRight, &pNode->m_Data);
	}
}

template <class T>
void i3AVLTree<T>::CheckValue(void)
{
	if( m_pRoot != NULL)
	{
		if( m_pRoot->m_pLeft != NULL)
			CheckLeftValue( m_pRoot->m_pLeft, & m_pRoot->m_Data);

		if( m_pRoot->m_pRight != NULL)
			CheckRightValue( m_pRoot->m_pRight, & m_pRoot->m_Data);
	}
}

template <class T>
void	i3AVLTree<T>::CheckIntegrity(void)
{
	if( m_pRoot != NULL)
	{
		CheckBalanceFactor( m_pRoot);
		CheckOrder();
		CheckValue();
	}
}

#endif
