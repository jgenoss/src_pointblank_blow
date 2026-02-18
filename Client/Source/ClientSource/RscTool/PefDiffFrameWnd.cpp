// PefDiffFrameWnd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RscTool.h"
#include "PefDiffFrameWnd.h"

#include "MainFrm.h"

#include "FindPefDiff.h"
#include "i3Base/string/ext/num_to_str.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/format_string.h"

#include "PefTxtDiffInputInfo.h"

// PefDiffFrameWnd

IMPLEMENT_DYNCREATE(PefDiffFrameWnd, CFrameWndEx)

PefDiffFrameWnd::PefDiffFrameWnd() : m_exp_key(NULL), m_local_key(NULL)
{

}

PefDiffFrameWnd::~PefDiffFrameWnd()
{
	m_local_key->Release();
	m_exp_key->Release();
}


BEGIN_MESSAGE_MAP(PefDiffFrameWnd, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// PefDiffFrameWnd 메시지 처리기입니다.

int PefDiffFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	m_listCtrl.CreateEx(this, 1, LVS_NOSORTHEADER);
	m_listCtrl.InsertColumn(0, L"Pef Item Location");
	m_listCtrl.InsertColumn(1, L"Diff Type");
	m_listCtrl.InsertColumn(2, L"Data Type");
	m_listCtrl.InsertColumn(3, L"Latest Revision Value");
	m_listCtrl.InsertColumn(4, L"Local File Value");
	
	return 0;
}

void PefDiffFrameWnd::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	GetMainFrame()->DestroyPopupWndFromList(this);

	CFrameWndEx::PostNcDestroy();
}

BOOL PefDiffFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;

	int fullscreen_w = ::GetSystemMetrics(SM_CXSCREEN);
	int fullscreen_h = ::GetSystemMetrics(SM_CYSCREEN);

	cs.cx = fullscreen_w * 9 / 10;
	cs.cy = fullscreen_h * 5 / 6;

	cs.y = (fullscreen_h - cs.cy) / 2;
	cs.x = (fullscreen_w - cs.cx) / 2;

	return TRUE;
}

BOOL PefDiffFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CFrameWndEx::OnEraseBkgnd(pDC);
	return TRUE;
}

void PefDiffFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	if (m_listCtrl.GetSafeHwnd() == NULL)
		return;

	m_listCtrl.SetSize(cx, cy);

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL);
	}

	m_listCtrl.SetColumnWidth( 0, cx / 3);	cx -= cx/3;
	m_listCtrl.SetColumnWidth( 1, 80);		cx -= 80;
	m_listCtrl.SetColumnWidth( 2, 80);		cx -= 80;

	cx = (cx < 300) ? 300 : cx;

	m_listCtrl.SetColumnWidth(3, cx / 2);
	m_listCtrl.SetColumnWidth(4, cx / 2);

}

namespace
{
	void MakePefItemLocationString_Rec(const i3RegKey* k, i3::wstring& inout)
	{
		const i3RegKey* parent = static_cast<const i3RegKey*>( const_cast<i3RegKey*>(k)->getParent());
		if (parent)
		{
			MakePefItemLocationString_Rec(parent, inout);
	
			i3::wstring wstr;								// 모두 블록안에 포함시킴으로 최상단 루트에 대한 문자열 기록은 제외함..
			i3::mb_to_utf16(k->GetNameString(), wstr);
			inout.append( wstr );
			inout += L'/';
		}
	}

	void MakePefItemLocationString(const i3RegKey* k, const i3RegData* d, i3::wstring& out)
	{
		out.clear();
		MakePefItemLocationString_Rec(k, out);
		i3::wstring wstr;
		i3::mb_to_utf16(d->GetNameString(), wstr);
		out.append(wstr);
	}
	
