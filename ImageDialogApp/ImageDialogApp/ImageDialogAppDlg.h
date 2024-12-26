
// ImageDialogAppDlg.h: 헤더 파일
//

#pragma once


// CImageDialogAppDlg 대화 상자
class CImageDialogAppDlg : public CDialogEx
{
// 생성입니다.
public:
	CImageDialogAppDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	afx_msg void OnBnClickedButtonDraw();
	afx_msg void OnBnClickedButtonAction();
	afx_msg void OnBnClickedButtonOpen();
	virtual BOOL OnInitDialog();
	


    virtual ~CImageDialogAppDlg();                  // 소멸자 추가
	// 대화 상자 데이터입니다.
	#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_IMAGEDIALOGAPP_DIALOG };
	#endif
private:
    int m_x;
	CImage m_offScreen; // 오프스크린 이미지
    int m_y;
	ULONG_PTR gdiplusToken;
	Gdiplus::Bitmap* m_Bitmap;
    

    int m_radius; // 반지름을 저장하는 멤버 변수

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	

	// 구현입니다.
	protected:
		HICON m_hIcon;
		CEdit m_editX1;
		CEdit m_editY1;
		CEdit m_editX2;
		CEdit m_editY2;
		CStatic m_imageCtrl;
		// 생성된 메시지 맵 함수
		
		DECLARE_MESSAGE_MAP()
};
