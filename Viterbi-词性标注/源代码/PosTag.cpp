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
		ar << m_nCurTypeNum;				//ѵ�������ѵò�ͬ�����͸���
		ar << m_nCurWordsNum;				//ѵ�������ѵò�ͬ�������
											// ��¼������Ϊ���״ʵĴ���
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
		m_lstStrTypes.Serialize(ar);//�����б�
		m_lstStrWords.Serialize(ar);//���б�

	}
	else {
		ar >> m_nCurTypeNum;				//ѵ�������ѵò�ͬ�����͸���
		ar >> m_nCurWordsNum;				//ѵ�������ѵò�ͬ�������
											// ��¼������Ϊ���״ʵĴ���
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
		m_lstStrTypes.Serialize(ar);//�����б�
		m_lstStrWords.Serialize(ar);//���б�

	}
}

//������������
CString    IndexSample()
{
	//��ʼ������
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
		MessageBox(NULL, "���Ͽ��ļ�������", "", MB_OK);
		return "";
	}
	char pChar[10240];
	char sPos[3];
	sPos[2] = 0;
	int j, nTotal = 0;
	//���趨��������
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
		int nWords = 0;//�����
		int nType = 0;//�������
		int nLastType = -1;//��һ�ʵĴ���
		int nStart = 0, nEnd;
		bool bPart = false;//���ű�־
		bool bFrist = true; //���ױ�־
		int nName = 0;//�������̳��ֵĴ���
		bool bOrder = true;//��һ����Ų�Ҫ
		for (int i = 0; i<nLen; i++) {
			if (pChar[i] == ' ') {//���ֿո�ָ��
				if (bOrder) {
					//ȥ�������ظ��Ŀո�
					while (pChar[i + 1] == ' ')
						i++;
					//ȥ������ǰ��ʶ
					if (pChar[i + 1] == '[')
						nStart = i + 2;
					else
						nStart = i + 1;
					bOrder = false;
					continue;
				}
				pChar[i] = 0;
				nEnd = i - 1;
				//ȥ������nt�ĺ��ʶ  ���⻹��ns��nz
				if (pChar[nEnd - 1] == 'n'&&pChar[nEnd - 2] == ']') {
					pChar[nEnd - 2] = 0;
					nEnd = nEnd - 3;
					bPart = true;
				}
				//ȥ��������ʶ
				else if ((pChar[nEnd] == 'i' || pChar[nEnd] == 'l') && pChar[nEnd - 1] == ']') {
					pChar[nEnd - 1] = 0;
					nEnd = nEnd - 2;
				}
				ASSERT(nEnd>nStart);
				//�ҵ�'/'
				while (pChar[nEnd] != '/')
					nEnd--;
				ASSERT(nEnd>nStart);
				//�õ��ʵ�λ
				//�õ�����
				pChar[nEnd] = 0;
				if (pChar[nEnd + 1] != 'w') {//���Ǳ�����
											 //�õ�������
					if (!m_lstStrTypes.Lookup(pChar + nEnd + 1, nType)) {
						//������
						nType = m_nCurTypeNum;
						m_strTypes[m_nCurTypeNum] = pChar + nEnd + 1;
						m_lstStrTypes[pChar + nEnd + 1] = m_nCurTypeNum++;
					}
					m_nTypeSum[nType]++;
					if (!m_lstStrWords.Lookup(pChar + nStart, nWords)) {
						//�õ��´�
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
				else {//�Ǳ���
					if (!m_lstStrWords.Lookup(pChar + nStart, nWords)) {
						//�õ��´�
						nWords = m_nCurWordsNum;
						m_lstStrWords[pChar + nStart] = m_nCurWordsNum++;
					}
					m_nTypeSum[0]++;
					m_fTableW2T[0][nWords]++;//0Ϊ������
					if (!bFrist) {
						m_fTypeTwoWords[nLastType][0]++;
						bFrist = true;
					}
				}
				if (bPart) {
					//	strcat(pStr,"\r\n");
					bPart = false;
				}

				//ȥ�������ظ��Ŀո�
				while (pChar[i + 1] == ' ')
					i++;
				//ȥ������ǰ��ʶ
				if (pChar[i + 1] == '[')
					nStart = i + 2;
				else
					nStart = i + 1;
			}
		}
		inFile.getline(pChar, 10240);
	}
	//����һ����Ϊδ��¼��
	m_nCurWordsNum++;

	m_bModify = true;
	CString str, strTemp;
	str.Format("��������%d,������%d\r\n", m_nCurTypeNum, m_nCurWordsNum);
	for (int i = 0; i<m_nCurTypeNum; i++) {
		strTemp.Format("%s:��%d��\r\n", m_strTypes[i], m_nTypeSum[i]);
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

	//������׳��ֵ��ܴ���
	for (j = 0; j<m_nCurTypeNum; j++)
	{
		dTotal += m_fStart[j];
	}
	//����ÿ�������Ϊ���׳��ֵı���
	for (j = 0; j<m_nCurTypeNum; j++)
	{
		m_fstart[j] = (double)m_fStart[j] / dTotal;
	}
	dTotal = 0;
	//ռ�����ı���
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		//�õ�ǰ��Ϊ[i]����ʱ�����к�ʵ��ܸ���
		//dTotal=0;
		for (j = 0; j<m_nCurTypeNum; j++)
		{
			dTotal += m_fTypeTwoWords[i][j];
		}
	}
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		//����ǰ��Ϊ����[i]ʱ�����Ϊ����[j]ռ������ı���
		for (j = 0; j<m_nCurTypeNum; j++)
		{
			m_fshift[i][j] = (float)m_fTypeTwoWords[i][j] / dTotal;
		}
	}
	
	//���Գ��ָ��ʣ���ͬ���Ϊ�Ƚ�����
	for (i = 0; i<m_nCurTypeNum; i++)
	{
		//����ÿ�ִ��Գ��ֵĴ����������ظ���
		dTotal = 0;
		for (j = 0; j<m_nCurWordsNum; j++)
		{
			dTotal += m_fTableW2T[i][j];
		}
		ASSERT(dTotal>0);
		//������Ϊ����[i]���ֵĴ�[j]��ռ���г��ֵ�[i]��ʵı���
		for (j = 0; j<m_nCurWordsNum; j++)
		{
			m_ffashe[i][j] = (double)m_fTableW2T[i][j] / dTotal;
		}
	}
	
}

