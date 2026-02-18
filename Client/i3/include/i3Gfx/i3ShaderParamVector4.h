#if !defined( __I3G_SHADER_PARAM_VECTOR4_H)
#define __I3G_SHADER_PARAM_VECTOR4_H

#include "i3ShaderParam.h"

class I3_EXPORT_GFX i3ShaderParamVector4 : public i3ShaderParam
{
	I3_EXPORT_CLASS_DEFINE( i3ShaderParamVector4, i3ShaderParam);
protected:
	VEC4D *			m_pVector = nullptr;

public:
	i3ShaderParamVector4(void);
	virtual ~i3ShaderParamVector4(void);

	bool			Create(INT32 nCount);

	VEC4D *			getVector(void)					{ return m_pVector;					}
	void			setVector( VEC4D * pVec)		{ i3Vector::Copy( m_pVector, pVec); }
		
	void			setParamX(REAL32 val)			{ i3Vector::SetX( m_pVector, val);	}
	void			setParamY(REAL32 val)			{ i3Vector::SetY( m_pVector, val);	}
	void			setParamZ(REAL32 val)			{ i3Vector::SetZ( m_pVector, val);	}
	void			setParamW(REAL32 val)			{ i3Vector::SetW( m_pVector, val);	}

	void			setParamR(REAL32 val)			{ i3Vector::SetX( m_pVector, val);	}
	void			setParamG(REAL32 val)			{ i3Vector::SetY( m_pVector, val);	}
	void			setParamB(REAL32 val)			{ i3Vector::SetZ( m_pVector, val);	}
	void			setParamA(REAL32 val)			{ i3Vector::SetW( m_pVector, val);	}

	virtual void	Update( i3RenderContext * pCtx) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
