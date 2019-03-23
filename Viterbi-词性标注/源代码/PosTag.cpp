#include "PosTag.h"
//#include "PathFind.h"
#include "fstream"
#include "math.h"
using namespace std;

  CPosTag(void)
{
	m_nTags[0][0] = 0;//Begin tag
	m_bModify = false;
	m_nTags[0][1] = -1;
	m_strDataFileName = "Pos.dat";
	m_lstStrTypes.InitHashTable(47);
	m_lstStrWords.InitHashTable(56473);

	CFile f;
	static char pOpenFilebuf[20480] = { 0 };
	if (!f.Open(m_strDataFileName, CFile::modeNoTruncate | CFile::modeCreate | CFile::modeRead)) {
#ifdef _DEBUG
		MessageBox(NULL, "Unable to open Data file ", "", MB_OK);
		exit(1);
#endif
	}
	CArchive ar(&f, CArchive::load, 10240, pOpenFilebuf);
	int nLen = f.GetLength();
	if (nLen == 0)//.FindFile(strMsnAccounts))
	{
		m_nCurTypeNum = 0;
		m_nCurWordsNum = 0;
	}
	else
	{
		Serialize(ar);
		Train();
	}

	ar.Close();
	f.Close();
}

 
void   Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_nCurTypeNum;				//训练集中已得不同词类型个数
		ar << m_nCurWordsNum;				//训练集中已得不同词语个数
											// 记录词性作为句首词的次数
		ar.Write(m_fStart, sizeof(m_fStart));
		ar.Write(m_fTypeTwoWords, sizeof(m_fTypeTwoWords));

		for (int i = 0; i<m_nCurTypeNum; i++) {
			ar << m_strTypes[i];
			//	ar<<m_fStart[i];
			for (int j = 0; j<m_nCurWordsNum; j++)
				ar << m_fTableW2T[i][j];
			for (int k = 0; k<m_nCurTypeNum; k++)
				ar << m_fTypeTwoWords[i][k];
		}
		m_lstStrTypes.Serialize(ar);//词性列表
		m_lstStrWords.Serialize(ar);//词列表

	}
	else {
		ar >> m_nCurTypeNum;				//训练集中已得不同词类型个数
		ar >> m_nCurWordsNum;				//训练集中已得不同词语个数
											// 记录词性作为句首词的次数
		ar.Read(m_fStart, sizeof(m_fStart));
		ar.Read(m_fTypeTwoWords, sizeof(m_fTypeTwoWords));
		for (int i = 0; i<m_nCurTypeNum; i++) {
			ar >> m_strTypes[i];
			//	ar>>m_fStart[i];
			for (int j = 0; j<m_nCurWordsNum; j++)
				ar >> m_fTableW2T[i][j];
			for (int k = 0; k<m_nCurTypeNum; k++)
				ar >> m_fTypeTwoWords[i][k];
		}
		m_lstStrTypes.Serialize(ar);//词性列表
		m_lstStrWords.Serialize(ar);//词列表

	}
}

