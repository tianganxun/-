// ictCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ictCtrl.h"
#include "ictCtrlDlg.h"
#include "fstream"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CictCtrlDlg 对话框




CictCtrlDlg::CictCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CictCtrlDlg::IDD, pParent)
	//, m_strSentence(_T("然后读上下文无关m_pContext链表中的内容"))
	, m_strResult(_T(""))
	, m_nMaxBest(0)
	, m_nStartPoint(0)
	, m_nSamples(2000)
	, m_nFreq(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CictCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_SENTENSE, m_strSentence);
	DDX_Text(pDX, IDC_EDT_RESULT, m_strResult);
	DDX_Text(pDX, IDC_EDIT2, m_nStartPoint);
	DDX_Text(pDX, IDC_EDIT3, m_nSamples);
}

BEGIN_MESSAGE_MAP(CictCtrlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_TRAIN, &CictCtrlDlg::OnBnClickedBtnTrain)
	ON_BN_CLICKED(IDC_BTN_POSTEST, &CictCtrlDlg::OnBnClickedBtnPostest)
	ON_BN_CLICKED(IDC_BTN_POS, &CictCtrlDlg::OnBnClickedBtnPos)
END_MESSAGE_MAP()


// CictCtrlDlg 消息处理程序

BOOL CictCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
									// TODO: 在此添加额外的初始化代码	// 字典选择，0为核心字典；1为大字典；2为nr;3为ns;4为tr

									//设置长度为500像素的水平滚动条
	SendDlgItemMessage(IDC_LIST_D, LB_SETHORIZONTALEXTENT, 500, 0);

	m_pPosTag = new CPosTag();
	CFile file;
	if (file.Open("data\\test.txt", CFile::modeRead)) {
		int nLen = file.GetLength();
		if (nLen>320000)
		{
			AfxMessageBox("txt文件不能大于32K");
			return 0;
		}
		char* pData = new char[nLen];
		file.Read(pData, nLen);
		m_strSentence = pData;
		file.Close();
		delete pData;
	}
	UpdateData(false);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CictCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CictCtrlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CictCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CictCtrlDlg::OnDestroy()
{
	CDialog::OnDestroy();

}

void CictCtrlDlg::OnBnClickedBtnTrain()
{
	UpdateData();
	m_strResult.Empty();
	CStringArray arrstr;
	m_strResult = m_pPosTag->IndexSample();
	if (m_strResult.IsEmpty())
		return;
	m_pPosTag->Train();
	m_strResult += "训练完成";
	UpdateData(false);
}

void CictCtrlDlg::OnBnClickedBtnPostest()
{
	using namespace std;
	if (!m_pPosTag->IsReady())
		return;
	UpdateData();
	CString str;
	m_strResult.Empty();
	ifstream inFile;
	inFile.open("data\\199801.txt");

	char pChar[10240];
	int nErrors = 0;
	int nTWords = 0;//标对的词数
	int nPosError = 0;
	int j, nTotal = 0;
	while (!inFile.eof() && nTotal++<m_nStartPoint)//
		inFile.getline(pChar, 10240);
	nTotal = 0;
	inFile.getline(pChar, 10240);
	while (!inFile.eof() && nTotal<m_nSamples) {//
		if (strlen(pChar)>0) {
			nTWords += m_pPosTag->VerifyPos(pChar);
			nPosError += m_pPosTag->m_nErrors;
			if (m_pPosTag->m_nErrors>0)
				nErrors++;
			nTotal++;
		}
		inFile.getline(pChar, 10240);
	}
	str.Format("总例句数=%d：全对的句数=%d,词性标错=%d个,准确率=%f%%\r\n", nTotal, nTotal - nErrors, nPosError, (nTWords - nPosError)*100.0 / nTWords);
	m_strResult += str;
	UpdateData(false);
	return;
}


void CictCtrlDlg::OnBnClickedBtnPos()
{
	if (!m_pPosTag->IsReady())
		return;
	UpdateData();
	m_strResult.Empty();
	CString str;
	char* pCh = m_strSentence.GetBuffer();
	int nLen = m_strSentence.GetLength();
	if (nLen>4028) {
		return;
	}
	int nStart = 0;
	int nCount = 0;
	for (int i = 0; i<nLen; i++) {
		if ((pCh[i] == ' ' || pCh[i] == 0x0d || pCh[i] == 0x0a) && i>nStart) {
			strncpy_s(m_pPosTag->m_pResult[nCount++].sWord, pCh + nStart, i - nStart);
			while (pCh[i + 1] == ' ' || pCh[i + 1] == 0x0d || pCh[i + 1] == 0x0a)
				i++;
			nStart = i + 1;
		}
	}
	m_pPosTag->POSTagging(m_pPosTag->m_pResult, nCount);
	char sPOS[4];
	sPOS[0] = '\\';
	sPOS[3] = 0;
	m_strResult += str;
	for (int i = 0; i<nCount; i++) {
		sPOS[1] = m_pPosTag->m_pResult[i].nHandle / 256;
		sPOS[2] = m_pPosTag->m_pResult[i].nHandle % 256;
		m_strResult += m_pPosTag->m_pResult[i].sWord;
		m_strResult += sPOS;
		m_strResult += "\r\n";
	}
	UpdateData(false);
}