	void MakePefItemLocationString(const i3RegKey* k, i3::wstring& out)
	{
		out.clear();
		
		const i3RegKey* parent = static_cast<const i3RegKey*>( const_cast<i3RegKey*>(k)->getParent());

		if (const_cast<i3RegKey*>(parent))
			MakePefItemLocationString_Rec(parent, out);
		
		i3::wstring wstr;								// 모두 블록안에 포함시킴으로 최상단 루트에 대한 문자열 기록은 제외함..
		i3::mb_to_utf16(k->GetNameString(), wstr);
		out.append( wstr );
	}

	void ConvertVec2DToString(const RT_VEC2D* v, i3::wstring& out)
	{
		i3::sprintf(out, L"%.4f, %.4f", v->x, v->y);		
	}
	
	void ConvertVec3DToString(const RT_VEC3D* v, i3::wstring& out)
	{
		i3::sprintf(out, L"%.4f, %.4f, %.4f", v->x, v->y, v->z);		
	}
	
	void ConvertVec4DToString(const RT_VEC4D* v, i3::wstring& out)
	{
		i3::sprintf(out, L"%.4f, %.4f, %.4f, %.4f", v->x, v->y, v->z, v->w);		
	}

	void ConvertColorToString(const I3COLOR* v, i3::wstring& out)
	{
		i3::sprintf(out, L"%d, %d, %d, %d", v->r, v->g, v->b, v->a);
	}

	void ConvertMatrixToString(const RT_MATRIX* v, i3::wstring& out)
	{
		i3::sprintf(out,	L"{%.4f, %.4f, %.4f, %.4f}, {%.4f, %.4f, %.4f, %.4f}, "
							L"{%.4f, %.4f, %.4f, %.4f}, {%.4f, %.4f, %.4f, %.4f}"
			, v->m00, v->m01, v->m02, v->m03, v->m10, v->m11, v->m12, v->m13
			, v->m20, v->m21, v->m22, v->m23, v->m30, v->m31, v->m32, v->m33);
	}



	
struct	PefDiffFrameWnd_Visitor	: PefDiffRegData_Visitor
{
	PefDiffFrameWnd* owner;	
	i3MFCListCtrl*	 list_ctrl;
	
	PefDiffFrameWnd_Visitor(PefDiffFrameWnd* owner) : owner(owner) { list_ctrl = owner->GetListCtrl(); }
	virtual ~PefDiffFrameWnd_Visitor() {}

