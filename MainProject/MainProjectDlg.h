
// MainProjectDlg.h: 헤더 파일
//

#pragma once
#include "../IncludeShare/ImageObject.h"
#include "DlgProgress.h"

// CMainProjectDlg 대화 상자
class CMainProjectDlg : public CDialogEx
{
// 생성입니다.
public:
	CMainProjectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINPROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void WorkerThread(DlgProgress* pDlg, bool isCustom);
	void ImageCompareThread(DlgProgress* pDlg);
	double calculateSSIM(const cv::Mat& img1, const cv::Mat& img2);
	double calculateMSE(const cv::Mat& img1, const cv::Mat& img2);
	void OnBnClickedConvertbtn();

private:

	vector<string>			m_ImgFilePaths;
	string					m_OutputPath;
	int						m_KernelSize;
	bool					m_IsError;
};
