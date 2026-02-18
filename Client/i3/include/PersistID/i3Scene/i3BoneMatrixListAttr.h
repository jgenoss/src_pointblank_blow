#if !defined( __I3_BONE_MATRIX_LIST_ATTR_H)
#define __I3_BONE_MATRIX_LIST_ATTR_H

#include "i3RenderAttr.h"
#include "i3Bone.h"
#include "i3AnimationContext.h"

class i3SceneTracer;

class i3BoneMatrixListAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3BoneMatrixListAttr);
protected:
	i3Bone *			m_pBone;
	INT32				m_Count;
	i3MatrixArray *		m_pTxBoneArray;
	i3MatrixArray *		m_pInvBoneArray;

public:
	i3BoneMatrixListAttr(void);
	virtual ~i3BoneMatrixListAttr(void);

	INT32				getBoneCount(void)						{ return m_Count; }
	void				SetBoneCount( INT32 count);
	i3Bone *			getBone( INT32 idx)						{ return &m_pBone[idx]; }

	INT32				getParentBoneIndex( INT32 idx)			{ return m_pBone[idx].getParentIndex(); }
	void				setParentBoneIndex( INT32 idx, INT32 parentIdx)
	{
		m_pBone[idx].setParentIndex( parentIdx);
	}

	MATRIX *			getMatrix( INT32 idx)					{ return m_pBone[idx].getMatrix(); }
	void				setMatrix( INT32 idx, MATRIX * pMtx)	{ m_pBone[idx].setMatrix( pMtx); }

	i3MatrixArray *		getInverseBoneArray(void)				{ return m_pInvBoneArray; }
	void				setInverseBoneArray( i3MatrixArray * pArray)
	{
		I3_REF_CHANGE( m_pInvBoneArray, pArray);
	}

	MATRIX *			getInverseMatrix( INT32 idx)			{ return m_pInvBoneArray->GetMatrix( idx); }
	void				setInverseMatrix( INT32 idx, MATRIX * pMtx)
	{
		m_pInvBoneArray->SetMatrix( idx, pMtx);
	}

	UINT32				getDepth( UINT32 idx)					{ return m_pBone[idx].getDepth(); }
	void				setDepth( UINT32 idx, UINT32 depth)		{ m_pBone[idx].setDepth( depth); }

	char *				getBoneName( INT32 idx)					{ return m_pBone[idx].getName(); }
	void				setBoneName( INT32 idx, char * pszName)
	{
		m_pBone[idx].setName( pszName);
	}

void				BuildTransformedMatrix( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);
	MATRIX *			getTransformedMatrix( INT32 idx)		{ return m_pTxBoneArray->GetMatrix( idx); }

	virtual void		Apply( i3RenderContext * pContext);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif
