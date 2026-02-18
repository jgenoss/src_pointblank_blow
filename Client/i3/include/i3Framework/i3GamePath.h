#if !defined( _I3_GAME_PATH_H_)
#define _I3_GAME_PATH_H_

#include "i3GameObj.h"
#include "i3PathPoint.h"
#include "i3PathLine.h"

typedef	INT32			I3_POINTIDX;
typedef	INT32			I3_LINKIDX;

struct I3_PATH_FIND_INFO
{
	I3_POINTIDX				m_idxTarget = 0;
	i3::pack::PATH_POINT_INFO *	m_pTarget = nullptr;

	I3_POINTIDX *			m_pTable = nullptr;

	INT32					m_idxCur = 0;
	INT32					m_maxLen = 0;
} ;

struct I3_NEXTPOINTINFO
{
	I3_POINTIDX			m_idxNext = 0;
	I3_LINKIDX			m_idxLink = 0;
	REAL32				m_dot = 0.0f;
} ;

class I3_EXPORT_FRAMEWORK i3GamePath : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3GamePath, i3GameObj);
protected:
	i3MemoryBuffer *		m_pBuff = nullptr;

	i3::pack::PATH_POINT_INFO *	m_pPoints = nullptr;
	INT32					m_PointCount = 0;

	i3::pack::PATH_LINK_INFO *		m_pLinks = nullptr;
	INT32					m_LinkCount = 0;

	I3_LINKIDX *			m_pIndexTable = nullptr;
	INT32					m_IndexCount = 0;

	// Volatile
	UINT32					m_FindID = 0;

protected:
	bool					_Rec_FindShortestPath( I3_POINTIDX idxCur, I3_PATH_FIND_INFO * pInfo);
	INT32					_FindLinkedPoint( I3_POINTIDX idxStart, I3_NEXTPOINTINFO * pArray, INT32 maxCount);
	INT32					_RemoveRedundantPoint( INT32 i, I3_POINTIDX * pTable);

public:
	virtual ~i3GamePath( void);

	void					Create( const i3::vector<i3PathPoint*>& PointList, const i3::vector<i3PathLine*>& LinkList);

	void					Create( i3MemoryBuffer * pBuffer, INT32 pointCount, INT32 linkCount);

	INT32					getPointCount(void)				{	return m_PointCount; }
	i3::pack::PATH_POINT_INFO *	getPoint( I3_POINTIDX idx)
	{
		I3ASSERT( m_pPoints != nullptr);
		I3_BOUNDCHK( idx, m_PointCount);

		return m_pPoints + idx;
	}

	INT32					getLinkCount(void)				{	return m_LinkCount; }
	i3::pack::PATH_LINK_INFO *		getLink(I3_LINKIDX idx)
	{
		I3ASSERT( m_pLinks != nullptr);
		I3_BOUNDCHK( idx, m_LinkCount);

		return m_pLinks + idx;
	}

	I3_POINTIDX		FindClosestPoint( VEC3D * pPos);
	I3_POINTIDX		FindClosestPoints( VEC3D * pPos, INT32 * pidxArray, INT32 maxLength);
	I3_POINTIDX		FindClosestPointByPriority( VEC3D * Pos, INT32 limitPriority);
	INT32			GetLinkedPoint( I3_POINTIDX idxStart, I3_NEXTPOINTINFO * pArray, INT32 maxCount);

	INT32			FindShortestPath( I3_POINTIDX idxStart, I3_POINTIDX idxEnd, I3_POINTIDX * pTable, INT32 maxLen);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	static i3GamePath *		LoadFromFile( const char * pszPath);
	static bool				SaveToFile( const char * pszPath, i3GamePath * pPath);
};

#endif