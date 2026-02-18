#if !defined ( _CUI_MERGED_TEXTURE_H__)
#define _CUI_MERGED_TEXTURE_H__
/*
typedef struct __tagi3UIResInfo			// 원본 resource의 정보, i3UIImage에서 texture를 선택하고, UV를 지정하여 merge하기위해 넘겨주는 구조체. 
{										// merged texture에서는 원본에 대한 정보를 통해 merge를 실행하며, 이 정보를 보관하였다가 필요한 경우 반환해준다.
	char			m_Path[MAX_PATH];	// 원본 resource texture path
	UINT32			hashCode;			// 원본 resource texture path의 hashcode( by name)
	i3Texture	*	m_pTexture;			// 원본 resource texture.

	i3::pack::RECT			uvwh;				// 원본 resource texture내에서의 texcoord position. width,height.

	void	Init(void)
	{
		m_Path[0] = 0;	m_pTexture = NULL;
		uvwh.left = uvwh.right = uvwh.top = uvwh.bottom = 0.0f;
	}
}I3UI_RESSRC_INFO;

typedef struct __tagi3UIVTRect	// merged texture의 한부분 영역을 정의하는 Rect구조체. overlap될수 있지만( 하나의 rect가 다른 rect를 완전히 포함하거나, 같은경우에만 유효), rotate될수 없다.
{
	UINT32	idHandle;			// 핸들, UIImage에서는 이 값을 통해 자신의 원본 정보를 알아낼수 있으며, 자신이 사라지는 경우 이값을 통해 merged texture의 정보를 제거할수 있다.

	I3UI_RESSRC_INFO	source;	// 원본 resource의 정보.
	i3::pack::RECT				uvwh;	// merged texture내에서의 texcoord position.width, height.

	void	Init(void)
	{
		idHandle = 0;
		source.Init();
		uvwh.left = uvwh.right = uvwh.top = uvwh.bottom = 0.0f;
	}
}I3UI_VTRECT_INFO;

#define UI_MERGEDTEXTURE_SIZE	4096

// merged texture의 동작
// 원본 resource의 정보를 토대로, merged texture내부에 해당 texture 영역을 삽입한다.
// - 이미 merge되어 있는 rect 중에서 삽입 요청된 정보와 동일한 texture를 사용하는 것들 중에, 서로 포함하는영역이 있는지 확인한다.
//	+- 서로 포함관계를 확인하여, 배치 영역 설정 및 parent handle relink 
// - merge texture에 배치한다.
//  +- todo: rectangle placement 알고리즘 파악.
// - 삭제 요청이 들어오면, 자신이 다른영역에 속한경우 그냥 삭제하되, refCount를 확인하여 자신에 포함된 rect가 있으면 부모에 연결하도록 한다. (완전히 포함하거나 같은경우에만 유효하도록 하였으므로, 유효.)

class CUIMergedTexture : public i3NamedElement
{
	I3_CLASS_DEFINE( CUIMergedTexture, i3NamedElement);

protected:
	List		*	m_plistRect;
	i3Texture	*	m_pMergedTexture;

public:
	CUIMergedTexture(void);
	virtual ~CUIMergedTexture(void);

	void	Create(void);									// 기본 size의 merged texture를 생성한다. 4096 * 4096, RGBA 32bit

public:
	UINT32	AddRect( I3UI_RESSRC_INFO * pInfo);				// 원본 이미지 정보를 토대로 영역 설정후 handle을 반환 한다.
	bool	RemoveRect( UINT32 handle);						// 해당하는 handle을 가진 rect를 제거한다.

	INT32	getRectInfoCount(void)	{	return m_plistRect->GetCount();}

	i3Texture	*	getMergedTexture(void)	{	return m_pMergedTexture;}

	I3UI_VTRECT_INFO * GetRectInfoByIndex( UINT32 handle);	// rect info를 list에 들어있는 순서에 의해 반환
	I3UI_VTRECT_INFO * GetRectInfo( UINT32 handle);			// rect info를 handle의 index로 반환.

	bool	VerifyHandle(void);								// 겹치는 핸들을 사용하고 있는지를 검증.

	///
	I3UI_VTRECT_INFO *  _FindRectByHandle( UINT32 handle, List * plist);
	UINT32				_FindRectByPath( char * pszFilePath, List * plist, List * pRectList);
	UINT32				_FindContainerRectByRect( i3::pack::RECT * pRect, List * poList, List * pInSrcList);
	UINT32				_FindIncludedRectByRect( i3::pack::RECT * pRect, List * pIncludedList, List * pInSrcList);
	UINT32				_AddRectToContainer( I3UI_RESSRC_INFO * pInfo, I3UI_VTRECT_INFO * pContainer);
	UINT32				_AddNewRect( I3UI_RESSRC_INFO * pInfo);
	UINT32				_AddContainerRect( I3UI_RESSRC_INFO * pInfo, List * plistIncluded);

public:
	virtual void	OnBuildObjectList( List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};
*/
#endif		//_CUI_MERGED_TEXTURE_H__