//t��Ӧ�������ţ�nTagΪ��һ�ʵĴ�����ţ�����һ��ȡ����nTag������£�ǰһ��ȡʲô���ԣ�������ϵĸ����ܹ����
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
	//��ס����t+1����Ϊ����nTagʱ����t���ʵ����ֵ��m_nTags�е�λ��
	m_nBestPrev[t + 1][nTag] = arg;
	return max;
};
//m_nWordsOrder����Ӧÿ���ʵı�ţ�tagnum:Ϊ����Ĵ���
void viterbi()
{
	int i, j, t, nTag;
	CString ssss;
	for (i = 0; i<m_nTagsCount[0]; i++)
	{//����ľ��״���Ϊ��ͬ���Գ��ֵĸ���*�ô���Ϊ���׵ĸ���
		nTag = m_nTags[0][i];
		m_dDelt[0][i] = log(m_fstart[nTag] * m_ffashe[nTag][m_nWordsOrder[0]]);
	}
	//m_dDelt[][]�еõ������ÿ����ȡ��ͬ�Ĵ���jʱ����ǰ��Ĵ�ȡ���ִ��Ժ͸ô���j��������ĸ���ֵ��
	//�磺m_dDelt[1]��Ӧ����Ӱ����m_dDelt[0]��Ӧ������
	//����m_dDelt[��Ӱ][����]�ĸ���=MAX(m_dDelt[��][x]*m_fshift[x][����]).������һ�ִ���x��ȡ�˻����������Ǹ�����
	for (t = 1; t<m_nWords; t++) {
		for (j = 0; j<m_nTagsCount[t]; j++)
		{
			nTag = m_nTags[t][j];
			m_dDelt[t][j] = Maxdelt(t - 1, nTag) + log(m_ffashe[nTag][m_nWordsOrder[t]]);
		}
	}
	//�õ����һ���ʵĸ������Ĵ��ԣ�����q[t-1]
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
	//��Ϊȷ�������һ���ʵĴ��ԣ��͵õ���ǰһ���ʵ���Ѵ��ԣ�������ǰ��ֵ��
	for (i = t - 1; i >= 0; i--)
	{
		m_nBestTag[i] = m_nBestPrev[i + 1][m_nBestTag[i + 1]];
	}
};

