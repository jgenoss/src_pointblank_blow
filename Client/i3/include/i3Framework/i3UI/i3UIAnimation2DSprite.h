#pragma once


class i3UIShape;

struct I3_UVINFO
{
	char	_ResName[MAX_PATH] = { 0 };
	RECT	_Rect = { 0, 0, 0, 0 };
} ;

class I3_EXPORT_FRAMEWORK i3UIAnimation2DSprite : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE(i3UIAnimation2DSprite, i3NamedElement);


	i3XMLElement*	newXMLElement(const char* pszName, i3XMLElement * pXML);
	i3XMLElement*	GetFromXML(const char * pszName, i3XMLElement * pXML);

	REAL32			m_fLifeTime = 0.0f;
	I3_UVINFO*		m_pUVInfo = nullptr;
	INT32			m_nSpriteCount = 0;
	REAL32			m_fTimeDelta = 0.0f;
	INT32			m_nCurSpriteIndex = 0;
	i3::pack::RECT		m_TexCoord = { 0.0f, 0.0f, 0.0f, 0.0f };
	i3::pack::POINT2D	m_VTexOffset = { 0, 0 };
	bool			m_bRoof = false;
	bool			m_bDisable = false;
public:
	virtual ~i3UIAnimation2DSprite();

	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual void	CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method) override;

	void			SetUVInfo(INT32 nIndex, i3::string resName, RECT rt);
	void			CreateSprite(INT32 nCount);
	void			Drive(REAL32 rDeltaSeconds);
	void			Init(i3UIShape* pShape, i3UIAnimation2DSprite* p2dNode);
	void			Reset();

	REAL32			GetLifeTime() { return m_fLifeTime; }
	void			SetLifeTime(REAL32 fTime) { m_fLifeTime = fTime; }
	RECT			GetUVPos(INT32 nIndex) { return m_pUVInfo[nIndex]._Rect; }
	i3::string		GetUVRes(INT32 nIndex) { return m_pUVInfo[nIndex]._ResName; }
	INT32			GetSpriteCount() {return m_nSpriteCount;}
	void			SetSpriteCount(INT32 nCount) { m_nSpriteCount = nCount; }

	i3::pack::RECT			GetTexCood() {return m_TexCoord;}
	i3::pack::POINT2D			GetVTexOffset() { return m_VTexOffset; }
	void			SetTexCood(i3::pack::RECT rt) { m_TexCoord = rt; }
	void			SetVTexOffset(i3::pack::POINT2D pt) { m_VTexOffset = pt; }
	void			SetRoof(bool bState) { m_bRoof = bState; }
};

