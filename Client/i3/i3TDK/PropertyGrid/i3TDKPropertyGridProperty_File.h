#ifndef		__I3_TDK_PROPERTY_GRID_PROPERTY_FILE_H__
#define		__I3_TDK_PROPERTY_GRID_PROPERTY_FILE_H__

#if defined( I3_WINDOWS)

class I3_EXPORT_TDK i3TDKPropertyGridProperty_File : public CMFCPropertyGridFileProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_File)
private:
	char*		m_pData;
	INT32		m_nBufSize;

protected:
	
public:
	i3TDKPropertyGridProperty_File( PROPERTYINFO* pInfo);
	
	virtual void OnClickButton( CPoint point);

	virtual void	UpdateFromProperty( void* pData = nullptr);
	virtual void	UpdateToProperty( void);
};

class I3_EXPORT_TDK i3TDKPropertyGridProperty_File_RcString : public CMFCPropertyGridFileProperty
{
	DECLARE_DYNAMIC( i3TDKPropertyGridProperty_File_RcString)
private:
	i3::rc_string*	m_pData;
protected:

public:
	i3TDKPropertyGridProperty_File_RcString( PROPERTYINFO* pInfo);

	virtual void	OnClickButton( CPoint point);
	virtual void	UpdateFromProperty( void* pData = nullptr);
	virtual void	UpdateToProperty( void);
};



#endif

#endif