//检索样本数据
CString    IndexSample()
{
	//初始化数组
	m_nCurTypeNum = 0;
	m_nCurWordsNum = 0;
	memset(m_fTableW2T, 0, sizeof(m_fTableW2T));
	memset(m_fStart, 0, sizeof(m_fStart));
	memset(m_fTypeTwoWords, 0, sizeof(m_fTypeTwoWords));
	memset(m_nTypeSum, 0, sizeof(m_nTypeSum));
	m_lstStrTypes.RemoveAll();
	m_lstStrWords.RemoveAll();

	ifstream inFile;
	inFile.open("data\\199801.txt");
	if (!inFile) {
		MessageBox(NULL, "语料库文件不存在", "", MB_OK);
		return "";
	}
	char pChar[10240];
	char sPos[3];
	sPos[2] = 0;
	int j, nTotal = 0;
	//先设定符号类型
	m_strTypes[0] = "w";
	m_lstStrTypes["w"] = m_nCurTypeNum++;

	nTotal = 0;
	inFile.getline(pChar, 10240);
	while (!inFile.eof()) {//
		int nLen = strlen(pChar);
		if (nLen == 0) {
			inFile.getline(pChar, 10240);
			continue;
		}
		int nWords = 0;//词序号
		int nType = 0;//词性序号
		int nLastType = -1;//上一词的词性
		int nStart = 0, nEnd;
		bool bPart = false;//部门标志
		bool bFrist = true; //句首标志
		int nName = 0;//名字连继出现的次数
		bool bOrder = true;//第一个序号不要
		for (int i = 0; i<nLen; i++) {
			if (pChar[i] == ' ') {//发现空格分割符
				if (bOrder) {
					//去除后面重复的空格
					while (pChar[i + 1] == ' ')
						i++;
					//去除机构前标识
					if (pChar[i + 1] == '[')
						nStart = i + 2;
					else
						nStart = i + 1;
					bOrder = false;
					continue;
				}
				pChar[i] = 0;
				nEnd = i - 1;
				//去除机构nt的后标识  另外还有ns和nz
				if (pChar[nEnd - 1] == 'n'&&pChar[nEnd - 2] == ']') {
					pChar[nEnd - 2] = 0;
					nEnd = nEnd - 3;
					bPart = true;
				}
				//去除成语后标识
				else if ((pChar[nEnd] == 'i' || pChar[nEnd] == 'l') && pChar[nEnd - 1] == ']') {
					pChar[nEnd - 1] = 0;
					nEnd = nEnd - 2;
				}
				ASSERT(nEnd>nStart);
				//找到'/'
				while (pChar[nEnd] != '/')
					nEnd--;
				ASSERT(nEnd>nStart);
				//得到词单位
				//得到词性
				pChar[nEnd] = 0;
				if (pChar[nEnd + 1] != 'w') {//不是标点符号
											 //得到词类型
					if (!m_lstStrTypes.Lookup(pChar + nEnd + 1, nType)) {
						//新类型
						nType = m_nCurTypeNum;
						m_strTypes[m_nCurTypeNum] = pChar + nEnd + 1;
						m_lstStrTypes[pChar + nEnd + 1] = m_nCurTypeNum++;
					}
					m_nTypeSum[nType]++;
					if (!m_lstStrWords.Lookup(pChar + nStart, nWords)) {
						//得到新词
						nWords = m_nCurWordsNum;
						m_lstStrWords[pChar + nStart] = m_nCurWordsNum++;
					}
					m_fTableW2T[nType][nWords]++;
					if (bFrist) {
						m_fStart[nType]++;
						bFrist = false;
					}
					else {
						m_fTypeTwoWords[nLastType][nType]++;
					}
					nLastType = nType;
				}
				else {//是标点符
					if (!m_lstStrWords.Lookup(pChar + nStart, nWords)) {
						//得到新词
						nWords = m_nCurWordsNum;
						m_lstStrWords[pChar + nStart] = m_nCurWordsNum++;
					}
					m_nTypeSum[0]++;
					m_fTableW2T[0][nWords]++;//0为标点符号
					if (!bFrist) {
						m_fTypeTwoWords[nLastType][0]++;
						bFrist = true;
					}
				}
				if (bPart) {
					//	strcat(pStr,"\r\n");
					bPart = false;
				}

				//去除后面重复的空格
				while (pChar[i + 1] == ' ')
					i++;
				//去除机构前标识
				if (pChar[i + 1] == '[')
					nStart = i + 2;
				else
					nStart = i + 1;
			}
		}
		inFile.getline(pChar, 10240);
	}
	//增加一个作为未登录词
	m_nCurWordsNum++;

	m_bModify = true;
	CString str, strTemp;
	str.Format("类型数：%d,词数：%d\r\n", m_nCurTypeNum, m_nCurWordsNum);
	for (int i = 0; i<m_nCurTypeNum; i++) {
		strTemp.Format("%s:共%d个\r\n", m_strTypes[i], m_nTypeSum[i]);
		str += strTemp;
	}
	return str;
}
void   Train()
{
	int i, j;
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		if (m_fStart[i] == 0)m_fStart[i] = PH;
		for (j = 0; j<m_nCurTypeNum; j++)
		{
			if (m_fTypeTwoWords[i][j] == 0)m_fTypeTwoWords[i][j] = PH;
		}
		for (int k = 0; k<m_nCurWordsNum; k++)
		{
			if (m_fTableW2T[i][k] == 0)m_fTableW2T[i][k] = PH;
		}
	}
	double dTotal = 0;

	//计算句首出现的总次数
	for (j = 0; j<m_nCurTypeNum; j++)
	{
		dTotal += m_fStart[j];
	}
	//计算每类词性作为句首出现的比例
	for (j = 0; j<m_nCurTypeNum; j++)
	{
		m_fstart[j] = (double)m_fStart[j] / dTotal;
	}
	dTotal = 0;
	//占总量的比例
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		//得到前词为[i]词性时，所有后词的总个数
		//dTotal=0;
		for (j = 0; j<m_nCurTypeNum; j++)
		{
			dTotal += m_fTypeTwoWords[i][j];
		}
	}
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		//计算前词为词性[i]时，后词为词性[j]占总情况的比例
		for (j = 0; j<m_nCurTypeNum; j++)
		{
			m_fshift[i][j] = (float)m_fTypeTwoWords[i][j] / dTotal;
		}
	}
	
	//词性出现概率，以同类词为比较总体
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		//计算每种词性出现的词数（包括重复）
		dTotal = 0;
		for (j = 0; j<m_nCurWordsNum; j++)
		{
			dTotal += m_fTableW2T[i][j];
		}
		ASSERT(dTotal>0);
		//计算作为词性[i]出现的词[j]，占所有出现的[i]类词的比例
		for (j = 0; j<m_nCurWordsNum; j++)
		{
			m_ffashe[i][j] = (double)m_fTableW2T[i][j] / dTotal;
		}
	}
	
}

