#pragma once

// 신규 uil포맷에 의해 생성해야될 'i3UILibHdr'확장자 포맷 저장용 클래스로..
// i3UILibrary의 생성함수에 의해 생성되면 됨...
// 로딩 혹은 저장시에 쓰인뒤, i3UILibrary에 셋팅이 끝나면 바로 버린다.

// 익스포트 대상이 아니므로, I3_EXPORT_CLASS_DEFINE을 쓰지 않는다.

class I3_EXPORT_FRAMEWORK i3UILibHdr : public i3NamedElement
{
	I3_CLASS_DEFINE(i3UILibHdr, i3NamedElement);
public:
	void			SetData(UINT32 nVersion, const i3::rc_string& strVTexPath, const i3::rc_string& strScript);

	UINT32				 GetVersion() const { return m_nVersion; }
	const i3::rc_string& GetVTexPath() const { return m_strVTexPath; }
	const i3::rc_string& GetScriptPath() const { return m_strScript; }

	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;

	virtual bool	OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;

private:
	UINT32					m_nVersion = 2;
	i3::rc_string			m_strVTexPath;
	i3::rc_string			m_strScript;

};
