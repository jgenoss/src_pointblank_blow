#if !defined( __I3_BONE2_MATRIX_LIST_ATTR_H)
#define __I3_BONE2_MATRIX_LIST_ATTR_H

#include "i3BoneMatrixListAttr.h"
#include "i3Bone2.h"
#include "i3AnimationContext.h"

class i3SceneTracer;

/** \brief Bone2 구조체를 관리하는 attribute class */
class I3_EXPORT_SCENE i3Bone2MatrixListAttr : public i3BoneMatrixListAttr
{
	I3_CLASS_DEFINE( i3Bone2MatrixListAttr);
protected:
	i3Bone2 *			m_pBone2;

public:
	i3Bone2MatrixListAttr(void);
	virtual ~i3Bone2MatrixListAttr(void);

	/** \brief Bone을 생성한다.
		\param[in] count Bone 개수
		\return N/A */
	virtual void	SetBoneCount( INT32 count);

	//----------------------------------------------------------------------------------------------------------//
	// override function from i3BoneMatrixListAttr
	/** \brief Bone을 반환한다.
		\param[in] idx Bone Index
		\return i3Bone2 구조체 포인터 */
	i3Bone2 *		getBone( INT32 idx)						{ I3_BOUNDCHK(idx,m_Count); return &m_pBone2[idx]; }

	/** \brief Parent Bone Index를 반환한다.
		\param[in] idx Bone Index
		\return Parent Bone Index */
	virtual INT32	getParentBoneIndex( INT32 idx)			{ return getBone(idx)->getParentIndex(); }

	/** \brief Parent Bone Index를 설정한다.
		\param[in] idx Bone Index
		\param[in] parentIdx Parent Bone Index
		\return N/A */
	virtual void	setParentBoneIndex( INT32 idx, INT32 parentIdx)	{ getBone( idx)->setParentIndex( parentIdx);	}

	/** \brief Local Matrix를 반환한다.
		\param[in] idx Bone Index
		\return MATRIX 구조체 포인터 */
	virtual MATRIX *getMatrix( INT32 idx)					{ return getBone( idx)->getMatrix(); }

	/** \brief Local Matrix를 설정한다.
		\param[in] idx Bone Index
		\param[in] pMtx MATRIX 구조체 포인터
		\return N/A */
	virtual void	setMatrix( INT32 idx, MATRIX * pMtx)	{ getBone(idx)->setMatrix( pMtx); }

	/** \brief Hierarchy Depth값을 반환한다.
		\param[in] idx Bone Index
		\return Depth값 */
	virtual UINT32	getDepth( INT32 idx)					{ return getBone(idx)->getDepth(); }

	/** \brief Hierarchy Depth값을 설정한다.
		\param[in] idx Bone Index
		\param[in] depth Depth값
		\return N/A */
	virtual void	setDepth( INT32 idx, UINT32 depth)		{ getBone(idx)->setDepth( depth); }

	/** \brief Bone Name을 반환한다.
		\param[in] idx Bone Index
		\return Bone Name */
	virtual const char * getBoneName( INT32 idx)			{ return (const char*) getBone(idx)->getName(); }

	/** \brief Bone Name을 설정한다.
		\param[in] idx Bone Index
		\param[in] pszName Bone Name
		\return N/A */
	virtual void		setBoneName( INT32 idx, const char * pszName)	{ getBone(idx)->setName( (char*)pszName);}

	//-------------------------------------------------------------------------------------------------------------------------//
	//										For only Bone2
	//-------------------------------------------------------------------------------------------------------------------------//
	// addable function for new version
	/** \brief Bone Flag를 반환한다.
		\param[in] idx Bone Index
		\return I3_BONEFLAG */
	I3_BONEFLAG			getFlag( INT32 idx)						{ return getBone(idx)->getFlag(); }

	/** \brief Bone Flag를 설정한다.
		\param[in] idx Bone Index
		\param[in] flag I3_BONEFLAG
		\return N/A */
	void				setFlag( INT32 idx, I3_BONEFLAG flag)	{ getBone(idx)->setFlag( flag); }

	/** \brief IK Info Index를 반환한다.
		\param[in] idx Bone Index
		\return IK Info Index */
	INT32				getIKBoneInfoIndex( INT32 idx)			{ return getBone(idx)->getIKBoneInfoIndex(); }

	/** \brief IK Info Index를 설정한다.
		\param[in] idx Bone Index
		\param[in] iIKIdx IK Info Index
		\return N/A */
	void				setIKBoneInfoIndex( INT32 idx, INT32 iIKIdx)	{ getBone(idx)->setIKBoneInfoIndex( iIKIdx); }
	
	/**
	\brief i3Bone2MatrixListAttr을 복사한다.
	\param[out] pObj 복사할 대상 i3Bone2MatrixListAttr 객체 포인터.
	\param[in] method 복사하는 방식
	\return N/A
	*/
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	/**
	\brief 파일로 저장한다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32		OnSave( i3ResourceFile * pResFile);

	/**
	\brief 파일에서 읽어온다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	/** \brief pSrc로부터 Bone 정보를 복사해 온다.
		\param[in] pSrc i3BoneMatrixListAttr 객체 포인터
		\param[in] method I3_COPY_METHOD. (default I3_COPY_INSTANCE)
		\return N/A */
	void				CopyFrom( i3BoneMatrixListAttr * pSrc, I3_COPY_METHOD method = I3_COPY_INSTANCE);
};

#endif
