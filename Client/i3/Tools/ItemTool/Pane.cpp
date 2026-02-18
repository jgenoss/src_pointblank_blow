/////////////////////////////////////////////////////////////////////////////
//
//  Written by Georges Kanaan
//  Email: gkanaan@hotmail.com
//
//  Date: Nov 10, 2001
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Pane.h"

CMyPane::CMyPane() 
{
  //
  //Initializations
  //
  m_nLeftMinWidth = 0;
  m_nLeftMinHeight = 0;
  m_nRightMinWidth = 0;
  m_nRightMinHeight = 0;
  m_nWidth = 0;
  m_nHeight = 0;
  m_X = m_Y = 0;
  m_nLeftType = PT_INVALID;
  m_nRightType = PT_INVALID;
  m_nSplitType = ST_VERTICAL;
  m_bLeftVisible = TRUE;
  m_bRightVisible = TRUE;
  m_bVisible = TRUE;
  m_bSizeableControls = TRUE;
  m_nPerc = 20;
}

CMyPane::~CMyPane() 
{
  //
  //Memory management... be Tidy!
  //
  if (m_nLeftType == PT_PANE)
    delete left.pPane;
  if (m_nRightType == PT_PANE)
    delete right.pPane;
}

//
//The following 2 functions check if a pane is visible or not...
//
BOOL CMyPane::IsLeftVisible()
{
  if (m_nLeftType == PT_WINDOW) {
    return m_bLeftVisible;
  } else if (m_nLeftType == PT_PANE) {
    return left.pPane->m_bVisible;
  }
  return FALSE;
}

BOOL CMyPane::IsRightVisible()
{
  if (m_nRightType == PT_WINDOW) {
    return m_bRightVisible;
  } else if (m_nRightType == PT_PANE) {
    return right.pPane->m_bVisible;
  }
  return FALSE;
}

//
//This function hides/shows both panes in the given pane
//
void CMyPane::Show(BOOL bShow) 
{
  m_bVisible = bShow;
  ShowLeft(bShow);
  ShowRight(bShow);
}

//
//Just the left part of the pane gets hidden
//
void CMyPane::ShowLeft(BOOL bShow) 
{
  m_bLeftVisible = bShow;
  if (m_nLeftType == PT_WINDOW) {
    ShowWindow(left.pWnd->m_hWnd,(bShow ? SW_SHOW : SW_HIDE));
  } else if (m_nLeftType == PT_PANE) {
    left.pPane->Show(bShow);
  }
}

//
//Just the right part of the pane gets hidden
//
void CMyPane::ShowRight(BOOL bShow) 
{
  if (m_bRightVisible == bShow) return;
  m_bRightVisible = bShow;
  if (m_nRightType == PT_WINDOW) {
    ShowWindow(right.pWnd->m_hWnd,(bShow ? SW_SHOW : SW_HIDE));
  } else if (m_nRightType == PT_PANE) {
    right.pPane->Show(bShow);
  }

  //we have to remember the left Width/Height depending
  //on split and reset it when this pane is reshown
  if (!bShow) { //save
    if (m_nSplitType == ST_HORIZONTAL)
      m_nSavedVal = GetLeftHeight();
    else if (m_nSplitType == ST_VERTICAL)
      m_nSavedVal = GetLeftWidth();
  } else { //restore
    if (m_nSplitType == ST_HORIZONTAL)
      SetLeftHeight(m_nSavedVal);
    else if (m_nSplitType == ST_VERTICAL)
      SetLeftWidth(m_nSavedVal);
  }
}

//
//Resizes the pane's width/height without moving it's X and Y
//
void CMyPane::MovePane(int nWidth, int nHeight) 
{
  if (nWidth==-1 && nHeight==-1) {
    nWidth = m_nWidth;
    nHeight = m_nHeight;
  }
  MovePane(m_X,m_Y,nWidth,nHeight);
}

//
//The following functions get/set the pane's left item width/height
//
//Only the left item needs be set since the right one is dynamic
//depending on the dialog's size
//
int CMyPane::GetLeftWidth() 
{
  if (m_nLeftType == PT_PANE) return left.pPane->GetLeftWidth();
  else if (m_nLeftType == PT_WINDOW) {
    RECT rect;
    GetWindowRect(left.pWnd->m_hWnd,&rect);
    return (rect.right-rect.left);
  }
  return 0;
}

int CMyPane::GetLeftHeight() 
{
  if (m_nLeftType == PT_PANE) return left.pPane->GetLeftHeight();
  else if (m_nLeftType == PT_WINDOW) {
    RECT rect;
    left.pWnd->GetWindowRect(&rect);
    return (rect.bottom-rect.top);
  }
  return 0;
}

