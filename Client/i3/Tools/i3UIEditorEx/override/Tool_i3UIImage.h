#ifndef __I3UIE_OVERRIDE_FOR_TOOL_H_
#define __I3UIE_OVERRIDE_FOR_TOOL_H_
/*
class FT_i3UIImage: public i3UIImage
{
	I3_CLASS_DEFINE( FT_i3UIImage );

protected:
	char			m_Path[MAX_PATH];
	COLOR			m_ColorKey;

public:
	FT_i3UIImage();
	virtual ~FT_i3UIImage();

	void			setResPath( char * pszPath)	{	i3String::NCopy( m_Path, pszPath, MAX_PATH-1);}
	void			setColorKey( COLOR * Color)	{	i3Color::Copy( &m_ColorKey, Color);}

	virtual void	GetTDKGridProperties(i3List * pList);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML);
};
*/
#endif //__I3UI_DEFINES_H__
