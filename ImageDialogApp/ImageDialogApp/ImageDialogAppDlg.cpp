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
#include <opencv2/opencv.hpp>
using namespace cv;



// #ifdef _DEBUG
// #define new DEBUG_NEW
// #endif

BEGIN_MESSAGE_MAP(CImageDialogAppDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_DRAW, &CImageDialogAppDlg::OnBnClickedButtonDraw)
    ON_BN_CLICKED(IDC_BUTTON_ACTION, &CImageDialogAppDlg::OnBnClickedButtonAction)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, &CImageDialogAppDlg::OnBnClickedButtonOpen)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_EN_CHANGE(IDC_EDIT_X1, &CImageDialogAppDlg::OnEnChangeEditX1)
    
    
END_MESSAGE_MAP()

CImageDialogAppDlg::CImageDialogAppDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_IMAGEDIALOGAPP_DIALOG, pParent), m_radius(0), m_x(0), m_y(0)
    , m_bitmap(nullptr)
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
    m_radius = (rand() % 90) + 20;
    SetIcon(m_hIcon, TRUE);         
    SetIcon(m_hIcon, FALSE);        
    

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


    return TRUE;  
}

CImageDialogAppDlg::~CImageDialogAppDlg()
{
    if (m_bitmap != nullptr)
    {
        delete m_bitmap; 
        m_bitmap = nullptr;
    }
    CDialogEx::OnDestroy();
}

void CImageDialogAppDlg::OnEnChangeEditX1()
{

    if (!m_bFirstWarningShown)
    {
        AfxMessageBox(_T("open cv의 문제로 22이하의 값 입력시 중심좌표를 발견하지 못할 수 있습니다.(중심좌표가 윈도우 밖에 생깁니다)"));
        m_bFirstWarningShown = true; 
    }
}

void CImageDialogAppDlg::OnBnClickedButtonDraw()
{
    
    try
    {
        
            
        CString strX1, strY1;
        m_editX1.GetWindowText(strX1);
        m_editY1.GetWindowText(strY1);
        int x1 = _ttoi(strX1);
        int y1 = _ttoi(strY1);

        CRect crect;
        GetClientRect(&crect); 
        int width = crect.Width()-244;
        int height = crect.Height();
        
        Gdiplus::Bitmap* pBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
        Gdiplus::Graphics graphics(pBitmap);
        graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

        

        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

        width = pBitmap->GetWidth();
        
        int random_radius = m_radius;
        graphics.FillEllipse(&brush, x1 - random_radius, y1 - random_radius, random_radius * 2, random_radius * 2);

        
        if (m_bitmap != nullptr)
        {
            delete m_bitmap; 
        }
        m_bitmap =  pBitmap->Clone(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight(), PixelFormat24bppRGB);
        //사용한 후 해제
        delete pBitmap;


        Invalidate();  

    }
    catch(const std::exception& )
    {
        AfxMessageBox(_T("Invalid input for coordinates."));
    }



}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;         
    UINT size = 0;         

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if(size == 0)
        return -1;  // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL)
        return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j)
    {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }    
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}



