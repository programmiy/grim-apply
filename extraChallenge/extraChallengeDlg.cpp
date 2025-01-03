// extraChallengeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "extraChallenge.h"
#include "extraChallengeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CextraChallengeDlg 대화 상자



CextraChallengeDlg::CextraChallengeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXTRACHALLENGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CextraChallengeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CextraChallengeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN() // 마우스 클릭 이벤트 추가
END_MESSAGE_MAP()


// CextraChallengeDlg 메시지 처리기

BOOL CextraChallengeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.
	SetWindowText(_T("추가문제 - 정원그리기"));
	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CextraChallengeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CextraChallengeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{

		CPaintDC dc(this);
        DrawAllCircles(&dc);

	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CextraChallengeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CextraChallengeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_clickPoints.size() < 3)
    {
        m_clickPoints.push_back(point);
        // 클릭할 때마다 다른 색상 선택
        COLORREF colors[] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };
        m_colors.push_back(colors[m_clickPoints.size() - 1]);

        Invalidate(); // 화면 갱신
    }
    else
    {
        AfxMessageBox(_T("이미 3개의 클릭 지점이 설정되었습니다."));
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

void CextraChallengeDlg::DrawCircle(CDC* pDC, CPoint center, int radius, int thickness, COLORREF color, bool fill)
{
    CPen pen(PS_SOLID, thickness, color);
    CPen* pOldPen = pDC->SelectObject(&pen);
    CBrush brush(color);
    CBrush* pOldBrush = nullptr;

    if (fill)
    {
        pOldBrush = pDC->SelectObject(&brush); // 내부를 채움
    }
    else
    {
        pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH); // 내부를 채우지 않음
    }

    pDC->Ellipse(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
}

void CextraChallengeDlg::DrawAllCircles(CDC* pDC)
{
    for (size_t i = 0; i < m_clickPoints.size(); ++i)
    {
        DrawCircle(pDC, m_clickPoints[i], 10, 2, m_colors[i], true); // 클릭 지점 원 그리기 (내부 채움)
    }

    if (m_clickPoints.size() == 3)
    {
        // 세 클릭 지점을 모두 지나가는 정원 그리기
        // 예외 처리 추가
        try
        {
            // 세 점을 모두 지나가는 원의 중심과 반지름 계산
            // 세 점을 이용하여 원의 중심과 반지름을 계산하는 알고리즘 사용
            CPoint center;
            double x1 = m_clickPoints[0].x, y1 = m_clickPoints[0].y;
            double x2 = m_clickPoints[1].x, y2 = m_clickPoints[1].y;
            double x3 = m_clickPoints[2].x, y3 = m_clickPoints[2].y;

            double a = x1 * (y2 - y3) - y1 * (x2 - x3) + x2 * y3 - x3 * y2;
            double b = (x1 * x1 + y1 * y1) * (y3 - y2) + (x2 * x2 + y2 * y2) * (y1 - y3) + (x3 * x3 + y3 * y3) * (y2 - y1);
            double c = (x1 * x1 + y1 * y1) * (x2 - x3) + (x2 * x2 + y2 * y2) * (x3 - x1) + (x3 * x3 + y3 * y3) * (x1 - x2);
            double d = (x1 * x1 + y1 * y1) * (x3 * y2 - x2 * y3) + (x2 * x2 + y2 * y2) * (x1 * y3 - x3 * y1) + (x3 * x3 + y3 * y3) * (x2 * y1 - x1 * y2);

            center.x = -b / (2 * a);
            center.y = -c / (2 * a);
            int radius = sqrt((b * b + c * c - 4 * a * d) / (4 * a * a));

            DrawCircle(pDC, center, radius, 2, RGB(0, 0, 0), false); // 기본 색상으로 검정색 사용, 내부 채우지 않음
        }
        catch (...)
        {
            AfxMessageBox(_T("정원을 그릴 수 없습니다."));
        }
    }
}