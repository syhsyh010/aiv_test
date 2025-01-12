#pragma once
#include "afxdialogex.h"
#include <mutex>

#define WM_UPDATE_PROGRESS (WM_USER + 1)
#define WM_THREAD_DONE (WM_USER + 2)
using namespace std;


// DlgProgress 대화 상자

class DlgProgress : public CDialogEx
{
	DECLARE_DYNAMIC(DlgProgress)

public:
	DlgProgress(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	void Init(LPCTSTR text, int start, int end);
	virtual ~DlgProgress();


	afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThreadDone(WPARAM wParam, LPARAM lParam);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ctlProgressBar;

private:
	int			m_Start;
	int			m_End;
	mutex		m_Mutex;
	CStatic		m_ctlText;
	LPCTSTR		m_LeftText;
public:
	virtual BOOL OnInitDialog();
};
