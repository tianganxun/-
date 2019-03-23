// ictCtrlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ictCtrl.h"
#include "ictCtrlDlg.h"
#include "fstream"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

														// ʵ��
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


// CictCtrlDlg �Ի���




CictCtrlDlg::CictCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CictCtrlDlg::IDD, pParent)
	//, m_strSentence(_T("Ȼ����������޹�m_pContext�����е�����"))
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


// CictCtrlDlg ��Ϣ�������

BOOL CictCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
									// TODO: �ڴ���Ӷ���ĳ�ʼ������	// �ֵ�ѡ��0Ϊ�����ֵ䣻1Ϊ���ֵ䣻2Ϊnr;3Ϊns;4Ϊtr

									//���ó���Ϊ500���ص�ˮƽ������
	SendDlgItemMessage(IDC_LIST_D, LB_SETHORIZONTALEXTENT, 500, 0);

	m_pPosTag = new CPosTag();
	CFile file;
	if (file.Open("data\\test.txt", CFile::modeRead)) {
		int nLen = file.GetLength();
		if (nLen>320000)
		{
			AfxMessageBox("txt�ļ����ܴ���32K");
			return 0;
		}
		char* pData = new char[nLen];
		file.Read(pData, nLen);
		m_strSentence = pData;
		file.Close();
		delete pData;
	}
	UpdateData(false);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CictCtrlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	m_strResult += "ѵ�����";
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
	int nTWords = 0;//��ԵĴ���
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
	str.Format("��������=%d��ȫ�Եľ���=%d,���Ա��=%d��,׼ȷ��=%f%%\r\n", nTotal, nTotal - nErrors, nPosError, (nTWords - nPosError)*100.0 / nTWords);
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
