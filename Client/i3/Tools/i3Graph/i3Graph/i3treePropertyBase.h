#pragma once


///////////////////////////////////////////////////////////////////////////
//For Style 
#define I3_READONLY			0x00000001
#define I3_INVISIBLE		0x00000002
#define I3_USER_DRAW		0x00000004

//For Select 
#define I3_LIST				0x00000010			
#define I3_EDIT				0x00000020			
#define I3_DIALOG			0x00000040
#define I3_BIG_DIALOG		0x00000080

///////////////////////////////////////////////////////////////////////////////////////////
/////// Base Proeprty
class CpropertyBase 
{
//For Value
private:	
	char	m_szName[64]; 	
public:

protected:
	INT32	m_Style; 
//For Function
private:	

public:
	CpropertyBase(); 
	~CpropertyBase();

	const char *	GetName(void)										{ return m_szName; }
	void			SetName( const char * pszName )
	{
		I3ASSERT( strlen( pszName ) < sizeof(m_szName) );
		strcpy( m_szName, pszName);
	}


	void			SetStyle(INT32 Style)								{ m_Style = Style; }
	INT32			GetStyle(void)										{ return m_Style; }	
	void			AddStyle(INT32 Style) 								{ m_Style = m_Style | Style;	}
	void			DelStyle(INT32 Style)						 		{ m_Style = m_Style & ~(Style );}

	//For ListBox
	virtual void OnChangeList(void)										{ ; }		
	virtual void GetValueList(i3StringList * ListBoxList)				{ ; }		

	//For EditBox
	virtual void Get( char * pszValueStr)								{ ; }
	virtual void Set( const char * pszValueStr)							{ ; }

	//For User Draw 
	virtual void UserDraw(CPaintDC *pDc, RECT *rect)					{ ; }

	//For DialogBox 
	virtual void PopUpDialog(void)										{ ; }
}; 

