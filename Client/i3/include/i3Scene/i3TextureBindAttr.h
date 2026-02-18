#if !defined( __I3_TEXTURE_BIND_ATTR_H)
#define __I3_TEXTURE_BIND_ATTR_H

#include "i3RenderAttr.h"
#include "i3Gfx.h"

class I3_EXPORT_SCENE i3TextureBindAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextureBindAttr, i3RenderAttr);

protected:

	// bool 플래그를 위해서, m_BindType을 WORD로 바꾸고 함수에서 전환하도록 한다.
	WORD				m_BindType = static_cast<WORD>(I3G_TEXTURE_BIND_EMISSIVE);
	bool				m_bApplyEnable = true;
	bool				m_bChangeTex = false;

	i3Texture *			m_pTexture = nullptr;
	
	i3::rc_string		m_skipped_tex_filename;			// 올바르게 텍스쳐가 들어오면 리셋해주고, 텍스쳐가 없으면 
														// 파일 이름을 보관해둔다..텍스쳐없는 로딩시에만 쓰면 된다..
public:													// 파생클래스 OnLoad()가 기본클래스쪽 함수 호출을 하지않으므로..
	i3TextureBindAttr(void);							// 모든 파생클래스에 구현이 분배되어야한다..
	virtual ~i3TextureBindAttr(void);

	void				SetBindType( I3G_TEXTURE_BIND bind)	{ m_BindType = static_cast<WORD>(bind);  }

	I3G_TEXTURE_BIND	GetBindType(void)					{ return static_cast<I3G_TEXTURE_BIND>(m_BindType); }

	void				SetTexture( i3Texture * pTexture);
	i3Texture *			GetTexture(void)					{ return m_pTexture; }
	
	const i3::rc_string&	GetSkippedTextureFilename() const { return m_skipped_tex_filename; }
	void					ClearSkippedTextureFilename()   { m_skipped_tex_filename.clear(); }
	
	void				SetApplyEnable(bool bEnable)	{ m_bApplyEnable = bEnable; }
	bool				IsApplyEnable() const			{ return m_bApplyEnable; }

	void				SetTextureChange(bool bChange)	{ m_bChangeTex = bChange; }
	bool				IsTextureChange() const			{ return m_bChangeTex; }

	virtual void	Apply( i3RenderContext * pContext) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual bool	IsSame( i3RenderAttr * pAttr) override;

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void	Dump( void) override;
	#endif
};

#endif
