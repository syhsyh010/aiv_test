
// MainProjectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MainProject.h"
#include "MainProjectDlg.h"
#include "afxdialogex.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "../IncludeShare/ImageObject.h"
#include "../IncludeShare/Log.h"
#include "Convert.h"

using namespace cv;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BLOCKBYTE 256000000


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


// CMainProjectDlg 대화 상자



CMainProjectDlg::CMainProjectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAINPROJECT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainProjectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONVERTBTN, &CMainProjectDlg::OnBnClickedConvertbtn)
END_MESSAGE_MAP()


// CMainProjectDlg 메시지 처리기

BOOL CMainProjectDlg::OnInitDialog()
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

	// TODO: 여기에 추가 초기화 작업을 추가합니다.


	filesystem::path settingFile("setting.txt");

	// 파일이 존재하는지 확인
	if (!filesystem::exists(settingFile)) 
	{
		AfxMessageBox(L"setting.txt 파일이 존재하지 않습니다.", MB_ICONERROR);
		return TRUE;
	}

	std::ifstream file(settingFile);
	std::string line;
	int step = 0;

	while (std::getline(file, line)) 
	{
		if (line.empty())
			continue;
		

		if (line == "?") 
		{
			step++;
			continue;
		}

		if (step == 0)
			m_ImgFilePaths.push_back(line);
		else if (step == 1) 
		{
			m_KernelSize = std::stoi(line); 
			step++;
		}
		else 
			m_OutputPath = line;
	}

	file.close();
	// 입력 데이터 유효성 검사
	if (m_ImgFilePaths.empty())
	{
		AfxMessageBox(L"이미지 경로를 최소 1개 이상 입력해주세요.", MB_ICONERROR);
		return TRUE;
	}

	if (m_KernelSize < 21)
	{
		AfxMessageBox(L"커널 사이즈는 21 이상이어야 합니다.", MB_ICONERROR);
		return TRUE;
	}

	if (m_OutputPath.empty())
	{
		AfxMessageBox(L"출력 경로를 입력해주세요.", MB_ICONERROR);
		return TRUE;
	}


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMainProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMainProjectDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMainProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMainProjectDlg::WorkerThread(DlgProgress * pDlg, bool isCustom)
{
	m_IsError = Convert::getInstance().Start(pDlg, isCustom, m_ImgFilePaths, m_OutputPath, m_KernelSize);
	if (m_IsError)
		AfxMessageBox(L"오류가 발생했습니다.", MB_ICONERROR);
}

void CMainProjectDlg::ImageCompareThread(DlgProgress* pDlg)
{
	bool sameAll = true;
	bool isSame[100] = { 0 };
	int i;

	pDlg->Init(_T("processing Image Compare..."), 0, (int)m_ImgFilePaths.size());
	Log::GetInstance().WriteStart();
	for (i = 0; i < m_ImgFilePaths.size(); i++)
	{
		std::filesystem::path curPath(m_ImgFilePaths[i]);
		string openCVName = m_OutputPath + "\\" + "OpenCV_" + curPath.filename().string();
		string customName = m_OutputPath + "\\" + "Custom_" + curPath.filename().string();

		cv::Mat img1 = cv::imread(openCVName, cv::IMREAD_UNCHANGED);
		cv::Mat img2 = cv::imread(customName, cv::IMREAD_UNCHANGED);
		cv::Mat diff;
		cv::subtract(img1, img2, diff);
		
		//isSame[i] = equal(img1.begin<uchar>(), img1.end<uchar>(), img2.begin<uchar>());
		isSame[i] = (cv::countNonZero(diff) == 0);


		if (isSame[i] == false)
		{
			if (sameAll == true)
			{
				sameAll = false;
				Log::GetInstance() << "서로 다른 이미지가 있습니다." << endl;
			}

			Log::GetInstance() << curPath.filename() << endl;
			Log::GetInstance() << "MSE : " << calculateMSE(img1, img2) << endl;
			Log::GetInstance() << "SSIM : " << calculateSSIM(img1, img2) << endl << endl;
		}
		pDlg->PostMessage(WM_UPDATE_PROGRESS, 1, 0);
		
	}
	Log::GetInstance().WriteClose();

	pDlg->PostMessage(WM_THREAD_DONE, IDOK, 0);

	if (sameAll)
		AfxMessageBox(L"모든 이미지가 같습니다.", MB_ICONINFORMATION);
	else	
		AfxMessageBox(L"서로 다른 이미지가 있습니다. image_blur.log를 확인해주시기 바랍니다.", MB_ICONWARNING);
	
}


double CMainProjectDlg::calculateSSIM(const cv::Mat& img1, const cv::Mat& img2)
{
	cv::Mat img1f, img2f;
	img1.convertTo(img1f, CV_32F);
	img2.convertTo(img2f, CV_32F);

	// 커널 설정 (GaussianBlur 사용)
	cv::Mat mu1, mu2;
	cv::GaussianBlur(img1f, mu1, cv::Size(11, 11), 1.5);
	cv::GaussianBlur(img2f, mu2, cv::Size(11, 11), 1.5);

	// 평균 계산
	cv::Mat mu1_mu2 = mu1.mul(mu2);

	// 제곱 계산
	cv::Mat mu1_sq = mu1.mul(mu1);
	cv::Mat mu2_sq = mu2.mul(mu2);

	cv::Mat sigma1, sigma2, sigma12;
	// 이미지의 픽셀 값이 평균에서 얼마나 벗어나 있는지
	cv::GaussianBlur(img1f.mul(img1f), sigma1, cv::Size(11, 11), 1.5);
	cv::GaussianBlur(img2f.mul(img2f), sigma2, cv::Size(11, 11), 1.5);


	// 두 이미지가 얼마나 함께 변하는지
	cv::GaussianBlur(img1f.mul(img2f), sigma12, cv::Size(11, 11), 1.5);

	sigma1 -= mu1_sq;
	sigma2 -= mu2_sq;
	sigma12 -= mu1_mu2;

	// SSIM 계산
	const double C1 = 6.5025, C2 = 58.5225;

	// 밝기 비교
	cv::Mat numerator = (2 * mu1_mu2 + C1).mul(2 * sigma12 + C2);

	// 명암 비교
	cv::Mat denominator = (mu1_sq + mu2_sq + C1).mul(sigma1 + sigma2 + C2);

	cv::Mat ssim_map;
	cv::divide(numerator, denominator, ssim_map);

	return cv::mean(ssim_map)[0];
}
double CMainProjectDlg::calculateMSE(const cv::Mat& img1, const cv::Mat& img2)
{
	cv::Mat diff;

	// 절대 차이 계산
	cv::absdiff(img1, img2, diff);

	// 범위 안벗어나게
	diff.convertTo(diff, CV_32F);

	// 차이의 제곱
	diff = diff.mul(diff);

	// MSE 계산
	double mse = cv::mean(diff)[0];
	return mse;
}


void CMainProjectDlg::OnBnClickedConvertbtn()
{
	m_IsError = false;
	DlgProgress progressDlg , progressDlg2, progressDlg3;	
	
	thread openCVThread(&CMainProjectDlg::WorkerThread, this, &progressDlg, false);
	progressDlg.DoModal();
	openCVThread.join();
	if (m_IsError)
		return;
	
	thread customThread(&CMainProjectDlg::WorkerThread, this, &progressDlg2, true);
	progressDlg2.DoModal();
	customThread.join();
	if (m_IsError)
		return;
	
	thread imageCompareThread(&CMainProjectDlg::ImageCompareThread, this, &progressDlg3);
	progressDlg3.DoModal();
	imageCompareThread.join();
}
