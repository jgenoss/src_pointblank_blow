#if !defined( __I3_SHADER_GEN_H)
#define __I3_SHADER_GEN_H

#include "i3Shader.h"

enum _SPT_SEMANTIC
{
	_SPT_SEMANTIC_TEX0 = 0,
	_SPT_SEMANTIC_TEX1,
	_SPT_SEMANTIC_TEX2,
	_SPT_SEMANTIC_TEX3,
	_SPT_SEMANTIC_NORMAL,
	_SPT_SEMANTIC_TANGENT,
	_SPT_SEMANTIC_BINORMAL,
	_SPT_SEMANTIC_HALF,
	_SPT_SEMANTIC_LS_POS,
	_SPT_SEMANTIC_CS_POS,
	_SPT_SEMANTIC_REFLECT,
	_SPT_SEMANTIC_SPECULAR,
	_SPT_SEMANTIC_VERTEX_COLOR,
};

enum _SPT_VIA
{
	_SPT_VIA_NA = -1,
	_SPT_VIA_TEXCOORD0 = 0,
	_SPT_VIA_TEXCOORD1,
	_SPT_VIA_TEXCOORD2,
	_SPT_VIA_TEXCOORD3,
	_SPT_VIA_TEXCOORD4,
	_SPT_VIA_TEXCOORD5,
	_SPT_VIA_TEXCOORD6,
	_SPT_VIA_TEXCOORD7,
	_SPT_VIA_COLOR,
};

typedef struct _tagSPTInfo
{
	char			_name[32];
	_SPT_SEMANTIC	_semantic;
	_SPT_VIA		_via;
} SPT_INFO;

#define MAX_SPT		12

class I3_EXPORT_GFX i3ShaderGen : public i3ElementBase
{
	I3_CLASS_DEFINE( i3ShaderGen);
protected:
	SPT_INFO	m_SPT[ MAX_SPT];
	INT32		m_SPCount;

	char *		m_pszWorkDir;
	
protected:
	void	_addParam( char * pszName, _SPT_SEMANTIC semantic, _SPT_VIA via = _SPT_VIA_NA)
	{
		I3ASSERT( m_SPCount < (MAX_SPT - 1));

		i3String::Copy( m_SPT[ m_SPCount]._name, pszName);
		m_SPT[ m_SPCount]._semantic = semantic;

		if( via == _SPT_VIA_NA)
		{
			m_SPT[ m_SPCount]._via = (_SPT_VIA) m_SPCount;
		}
		else
		{
			m_SPT[ m_SPCount]._via = via;
		}

		m_SPCount++;
	}

	void	_resetParams(void)
	{
		m_SPCount = 0;
	}

public:
	i3ShaderGen(void);
	virtual ~i3ShaderGen(void);

	void	setWorkDir( char * pszPath)			{ m_pszWorkDir = pszPath; }

	bool	Generate( i3Shader * pShader, i3Stream * pStream = NULL);
};

#endif