	virtual void SameType(const PefDiffRegData* a, INT32* val1, INT32* val2) 
	{
		i3::wstring strLoc;	MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	i3::integral_to_str(*val1, strval1);
		i3::wstring strval2;	i3::integral_to_str(*val2, strval2);
		
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"INT32");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());
	}

	virtual void SameType(const PefDiffRegData* a, RT_REAL32* val1, RT_REAL32* val2) 
	{
		i3::wstring strLoc;	MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	i3::real_to_str(*val1, strval1);
		i3::wstring strval2;	i3::real_to_str(*val2, strval2);

		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"REAL32");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());
	}

	virtual void SameType(const PefDiffRegData* a, const i3::rc_wstring& val1, const i3::rc_wstring& val2) 
	{
		i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
	
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"STRING");
		list_ctrl->SetItemText(idx, 3, val1.c_str());
		list_ctrl->SetItemText(idx, 4, val2.c_str());	
	}

	virtual void SameType(const PefDiffRegData* a, RT_VEC2D* val1, RT_VEC2D* val2) 
	{
		i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	ConvertVec2DToString(val1, strval1);
		i3::wstring strval2;	ConvertVec2DToString(val2, strval2);

		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"VEC2D");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());		
	}

	virtual void SameType(const PefDiffRegData* a, RT_VEC3D* val1, RT_VEC3D* val2) 
	{
		i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	ConvertVec3DToString(val1, strval1);
		i3::wstring strval2;	ConvertVec3DToString(val2, strval2);

		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"VEC3D");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());				
	}

	virtual void SameType(const PefDiffRegData* a, RT_VEC4D* val1, RT_VEC4D* val2) 
	{
		i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	ConvertVec4DToString(val1, strval1);
		i3::wstring strval2;	ConvertVec4DToString(val2, strval2);

		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"VEC4D");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());					
	}

	virtual void SameType(const PefDiffRegData* a, I3COLOR* val1, I3COLOR* val2) 
	{
		i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	ConvertColorToString(val1, strval1);
		i3::wstring strval2;	ConvertColorToString(val2, strval2);

		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"COLOR");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());						
		
	}
	virtual void SameType(const PefDiffRegData* a, RT_MATRIX* val1, RT_MATRIX* val2) 
	{
		i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		i3::wstring strval1;	ConvertMatrixToString(val1, strval1);
		i3::wstring strval2;	ConvertMatrixToString(val2, strval2);

		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, L"MODIFIED");
		list_ctrl->SetItemText(idx, 2, L"MATRIX");
		list_ctrl->SetItemText(idx, 3, strval1.c_str());
		list_ctrl->SetItemText(idx, 4, strval2.c_str());			
	}

	virtual void DiffType(const PefDiffRegData* a, INT32* val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	i3::integral_to_str(*val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"INT32->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	i3::integral_to_str(*val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"INT32";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}
	}

	virtual void DiffType(const PefDiffRegData* a, RT_REAL32* val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	i3::real_to_str(*val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"REAL32->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	i3::real_to_str(*val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"REAL32";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}		
		
	}
	virtual void DiffType(const PefDiffRegData* a, const i3::rc_wstring& val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
		
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"STRING->");
			list_ctrl->SetItemText(idx, 3, val.c_str());			
		}
		else
		{
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"STRING";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, val.c_str());
		}				
	}

	virtual void DiffType(const PefDiffRegData* a, RT_VEC2D* val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	ConvertVec2DToString(val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"VEC2D->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	ConvertVec2DToString(val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"VEC2D";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}						
		
	}

	virtual void DiffType(const PefDiffRegData* a, RT_VEC3D* val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	ConvertVec3DToString(val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"VEC3D->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	ConvertVec3DToString(val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"VEC3D";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}							
	}

	virtual void DiffType(const PefDiffRegData* a, RT_VEC4D* val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	ConvertVec4DToString(val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"VEC4D->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	ConvertVec4DToString(val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"VEC4D";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}					
	}

	virtual void DiffType(const PefDiffRegData* a, I3COLOR* val, INT32 lhs_rhs) 
	{
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	ConvertColorToString(val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"COLOR->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	ConvertColorToString(val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"COLOR";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}						
		
	}

	virtual void DiffType(const PefDiffRegData* a, RT_MATRIX* val, INT32 lhs_rhs) 
	{	
		if (lhs_rhs == 0)
		{
			i3::wstring strLoc;		MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);
			i3::wstring strval1;	ConvertMatrixToString(val, strval1);
			INT32 idx = list_ctrl->GetItemCount();
			list_ctrl->InsertItem(idx, strLoc.c_str());
			list_ctrl->SetItemText(idx, 1, L"TYPE_CHANGED");
			list_ctrl->SetItemText(idx, 2, L"MATRIX->");
			list_ctrl->SetItemText(idx, 3, strval1.c_str());			
		}
		else
		{
			i3::wstring strval2;	ConvertMatrixToString(val, strval2);
			INT32 idx = list_ctrl->GetItemCount() - 1;
			CString str = list_ctrl->GetItemText(idx, 2);
			str += L"MATRIX";
			list_ctrl->SetItemText(idx, 2, str);
			list_ctrl->SetItemText(idx, 4, strval2.c_str());
		}						
		
	}

	virtual void PartialType(const PefDiffRegData* a, INT32* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	i3::integral_to_str(*val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"INT32");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());	
	}

	virtual void PartialType(const PefDiffRegData* a, RT_REAL32* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	i3::real_to_str(*val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"REAL32");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());	

	}

	virtual void PartialType(const PefDiffRegData* a, const i3::rc_wstring& val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

	
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"STRING");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, val.c_str());	
	}

	virtual void PartialType(const PefDiffRegData* a, RT_VEC2D* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	ConvertVec2DToString(val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"VEC2D");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());			
	}
	
	virtual void PartialType(const PefDiffRegData* a, RT_VEC3D* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	ConvertVec3DToString(val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"VEC3D");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());		
	}

	virtual void PartialType(const PefDiffRegData* a, RT_VEC4D* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	ConvertVec4DToString(val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"VEC4D");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());				
	}
	virtual void PartialType(const PefDiffRegData* a, I3COLOR* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	ConvertColorToString(val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"COLOR");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());					
	}
	virtual void PartialType(const PefDiffRegData* a, RT_MATRIX* val, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		if (lhs_rhs == 0)
			MakePefItemLocationString(a->lhs_key, a->lhs, strLoc);
		else
			MakePefItemLocationString(a->rhs_key, a->rhs, strLoc);

		i3::wstring strval;	ConvertMatrixToString(val, strval);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"MATRIX");
		list_ctrl->SetItemText(idx, (!lhs_rhs) ? 3 : 4, strval.c_str());					
	}

	virtual void PartialKey( i3RegKey* key, INT32 lhs_rhs) 
	{
		i3::wstring strLoc;		
		MakePefItemLocationString(key, strLoc);
		INT32 idx = list_ctrl->GetItemCount();
		list_ctrl->InsertItem(idx, strLoc.c_str());
		list_ctrl->SetItemText(idx, 1, (!lhs_rhs) ? L"DELETED" : L"ADDED");
		list_ctrl->SetItemText(idx, 2, L"ITEM");
		
	}
	//	

};

}

