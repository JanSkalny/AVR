#pragma once

class CRegion {
public:
	CRect rc;	
	int nOutput;

	CRegion(CRect rc) { this->rc = rc; };
	~CRegion() {};
};


class CDisplay : public CStatic
{
	DECLARE_DYNAMIC(CDisplay)

	friend class CAmbiLEDDlg;

private:
	void CreateBuf(CDC *pDC);
	void DestroyBuf();
	void Draw();

	CDC *m_pDC;
	CBitmap *m_pBitmap;

	list<CRegion*> m_List;

	CRegion *m_pSelected;
	CRegion *m_pDrag;

	bool m_bDrag, m_bSize;
	CPoint m_ptDragOffset, m_ptDragStart;
	CSize m_szDragStart;

	void DragStart(CRegion *pRegion, CPoint pt);
	void DragStop();
	void Drag(CPoint pt);

	void SetParent(CAmbiLEDDlg *pParent) { m_pParent = pParent; };
	CAmbiLEDDlg *m_pParent;

public:
	CDisplay();
	virtual ~CDisplay();
	void SelectRegion(CRegion *pRegion);

	void AddRegion(CRegion *pRegion);
	void DeleteRegion(CRegion *pRegion);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


