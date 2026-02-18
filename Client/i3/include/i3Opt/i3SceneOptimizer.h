#if !defined( __I3_SCENE_OPTIMIZER_H)
#define __I3_SCENE_OPTIMIZER_H

#include "i3Base.h"
#include "i3Scene.h"

enum OPT_PHASE
{
	PHASE_NA = -1,
	PHASE_COMMON_SG,
	PHASE_INSTANCE_SG,
};

#define	OPT_STYLE_INTERNAL_ONLY		0x80000000
#define OPT_STYLE_APPLY_NODE		0x00000001
#define OPT_STYLE_APPLY_SG			0x00000002

enum I3OPT_PROGRESS_REPORT
{
	I3OPT_PROGRESS_REPORT_START = 1,
	I3OPT_PROGRESS_REPORT_END,
	I3OPT_PROGRESS_REPORT_CANCEL,
	I3OPT_PROGRESS_REPORT_POS,
	I3OPT_PROGRESS_REPORT_SUBSTART,
	I3OPT_PROGRESS_REPORT_SUBEND,
	I3OPT_PROGRESS_REPORT_SUBPOS,
};

typedef void	(*I3OPT_PROGRESS)( I3OPT_PROGRESS_REPORT report, INT32 param1, INT32 param2, const char * pszMsg, void * pUserData);

class I3_EXPORT_OPT i3SceneOptimizer : public i3ElementBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3SceneOptimizer, i3ElementBase);

public:
	enum CLASS
	{
		CLASS_NA = 0,
		CLASS_ANIMATION,
		CLASS_GEOMETRY,
		CLASS_PHYSICS,
		CLASS_SCENEGRAPH,
		CLASS_SOUND,
		CLASS_TEXTURE,

		CLASS_MAX
	};

protected:
	CLASS					m_Class = CLASS_NA;
	UINT32					m_Style = OPT_STYLE_APPLY_NODE | OPT_STYLE_INTERNAL_ONLY;
	OPT_PHASE				m_Phase = PHASE_NA;
	i3SceneGraphInfo *		m_pSg = nullptr;
	i3Node *				m_pTraceRoot = nullptr;

	i3MatrixStack *		m_pStack = nullptr;
	bool				m_bMatrixStackEnable = false;

protected:
	virtual void		Rec_Trace( i3Node * pNode, i3Node * pParent);
	virtual bool		OnPreNode(i3Node * pNode, i3Node * pParent) { return true; }
	virtual bool		OnNode(i3Node * pNode, i3Node * pParent) { return true; }
	virtual bool		OnPostNode(i3Node * pNode, i3Node * pParent) { return true; }

public:
	virtual ~i3SceneOptimizer(void);

	UINT32				getStyle(void)				{ return m_Style; }
	void				setStyle( UINT32 mask)		{ m_Style = mask; }
	void				addStyle( UINT32 mask)		{ m_Style |= mask; }
	void				removeStyle( UINT32 mask)	{ m_Style &= ~mask; }
	bool				isStyle( UINT32 mask)		{ return (m_Style & mask) != 0; }
	bool				isInternalOnly(void)		{ return isStyle( OPT_STYLE_INTERNAL_ONLY); }

	i3SceneGraphInfo *	getSg(void)				{ return m_pSg; }
	void				setSg( i3SceneGraphInfo * pSg)
	{
		I3_REF_CHANGE( m_pSg, pSg);
	}

	i3Node *			getTraceRoot(void)			{ return m_pTraceRoot; }
	void				SetTraceRoot( i3Node * pNewRoot);

	bool				GetMatrixStackEnable(void)		{ return m_bMatrixStackEnable; }
	void				SetMatrixStackEnable( bool bFlag);
	
	virtual const char *	getHelpText(void);
	virtual bool			isSupportProgress(void);

	CLASS					getClass(void)				{ return m_Class; }
	virtual const char *	getDescName(void);
	virtual const char *	getDesc(void) { return ""; }
	virtual void			GetConfigString( char * pszStr, INT32 len);

	virtual void		Trace( i3Node * pRoot);
	virtual void		Optimize( i3SceneGraphInfo * pSg);

	//	i3Optimizer 툴에서 ini 파일을 파싱한 Label 이름과 값을 각각 char 형으로 넘겨줍니다.
	//	각 i3Opt 클래스에서는 Trace가 실행되므로 값의 설정이 필요한 경우 이 함수를 사용하여 적절히 적용시켜주세요. by KOMET
	virtual void SetProperty(char * pszFieldName, char * pszValue) {}

	void	CallOptimizer( i3ClassMeta * pMeta);

	static void			SetOptimizeProgressProc( I3OPT_PROGRESS proc, void * pUserData);

	static void			ReportStart( INT32 max, const char * pszMsg = nullptr);
	static void			ReportEnd(void);
	static void			ReportCancel(void);
	static void			ReportSubStart( INT32 max, const char * pszTitle = nullptr);
	static void			ReportSubEnd(void);
	static void			ReportProgress( INT32 pos, const char * pszMsg = nullptr);
	static void			ReportSubProgress( INT32 pos, const char * pszMsg = nullptr);

	virtual bool		SaveToXML( i3XMLElement * pXML);
	virtual bool		LoadFromXML( i3XMLElement * pXML);
};

#endif