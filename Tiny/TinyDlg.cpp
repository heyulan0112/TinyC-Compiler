// TinyDlg.cpp: 实现文件
//
#include "framework.h"
#include "Tiny.h"
#include "TinyDlg.h"
#include "globals.h"
#include "afxdialogex.h"
#include<iostream>
#include<fstream>
#include<string>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma warning(disable : 4996)
#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"

#endif

 /* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;
int Error = TRUE;

int linepos = 0; /* current position in LineBuf */
int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */
char lineBuf[256]; /* holds the current line */
int bufsize = 0; /* current size of buffer string */

TreeNode * syntaxTree;
char pgm[120];


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTinyDlg 对话框



CTinyDlg::CTinyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TINY_DIALOG, pParent)
	, m_input(_T(""))
	, m_result(_T(""))
	, m_tree(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTinyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_input);
	DDX_Text(pDX, IDC_EDIT2, m_result);
	DDX_Control(pDX, IDC_EDIT2, CResult);
	DDX_Control(pDX, IDC_EDIT3, CTree);
	DDX_Text(pDX, IDC_EDIT3, m_tree);
	DDX_Control(pDX, IDC_EDIT1, CInput);
}

BEGIN_MESSAGE_MAP(CTinyDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTinyDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTinyDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTinyDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTinyDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CTinyDlg 消息处理程序

BOOL CTinyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTinyDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTinyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTinyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTinyDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码  submmit

	source = fopen(pgm, "r");
	if (source == NULL)
	{
		fprintf(stderr, "File %s not found\n", pgm);
		exit(1);
	}
	listing = fopen("compiler.txt", "w+"); 
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE
	while (getToken() != ENDFILE);
#else
	syntaxTree = parse();
	fclose(source);
	fclose(listing);
#endif
	CFile fp;
	fp.Open("compiler.txt", CFile::modeRead);
	DWORD sz = fp.GetLength();
	char* buffer_src = new char[sz + 1];
	buffer_src[sz] = '\0';
	fp.Read(buffer_src, sz);
	CResult.SetWindowText(buffer_src);
	fp.Close();
}

// TODO: 在此添加控件通知处理程序代码   print
void CTinyDlg::OnBnClickedButton2()
{
		source = fopen(pgm, "r");
		if (source == NULL)
		{
			fprintf(stderr, "File %s not found\n", pgm);
			exit(1);
		}
		listing = fopen("compiler.txt", "w+");
		fprintf(listing, "\nTINY COMPILATION: %s\n",pgm);
	#if NO_PARSE
		while (getToken() != ENDFILE);
	#else
		if (syntaxTree == NULL)
			syntaxTree = parse();
		if (TraceParse) {
			fprintf(listing, "\nSyntax tree:\n");
			printTree(syntaxTree);
		}
		fclose(source);
	#endif
		fclose(listing);
		CFile fp;
		fp.Open("compiler.txt", CFile::modeRead);
		DWORD sz = fp.GetLength();
		char* buffer_src = new char[sz + 1];
		buffer_src[sz] = '\0';
		fp.Read(buffer_src, sz);
		CTree.SetWindowText(buffer_src);
		fp.Close();
}

CString GetFileName(CString filePath, BOOL isWithFileSuffix)
{
	int pos = filePath.ReverseFind('\\');
	CString fileName = filePath.Right(filePath.GetLength() - pos - 1);
	if (FALSE == isWithFileSuffix)
	{
		pos = fileName.Find('.');
		fileName = fileName.Left(pos);
	}
	return fileName;
}

void CTinyDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码  open
	GetDlgItem(IDC_EDIT1)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT2)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT3)->SetWindowText(_T(""));

	linepos = 0;
	EOF_flag = FALSE;
	bufsize = 0;
	lineno = 0;
	for (int i = 0; i < 256; i++) {
		lineBuf[i]='\0';
	}


	BOOL isOpen = TRUE;      
	CString defaultDir = "E:\\";  
	CString fileName = "";         
	CString filter = "文件 *.TNY";  
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_READONLY, NULL, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath;
	CString name;
	if (result == IDOK) 
	{
		filePath = openFileDlg.GetPathName();
		name = GetFileName(filePath,true);
		(LPTSTR)(LPCTSTR)name;
		for (int i = 0; i<name.GetLength(); i++) 
		{
			pgm[i] = name[i];
		}
	}
	CFile fp1;
	fp1.Open(pgm, CFile::modeRead);
	DWORD sz = fp1.GetLength();
	char* buffer_src1 = new char[sz + 1];
	buffer_src1[sz] = '\0';
	fp1.Read(buffer_src1, sz);
	CInput.SetWindowText(buffer_src1);
	fp1.Close();
}


void CTinyDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码  save
	CStdioFile file;
	BOOL isOpen = FALSE;			
	CString defaultDir = "";
	CString fileName ="";  
	CString filter = "文件 (*.txt; *.ppt; *.xls)|*.txt;*.ppt;*.xls||";
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL);
	INT_PTR result = openFileDlg.DoModal();
	CString filePath = defaultDir + "\\" + fileName;	
	CString str;	
	int *p = (int*)malloc(100 * sizeof(int));	
	for (int i = 0; i < 100; i++)	
	{		
		p[i] = i;	
	}	
	if (result == IDOK) 
	{		
		filePath = openFileDlg.GetPathName();
		file.Open(filePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		GetDlgItemText(IDC_EDIT1, str);
		file.WriteString(str);
		file.Close();
	}		
}