void CImageDialogAppDlg::OnBnClickedButtonAction()
{

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    int random_radius = ( rand() % 20)+ 20;

    // 별도의 스레드에서 작업을 수행합니다.
    std::thread([this, gdiplusToken , random_radius]() {
        try {
            CString strX1, strY1, strX2, strY2;
            m_editX1.GetWindowText(strX1);
            m_editY1.GetWindowText(strY1);
            m_editX2.GetWindowText(strX2);
            m_editY2.GetWindowText(strY2);

            int x1 = _ttoi(strX1);
            int y1 = _ttoi(strY1);
            int x2 = _ttoi(strX2);
            int y2 = _ttoi(strY2);

            // 이동할 스텝 설정
            int stepCount = 100; 
            int dx = x2 - x1;
            int dy = y2 - y1;

            double stepSize = 1.0 / stepCount;  // 이동 간격 비율
            for (int i = 0; i <= stepCount; ++i) {
                double t = stepSize * i;  // 현재 비율
                int x= static_cast<int>(x1 + t * dx);
                int y= static_cast<int>(y1 + t * dy);
 
                CRect crect;
                GetClientRect(&crect);
                Gdiplus::Bitmap bitmap(crect.Width()-244, crect.Height(), PixelFormat24bppRGB);
                Gdiplus::Graphics graphics(&bitmap);
                graphics.Clear(Gdiplus::Color(0, 0, 0, 0));



                

                int width = bitmap.GetWidth();
                int height = bitmap.GetHeight();
                

                SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));
                graphics.FillEllipse(&brush, x - random_radius, y - random_radius, random_radius * 2, random_radius * 2);

                
                // 디렉토리 생성
                if (_mkdir("image") == -1)
                {
                    if (errno != EEXIST)
                    {
                        AfxMessageBox(_T("디렉토리 생성 실패"));
                        return;
                    }
                }
                // 이미지 저장
                CString filepath;
                CTime datetime = CTime::GetCurrentTime();
                CString datetimeStr = datetime.Format(_T("%Y%m%d%H%M%S"));
                filepath.Format(_T("image/frame_%s.jpg"), datetimeStr.GetString());
                CLSID clsid;
                GetEncoderClsid(L"image/jpeg", &clsid);
                bitmap.Save(filepath.GetString(), &clsid);


                // 그린 이미지를 다이얼로그에 표시
                if (m_bitmap != nullptr)
                {
                    delete m_bitmap; // 기존 Bitmap 해제
                }
                m_bitmap = bitmap.Clone(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), PixelFormat24bppRGB);


                Invalidate();
               
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

            }
        }
        catch (const std::exception&) {
            AfxMessageBox(_T("Invalid input for coordinates."));
        }
    }).detach();
}
LRESULT CImageDialogAppDlg::OnUpdateBitmap(WPARAM wParam, LPARAM lParam)
{
    if (m_bitmap != nullptr)
    {
        delete m_bitmap; // 기존 Bitmap 해제
    }
    m_bitmap = reinterpret_cast<Gdiplus::Bitmap*>(wParam);


    Invalidate();
    return 0;
}
void CImageDialogAppDlg::OnBnClickedButtonOpen()
{
    CFileDialog dlg(TRUE, _T("Image Files"), NULL, OFN_FILEMUSTEXIST, _T("Images|*.bmp;*.jpg;*.jpeg|All Files|*.*||")); 
    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();

        // CString을 std::string으로 변환
        CT2CA pszConvertedAnsiString(filePath);
        std::string strStd(pszConvertedAnsiString);

        // OpenCV를 사용하여 이미지 불러오기
        cv::Mat src = cv::imread(strStd);
        if (src.empty())
        {
            AfxMessageBox(_T("이미지를 읽을 수 없습니다."));
            return;
        }
        // 이미지를 그레이스케일로 변환
        cv::Mat src_gray;
        cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

        // 블러링 처리
        cv::Mat blurred;
        cv::blur(src_gray, blurred, cv::Size(7, 7));

        // 원 검출
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(blurred, circles, cv::HOUGH_GRADIENT, 1, 30, 150, 60);

        // GDI+ Bitmap으로 변환
        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(src.cols, src.rows, PixelFormat24bppRGB);
       
        // 다이얼로그의 클라이언트 영역 크기 가져오기
        CRect clientRect;
        GetClientRect(&clientRect);

        // 이미지의 중앙 좌표 계산
        int imgX = (clientRect.Width() - 244 - src.cols) / 2;
        int imgY = (clientRect.Height() - src.rows) / 2;

        // 다이얼로그에 이미지 출력
        CClientDC dc(this);
        Gdiplus::Graphics graphics(dc);
        Gdiplus::SolidBrush blackBrush(Gdiplus::Color(0, 0, 0));
        graphics.FillRectangle(&blackBrush, 0, 0, clientRect.Width() - 244, clientRect.Height());
        graphics.DrawImage(bitmap, imgX, imgY, src.cols - 244, src.rows);

        // 원의 중심 좌표에 X 모양 그리기
        Gdiplus::Pen rpen(Gdiplus::Color(255, 0, 0), 2);
        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255));
        Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255), 2);
        for (const auto& c : circles)
        {
            int x = imgX + cvRound(c[0]);
            int y = imgY + cvRound(c[1]);
            int radius = cvRound(c[2]); 
            // x + radius와 y + radius가 이미지 경계를 벗어나지 않도록 조정
            if (x + radius > clientRect.Width() - 50) {
                x = clientRect.Width() - radius - 50;
            }
            if (y + radius > clientRect.Height() - 50) {
                y = clientRect.Height() - radius - 50;
            }

            graphics.FillEllipse(&whiteBrush, x - radius, y - radius, radius * 2, radius * 2);
            graphics.DrawEllipse(&pen, x - radius, y - radius, radius * 2, radius * 2);
            
            // X 모양 그리기
            graphics.DrawLine(&rpen, x - 10, y - 10, x + 10, y + 10); // 대각선 1
            graphics.DrawLine(&rpen, x - 10, y + 10, x + 10, y - 10); // 대각선 2
            // 좌표값 표시
            CString str;
            str.Format(_T("(%d, %d)"), x, y);
            dc.TextOutW(x + 15, y, str);
        }

        // 메모리 해제
        delete bitmap;
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
    if (m_bitmap != nullptr)
    {
        CRect crect;
        GetClientRect(&crect);
        CDC memDC;
        memDC.CreateCompatibleDC(&dc);
        CBitmap bmp;
        bmp.CreateCompatibleBitmap(&dc, crect.Width()-244, crect.Height());
        CBitmap* pOldBmp = memDC.SelectObject(&bmp);

        Gdiplus::Graphics graphics(memDC.m_hDC);
        graphics.DrawImage(m_bitmap, 0, 0, m_bitmap->GetWidth(), m_bitmap->GetHeight());


        dc.BitBlt(0, 0, crect.Width()-224, crect.Height(), &memDC, 0, 0, SRCCOPY);
        memDC.SelectObject(pOldBmp);

    }
    else
    {
        CDialogEx::OnPaint();
    }

    CDialogEx::OnPaint();
}
