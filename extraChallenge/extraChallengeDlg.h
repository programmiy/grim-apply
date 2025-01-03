// extraChallengeDlg.h: 헤더 파일
//

#pragma once
#include <vector>

// CextraChallengeDlg 대화 상자
class CextraChallengeDlg : public CDialogEx
{
// 생성입니다.
public:
    CextraChallengeDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

private:
	
	std::vector<COLORREF> m_colors; // 클릭 지점 색상 저장
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_EXTRACHALLENGE_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
    HICON m_hIcon;
    std::vector<CPoint> m_clickPoints; // 클릭 지점 저장
    
	void DrawCircle(CDC *pDC, CPoint center, int radius, int thickness, COLORREF color, bool fill);

    void DrawAllCircles(CDC* pDC);

    // 생성된 메시지 맵 함수
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point); // 마우스 클릭 이벤트 처리
    DECLARE_MESSAGE_MAP()
};