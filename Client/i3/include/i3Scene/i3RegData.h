#ifndef _I3REG_DATA_H__
#define _I3REG_DATA_H__

#define	I3REG_STYLE_ROOT	0x00010000

#define	I3REG_TYPE_MASK		0x000000FF
#define I3REG_TYPE_INT32	0x00000000
#define I3REG_TYPE_REAL32	0x00000001
#define I3REG_TYPE_STRING	0x00000002
#define I3REG_TYPE_VEC2D	0x00000003
#define I3REG_TYPE_VEC3D	0x00000004
#define I3REG_TYPE_VEC4D	0x00000005
#define I3REG_TYPE_COLOR	0x00000006
#define I3REG_TYPE_MATRIX	0x00000007
#define I3REG_TYPE_INT32EX	0x00000008
#define I3REG_TYPE_ARRAY	0x00000009

//Registry 타입 추가할 경우 I3REG_TYPE_MAX 값을 변경해 주세요.
//설명 : Registry 파일을 Loading 할 경우 특정 나라만 Load 할 경우에 FilePointer 이동에 사용된다.
//만든이 : 이정우(2011년 1월 21일)
#define I3REG_TYPE_MAX	0x0000000A


class I3_EXPORT_SCENE i3RegData : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3RegData, i3NamedElement);
	
protected:
	UINT32		m_nDataType = 0;

public:
	void		GetValue( INT32 * nValue);
	void		GetValue( REAL32 * nValue);
	void		GetValue( i3::rc_wstring& outString);
	void		GetValue( VEC2D * nValue);
	void		GetValue( VEC3D * nValue);
	void		GetValue( VEC4D * nValue);
	void		GetValue( I3COLOR * pCol);
	void		GetValue( MATRIX * pMtx);

	virtual void		GetValueByString(i3::wstring& out) = 0;
public:
	UINT32		getDataType()	{	return m_nDataType;}

	virtual	void	OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif // _I3REG_DATA_H__