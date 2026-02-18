#if !defined( __I3_LOD_H)
#define __I3_LOD_H

#include "i3Node.h"
#include "i3Skeleton.h"

class i3Body;
class i3BoneRef;

/** \brief
	\desc leaf class */

class I3_EXPORT_SCENE i3LOD : public i3PersistantElement
{
	I3_EXPORT_CLASS_DEFINE( i3LOD, i3PersistantElement);
protected:
	i3Skeleton *		m_pSkel = nullptr;
	i3Node *			m_pShapeNode = nullptr;
	i3MemoryBuffer *	m_pBoneMap = nullptr;

	// 임시
	i3Skeleton *		m_pSharedSkel = nullptr;

public:
	virtual ~i3LOD(void);

	INT32				findBoneByName( const char * pszName)
	{
		if( m_pSkel != nullptr)	return m_pSkel->FindBoneByName((char *)pszName);
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "not set skeleton.");
			return -1;
		}
	}

	INT32				getParentBoneIndex( INT32 idx)
	{
		if( m_pSkel != nullptr)		return m_pSkel->getParentBoneIndex( idx);
		else
		{
			I3PRINTLOG(I3LOG_FATAL,  "not set skeleton.");
			return -1;
		}
	}

	i3Skeleton *		getSkeleton(void)					{ return m_pSkel; }
	void				setSkeleton( i3Skeleton * pSkel)	{ I3_REF_CHANGE( m_pSkel, pSkel); }

	UINT32				getBoneCount(void)					{ if(m_pSkel != nullptr) return m_pSkel->getBoneCount(); return 0;}

	i3Node *			getShapeNode(void)					{ return m_pShapeNode; }
	void				setShapeNode( i3Node * pNode)
	{
		I3_REF_CHANGE( m_pShapeNode, pNode);
#if defined( I3_DEBUG)
		if( m_pShapeNode != nullptr)
		{
			m_pShapeNode->SetOwner( this);
		}
#endif
	}

	INT32 *				getBoneMap(void)					
	{ 
		if(m_pBoneMap == nullptr) 
			return nullptr;
		
		return (INT32 *) m_pBoneMap->getBuffer(); 
	}

	void				setSharedSkeleton( i3Skeleton * pSkel)	{ m_pSharedSkel = pSkel; }

	void				setBoneMap( i3MemoryBuffer * pBuf)	{ I3_REF_CHANGE( m_pBoneMap, pBuf); }

	// Bone 내에 포함하고 있는 i3PhysixShapeSet의 개수를 반환합니다.
	INT32				hasShapeSet(void);

	// pTable can be nullptr. In this case, the function just allocates memory for a room
	void				SetBoneMapTable( INT32 * pTable, UINT32 Count);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	void			OnInstancing( I3_ONINSTANCING_INFO * pInfo);

	//////////////////////////////////////////////////////////////////////////////////////////
	//									Debug renderable axis								//
	//////////////////////////////////////////////////////////////////////////////////////////
#if defined( I3_DEBUG)
private:
	i3Node *		m_pRenderableBoneAxis = nullptr;
	REAL32			m_rRenderableBoneAxisLen = 0.0f;

protected:
	i3BoneRef *		_CreateRenderableBoneRef( INT32 iBoneIdx, const char * pszBoneName);

public:
	/** \brief Renderable Bone Axis를 생성한다. (for Debug)
		\param[in] rAxisLen 축의 길이. (default 0.5m)
		\return N/A */
	void			CreateRenderableBoneAxis( REAL32 rAxisLen = 0.5f);

	/** \brief Renderable Bone Axix의 활성 유무를 설정한다.
		\param[in] iBoneIdx Bone index
		\param[in] bEnable true : 활성. false : 비활성 (default true)
		\return N/A */
	void			EnableRenderableBoneAxis( INT32 iBoneIdx, bool bEnable = true);

	/** \brief 모든 Renderable Bone Axis의 활성 유무를 설정한다.
		\param[in] bEnable true : 활성. false : 비활성 (default true)
		\return N/A */
	void			EnableAllRenderableBoneAxis( bool bEnable = true);

	/** \brief Renderable Bone Axis를 삭제한다. */
	void			ReleaseRenderableBoneAxis( void);

private:
	i3Body *		m_pBody = nullptr;						///< i3Body Reference pointer

public:
	/** \brief Body를 반환한다. (for TDK)
		\return i3Body 객체 포인터 */
	i3Body *		getBody( void)					{ return m_pBody; }

	/** \brief Body를 설정한다. (for TDK)
		\param[in] pBody i3Body 객체 포인터
		\return N/A */
	void			setBody( i3Body* pBody)			{ m_pBody = pBody; }
#endif
};

#endif
