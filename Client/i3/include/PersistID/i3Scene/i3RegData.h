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

class I3_EXPORT_SCENE i3RegData : public i3NamedElement
{
	I3_CLASS_DEFINE( i3RegData);
	
protected:
	UINT32		m_nDataType;

public:
	void		GetValue( INT32 * nValue);
	void		GetValue( REAL32 * nValue);
	void		GetValue( char * pszString);
	void		GetValue( VEC2D * nValue);
	void		GetValue( VEC3D * nValue);
	void		GetValue( VEC4D * nValue);
	void		GetValue( I3COLOR * pCol);
	void		GetValue( MATRIX * pMtx);
public:
	UINT32		getDataType()	{	return m_nDataType;}

	i3RegData();
	virtual ~i3RegData();
	virtual	void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif // _I3REG_DATA_H__