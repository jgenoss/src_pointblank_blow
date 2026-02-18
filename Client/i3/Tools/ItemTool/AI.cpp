#include "stdafx.h"
#include "AI.h"
#include "GICDiagramAI.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/compare/generic_is_equal.h"

static char s_headerHeadTemplate[] = 
	"#if !defined( __%AINAME%_AIDRIVER_H)\n" \
	"#define __%AINAME%_AIDRIVER_H\n" \
	"\n" \
	"#include \"i3Framework.h\"\n" \
	"\n";

static char s_headerTailTemplate[] =
	"class AIDriver%AINAME% : public i3AIDriver\n" \
	"{\n" \
	"	I3_CLASS_DEFINE( AIDriver%AINAME%);\n" \
	"protected:\n" \
	"public:\n" \
	"	AIDriver%AINAME%(void);\n" \
	"	virtual ~AIDriver%AINAME%(void);\n" \
	"};\n" \
	"\n" \
	"#endif\n";

static char s_srcHeadTemplate[] =
	"#include \"stdafx.h\"\n" \
	"#include \"AIDriver%AINAME%.h\"\n" \
	"\n" \
	"I3_CLASS_INSTANCE( AIDriver%AINAME%, i3AIDriver);\n" \
	"\n";

static char s_srcTailTemplate[] =
	"AIDriver%AINAME%::AIDriver%AINAME%(void)\n" \
	"{\n" \
	"	setAIProcTable( s_%AINAME%Table, (sizeof(s_%AINAME%Table)/sizeof(s_%AINAME%Table[0])) );\n" \
	"}\n" \
	"\n"
	"AIDriver%AINAME%::~AIDriver%AINAME%(void)\n" \
	"{\n" \
	"}\n";


I3_CLASS_INSTANCE( AI); //, i3ResourceObject);

AI::AI(void)
{
	m_pDiagram = GICDiagramAI::new_object();
	m_pDiagram->setAI( this);

	m_pCtx = NULL;

	m_szDriverName[0] = 0;
}

AI::~AI(void)
{
	I3_SAFE_RELEASE( m_pCtx);
	I3_SAFE_RELEASE( m_pDiagram);
}

void AI::SetAI( i3AI * pAI)
{
	if( m_pCtx == NULL)
	{
		m_pCtx = i3AIContext::new_object();
	}

	m_pCtx->SetAI( pAI);
}

void AI::OnSelect( bool bFlag)
{
	i3GameNode * pTemp = m_pCtx;

	while((pTemp != NULL))
	{
		pTemp->SetEnable( bFlag);

		pTemp = pTemp->getParent();
	}
}

void AI::GetFullAIPath( char * pszBuf)
{
	i3::vector<i3GameNode*> List;
	i3GameNode * pCur = m_pCtx;

	do
	{
		List.push_back( pCur);

		pCur = pCur->getParent();

	} while( i3::kind_of<i3AIContext*>(pCur)); //->IsTypeOf( i3AIContext::GetClassMeta()));

	I3ASSERT( i3::kind_of<i3Chara*>(pCur)); //->IsTypeOf( i3Chara::GetClassMeta()));
	List.push_back( pCur);

	pszBuf[0] = 0;
	
	for(INT32 i = (INT32)List.size() - 1; i >= 0; i--)
	{
		pCur = List[i];

		if( pCur->hasName())
		{
			strcat( pszBuf, pCur->GetName());
		}

		if( i > 0)
		{
			strcat( pszBuf, "_");
		}
	}

}

static char s_szBuf[0xFFFF];

static I3_SYMBOLTABLE_INFO	s_Symbol[] =
{
	{	"AINAME",		"" },
};

