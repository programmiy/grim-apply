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

// HACK: 4일간 나를 괴롭혔던 오류 해결 완료
// FIXME:  디버그 >> release로 변경

// HACK: 파이썬으로 알고리즘을 한번 구현을 하니 mfc로 구현하는데 어려움이 없어짐
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
    
    
END_MESSAGE_MAP()

CImageDialogAppDlg::CImageDialogAppDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_IMAGEDIALOGAPP_DIALOG, pParent), m_radius(0), m_x(0), m_y(0)
    , m_Bitmap(nullptr)
    , gdiplusToken(0)
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
    
}


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
    m_radius = (rand() % 90) + 10;
    SetIcon(m_hIcon, TRUE);         // 큰 아이콘을 설정합니다.
    SetIcon(m_hIcon, FALSE);        // 작은 아이콘을 설정합니다.
    

    // 컨트롤 초기화
    if (!m_editX1.SubclassDlgItem(IDC_EDIT_X1, this) ||
        !m_editY1.SubclassDlgItem(IDC_EDIT_Y1, this) ||
        !m_editX2.SubclassDlgItem(IDC_EDIT_X2, this) ||
        !m_editY2.SubclassDlgItem(IDC_EDIT_Y2, this))
    {
        AfxMessageBox(_T("컨트롤 초기화 실패"));
        return FALSE;
    }


    // 레이블 초기화
    SetDlgItemText(IDC_STATIC_X1_LABEL, _T("X1:"));
    SetDlgItemText(IDC_STATIC_Y1_LABEL, _T("Y1:"));
    SetDlgItemText(IDC_STATIC_X2_LABEL, _T("X2:"));
    SetDlgItemText(IDC_STATIC_Y2_LABEL, _T("Y2:"));


    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

CImageDialogAppDlg::~CImageDialogAppDlg()
{
    if (m_Bitmap != nullptr)
    {
        delete m_Bitmap; // 메모리 해제
        m_Bitmap = nullptr;
    }
    CDialogEx::OnDestroy();



}



void CImageDialogAppDlg::OnBnClickedButtonDraw()
{
    // 별도의 스레드에서 작업을 수행합니다.
    std::thread([this]() {
        try
        {
            // TODO: 파이썬으로 구현한 알고리즘 토대로 사용하기 위해 전부 삭제, 최우선 개발 사항
            CString strX1, strY1;
            m_editX1.GetWindowTextW(strX1);
            m_editY1.GetWindowTextW(strY1);
            int x1 = _ttoi(strX1);
            int y1 = _ttoi(strY1);

            CRect crect;
            GetClientRect(&crect); 

            // GDI+ Bitmap 생성
            Gdiplus::Bitmap bitmap(crect.Width() - 244, crect.Height(), PixelFormat24bppRGB);
            Gdiplus::Graphics graphics(&bitmap);
            graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

            Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

            int width = bitmap.GetWidth();
            int proportional_radius = min(width, crect.Height()) / 20;  // 이미지 크기의 1/20로 설정
            int random_radius = (m_radius + proportional_radius) / 2;  // 두 값의 평균
            graphics.FillEllipse(&brush, x1 - random_radius, y1 - random_radius, random_radius * 2, random_radius * 2);

            // 그린 이미지를 다이얼로그에 표시
            if (m_Bitmap != nullptr)
            {
                delete m_Bitmap; // 기존 Bitmap 해제
            }
            m_Bitmap = bitmap.Clone(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), PixelFormat24bppRGB);

            // UI 업데이트는 메인 스레드에서 수행해야 합니다.
            PostMessage(WM_PAINT);
        }
        catch(const std::exception&)
        {
            AfxMessageBox(_T("Invalid input for coordinates."));
        }
    }).detach(); // 스레드를 분리하여 백그라운드에서 실행되도록 합니다.
}

