#if !defined( __I3_GEOMETRYATTR2_H_)
#define __I3_GEOMETRYATTR2_H_

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3GeometryAttr.h"
#include "i3Base/smart_ptr/shared_ptr.h"

struct I3GEOM_VSPLIT
{
	VEC3D			v0, v1, vf;
	VEC2D			uv0, uv1, uvf;
	UINT32			vOldID = 0, vNewID = 0;		//이번 Contract에서는 OldID인것들이 NewID로 변경되었고,
	INT32			nSplitCount = 0;		//split count의 갯수만큼
	INT32			nOldCount = 0;
	INT32			nNewCount = 0;
	INT32			nFaceCount = 0;			//nFaceCount만큼
	INT32			*iIndicId = nullptr;			//새 VertexID로 id가 변경된 index들이 있고,
	INT32			*iFaceID = nullptr;			//유효하지 않게 설정된 face들이 있다.
	UINT32			*iOldID = nullptr;
	UINT32			*iNewID = nullptr;
	REAL32			err = 0.0f;
};

class I3_EXPORT_SCENE i3GeometryAttr2 : public i3GeometryAttr
{
	I3_EXPORT_CLASS_DEFINE( i3GeometryAttr2, i3GeometryAttr);

public:
	virtual ~i3GeometryAttr2( void);

protected:
	i3::shared_ptr< i3::vector< I3GEOM_VSPLIT*> >	m_pSplitData;
	INT32				m_curSplitIndex = 0;

	REAL32	m_curLODLevel = 0.0f;
	bool	m_bAutoUpdate = false;

	bool	_refreshLOD();
	void	_collapse();
	void	_split();

	void	_removeSplitData(void);

public:
	void			SetDetailLevel( REAL32 rate);	
	REAL32			GetDetailLevel( void);

	void	Collapse(bool oneStep, bool bLast = false);
	void	Split(bool oneStep, bool bFirst = false);

public:
	void			SetSplitData( const i3::shared_ptr<	i3::vector< I3GEOM_VSPLIT*> >& pSplitData);
	// for i3RenderAttr
	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;

	// for i3PersistantElement
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};
#endif