void PefDiffFrameWnd::SetInfo(const PefTxtDiffInputInfo& input_info, i3RegKey* exp_key, i3RegKey* local_key)
{

	exp_key->AddRef();	m_exp_key = exp_key;		
	local_key->AddRef();	m_local_key = local_key;	


	i3::const_char_range str_mode[3] = { i3::as_literal("MEDIA_PATH"), 
		i3::as_literal("QA_PATH"), i3::as_literal("LIVE_PATH") };

	i3::string str_rev[2];

	if (input_info.revision[0] != -1)
		str_rev[0] = i3::format_string("r%d", input_info.revision[0]);
	else
		str_rev[0] = "Local";

	if (input_info.revision[1] != -1)
		str_rev[1] = i3::format_string("r%d", input_info.revision[1]);
	else
		str_rev[1] = "Local";

	LVCOLUMNW col_info;
	::memset(&col_info, 0, sizeof(LVCOLUMNW));
	col_info.mask = LVCF_TEXT;
	
	i3::string str = i3::format_string("%s : %s of %s", input_info.rel_path[0], str_rev[0], 
		str_mode[input_info.mode[0]]);

	i3::wstring wstr;		i3::mb_to_utf16(str, wstr);

	col_info.pszText = const_cast<wchar_t*>(wstr.c_str());
	m_listCtrl.SetColumn(3, &col_info);
	
	str = i3::format_string("%s : %s of %s", input_info.rel_path[1], str_rev[1], 
		str_mode[input_info.mode[1]] );

	i3::mb_to_utf16(str, wstr);

	col_info.pszText = const_cast<wchar_t*>(wstr.c_str());
	m_listCtrl.SetColumn(4, &col_info);



	PefDiffSet set;
	FindPefDiff( m_exp_key, m_local_key, set);

	if (set.m_regDataDiffList.empty() && set.m_regKeyDiffList.empty() )
	{
		INT32 idx = m_listCtrl.GetItemCount();
		m_listCtrl.InsertItem(idx, L"No Different Pef Item & Data");
	}
	else
	{
		PefDiffFrameWnd_Visitor v(this);
		VisitPefDiff(set, &v);
	}
}

