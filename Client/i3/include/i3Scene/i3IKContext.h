#if !defined( __I3_IKCONTEXT_H)
#define __I3_IKCONTEXT_H

#include "i3IKBoneInfo.h"
#include "i3Base/smart_ptr/pscoped_ptr.h"

/** \defgroup I3_IKBONE_STYLE
*/
//@{
typedef UINT32 I3_IKBONE_STYLE;
#define		I3_IKBONE_STYLE_ENABLE			0x00000001
//@}


class i3Skeleton;
class i3SceneObject;
class i3IKConstraint;

/** \brief 하나의 IK Bone chain에 대한 처리를 하는 class
*/
class I3_EXPORT_SCENE i3IKContext : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3IKContext, i3PersistantElement);
protected:
	// Persistent members
	char			m_szName[ 64];

	INT32			m_iIKBoneCount;				///< IK가 적용되는 Bone count
	i3IKBoneInfo *	m_pIKBoneInfoArray;
	
	i3IKConstraint *	m_pConstraint;			///< TargetPoint에 대한 Handler

	// Volatile members
	I3_IKBONE_STYLE	m_nIKStyle;

	// test
	i3::pscoped_ptr<i3::vector<class i3BoneRef*> >	m_pTestIKRotateAxisList;	/// 현재 쓰이지 않고 있지만, 삭제는 어려워보여 벡터로만 치환..(2012.05.18.수빈)

public:
	/** \brief IK Name을 반환한다.
		\return Name */
	char *			getName( void)									{ return m_szName; }

	/** \brief IK Name을 설정한다.
		\param[in] pszName Name
		\return N/A */
	void			setName( char * pszName)						{ i3String::NCopy( m_szName, pszName, sizeof(char) * 64); }

	/** \brief IK를 하는 Bone의 개수를 반환한다.
		\return IK Bone Count */
	INT32			getIKBoneCount( void)							{ return m_iIKBoneCount; }

	/** \brief IK를 하는 Bone의 개수를 설정한다.
		\note IK Info array가 없는 경우 생성한다.
		\param[in] count IK Bone Count
		\return N/A */
	void			setIKBoneCount( INT32 count)
	{
		if( m_iIKBoneCount != count)
		{
			m_iIKBoneCount = count;
			I3MEM_SAFE_FREE( m_pIKBoneInfoArray);
			m_pIKBoneInfoArray = (i3IKBoneInfo*) i3MemAlloc( sizeof( i3IKBoneInfo) * count);
		}
	}

	/** \brief IK Bone Info를 반환한다.
		\param[in] idx IK Bone Index
		\return i3IKBoneInfo 구조체 포인터 */
	i3IKBoneInfo *	getIKBoneInfo( INT32 idx)
	{
		I3ASSERT( idx >=0 && idx < m_iIKBoneCount);
		return &m_pIKBoneInfoArray[idx];
	}

	/** \brief IK Bone Info를 설정한다.
		\param[in] idx IK Bone Index
		\param[in] pInfo i3IKBoneInfo 구조체 포인터
		\return N/A */
	void			setIKBoneInfo( INT32 idx, i3IKBoneInfo * pInfo)
	{
		I3ASSERT( idx >= 0 && idx < m_iIKBoneCount);
		i3mem::Copy( &m_pIKBoneInfoArray[idx], pInfo, sizeof( i3IKBoneInfo));
	}

	/** \brief IK Constraint를 반환한다.
		\param[in] idx IK Constraint Index
		\return i3IKConstraint 객체 포인터	*/
	i3IKConstraint * getIKConstraint( void)					{ return m_pConstraint; }

	/** \brief IK Constraint를 설정한다.
		\param[in] idx IK Constraint Index
		\param[in] pConstraint i3IKConstraint 객체 포인터
		\return N/A	*/
	void			SetIKConstraint( i3IKConstraint * pConstraint);
	
	/** \brief IK Style을 반환한다.
		\return I3_IKBONE_STYLE */
	I3_IKBONE_STYLE	getIKStyle( void)								{ return m_nIKStyle; }

	/** \brief IK Style을 설정한다.
		\param[in] style I3_IKBONE_STYLE
		\return N/A */
	void			setIKStyle( I3_IKBONE_STYLE style)				{ m_nIKStyle = style; }

	/** \brief IK Style을 추가한다.
		\param[in] style I3_IKBONE_STYLE
		\return N/A */
	void			addIKStyle( I3_IKBONE_STYLE style)				{ m_nIKStyle |= style; }

	/** \brief IK Style을 제거한다.
		\param[in] style I3_IKBONE_STYLE
		\return N/A */
	void			removeIKStyle( I3_IKBONE_STYLE style)			{ m_nIKStyle &= ~style; }

	/** \brief for debug */
	i3::vector<i3BoneRef*>*	getIKRotateAxisList( void)				{ return m_pTestIKRotateAxisList.get(); }

protected:
	void			_CreateIKBoneInfo( INT32 boneCount);
	
public:
	i3IKContext();
	virtual ~i3IKContext();

	/** \brief IK Constraint를 생성한다.
		\param[in] iStarterIdx Starter Bone Index
		\param[in] iEffectorIdx Effector Bone Index
		\param[in] rDistanceThresh 최소 접근 거리. (default 0.01)
		\return N/A */
	void			CreateIKConstraint( INT32 iStarterIdx, INT32 iEffectorIdx, REAL32 rDistanceThresh = 0.01f);

	/** \brief IK Bone을 찾는다.
		\param[in] iBoneIndex Bone index
		\return IK Bone Index */
	INT32			FindIKBone( INT32 iBoneIndex);

	/** \brief 거리를 검사한다.
		\param[in] pMat MATRIX 구조체 포인터. 목표점에 대한 Matrix
		\return true : 도달. false : 미달 */
	bool			CheckDistance( MATRIX * pMat );

	/**
	\brief i3IKContext을 복사한다.
	\param[out] pObj 복사할 대상 i3IKContext 객체 포인터.
	\param[in] method 복사하는 방식
	\return N/A
	*/
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);

	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List);

	/**
	\brief 파일로 저장한다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32	OnSave( i3ResourceFile * pResFile);

	/**
	\brief 파일에서 읽어온다.
	\param[in] pResFile i3ResourceFile 객체 포인터
	\return 파일 사이즈
	*/
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);

	/** \brief Debug용 Renderable Axis를 만든다. (for Debug) */
	void			CreateRenderableRotateAxis( i3SceneObject * pSceneObject);
};

#endif