void CMyPane::SetLeftWidth(int nWidth) 
{
  if (m_nLeftType == PT_PANE) {
    left.pPane->m_nWidth = nWidth;
    left.pPane->SetLeftWidth(nWidth);
  } else if (m_nLeftType == PT_WINDOW) {
    RECT rect;
    GetWindowRect(left.pWnd->m_hWnd,&rect);
    left.pWnd->GetParent()->ScreenToClient(&rect);
    MoveWindow(left.pWnd->m_hWnd,rect.left, rect.top, nWidth, rect.bottom - rect.top, TRUE);
  }
}

void CMyPane::SetLeftHeight(int nHeight) 
{
  if (m_nLeftType == PT_PANE) {
    left.pPane->m_nHeight = nHeight;
    left.pPane->SetLeftHeight(nHeight);
  } else if (m_nLeftType == PT_WINDOW) {
    RECT rect;
    GetWindowRect(left.pWnd->m_hWnd,&rect);
    left.pWnd->GetParent()->ScreenToClient(&rect);
    MoveWindow(left.pWnd->m_hWnd,rect.left, rect.top, rect.right - rect.left, nHeight, TRUE);
  }
}

BOOL CMyPane::GetWndSize(SIZE &size, int nWhich)
{
  RECT rect;

  switch (nWhich) {
  case 1:
    if (m_nLeftType != PT_WINDOW) return FALSE;
    left.pWnd->GetWindowRect(&rect);
    size.cx = rect.right - rect.left;
    size.cy = rect.bottom - rect.top;
    if (!m_bRightVisible) {
      if (m_nSplitType == ST_HORIZONTAL)
        size.cy = m_nSavedVal;
      else if (m_nSplitType == ST_VERTICAL)
        size.cx = m_nSavedVal;
    }
    break;
  case 2:
    if (m_nRightType != PT_WINDOW) return FALSE;
    right.pWnd->GetWindowRect(&rect);
    size.cx = rect.right - rect.left;
    size.cy = rect.bottom - rect.top;
    break;
  }

  return TRUE;
}

//
//Checks to see if the given point is on any splitter bar
//
//it returns the direction in which the cursor should be or NULL
//
char* CMyPane::IsPointInSplitter(CPoint point, CMyPane **dpPane) 
{
  char *res = NULL;
  if (m_rectSplitter.PtInRect(point)) {
    *dpPane = this;
    if (m_nSplitType == ST_HORIZONTAL)
      return IDC_SIZENS;
    else if (m_nSplitType == ST_VERTICAL)
      return IDC_SIZEWE;
  } else {
    if (m_nLeftType == PT_PANE)
      res = left.pPane->IsPointInSplitter(point,dpPane);
    if (res) return res;
    if (m_nRightType == PT_PANE)
      return right.pPane->IsPointInSplitter(point,dpPane);
  }
  return NULL;
}

//
//The following functions compute the minimum width/height needed by the panes
//
int CMyPane::GetMinWidth() 
{
  int width = 0;
  int width2 = 0;
  //has to get the min width of the two panes
  //and add to them the width of the splitter bar
  if (m_nLeftType == PT_PANE)
    width = left.pPane->GetMinWidth();
  else if (m_nLeftType == PT_WINDOW)
    width = m_nLeftMinWidth;

  if (m_nRightType == PT_PANE)
    width2 = right.pPane->GetMinWidth();
  else if (m_nRightType == PT_WINDOW)
    width2 = m_nRightMinWidth;

  if (m_nSplitType == ST_VERTICAL)
    width += width2;
  else
    if (width2 > width) width = width2;

  return width + 5;
}

int CMyPane::GetMinHeight() 
{
  int height = 0;
  int height2 = 0;
  
  if (m_nLeftType == PT_PANE)
    height = left.pPane->GetMinHeight();
  else if (m_nLeftType == PT_WINDOW)
    height = m_nLeftMinHeight;

  if (m_nRightType == PT_PANE)
    height2 = right.pPane->GetMinHeight();
  else if (m_nRightType == PT_WINDOW)
    height2 = m_nRightMinHeight;

  if (m_nSplitType == ST_HORIZONTAL)
    height += height2;
  else
    if (height2 > height) height = height2;

  return height + 5;
}

int CMyPane::GetLeftMinWidth() 
{
  if (m_nLeftType == PT_PANE) return left.pPane->GetMinWidth();
  else if (m_nLeftType == PT_WINDOW) return m_nLeftMinWidth;
  return 0;
}

int CMyPane::GetLeftMinHeight() 
{
  if (m_nLeftType == PT_PANE) return left.pPane->GetMinHeight();
  else if (m_nLeftType == PT_WINDOW) return m_nLeftMinHeight;
  return 0;
}

int CMyPane::GetRightMinHeight() 
{
  if (m_nRightType == PT_PANE) return right.pPane->GetMinHeight();
  else if (m_nRightType == PT_WINDOW) return m_nRightMinHeight;
  return 0;
}

