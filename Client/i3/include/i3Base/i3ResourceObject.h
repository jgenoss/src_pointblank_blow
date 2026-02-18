#if !defined( __I3_RESOURCE_OBJECT_H)
#define __I3_RESOURCE_OBJECT_H

#include "i3CommonType.h"
#include "i3NamedElement.h"
#include "i3ResourceFile.h"

typedef UINT8	I3_RES_STYLE;
#define		I3_RES_STYLE_COMMON			0x01		// Common Resource는 처음부터 로드한다.	// stage 전환시에도 해제되지 않는다.
#define		I3_RES_STYLE_EXTERNAL		0x02		// 외부 리소스 참조하는 리소스 오브젝트
#define		I3_RES_STYLE_SUBCOMMON		0x04		// Sub common res는 중간에 로드한다. // stage 전환시에도 해제되지 않는다.
#define		I3_RES_STYLE_PREPARED		0x08		// 게임에서 재가공된 경우 이 style을 갖는다. (macro texture로 texture를 변경한 경우)

class I3_EXPORT_BASE i3ResourceObject : public i3NamedElement
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3ResourceObject, i3NamedElement);
protected:
	UINT128			m_Checksum = { 0, 0 };
	I3_RES_STYLE	m_ResStyle = 0;

protected:
	UINT32		_CalcChecksum(void);

public:
	UINT128		getChecksum(void)	const			{ return m_Checksum; }
	UINT64 		getChecksum( INT32 idx)	const		{ return m_Checksum[idx]; }

	I3_RES_STYLE	getResStyle( void) const			{ return m_ResStyle; }
	void		setResStyle( I3_RES_STYLE style)		{ m_ResStyle = style; }
	void		addResStyle( I3_RES_STYLE style)		{ m_ResStyle |= style; }
	void		removeResStyle( I3_RES_STYLE style)		{ m_ResStyle &= ~style; }

	bool		isResStyle( I3_RES_STYLE mask) const{ return (m_ResStyle & mask) == mask; }
	bool		isCommonRes(void) const				{ return (getResStyle() & (I3_RES_STYLE_COMMON|I3_RES_STYLE_SUBCOMMON)) != 0; }
	bool		isExternal(void) const				{ return isResStyle( I3_RES_STYLE_EXTERNAL); }

	void		setCommonRes( bool bState)
	{
		if( bState)
			m_ResStyle |= I3_RES_STYLE_COMMON;
		else
			m_ResStyle &= ~I3_RES_STYLE_COMMON;
	}
	void		setSubCommonRes( bool bState)
	{
		if( bState)	m_ResStyle |= I3_RES_STYLE_SUBCOMMON;
		else		m_ResStyle &= ~I3_RES_STYLE_SUBCOMMON;
	}

	void		SetResourcePath( const char * pszPath);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

#if defined( I3_DEBUG)
public:
	UINT32		m_FileSize = 0;
#endif

};

#endif
