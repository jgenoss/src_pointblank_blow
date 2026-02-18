#if !defined( __I3_BONE_MATRIX_LIST_ATTR_H)
#define __I3_BONE_MATRIX_LIST_ATTR_H

#include "i3RenderAttr.h"
#include "i3Bone.h"
#include "i3AnimationContext.h"

class i3SceneTracer;

//
// 익스포트 옵션 추가..(2012.09.28.수빈)
//

/** \brief
	\desc leaf class, volatile class */

class I3_EXPORT_SCENE i3BoneMatrixListAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3BoneMatrixListAttr, i3RenderAttr);
protected:
	i3Bone *			m_pBone = nullptr;
	INT32				m_Count = 0;
	i3MatrixArray *		m_pTxBoneArray = nullptr;
	i3MatrixArray *		m_pInvBoneArray = nullptr;

	UINT32				m_Version = 0;

public:
	i3BoneMatrixListAttr(void);
	virtual ~i3BoneMatrixListAttr(void);

	INT32			getBoneCount(void) const				{ return m_Count; }
	void			SetBoneCount( INT32 count, INT32 ver = 1);

	i3Bone *		getBone( INT32 idx) const
	{
		I3_BOUNDCHK(idx,m_Count);
		return &m_pBone[idx];
	}

	INT32	getParentBoneIndex( INT32 idx)	const		{ return getBone(idx)->getParentIndex(); }
	void	setParentBoneIndex( INT32 idx, INT32 parentIdx)	{getBone(idx)->setParentIndex( parentIdx);	}

	MATRIX *getMatrix( INT32 idx) const				{ return getBone(idx)->getMatrix(); }
	void	setMatrix( INT32 idx, MATRIX * pMtx)	{ getBone(idx)->setMatrix( pMtx); }

	i3MatrixArray *	getInverseBoneArray(void)				{ return m_pInvBoneArray; }
	void			setInverseBoneArray( i3MatrixArray * pArray){	I3_REF_CHANGE( m_pInvBoneArray, pArray);	}

	MATRIX *		getInverseMatrix( INT32 idx)			{ return m_pInvBoneArray->GetMatrix( idx); }
	void			setInverseMatrix( INT32 idx, MATRIX * pMtx)	{	m_pInvBoneArray->SetMatrix( idx, pMtx);	}

	UINT32	getDepth( UINT32 idx) const					{ return getBone(idx)->getDepth(); }
	void	setDepth( UINT32 idx, UINT32 depth)			{ getBone(idx)->setDepth( depth); }

	const char *getBoneName( INT32 idx)	const				{ return getBone(idx)->getName(); }
	const i3::rc_string&	getBoneNameString(INT32 idx) const { return getBone(idx)->getNameString(); }
	void		setBoneName( INT32 idx, const i3::rc_string& strName)	{ getBone(idx)->setName( strName);	}

	void			BuildTransformedMatrix( i3SceneTracer * pTracer, i3AnimationContext * pCtx, INT32 * pBoneMap);
	MATRIX *		getTransformedMatrix( INT32 idx)		{ return m_pTxBoneArray->GetMatrix( idx); }

	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
