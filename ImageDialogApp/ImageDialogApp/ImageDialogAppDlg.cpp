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

// TODO: open 마무리후 필요한 코드만 남기고 가독성 향상을 위해 규칙을 세워 코드간 간격 확보, 주석 추가 예정


// TODO: 4일간 나를 괴롭혔던 오류 해결 완료
// 디버그 >> release로 변경

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
    if (m_bitmap != nullptr)
    {
        delete m_bitmap; // 메모리 해제
        m_bitmap = nullptr;
    }
    CDialogEx::OnDestroy();
}



void CImageDialogAppDlg::OnBnClickedButtonDraw()
{
    try
    {
        // 작업완료
            
        CString strX1, strY1;
        m_editX1.GetWindowText(strX1);
        m_editY1.GetWindowText(strY1);
        int x1 = _ttoi(strX1);
        int y1 = _ttoi(strY1);

        CRect crect;
        GetClientRect(&crect); 
        int width = crect.Width()-244;
        int height = crect.Height();
        // GDI+ Bitmap 생성
        Gdiplus::Bitmap bitmap(width, height, PixelFormat24bppRGB);
        Gdiplus::Graphics graphics(&bitmap);
        graphics.Clear(Gdiplus::Color(0, 0, 0, 0));

        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255, 255));

        width = bitmap.GetWidth();
        
        int random_radius = m_radius;
        graphics.FillEllipse(&brush, x1 - random_radius, y1 - random_radius, random_radius * 2, random_radius * 2);


        // 그린 이미지를 다이얼로그에 표시
        if (m_bitmap != nullptr)
        {
            delete m_bitmap; // 기존 Bitmap 해제
        }
        m_bitmap = bitmap.Clone(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), PixelFormat24bppRGB);


        Invalidate();  // 다이얼로그를 다시 그리도록 요청
    }
    catch(const std::exception& )
    {
        AfxMessageBox(_T("Invalid input for coordinates."));
    }



}


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;          // number of image encoders
    UINT size = 0;         // size of the image encoder array in bytes

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
// 작업완료

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);



    // 별도의 스레드에서 작업을 수행합니다.
    std::thread([this, gdiplusToken]() {
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

            double dx = (x2 - x1) / 50.0;
            double dy = (y2 - y1) / 50.0;
            std::vector<std::pair<int, int>> steps;
            for (int i = 0; i <= 50; ++i) {
                steps.push_back({ static_cast<int>(x1 + i * dx), static_cast<int>(y1 + i * dy) });
            }           
            for (const auto& step : steps) {
                int x = step.first;
                int y = step.second;
                CRect crect;
                GetClientRect(&crect);
                Gdiplus::Bitmap bitmap(crect.Width()-244, crect.Height(), PixelFormat24bppRGB);
                Gdiplus::Graphics graphics(&bitmap);
                graphics.Clear(Gdiplus::Color(0, 0, 0, 0));



                

                int width = bitmap.GetWidth();
                int height = bitmap.GetHeight();
                int random_radius = m_radius;

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
                // PostMessage(WM_USER_UPDATE_BITMAP, reinterpret_cast<WPARAM>(bitmap.Clone(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), PixelFormat24bppRGB)), 0);  
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
    CFileDialog dlg(TRUE, _T("Image Files"), NULL, OFN_FILEMUSTEXIST, _T("Images|*.bmp;*.jpg;*.jpeg|All Files|*.*||")); // TODO: all files 선택지 제거
    if (dlg.DoModal() == IDOK)
    {
        CString filePath = dlg.GetPathName();

        // CString을 std::string으로 변환
        CT2CA pszConvertedAnsiString(filePath);
        std::string strStd(pszConvertedAnsiString);

        // OpenCV를 사용하여 이미지 불러오기
        cv::Mat img = cv::imread(strStd);
        if (img.empty())
        {
            AfxMessageBox(_T("이미지를 읽을 수 없습니다."));
            return;
        }

        // 이미지를 GDI+ Bitmap으로 변환
        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(img.cols, img.rows, img.step, PixelFormat24bppRGB, img.data);

        // 다이얼로그에 이미지 출력
        CClientDC dc(this);
        Gdiplus::Graphics graphics(dc);
        graphics.DrawImage(bitmap, 0, 0, img.cols, img.rows);

        // 원의 중심 좌표에 X 모양 그리기
        int x = img.cols / 2;
        int y = img.rows / 2;
        Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0), 2);
        graphics.DrawLine(&pen, x - 10, y, x + 10, y);
        graphics.DrawLine(&pen, x, y - 10, x, y + 10);

        // 좌표값 표시
        CString str;
        str.Format(_T("(%d, %d)"), x, y);
        dc.TextOutW(x + 15, y, str);

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
        // BUG:  캔버스 영향을 주는 crect.Width()에 일괄적으로 244를 빼주게 함
        // 아래의 코드를 전 함수에 적용해서 발견
        // CString msg;
        // msg.Format(_T("Width: %d"), crect.Width());
        // AfxMessageBox(msg);
    }
    else
    {
        CDialogEx::OnPaint();
    }

    CDialogEx::OnPaint();
}


