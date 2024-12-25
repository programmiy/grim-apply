// ImageDialogAppDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ImageDialogApp.h"
#include "ImageDialogAppDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <random>
#include <direct.h>
#include <errno.h>
#include <ctime>
#include <gdiplus.h>
using namespace Gdiplus; 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// TODO: 불필요한 주석 제거
BEGIN_MESSAGE_MAP(CImageDialogAppDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_DRAW, &CImageDialogAppDlg::OnBnClickedButtonDraw)
    ON_BN_CLICKED(IDC_BUTTON_ACTION, &CImageDialogAppDlg::OnBnClickedButtonAction)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &CImageDialogAppDlg::OnBnClickedButtonOpen)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    
    ON_BN_CLICKED(IDOK, &CImageDialogAppDlg::OnBnClickedOk)
    ON_EN_CHANGE(IDC_EDIT_X1, &CImageDialogAppDlg::OnEnChangeEditX1)
END_MESSAGE_MAP()

CImageDialogAppDlg::CImageDialogAppDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_IMAGEDIALOGAPP_DIALOG, pParent), m_radius(0), m_x(0), m_y(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageDialogAppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_X1, m_editX1);
    DDX_Control(pDX, IDC_EDIT_Y1, m_editY1);
    DDX_Control(pDX, IDC_EDIT_X2, m_editX2);
    DDX_Control(pDX, IDC_EDIT_Y2, m_editY2);
    DDX_Control(pDX, IDC_STATIC_IMAGE, m_imageCtrl);
}

ULONG_PTR gdiplusToken;

BOOL CImageDialogAppDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


    // 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
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

    SetIcon(m_hIcon, TRUE);         // 큰 아이콘을 설정합니다.
    SetIcon(m_hIcon, FALSE);        // 작은 아이콘을 설정합니다.

    // 컨트롤 초기화
    if (!m_editX1.SubclassDlgItem(IDC_EDIT_X1, this) ||
        !m_editY1.SubclassDlgItem(IDC_EDIT_Y1, this) ||
        !m_editX2.SubclassDlgItem(IDC_EDIT_X2, this) ||
        !m_editY2.SubclassDlgItem(IDC_EDIT_Y2, this) ||
        !m_imageCtrl.SubclassDlgItem(IDC_STATIC_IMAGE, this))
    {
        AfxMessageBox(_T("컨트롤 초기화 실패"));
        return FALSE;
    }


    // 레이블 초기화
    SetDlgItemText(IDC_STATIC_X1_LABEL, _T("X1:"));
    SetDlgItemText(IDC_STATIC_Y1_LABEL, _T("Y1:"));
    SetDlgItemText(IDC_STATIC_X2_LABEL, _T("X2:"));
    SetDlgItemText(IDC_STATIC_Y2_LABEL, _T("Y2:"));

    // TODO: crect가 아닌 cimage로 변경 예정이라 삭제 예정
    // 그리기 영역 초기화
    CRect rect;
    m_imageCtrl.GetClientRect(&rect);
    CDC* pDC = m_imageCtrl.GetDC();
    pDC->FillSolidRect(&rect, RGB(0, 0, 0)); // 바탕색 설정
    pDC->Rectangle(&rect); // 테두리 설정
    m_imageCtrl.ReleaseDC(pDC);

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

CImageDialogAppDlg::~CImageDialogAppDlg()
{
    CDialogEx::OnDestroy();

    // GDI+ 종료
    GdiplusShutdown(gdiplusToken);
}

void CImageDialogAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CDialogEx dlgAbout(IDD_ABOUTBOX);
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CImageDialogAppDlg::OnBnClickedButtonDraw()
{
    // TODO: 파이썬으로 구현한 알고리즘 토대로 사용하기 위해 전부 삭제, 최우선 개발 사항

}

