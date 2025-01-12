// DlgProgress.cpp: 구현 파일
//

#include "pch.h"
#include "MainProject.h"
#include "afxdialogex.h"
#include "DlgProgress.h"
using namespace std;

// DlgProgress 대화 상자

IMPLEMENT_DYNAMIC(DlgProgress, CDialogEx)

DlgProgress::DlgProgress(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGRESS, pParent)
{
    m_Start = 0; 
    m_End = 0;
    m_LeftText = L"";
}


DlgProgress::~DlgProgress()
{
}

void DlgProgress::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESSBAR, m_ctlProgressBar);
    DDX_Control(pDX, IDC_TEXT, m_ctlText);
}


BEGIN_MESSAGE_MAP(DlgProgress, CDialogEx)
    ON_MESSAGE(WM_UPDATE_PROGRESS, &DlgProgress::OnUpdateProgress)
    ON_MESSAGE(WM_THREAD_DONE, &DlgProgress::OnThreadDone)
END_MESSAGE_MAP()


BOOL DlgProgress::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    ModifyStyle(WS_SYSMENU, 0);

    CString progressText;
    m_ctlProgressBar.SetRange(m_Start, m_End);
    m_ctlProgressBar.SetPos(m_Start);
    progressText.Format(L"%s [%d / %d]", m_LeftText, m_Start, m_End);
    m_ctlText.SetWindowText((LPCTSTR)progressText);

    return TRUE;
}

LRESULT DlgProgress::OnUpdateProgress(WPARAM wParam, LPARAM lParam)
{
    // 뮤텍스로 잠궈줘야 함
    lock_guard<mutex> lock(m_Mutex);

    int increment = static_cast<int>(wParam);
    CString progressText;
    m_ctlProgressBar.OffsetPos(increment);
    progressText.Format(L"%s [%d / %d]", m_LeftText, m_ctlProgressBar.GetPos(), m_End);
    m_ctlText.SetWindowText(progressText);
    return 0;
}

LRESULT DlgProgress::OnThreadDone(WPARAM wParam, LPARAM lParam)
{
    EndDialog(IDOK);
    return 0;
}

void DlgProgress::Init(LPCTSTR text, int start, int end)
{
    m_Start = start, m_End = end;
    m_LeftText = text;
}