int CMyPane::GetRightMinWidth() 
{
  if (m_nRightType == PT_PANE) return right.pPane->GetMinWidth();
  else if (m_nRightType == PT_WINDOW) return m_nRightMinWidth;
  return 0;
}

//
//Handles moving the splitter bar and resizing the controls around it
//
void CMyPane::MoveSplitter(CPoint point)
 {
  //first make sure what type of splitter we are
  //then check that the point does not exceed the minimum size set for the controls
  if (m_nSplitType == ST_HORIZONTAL) {
    if (point.y < m_Y + GetLeftMinHeight()) point.y = m_Y + GetLeftMinHeight();
    if (point.y > ((m_Y + m_nHeight) - GetRightMinHeight())) point.y = (m_Y + m_nHeight) - GetRightMinHeight();
    SetLeftHeight(point.y - m_Y);
  } else if (m_nSplitType == ST_VERTICAL) {
    if (point.x < (m_X + GetLeftMinWidth())) point.x = (m_X +GetLeftMinWidth());
    if (point.x > ((m_nWidth + m_X) - GetRightMinWidth())) point.x = ((m_nWidth + m_X) - GetRightMinWidth());
    SetLeftWidth(point.x-m_X);
  }
  MovePane();
}

//
//The core move function, this basically sets all the panes and the
//controls where they need to be
//
void CMyPane::MovePane(int X, int Y, int nWidth, int nHeight)
 {
  //make sure pane isn't hidden...
  if (!m_bRightVisible && !m_bLeftVisible) return;

  int leftHeight = 0, rightHeight = 0, leftWidth = 0, rightWidth = 0;
  int rightX = 0, rightY = 0, leftX = 0, leftY = 0;


  //respect the minimum size
  if (nWidth < GetMinWidth()) nWidth = GetMinWidth();
  if (nHeight < GetMinHeight()) nHeight = GetMinHeight();

  //initialize
  m_nWidth = nWidth;
  m_nHeight = nHeight;
  m_X = X;
  m_Y = Y;
  leftX = X;
  leftY = Y;

  //compute split dimensions
  if (m_nSplitType == ST_HORIZONTAL) {
    leftWidth = rightWidth = m_nWidth;
    leftHeight = GetLeftHeight();
    if ((m_nHeight - leftHeight) < GetRightMinHeight())
      leftHeight = m_nHeight - GetRightMinHeight();
    rightHeight = m_nHeight - leftHeight - 5;
    rightX = X;
    rightY = leftY + leftHeight + 5;

    //here left and right are visible, now check
    //if one of them is hidden and hide it
    if (!IsLeftVisible()) { //top one is hidden
      rightY = Y;
      rightHeight = m_nHeight;
    } else if (!IsRightVisible()) { //bottom one is hidden
      leftHeight = m_nHeight;
    }

    //compute the dimensions of the splitterbar
    m_rectSplitter.SetRect(m_X,rightY - 5,m_X+m_nWidth,rightY);
  } else if (m_nSplitType == ST_VERTICAL) {
    leftHeight = rightHeight = m_nHeight;
    leftWidth = GetLeftWidth();
    if ((m_nWidth - leftWidth) < GetRightMinWidth())
      leftWidth = m_nWidth - GetRightMinWidth();
    rightWidth = m_nWidth - leftWidth - 5;
    rightY = Y;
    rightX = leftX + leftWidth + 5;

    //here left and right are visible, now check
    //if one of them is hidden and hide it
    if (!IsLeftVisible()) { //left one is hidden
      rightX = X;
      rightWidth = m_nWidth;
    } else if (!IsRightVisible()) { //right one is hidden
      leftWidth = m_nWidth;
    }

    //compute the dimensions of the splitterbar
    m_rectSplitter.SetRect(rightX - 5,m_Y,rightX,m_Y + m_nHeight);
  }

  //if one of the panes is hidden, no need for a splitter
  if (!IsLeftVisible() || !IsRightVisible() || !m_bSizeableControls) {
    //no splitter if only 1 pane visible
    m_rectSplitter.SetRect(0,0,0,0);
  }

  //resize left side
  if (IsLeftVisible()) {
    if (m_nLeftType == PT_PANE)
      left.pPane->MovePane(leftX,leftY,leftWidth,leftHeight);
    else if (m_nLeftType == PT_WINDOW) {
      MoveWindow(left.pWnd->m_hWnd,leftX,leftY,leftWidth,leftHeight,TRUE);
      left.pWnd->RedrawWindow();
    }
  }

  //resize right side
  if  (IsRightVisible()) {
    if (m_nRightType == PT_PANE)
      right.pPane->MovePane(rightX,rightY,rightWidth,rightHeight);
    else if (m_nRightType == PT_WINDOW) {
      MoveWindow(right.pWnd->m_hWnd,rightX,rightY,rightWidth,rightHeight,TRUE);
      right.pWnd->RedrawWindow();
    }
  }
}