void AI::ExportC( const char * pszOnlyPath)
{
	INT32 i;
	char szFull[MAX_PATH], szName[MAX_PATH];

	if( getAI()->getAIStateCount() <= 0)
		return;

	if( m_szDriverName[0] == 0)
	{
		GetFullAIPath( szName);
		
	}
	else
	{
		strcpy( szName, getDriverName());
	}

	sprintf( s_Symbol[0].m_szValue, szName);

	{
		sprintf( szFull, "AIDriver%s", szName);

		getAI()->setDriverMetaName( szFull);
	}

	// Header
	{
		i3FileStream strm;

		sprintf( szFull, "%s\\AIDriver%s.h", pszOnlyPath, szName);

		if( strm.Create( szFull, STREAM_WRITE) == FALSE)
		{
			I3WARN( "%s 파일을 생성할 수 없습니다.", szFull);
			return;
		}

		i3String::PreprocessMacro( s_szBuf, sizeof(s_szBuf) - 1, s_headerHeadTemplate, s_Symbol, 1);
		strm.WriteLine( s_szBuf);

		for( i = 0; i < getAI()->getAIStateCount(); i++)
		{
			i3AIState * pState = getAI()->getAIState( i);

			sprintf( szFull, "AI_%s_%s", szName, pState->GetName());
			i3::to_upper( szFull);
			strm.Printf( s_szBuf, "#define %s	0x%08X\n", szFull, i);
		}
		strm.Printf( s_szBuf, "\n");

		i3String::PreprocessMacro( s_szBuf, sizeof(s_szBuf) - 1, s_headerTailTemplate, s_Symbol, 1);
		strm.WriteLine( s_szBuf);

		strm.Close();
	}

	// CPP
	{
		i3FileStream strm;
	
		sprintf( szFull, "%s\\AIDriver%s.cpp", pszOnlyPath, szName);

		if( strm.Create( szFull, STREAM_WRITE) == FALSE)
		{
			I3WARN( "%s 파일을 생성할 수 없습니다.", szFull);
			return;
		}

		// Head 부분
		{
			i3String::PreprocessMacro( s_szBuf, sizeof(s_szBuf) - 1, s_srcHeadTemplate, s_Symbol, 1);

			strm.WriteLine( s_szBuf);
		}

		// Procedures
		{
			for( i = 0; i < m_pDiagram->getShapeCount(); i++)
			{
				GICShapeAI * pShape = (GICShapeAI *) m_pDiagram->getShape( i);
				i3AIState * pState = pShape->getAIState();
				
				// Comment
			
				i3::vector<i3::rc_string>& StrList = pShape->getCommentText();

				strm.Printf( s_szBuf, "////////////////////////////////////////////////////////////////////////////////\n");
				strm.Printf( s_szBuf, "//\n// %s\n", pState->GetName());

				for(size_t j = 0; j < StrList.size(); j++)
				{
					strm.Printf( s_szBuf, "// %s\n", StrList[j].c_str());
				}

				if( pState->getAnim() != NULL)
				{
					strm.Printf( s_szBuf, "// ANIMATION : %s\n", pState->getAnim()->GetName());
				}
				strm.Printf( s_szBuf, "//\n");

				
				// Run function
				{
					strm.Printf( s_szBuf, "static void AIPROC_%s_Run( i3AIContext * pCtx, REAL32 tm)\n{\n", pState->GetName());

	
					i3::vector<i3::rc_string>& StrList = pShape->getSourceText();

					for( j = 0; j < StrList.size(); j++)
					{
						strm.Printf( s_szBuf, "\t%s\n", StrList[j].c_str());
					}


					strm.Printf( s_szBuf, "}\n\n");
				}

				// Finish function
				{
					strm.Printf( s_szBuf, "static void AIPROC_%s_Fin( i3AIContext * pCtx, REAL32 tm)\n{\n", pState->GetName());

					strm.Printf( s_szBuf, "}\n\n");
				}
			}
		}

		// Procedures Table
		{
			strm.Printf( s_szBuf, "\n\nstatic I3_AISTATE_PROC_TABLE s_%sTable[] =\n{\n", szName);

			for( i = 0; i < getAI()->getAIStateCount(); i++)
			{
				i3AIState * pState = getAI()->getAIState( i);

				strm.Printf( s_szBuf, "	{	\"%s\",	AIPROC_%s_Run,	AIPROC_%s_Fin	},\n", pState->GetName(), pState->GetName(), pState->GetName());
			}

			strm.Printf( s_szBuf, "};\n\n");
		}

		// Constructor & Destructor
		{
			i3String::PreprocessMacro( s_szBuf, sizeof(s_szBuf) - 1, s_srcTailTemplate, s_Symbol, 1);

			strm.WriteLine( s_szBuf);
		}

		strm.Close();
	}
}

void AI::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	AI * pAI = (AI *) pDest;

	pAI->SetAI( getAI());
}