void CImageDialogAppDlg::OnBnClickedButtonAction()
{
    // TODO: 파이썬 알고리즘, rect 방식 응용해서 수정할 계획
    CString strX1, strY1, strX2, strY2;
    m_editX1.GetWindowText(strX1);
    m_editY1.GetWindowText(strY1);
    m_editX2.GetWindowText(strX2);
    m_editY2.GetWindowText(strY2);

    int x1 = _ttoi(strX1);
    int y1 = _ttoi(strY1);
    int x2 = _ttoi(strX2);
    int y2 = _ttoi(strY2);

    // 별도의 스레드에서 작업을 수행합니다.
    std::thread([=]() {
        // 디렉토리 생성
        if (_mkdir("image") == -1)
        {
            if (errno != EEXIST)
            {
                AfxMessageBox(_T("디렉토리 생성 실패"));
                return;
            }
        }

        // 이동 간격 설정
        int step = 10; // TODO: timer 적용

        // 이동하면서 원을 그립니다.
        for (int x = x1, y = y1; x <= x2 && y <= y2; x += step, y += step)
        {
        // TODO: crect가 아닌 cimage로 변경, 저장 방식은 좌표값 포함

            // 그리기 영역을 가져옵니다.
            CRect rect;
            m_imageCtrl.GetClientRect(&rect);
            CDC* pDC = m_imageCtrl.GetDC();

            // 그리기 영역 초기화 (검은색 배경)
            pDC->FillSolidRect(&rect, RGB(0, 0, 0));

            // 원 그리기 (하얀색 내부, 검은색 테두리)
            CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
            CBrush brush(RGB(255, 255, 255));
            CPen* pOldPen = pDC->SelectObject(&pen);
            CBrush* pOldBrush = pDC->SelectObject(&brush);
            pDC->Ellipse(x - 50, y - 50, x + 50, y + 50);
            pDC->SelectObject(pOldPen);
            pDC->SelectObject(pOldBrush);

            // 현재 화면을 캡처하여 파일로 저장
            CImage image;
            image.Create(rect.Width(), rect.Height(), 32);
            CDC* pImageDC = CDC::FromHandle(image.GetDC());
            pImageDC->BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);
            image.ReleaseDC();

            // 파일 이름 생성 (타임스탬프 포함)
            CString fileName;
            CTime time = CTime::GetCurrentTime();
            fileName.Format(_T("image\\image_%04d%02d%02d%02d%02d%02d_%03d.jpg"),
                            time.GetYear(), time.GetMonth(), time.GetDay(),
                            time.GetHour(), time.GetMinute(), time.GetSecond(), x);

            // 파일 저장
            image.Save(fileName, Gdiplus::ImageFormatJPEG);

            // DC를 해제합니다.
            m_imageCtrl.ReleaseDC(pDC);

            // 잠시 대기 (애니메이션 효과)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
}

void CImageDialogAppDlg::OnBnClickedButtonOpen()
{
    CFileDialog dlg(TRUE, _T("Image Files"), NULL, OFN_FILEMUSTEXIST, _T("Images|*.bmp;*.jpg;*.jpeg|All Files|*.*||")); // TODO: all files 선택지 제거
    if (dlg.DoModal() == IDOK)
    {
    // TODO: 좌표값 출력은 파이썬 알고리즘 참고해서 파일명에 저장된 좌표값 가져오는걸로 수정 
        CString path = dlg.GetPathName();

        // 이미지 로드
        CImage image;
        HRESULT hr = image.Load(path);
        if (FAILED(hr))
        {
            AfxMessageBox(_T("이미지를 로드할 수 없습니다."));
            return;
        }

        // 그리기 영역을 가져옵니다.
        CRect rect;
        m_imageCtrl.GetClientRect(&rect);

        // DC(Device Context)를 가져옵니다.
        CDC* pDC = m_imageCtrl.GetDC();

        // 화면을 지웁니다.
        pDC->FillSolidRect(&rect, RGB(240, 240, 240)); // 바탕색 설정

        // 이미지 표시
        image.Draw(pDC->m_hDC, rect);

        // 원 중심의 X 표시 및 좌표 출력
        int x = m_x;
        int y = m_y;
        pDC->MoveTo(x - 10, y);
        pDC->LineTo(x + 10, y);
        pDC->MoveTo(x, y - 10);
        pDC->LineTo(x, y + 10);

        CString strCoords;
        strCoords.Format(_T("(%d, %d)"), x, y);
        pDC->TextOutW(x + 15, y, strCoords);

        // DC를 해제합니다.
        m_imageCtrl.ReleaseDC(pDC);
    }
}

void CImageDialogAppDlg::OnPaint()
{
    if (IsIconic())
    {
    // TODO: OnDraw()로 대체 여부 결정
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
        CDialogEx::OnPaint();
    }
}

void CImageDialogAppDlg::OnBnClickedOk()
{
    // TODO: ok 버튼 제거
    CDialogEx::OnOK();
}

void CImageDialogAppDlg::OnEnChangeEditX1()
{
    // TODO:  editx1 제거
}