//t对应输入词序号，nTag为后一词的词性序号，即后一词取词性nTag的情况下，前一词取什么词性，二者组合的概率能够最大。
double   Maxdelt(int t, int nTag)
{
	int arg = 0, i, k;
	double tmp, max = -100000;

	for (i = 0; i<m_nTagsCount[t]; i++)
	{
		tmp = m_dDelt[t][i] + log(m_fshift[m_nTags[t][i]][nTag]);
		if (tmp>max)
		{
			max = tmp;
			arg = m_nTags[t][i];
		}
	}
	//记住当第t+1个词为词性nTag时，第t个词的最大值在m_nTags中的位置
	m_nBestPrev[t + 1][nTag] = arg;
	return max;
};
//m_nWordsOrder：对应每个词的编号，tagnum:为输入的词数
void viterbi()
{
	int i, j, t, nTag;
	CString ssss;
	for (i = 0; i<m_nTagsCount[0]; i++)
	{//输入的句首词作为不同词性出现的概率*该词性为句首的概率
		nTag = m_nTags[0][i];
		m_dDelt[0][i] = log(m_fstart[nTag] * m_ffashe[nTag][m_nWordsOrder[0]]);
	}
	//m_dDelt[][]中得到输入的每个词取不同的词性j时，其前面的词取哪种词性和该词性j组合有最大的概率值。
	//如：m_dDelt[1]对应“电影”，m_dDelt[0]对应“看”
	//则有m_dDelt[电影][名词]的概率=MAX(m_dDelt[看][x]*m_fshift[x][名词]).即在任一种词性x中取乘积概率最大的那个词性
	for (t = 1; t<m_nWords; t++) {
		for (j = 0; j<m_nTagsCount[t]; j++)
		{
			nTag = m_nTags[t][j];
			m_dDelt[t][j] = Maxdelt(t - 1, nTag) + log(m_ffashe[nTag][m_nWordsOrder[t]]);
		}
	}
	//得到最后一个词的概率最大的词性，存入q[t-1]
	double pmax = -100000;
	t = m_nWords - 1;
	for (i = 0; i<m_nTagsCount[t]; i++)
	{
		if (m_dDelt[t][i]>pmax)
		{
			pmax = m_dDelt[t][i];
			m_nBestTag[t] = m_nTags[t][i];
		}
	}
	//因为确定了最后一个词的词性，就得到其前一个词的最佳词性，依次向前斌值。
	for (i = t - 1; i >= 0; i--)
	{
		m_nBestTag[i] = m_nBestPrev[i + 1][m_nBestTag[i + 1]];
	}
};

