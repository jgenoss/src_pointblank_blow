//*******************************************************************************************************
//	Nvidia 사이트에서 공개한 NvTriStip을 이용해 Triangle List <-> Triangle Strip 상호 변환시켜줍니다.
//	
//	by KOMET 2006.02.06
//*******************************************************************************************************
#if !defined( __I3_OPT_BUILD_TRI_STRIP_H)
#define __I3_OPT_BUILD_TRI_STRIP_H

#include "i3OptPCH.h"
#include "i3SceneOptimizer.h"
#include "i3NvTriStrip.h"
#include "i3OptBuildError.h"

#define		MIN_STRIP_SIZE		0


struct BuildStripInfo
{
	I3G_PRIMITIVE		_nTargetPrimType = I3G_PRIM_NONE;			//	변경될 타입 (default: I3G_PRIM_NONE)
	bool				_bAutoSwitching = false;					//	Triangle List <-> Triangle Strip 자동 switching 여부 (default: false)
	bool				_bStitchStrips = true;						//	m_TargetPrimType이 Strip일 경우 vertex를 하나로 연결할지 여부 (default: true)
	bool				_bValidateEnabled = false;					//	변경후 재확인할 여부 ( default: false)
	UINT32				_nMinStripSize = MIN_STRIP_SIZE;			//	m_TargetPrimType이 Strip일 경우 최소 vertex 갯수 ( default: 0)
	
	void SetDefault(void)
	{
		_nTargetPrimType = I3G_PRIM_NONE;
		_bAutoSwitching = false;
		_bStitchStrips = true;
		_bValidateEnabled = false;
		_nMinStripSize = MIN_STRIP_SIZE;
	}

} ;


class I3_EXPORT_OPT i3OptBuildTriStrip : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildTriStrip, i3SceneOptimizer);

protected:
	BuildStripInfo		m_BuildInfo;		
	unsigned short *	m_pTempIndices = nullptr;		//	임시 변환용 index 배열	
	PrimitiveGroup *	m_pPrimitiveGroups = nullptr;	//	primitive groups from NvTriStrip

	OPT_ERROR			m_Error;
    
public:
	i3OptBuildTriStrip(void);
	virtual ~i3OptBuildTriStrip(void);
	
	virtual bool OnNode( i3Node * pNode, i3Node * pParent) override;
	virtual void SetProperty( char * pszFieldName, char * pszValue) override;

public:	
	bool	Convert( i3Node * pNode);		

	//	IndexArray가 있어야 가능함
	bool	BuildStrip( i3GeometryAttr * pGeoAttr);		
	//void	RemapIndices( i3GeometryAttr * pGeoAttr);	

	//	 빌드 정보 설정 ( Convert 함수나 BuildStrip 함수를 호출하기 전에 설정해야 한다.)
	void	SetAutoSwitching( bool bVal)				{   m_BuildInfo._bAutoSwitching = bVal;				}
	void	SetTargetPrimType( I3G_PRIMITIVE nPrimType)	{	m_BuildInfo._nTargetPrimType = nPrimType;		}
	//void	SetBuildIndex( bool	nVal)					{	m_BuildInfo._bBuildIndex = nVal;				}
	void	SetStripStitch( bool bVal)					{	m_BuildInfo._bStitchStrips = bVal;				}
	void	SetStripValidateEnabled( bool bVal)			{	m_BuildInfo._bValidateEnabled = bVal;			}
	void	SetStripMinSize( UINT32 nSize)				{	m_BuildInfo._nMinStripSize = nSize;				}
	
	OPT_ERROR * GetError(void)							{	return &m_Error;								}

protected:

	void				SetResult( I3OPT_RESULT nResult, char * pMsg = nullptr)	{	m_Error.SetResult( nResult, pMsg);				}

	//	PrimitiveGroup값으로 다시 i3IndexArray를 만든다.	
	bool				MakeIndices( i3IndexArray ** pOutIA, unsigned short numSections, PrimitiveGroup * pPrimGroup);	
	bool				ConvertIndexStrip2List( i3IndexArray ** pOutIA, i3GeometryAttr * pGeoAttr);
	unsigned short *	ExtractIndices( i3IndexArray * pIA);				//	unsigned short 배열로 인덱스값을 리턴한다.

};

#endif //	__I3_OPT_BUILD_TRI_STRIP_H