void AI::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pCtx != NULL)
	{
		m_pCtx->OnBuildObjectList( List);
	}

	if( m_pDiagram != NULL)
	{
		m_pDiagram->OnBuildObjectList( List);
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#pragma pack( push, 8)

typedef struct _tagPersistAI
{
	UINT8		m_ID[4];
	OBJREF64	m_pCtx;
	OBJREF64	m_pDiagram;
	UINT32		m_DriverNameLength;

	UINT32		pad[16];
} PERSIST_AI;

#pragma pack( pop)

UINT32 AI::OnSave( i3ResourceFile * pResFile)
{
	PERSIST_AI data;
	UINT32 Rc, Result;

	Result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "AAI1", 4);
	data.m_pCtx = pResFile->GetObjectPersistID( m_pCtx);
	data.m_pDiagram = (OBJREF64) pResFile->GetObjectPersistID( m_pDiagram);
	data.m_DriverNameLength = strlen( m_szDriverName);

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_DriverNameLength > 0)
	{
		Rc = pResFile->Write( m_szDriverName, data.m_DriverNameLength + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 AI::OnLoad( i3ResourceFile * pResFile)
{
	PERSIST_AI data;
	UINT32 Rc, Result;

	Result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_DriverNameLength > 0)
	{
		Rc = pResFile->Read( m_szDriverName, data.m_DriverNameLength + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( data.m_pCtx != 0)
	{
		i3AIContext * pCtx;

		pCtx = (i3AIContext *) pResFile->FindObjectByID( (OBJREF) data.m_pCtx);
		I3_REF_CHANGE( m_pCtx, pCtx);

		I3ASSERT( m_pCtx != NULL);
		pCtx->SetName( GetName());

		{
			i3AI * pAI = pCtx->getAI();

			pAI->Sort();
		}
	}

	if( data.m_pDiagram != 0)
	{
		GICDiagramAI * pDiagram = (GICDiagramAI *) pResFile->FindObjectByID( (OBJREF) data.m_pDiagram);
		I3ASSERT( pDiagram != NULL);

		pDiagram->setAI( this);

		I3_REF_CHANGE( m_pDiagram, pDiagram);
	}

	return Result;
}

void AI::DumpTXT( const char * pszPath)
{
	i3FileStream file;
	char conv[4096];
	i3AI * pAI = m_pCtx->getAI();

	if( file.Create( pszPath, STREAM_WRITE) == FALSE)
		return;

	{
		char szPath[ MAX_PATH];

		GetFullAIPath( szPath);

		file.Printf( conv, "AI :\t%s\n", szPath);
		file.Printf( conv, "Total State : %d\n", pAI->getAIStateCount());

		file.Printf( conv, "State\tAnimation\n\n");
	}

	{
		INT32 i;

		for( i = 0; i < pAI->getAIStateCount(); i++)
		{
			i3AIState * pState = pAI->getAIState( i);
			i3Animation * pAnim = pState->getAnim();

			if( pAnim != NULL)
				file.Printf( conv, "%s\t%s\n", pState->GetName(), pAnim->GetName());
			else
				file.Printf( conv, "%s\t(none)\n", pState->GetName());
		}
	}

	file.Close();
}

void AI::ConvertToAnimTemplate( bool bSetAnimToNull)
{
	INT32 i;
	char conv[512];
	bool bReplaced;

	for( i = 0; i < m_pCtx->getAIStateCount(); i++)
	{
		i3AIState * pState = m_pCtx->getAIState( i);

		i3Animation * pAnim = pState->getAnim();

		if( pAnim != NULL)
			bReplaced = ReplaceSymbols( conv, sizeof(conv), pAnim->GetName());
		else
			bReplaced = ReplaceSymbols( conv, sizeof(conv), pState->getAnimName());

		if( bReplaced)
		{
			pState->setAnimName( conv);

			pState->addStyle( I3_AISTATE_STYLE_ANIM_TEMPLATE);
		}
		else
		{
			pState->setAnimName( "");
			pState->removeStyle( I3_AISTATE_STYLE_ANIM_TEMPLATE);
		}

		if( bSetAnimToNull)
		{
			pState->SetAnim( NULL);
		}
	}
}

struct NMONIC_INFO
{
	char	m_szSrc[64];
	char	m_szTarget[64];
};

static NMONIC_INFO	s_Symbols[] =
{
	{	"SWAT_MALE",			"%C"	},

	{	"",						""		}
};

static INT32	FindSymbol( const char * pszStr)
{
	INT32 i;

	for( i = 0; i < s_Symbols[i].m_szSrc[0] != 0; i++)
	{
		if( i3::generic_is_iequal( pszStr, s_Symbols[i].m_szSrc) )
		{
			return i;
		}
	}

	return -1;
}

bool AI::ReplaceSymbols( char * pszDestBase, INT32 len, const char * pszSrcBase)
{
	INT32 i;
//	bool bResult = false;

	i3::safe_string_copy( pszDestBase, pszSrcBase, len );

	for( i = 0; i < s_Symbols[i].m_szSrc[0] != 0; i++)
	{
		i3::generic_string_ireplace_all( pszDestBase, s_Symbols[i].m_szSrc, s_Symbols[i].m_szTarget);
	}
	
	bool bResult = !i3::generic_is_equal( pszSrcBase, pszDestBase);
	return bResult;
}