///////////////////////////////////////////////////////////////////////////////////////////
/////// INT Property 
class CpropertyInt : public CpropertyBase
{
//For Value
private:	
	INT32	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyInt(); 
	~CpropertyInt(); 
	INT32			GetValue(void)										{ return m_value;  }
	void			SetValue(INT32 value)								{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
}; 
typedef class CpropertyInt		I3_P_INT;

///////////////////////////////////////////////////////////////////////////////////////////
/////// BOOL Property 
class CpropertyBool : public CpropertyBase
{
//For Value
private:	
	BOOL	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyBool(); 
	~CpropertyBool(); 
	BOOL			GetValue(void)										{ return m_value;  }
	void			SetValue(BOOL value)								{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyBool		I3_P_BOOL;

///////////////////////////////////////////////////////////////////////////////////////////
/////// REAL Property 
class CpropertyReal32 : public CpropertyBase
{
//For Value
private:	
	REAL32	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyReal32(); 
	~CpropertyReal32(); 

	REAL32			GetValue(void)										{ return m_value;  }
	void			SetValue(REAL32 value)								{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set(const char * pszValueStr); 
}; 
typedef class CpropertyReal32		I3_P_REAL32;

///////////////////////////////////////////////////////////////////////////////////////////
/////// COLOR Property 
class CpropertyColor : public CpropertyBase
{
//For Value
private:	
	COLOR	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyColor(); 
	~CpropertyColor(); 	
	COLOR			GetValue(void)						{ return m_value;  }
	void			SetValue(COLOR value)				{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void UserDraw(CPaintDC *pDc, RECT *rect); 			
	virtual void PopUpDialog(void); 
}; 
typedef class CpropertyColor		I3_P_COLOR;

///////////////////////////////////////////////////////////////////////////////////////////
/////// COLORREAL Property 
class CpropertyColorReal : public CpropertyBase
{
//For Value
private:	
	COLORREAL	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyColorReal(); 
	~CpropertyColorReal(); 	
	COLORREAL		GetValue(void)											{ return m_value;  }
	void			SetValue(COLORREAL value)								{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void UserDraw(CPaintDC *pDc, RECT *rect); 			
	virtual void PopUpDialog(void); 
}; 
typedef class CpropertyColorReal		I3_P_COLORREAL;

///////////////////////////////////////////////////////////////////////////////////////////
class CpropertyVec2d : public CpropertyBase
{
private:
	VEC2D	m_value;
public:
	CpropertyVec2d();
	~CpropertyVec2d();
	VEC2D		GetValue(void)			{return m_value;}
	void		SetValue(VEC2D	 value)	{m_value = value;}

	virtual void Get(char * pszValueStr);
	virtual void Set(const char * pszValueStr);
};
typedef class CpropertyVec2d I3_P_VEC2D;
///////////////////////////////////////////////////////////////////////////////////////////
/////// VEC Property 
class CpropertyVec3d : public CpropertyBase
{
//For Value
private:	
	VEC3D	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyVec3d(); 
	~CpropertyVec3d(); 
	VEC3D			GetValue(void)										{ return m_value;  }
	void			SetValue(VEC3D value)								{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

}; 
typedef class CpropertyVec3d		I3_P_VEC3D;

///////////////////////////////////////////////////////////////////////////////////////////
///////VEC4D Property 
class CpropertyVec4d : public CpropertyBase
{
//For Value
private:	
	VEC4D	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyVec4d(); 
	~CpropertyVec4d(); 
	VEC4D			GetValue(void)										{ return m_value;  }
	void			SetValue(VEC4D value)								{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
}; 
typedef class CpropertyVec4d		I3_P_VEC4D;

///////////////////////////////////////////////////////////////////////////////////////////
/////// MATRIX Property 
class CpropertyMatrix : public CpropertyBase
{
//For Value
private:	
	MATRIX	m_value; 
public:

//For Function
private:	
	
public:
	CpropertyMatrix(); 
	~CpropertyMatrix(); 

	MATRIX			GetValue(void)										{ return m_value;  }
	void			SetValue(MATRIX * value)							{ m_value = *value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

}; 
typedef class CpropertyMatrix		I3_P_MATRIX;

///////////////////////////////////////////////////////////////////////////////////////////
/////// STRING Property 
class CpropertyString : public CpropertyBase
{
//For Value
private:	
	char	m_value[256]; 
public:

//For Function
private:	
	
public:
	CpropertyString(); 
	~CpropertyString(); 
	char *			GetValue(void)										{ return m_value;  }
	void			SetValue(const char * value)						{ strcpy(m_value, value); }
	virtual		void Get( char * pszValueStr)							{ strcpy(pszValueStr,m_value); } 
	virtual		void Set( const char * pszValueStr)						{ strcpy(m_value, pszValueStr); }

}; 
typedef class CpropertyString		I3_P_STRING;



///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_COMP_FUNC Property 
class CpropertyCompFunc : public CpropertyBase
{
//For Value
private:	
	I3G_COMP_FUNC	m_value;
public:

//For Function
private:	
	
public:
	CpropertyCompFunc(); 
	~CpropertyCompFunc(); 
	I3G_COMP_FUNC	GetValue(void)										{ return m_value;  }
	void			SetValue(I3G_COMP_FUNC value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyCompFunc		I3_P_COMPFUNC;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_BLEND Property 
class CpropertyBlend : public CpropertyBase
{
//For Value
private:	
	I3G_BLEND	m_value;
public:

//For Function
private:	
	
public:
	CpropertyBlend(); 
	~CpropertyBlend(); 
	I3G_BLEND	GetValue(void)										{ return m_value;  }
	void			SetValue(I3G_BLEND value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyBlend		I3_P_BLEND;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_FACE_CULL Property 
class CpropertyFaceCull : public CpropertyBase
{
//For Value
private:	
	I3G_FACE_CULL	m_value;
public:

//For Function
private:	
	
public:
	CpropertyFaceCull(); 
	~CpropertyFaceCull(); 
	I3G_FACE_CULL	GetValue(void)										{ return m_value;  }
	void			SetValue(I3G_FACE_CULL value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyFaceCull		I3_P_FACECULL;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_FOG_MODE Property 
class CpropertyFogMode : public CpropertyBase
{
//For Value
private:	
	I3G_FOG_MODE	m_value;
public:

//For Function
private:	
	
public:
	CpropertyFogMode(); 
	~CpropertyFogMode(); 
	I3G_FOG_MODE	GetValue(void)										{ return m_value;  }
	void			SetValue(I3G_FOG_MODE value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyFogMode		I3_P_FOGMODE;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_LIGHT_TYPE Property 
class CpropertyLightType : public CpropertyBase
{
//For Value
private:	
	I3G_LIGHT_TYPE	m_value;
public:

//For Function
private:	
	
public:
	CpropertyLightType(); 
	~CpropertyLightType(); 
	I3G_LIGHT_TYPE	GetValue(void)										{ return m_value;  }
	void			SetValue(I3G_LIGHT_TYPE value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyLightType		I3_P_LIGHTTYPE;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_FILTER Property 
class CpropertyTextureFilter : public CpropertyBase
{
//For Value
private:	
	I3G_TEXTURE_FILTER	m_value;
public:

//For Function
private:	
	
public:
	CpropertyTextureFilter(); 
	~CpropertyTextureFilter(); 
	I3G_TEXTURE_FILTER	GetValue(void)										{ return m_value;  }
	void				SetValue(I3G_TEXTURE_FILTER value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyTextureFilter		I3_P_TEXTUREFILTER;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_WRAP Property 
class CpropertyTextureWarp : public CpropertyBase
{
//For Value
private:	
	I3G_TEXTURE_WRAP	m_value;
public:

//For Function
private:	
	
public:
	CpropertyTextureWarp(); 
	~CpropertyTextureWarp(); 
	I3G_TEXTURE_WRAP	GetValue(void)										{ return m_value;  }
	void				SetValue(I3G_TEXTURE_WRAP value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyTextureWarp		I3_P_TEXTUREWRAP;


///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_SHADE_MODE Property 
class CpropertyShadeMode : public CpropertyBase
{
//For Value
private:	
	I3G_SHADE_MODE	m_value;
public:

//For Function
private:	
	
public:
	CpropertyShadeMode(); 
	~CpropertyShadeMode(); 
	I3G_SHADE_MODE	GetValue(void)											{ return m_value;  }
	void				SetValue(I3G_SHADE_MODE value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyShadeMode		I3_P_SHADEMODE;


///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_RENDER_MODE Property 
class CpropertyRenderMode : public CpropertyBase
{
//For Value
private:	
	I3G_RENDER_MODE	m_value;
public:

//For Function
private:	
	
public:
	CpropertyRenderMode(); 
	~CpropertyRenderMode(); 
	I3G_RENDER_MODE	GetValue(void)											{ return m_value;  }
	void				SetValue(I3G_RENDER_MODE value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyRenderMode		I3_P_RENDERMODE;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_IMAGE_FORMAT Property 
class CpropertyImageFormat : public CpropertyBase
{
//For Value
private:	
	I3G_IMAGE_FORMAT	m_value;
public:

//For Function
private:	
	
public:
	CpropertyImageFormat(); 
	~CpropertyImageFormat(); 
	I3G_IMAGE_FORMAT	GetValue(void)											{ return m_value;  }
	void				SetValue(I3G_IMAGE_FORMAT value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyImageFormat		I3_P_IMAGEFORMAT;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_IMAGE Property 
class CpropertyImage : public CpropertyBase
{
private:	
	i3Texture *			m_pTexture; 

private:	
	
public:
	CpropertyImage(); 
	~CpropertyImage(); 
	void SetTexture( i3Texture *pTex )
	{
		m_pTexture = pTex;
	}

	i3Texture *GetTexture(){ return m_pTexture;	}

	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 		
	virtual void PopUpDialog(void); 
}; 
typedef class CpropertyImage		I3_P_IMAGE;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_Geometry Property 
class CpropertyGeometry : public CpropertyBase
{
//For Value
private:	

public:
	i3GeometryAttr * m_pGeometry; 
//For Function
private:	
	
public:
	CpropertyGeometry(); 
	~CpropertyGeometry(); 
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 		
	virtual void PopUpDialog(void); 
}; 
typedef class CpropertyGeometry		I3_P_GEOMETRY;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_PRIMITIVE Property 
class CpropertyPrimitive : public CpropertyBase
{
//For Value
private:	
	I3G_PRIMITIVE	m_value;
public:

//For Function
private:	
	
public:
	CpropertyPrimitive(); 
	~CpropertyPrimitive(); 
	I3G_PRIMITIVE		GetValue(void)										{ return m_value;  }
	void				SetValue(I3G_PRIMITIVE value)						{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyPrimitive		I3_P_PRIMITIVE;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_COORD_SOURCE Property 
class CpropertyTextureCS : public CpropertyBase
{
//For Value
private:	
	I3G_TEXTURE_COORD_SOURCE	m_value;
public:

//For Function
private:	
	
public:
	CpropertyTextureCS(); 
	~CpropertyTextureCS(); 
	I3G_TEXTURE_COORD_SOURCE	GetValue(void)									{ return m_value;  }
	void						SetValue(I3G_TEXTURE_COORD_SOURCE value)		{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyTextureCS		I3_P_TEXTURE_COORDSOURCE;

///////////////////////////////////////////////////////////////////////////////////////////
///////I3G_TEXTURE_FUNCTION Property 
class CpropertyTextureFunc : public CpropertyBase
{
//For Value
private:	
	I3G_TEXTURE_FUNCTION	m_value;
public:

//For Function
private:	
	
public:
	CpropertyTextureFunc(); 
	~CpropertyTextureFunc(); 
	I3G_TEXTURE_FUNCTION	GetValue(void)								{ return m_value;  }
	void					SetValue(I3G_TEXTURE_FUNCTION value)		{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 

	virtual void GetValueList(i3StringList * plist); 
	virtual void OnChangeList(void); 								

}; 
typedef class CpropertyTextureFunc		I3_P_TEXTURE_FUNCTION;


////////////////////////////////////////////////////////////////////////////////////////////
//I3G_STENCIL_OP Property
class CpropertyStencilOP : public CpropertyBase
{
private:
	I3G_STENCIL_OP	m_value;
public:
	CpropertyStencilOP();
	~CpropertyStencilOP();

	I3G_STENCIL_OP	GetValue(void)						{return m_value;}
	void			SetValue(I3G_STENCIL_OP value)		{m_value = value;}

	virtual void Get(char * pszValueStr);
	virtual void Set(const char * pszValueStr);

	virtual void GetValueList(i3StringList * plist);
	virtual void OnChangeList(void);
};

typedef class CpropertyStencilOP	I3_P_STENCIL_OP;


////////////////////////////////////////////////////////////////////////////////
///I3_P_SET_NODEFLAG
class CPropertySetNodeFlag	:	public	CpropertyBase
{
//For Value
private:	
	INT32	m_value;
public:

public:
	CPropertySetNodeFlag(); 
	~CPropertySetNodeFlag(); 
	INT32			GetValue(void)				{ return m_value;  }
	void			SetValue(INT32 value)		{ m_value = value; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 
	virtual void PopUpDialog(void); 								
}; 

typedef	class	CPropertySetNodeFlag	I3_P_SET_NODEFLAG;

/////////////////////////////////////////////////////////////////////////////////
///I3_P_BILLBOARDINFO
class CPropertyBillboardSpace	: public CpropertyBase
{
private:
	INT32	m_value;
public:
	CPropertyBillboardSpace();
	~CPropertyBillboardSpace();
public:
	INT32		GetValue(void)			{return m_value;}
	void		SetValue(INT32 value)	{m_value = value;}

	virtual void Get(char * pszValueStr);
	virtual void Set(const char * pszValueStr);

	virtual void GetValueList(i3StringList * plist);
	virtual void OnChangeList(void);
};

typedef class CPropertyBillboardSpace	I3_P_BILLBOARD_SPACE;

/*
//////////////////////////////////////////////////////////////////////////////////
/// I3_P_BILLBOARD_EDITMASK
class CPropertyBillboard_EditMask	:public	CpropertyBase
{
private:
	INT32	m_value;
public:
	CPropertyBillboard_EditMask();
	~CPropertyBillboard_EditMask();
public:
	INT32	GetValue(void)		{return m_value;}
	void	SetValue(INT32 value)	{m_value = value;}

	virtual void Get(char * pszValueStr);
	virtual void Set(const char * pszValueStr);

	virtual void GetValueList(i3StringList * plist);
	virtual void OnChangeList(void);
};

typedef class CPropertyBillboard_EditMask	 I3_P_BILLBOARD_EDITMASK;
//*/

///////////////////////////////////////////////////////////////////////////////////////////
///////I3_P_COLLIDERSTYLE Property 
class CpropertyColliderStyle : public CpropertyBase
{
private:	

private:	
	i3ColliderSet * m_pColliderSet; 
public:
	CpropertyColliderStyle(); 
	~CpropertyColliderStyle(); 

	void	SetCollider(i3ColliderSet * pColliderSet) {m_pColliderSet = pColliderSet; }
	virtual void Get( char * pszValueStr); 
	virtual void Set( const char * pszValueStr); 		
	virtual void PopUpDialog(void); 
}; 
typedef class CpropertyColliderStyle		I3_P_COLLIDERSTYLE;


////////////////////////////////////////////////////////////////////////////////////////////
//I3_P_TERRAIN_TYPE Property
class CpropertyTerrainType : public CpropertyBase
{
private:
	INT32 m_value;
public:
	CpropertyTerrainType();
	~CpropertyTerrainType();

	INT32			GetValue(void)				{return m_value;}
	void			SetValue(INT32 value)		{m_value = value;}

	virtual void Get(char * pszValueStr);
	virtual void Set(const char * pszValueStr);

	virtual void GetValueList(i3StringList * plist);
	virtual void OnChangeList(void);
};
typedef class CpropertyTerrainType	I3_P_TERRAIN_TYPE;