void CImageDialogAppDlg::OnBnClickedButtonAction()
{
    // TODO: 파이썬 알고리즘, rect 방식 응용해서 수정할 계획, 저장
    // 핵심은 원의 출력, 저장, 초기화, 새 원 출력의 구조
    // 스레드 작업이 필수

    CString strX1, strY1, strX2, strY2;
    m_editX1.GetWindowText(strX1);
    m_editY1.GetWindowText(strY1);
    m_editX2.GetWindowText(strX2);
    m_editY2.GetWindowText(strY2);

    int x1 = _ttoi(strX1);
    int y1 = _ttoi(strY1);
    int x2 = _ttoi(strX2);
    int y2 = _ttoi(strY2);
    Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));


    // 별도의 스레드에서 작업을 수행합니다.
    std::thread([=]() {
        CRect crect;
        GetClientRect(&crect); 
        // 이미 완벽하니 수정하지 말것
        Gdiplus::Bitmap bitmap(crect.Width()-244, crect.Height(), PixelFormat24bppRGB);
        Gdiplus::Graphics graphics(&bitmap);
        graphics.Clear(Color(0, 0, 0, 0));
        int width = bitmap.GetWidth();
        int height = bitmap.GetHeight();
        int proportional_radius = min(width, height) / 20;  // 이미지 크기의 1/20로 설정
        int random_radius = (m_radius + proportional_radius) / 2;  // 두 값의 평균

        SolidBrush brush(Color(255, 255, 255, 255));
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
        int step = 10; // TODO: timer 적용 안해도 될지도

        // 이동하면서 원을 그립니다.
        for (int x = x1, y = y1; x <= x2 && y <= y2; x += step, y += step)
        {
        // TODO: crect가 아닌 cimage로 변경, 저장 방식은 좌표값 포함

            graphics.FillEllipse(&brush, x - random_radius, y - random_radius, random_radius * 2, random_radius * 2);


            // image save
            CImage image;
            image.Create(1280, 720, 24);
            HDC hdc = image.GetDC();
            Graphics imageGraphics(hdc);
            imageGraphics.DrawImage(&bitmap, 0, 0, 1280, 720);
            image.ReleaseDC();

 
            // 파일 이름 생성 (타임스탬프 포함)
            CString fileName;
            CTime time = CTime::GetCurrentTime();
            fileName.Format(_T("image\\image_%04d%02d%02d%02d%02d%02d_%03d.jpg"),
                            time.GetYear(), time.GetMonth(), time.GetDay(),
                            time.GetHour(), time.GetMinute(), time.GetSecond(), x);

            // // 파일 저장
            // image.Save(fileName, Gdiplus::ImageFormatJPEG);

            // bitmap 초기화
            graphics.Clear(Color(0, 0, 0, 0));

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
    // TODO: 좌표값 출력은 파일명 의존이 아니라 아래코드 참조해서(인터넷 검색한 코드) opencv로 검출 예정 
        // #include <opencv2/opencv.hpp>
        // #include <iostream>

        // int main()
        // {
        //     // 이미지 읽기
        //     cv::Mat image = cv::imread("path_to_your_image.png", cv::IMREAD_COLOR);
        //     if (image.empty())
        //     {
        //         std::cerr << "이미지를 읽을 수 없습니다." << std::endl;
        //         return -1;
        //     }

        //     // 그레이스케일로 변환
        //     cv::Mat gray;
        //     cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

        //     // 블러링 처리
        //     cv::Mat blurred;
        //     cv::medianBlur(gray, blurred, 5);

        //     // 허프 변환을 사용하여 원 감지
        //     std::vector<cv::Vec3f> circles;
        //     cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1.2, 100, 100, 30, 0, 0);

        //     // 원이 감지되었는지 확인
        //     if (!circles.empty())
        //     {
        //         for (size_t i = 0; i < circles.size(); i++)
        //         {
        //             cv::Vec3i c = circles[i];
        //             cv::Point center = cv::Point(c[0], c[1]);
        //             int radius = c[2];

        //             // 원의 중심 좌표 출력
        //             std::cout << "원 중심 좌표: (" << center.x << ", " << center.y << "), 반지름: " << radius << std::endl;

        //             // 원의 중심과 경계 그리기
        //             cv::circle(image, center, radius, cv::Scalar(0, 255, 0), 4);
        //             cv::rectangle(image, cv::Point(center.x - 5, center.y - 5), cv::Point(center.x + 5, center.y + 5), cv::Scalar(0, 128, 255), -1);
    //     }
    // }

//     // 결과 이미지 저장
//     cv::imwrite("output_image.png", image);

//     // 결과 이미지 표시
//     cv::imshow("output", image);
//     cv::waitKey(0);

//     return 0;
// }
    }
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

void CImageDialogAppDlg::OnPaint()
{
    CPaintDC dc(this);
    if (m_Bitmap != nullptr)
    {
        Graphics graphics(dc);
        graphics.DrawImage(m_Bitmap, 0, 0);
    }

    CDialogEx::OnPaint();
}


