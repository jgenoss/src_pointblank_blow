#if !defined( __I3_LEVEL_PATH_H)
#define __I3_LEVEL_PATH_H

#include "i3LevelElement3D.h"
#include "i3LevelPathLink.h"
#include "i3LevelPathPoint.h"

struct PATH_FIND_INFO
{
	i3LevelPathPoint * m_pTarget = nullptr;
	i3LevelPathPoint ** m_pTable = nullptr;
	INT32			m_idxCur = 0;
	INT32			m_maxLen = 0;
} ;

struct NEXTPOINTINFO
{
	i3LevelPathPoint *	m_pNext = nullptr;
	REAL32				m_dot = 0.0f;
} ;

class I3_EXPORT_TDK i3LevelPath : public i3LevelElement3D
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPath, i3LevelElement3D);
protected:
	i3GamePath *		m_pPath = nullptr;

	// Volatile 
	i3::vector<i3LevelPathPoint*>	m_PointList;
	i3::vector<i3LevelPathLink*>	m_LineList;

	INT32				m_FindID = 0;

protected:
	void			_UpdateLink( i3LevelPathPoint * pPoint);
	void			_RemoveAll(void);
	void			_RemoveAllRelatedLine( i3LevelControl * pElm);

	i3LevelPathPoint *	_CreatePointIcon(void);

	void			_MakeGameData(void);

	bool			_Rec_FindShortestPath( i3LevelPathPoint * pCur, PATH_FIND_INFO * pInfo);
public:
	i3LevelPath(void);
	virtual ~i3LevelPath(void);

	virtual void		BindRes( i3LevelRes * pRes) override;

	void				AddPoint( VEC3D * pPos);
	void				AddLine( i3LevelPathPoint * pStart, i3LevelPathPoint * pEnd);
	i3LevelPathLink *	FindLinkByPoint( i3LevelPathPoint * pStart, i3LevelPathPoint * pEnd);
	INT32				FindShortestPath( i3LevelPathPoint * pStart, i3LevelPathPoint * pEnd, i3LevelPathPoint ** pTable, INT32 maxLen);

	virtual void	OnAttachScene( i3LevelScene * pScene) override;
	virtual void	OnDetachScene( i3LevelScene * pScene) override;

	virtual bool	OnRemoveIcon( i3LevelControl * pElm) override;
	virtual void	OnMovingIcon( i3LevelControl * pElm) override;
	virtual void	OnScalingIcon( i3LevelControl * pElm) override;
	virtual void	OnRotatingIcon( i3LevelControl * pElm) override;
	virtual void	OnSelectedIcon( i3LevelControl * pElm) override;

	virtual void	OnPrepareSerialize(void) override;

	virtual i3GameObj *	CreateGameInstance(void) override;

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
