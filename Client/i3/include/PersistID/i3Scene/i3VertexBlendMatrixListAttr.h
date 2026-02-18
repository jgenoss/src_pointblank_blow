#if !defined( I3_SG_VERTEXBLENDMATRIXLISTATTR_H)
#define I3_SG_VERTEXBLENDMATRIXLISTATTR_H

#include "i3RenderAttr.h"
#include "i3Math.h"

class I3_EXPORT_SCENE i3VertexBlendMatrixListAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3VertexBlendMatrixListAttr);

protected:
	i3List 				m_List;
	RT_MATRIX			m_SkltMatrix;
	RT_MATRIX			m_InvSkltMatrix;

	void		RemoveAllMatrix(void);
public:
	i3VertexBlendMatrixListAttr(void);
	virtual ~i3VertexBlendMatrixListAttr(void);

	BOOL		Create( INT32 Count);

	void		SetMatrix( INT32 Idx, RT_MATRIX * pMatrix)
	{
		i3MatrixObject * pMtx = (i3MatrixObject *) m_List.Items[Idx];

		pMtx->SetMatrix( pMatrix);
	}

	RT_MATRIX *	GetMatrix( INT32 Idx)
	{
		i3MatrixObject * pMtx = (i3MatrixObject *) m_List.Items[Idx];

		return pMtx->GetMatrix();
	}

	void				SetMatrixObject( INT32 Idx, i3MatrixObject * pMatrixObject);
	i3MatrixObject *	GetMatrixObject( INT32 Idx);

	INT32		GetCount(void)
	{
		return m_List.GetCount();
	}

	void				SetSkeletonMatrix( RT_MATRIX * pMatrix, RT_MATRIX * pInvMatrix)
	{
		i3Matrix::Copy( &m_SkltMatrix, pMatrix);
		i3Matrix::Copy( &m_InvSkltMatrix, pInvMatrix);
	}

	RT_MATRIX *			GetSkeletonMatrix(void)					{ return &m_SkltMatrix; }
	RT_MATRIX *			GetInverseSkeletonMatrix(void)			{ return &m_InvSkltMatrix; }

	virtual void Apply( i3RenderContext * pContext);
	virtual void CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL IsSame( i3RenderAttr * pAttr);

	virtual void OnBuildObjectList( i3List * pList);
	virtual UINT32 OnSave( i3ResourceFile * pResFile);
	virtual UINT32 OnLoad( i3ResourceFile * pResFile);
};

extern i3VertexBlendMatrixListAttr * g_pLastBlendMatrixList;

#endif
