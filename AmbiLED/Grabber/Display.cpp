// Display.cpp : implementation file
//

#include "stdafx.h"
#include "AmbiLED.h"
#include "Display.h"

#include "AmbiLEDDlg.h"

// CDisplay

IMPLEMENT_DYNAMIC(CDisplay, CStatic)

CDisplay::CDisplay()
{
	m_pDC = NULL;
	m_pBitmap = NULL;
	
	m_pDrag = m_pSelected = NULL;

	m_bDrag = m_bSize = false;
	
	m_pParent = NULL;
}

CDisplay::~CDisplay()
{
	DestroyBuf();
}


BEGIN_MESSAGE_MAP(CDisplay, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



void CDisplay::CreateBuf(CDC *pDC) {
	CRect rc;

	DestroyBuf();
	m_pDC = new CDC();
	m_pBitmap = new CBitmap();

	pDC->GetClipBox(&rc);
	m_pDC->CreateCompatibleDC(pDC);
	pDC->LPtoDP(&rc);
	m_pBitmap->CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
	m_pDC->SelectObject(&m_pBitmap);
	m_pDC->SetMapMode(pDC->GetMapMode());
	m_pDC->SetWindowExt(pDC->GetWindowExt());
	m_pDC->SetViewportExt(pDC->GetViewportExt());
	pDC->DPtoLP(&rc);
	m_pDC->SetWindowOrg(rc.left, rc.top);
}

void CDisplay::DestroyBuf() {
	if (m_pDC!=NULL) {
		m_pDC->Detach();
		delete m_pDC;
		m_pDC = NULL;
	}
	if (m_pBitmap!=NULL) {
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
}

void CDisplay::Draw() {
	CRect rc;

	VERIFY(m_pDC);

	GetClientRect(rc);
	m_pDC->FillSolidRect(rc,RGB(255,0,0));
}

// CDisplay message handlers



BOOL CDisplay::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}


void CDisplay::OnPaint()
{
	CString sBuf;
	list<CRegion*>::iterator itRegion;
	CRegion *pRegion;
	CPaintDC dc(this);
	CRect rc;

	GetClientRect(rc);

	dc.FillSolidRect(rc,RGB(58,110,165));

	// render all windows
	for (itRegion=m_List.begin(); itRegion!=m_List.end(); itRegion++) {
		pRegion = *itRegion;
	
		if (pRegion == m_pSelected)
			dc.FillSolidRect(&pRegion->rc,RGB(255,0,0));
		else
			dc.FillSolidRect(&pRegion->rc,RGB(255,255,255));
		if (pRegion->nOutput>=0)
			sBuf.Format(_T("out%d"),pRegion->nOutput);
		else
			sBuf = _T("(none)");
		dc.TextOut(pRegion->rc.left,pRegion->rc.top,sBuf,sBuf.GetLength());
	}
}

// mouse down handler
// change selection and call drag/resize handler
void CDisplay::OnLButtonDown(UINT nFlags, CPoint point)
{
	list<CRegion*>::iterator itRegion;
	CRegion *pRegion;

	if (m_pDrag && !m_pDrag->rc.PtInRect(point))
		m_pDrag = NULL;

	for (itRegion=m_List.begin(); itRegion!=m_List.end(); itRegion++) {
		pRegion = *itRegion;

		if (pRegion->rc.PtInRect(point)) {
			SelectRegion(pRegion);
			
			// drag or resize
			if (nFlags & MK_SHIFT)
				m_bSize = true;
			else 
				m_bDrag = true;

			DragStart(pRegion,point);
		}
	}

	Invalidate();
}

// stop drag/resize process
void CDisplay::OnLButtonUp(UINT nFlags, CPoint point)
{
	DragStop();
}

// dragging resizing region
void CDisplay::OnMouseMove(UINT nFlags, CPoint point)
{

	if (nFlags&MK_SHIFT && m_pSelected && m_pSelected->rc.PtInRect(point)) 
		::SetCursor(::LoadCursor(NULL,IDC_SIZENWSE));

	if ((m_bDrag || m_bSize) && nFlags&MK_LBUTTON)
		Drag(point);
	else 
		m_bDrag = m_bSize = false;
}


void CDisplay::Drag(CPoint pt)
{
	CRect rcRegion, rcDisplay;
	CPoint ptDelta;
	int x, y, w, h;

	VERIFY(m_pDrag);
	if (!m_pDrag)
		return;

	rcRegion = m_pDrag->rc;
	GetClientRect(rcDisplay);

	if (m_bSize) {
		ptDelta = pt - m_ptDragStart;
		x = rcRegion.left;
		y = rcRegion.top;

		w = m_szDragStart.cx + ptDelta.x;
		h = m_szDragStart.cy + ptDelta.y;

		w = (w<3)?3:w;
		h = (h<3)?3:h;
		if (x+w >= rcDisplay.Width())
			w = rcDisplay.Width()-x;

		if (y+h >= rcDisplay.Height())
			h = rcDisplay.Height()-y;
	}

	if (m_bDrag) {
		pt -= m_ptDragOffset;
		x = pt.x;
		y = pt.y;
		w = rcRegion.Width();
		h = rcRegion.Height();

		if (x <= 0)
			x = 0;

		if (y <= 0)
			y = 0;

		if (x+w >= rcDisplay.Width())
			x = rcDisplay.Width()-w;

		if (y+h >= rcDisplay.Height())
			y = rcDisplay.Height()-h;
	}

	CString sDebug;
	sDebug.Format(_T("x=%d,y=%d,w=%d,h=%d\n"),x,y,w,h);
	OutputDebugString(sDebug);

	m_pDrag->rc = CRect(x,y,x+w,y+h);

	Invalidate();
}

void CDisplay::DragStart(CRegion *pRegion, CPoint pt)
{
	m_pDrag = pRegion;
	m_ptDragStart = pt;
	m_szDragStart = CSize(pRegion->rc.Width(),pRegion->rc.Height());
	m_ptDragOffset = CPoint(pt.x-pRegion->rc.left,pt.y-pRegion->rc.top);
}

void CDisplay::DragStop()
{
	m_bDrag = m_bSize = false;
	m_pDrag = NULL;
}

void CDisplay::AddRegion(CRegion *pRegion) {
	m_List.push_back(pRegion);
	Invalidate();
}

void CDisplay::DeleteRegion(CRegion *pRegion) {
	m_List.remove(pRegion);
	if (m_pSelected == pRegion)
		SelectRegion(NULL);
	delete pRegion;
	Invalidate();
}

void CDisplay::SelectRegion(CRegion *pRegion) {
	VERIFY(m_pParent);

	if (m_pSelected != pRegion) {
		m_pSelected = pRegion;
		// update parent
		m_pParent->SelectRegion(pRegion);
	}
}