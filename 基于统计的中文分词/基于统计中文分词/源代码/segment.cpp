#include "segment.h"
using namespace std;

SEGarticiple::SEGarticiple(std::string in, std::string out, size_t wordslen)
{
	infile = in;
	outfile = out;

	word_type = 0.0;
	threshold = 0.0;
	wlen = wordslen;
	words_length = wordslen * 2;

	//��ӱ����ŵȣ���ȡ����ʱ���˵������ŵ�
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("	");
	punctuation.push_back("-");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("{");
	punctuation.push_back("}");
	punctuation.push_back(" ");
	punctuation.push_back("��");
	punctuation.push_back("��");
	punctuation.push_back("1");
	punctuation.push_back("2");
	punctuation.push_back("3");
	punctuation.push_back("4");
	punctuation.push_back("5");
	punctuation.push_back("6");
	punctuation.push_back("7");
	punctuation.push_back("8");
	punctuation.push_back("9");
	punctuation.push_back("10");
	punctuation.push_back("11");
	punctuation.push_back("\n\n");

}

int SEGarticiple::SEG_init_filestream()
{
	in.open(infile, std::ios::in);
	out.open(outfile, std::ios::out);

	if (!in) {
		std::cerr << "Fatal Error:file " << infile.c_str() << "not exist!" << std::endl;
		return -1;
	}

	if (!out) {
		std::cerr << "Warning:file " << outfile.c_str() << "can't create!" << std::endl;
		return -1;
	}

	return 0;
}

int SEGarticiple::SEG_end_filestream()
{
	in.close();
	out.close();

	return 0;
}

int SEGarticiple::SEG_init_statistics()
{
	char buf[DWORD_SIZE];
	std::string tmp;
	source_string = "";

	while (!in.eof()) {
		memset(buf, '\0', DWORD_SIZE);
		in.read(buf, WORD_SIZE);
		tmp = buf;
		source.push_back(tmp);
		source_string += tmp;
	}

	return 0;
}

int SEGarticiple::SEG_word_statistics()
{
	std::string tmp;

	for (int ii = 0; ii < wlen; ++ii) {
		for (vec_it it = source.begin(); it != source.end(); ++it) {
			tmp = "";
			for (int j = 0; j <= ii && (it + j) < source.end(); ++j) {
				if (!(find(punctuation.begin(), punctuation.end(), *(it + j)) == punctuation.end()))
					break;
				tmp += *(it + j);
			}
			if ((int)tmp.size() < (2 * (ii + 1)))
				continue;		//��������Ҳ���ii�ʳ�������ӣ���ֹƵ�ȼ�һ
			++word_count[tmp];
#ifdef DEBUG
			std::cout << "SEG_word_statistics-�齨�ʣ�" << tmp.c_str() << std::endl;
#endif // DEBUG

		}
	}
	//ɾ�����е����ֺ�ɾ�����������ϵ�Ƶ��Ϊһ�Ĵ�
	SEG_word_erase();
	for (map_it it = word_count.begin(); it != word_count.end(); ++it)
		word_type += it->second;
	if (word_type)
		threshold = 1.0 / word_type;

	return 0;
}

int SEGarticiple::SEG_word_probability()
{
	for (map_it it = word_count.begin(); it != word_count.end(); ++it) {
		it->second = it->second / word_type;
	}
	threshold = 1.0 / word_type;

	return 0;
}

//���ҵ�ǰ�ʵĵڶ���������һ���ʵĵ�һ���֣����ҷ����Ǹ��ʵĵ�����
bool SEGarticiple::SEG_word_map_find(map_it &tmpit, std::string second, size_t &cnt)
{
	for (map_it it = tmpit; it != word_count.end(); ++it) {
		std::string tmp(it->first, 0, WORD_SIZE);
		cnt++;
		if (tmp.compare(second) == 0) {
			tmpit = it;
			return true;
		}
	}
	return false;
}

/*��source_string�в���substr,�鿴substr��ߵ����Ƿ���word����ʱ����������������
1.substr��û���ҵ���word					����ֵ��0  ��ʾ��������
2.substr���ҵ�����word							1  ֤����ͬһ�����飬�����������ԭ��
3.substr���ҵ���word��������ʱû����word			2  ˵���������ʣ��ֿ��洢���ִʵ�ʱ��
����substr���Ƿ����֣��������substr�ִʣ����򰴵ڶ����ʷִ�
*/
int SEGarticiple::SEG_word_string_find(std::string substr, std::string word)
{
	int flag = 0;
	std::string::size_type pos;
	std::string string = substr + word;

	pos = source_string.find(string, 0);
	if (pos == std::string::npos) {
		flag = 0;
	}
	else {
		std::string fisrt(substr, 0, WORD_SIZE);
		std::string second(substr, 2);
		std::string next_words = second + word;
		pos = 0;
		do {
			pos = source_string.find(next_words, pos * 2);
			if (pos != std::string::npos) {
				//if (pos - 2 < 0) {	��ʱ�ıȽ��Ǵ���ġ����޷��ź��з��űȽϣ�
				if (pos < 2) {			//���µĵ�һ�������⴦��
					flag |= 2;
					break;				//�����������һ����
				}
				else {
					std::string str(source_string, pos - 2, WORD_SIZE);
					if (str.compare(fisrt) == 0) {
						flag |= 1;
					}
					else {
						flag |= 2;
						break;
					}
				}
			}
			else {
				break;
			}
		} while (true);

	}
	return flag;
}

bool SEGarticiple::SEG_word_erase()
{
	
	int size = DWORD_SIZE;
	for (map_it it = word_count.begin(); it != word_count.end();) {
		size = it->first.size();
		if (size == WORD_SIZE || (/*size != DWORD_SIZE && */it->second <= 1)) {
#ifdef DEBUG
			std::cout << "SEG_word_erase-ɾ���ʣ�" << it->first.c_str() << std::endl;
#endif // DEBUG
			word_count.erase(it++);
		}
		else {
			++it;
		}
	}
	return true;
}

void SEGarticiple::SEG_word_analyze()
{
	
	size_t rcnt = 1;
	for (map_rit it = word_count.rbegin(); it != word_count.rend(); ++it, rcnt++) {
		size_t curr_wsize = it->first.size();
		std::string curr_words = it->first;

		//std::cout << "current word:" << curr_words.c_str() << std::endl;

		for (size_t ii = WORD_INDEX; ii < curr_wsize; ii += WORD_SIZE) {
			for (size_t cn = WORD_COUNT; cn <= curr_wsize - ii; cn += WORD_SIZE) {
				size_t cnt = 0;
				int erase_flag = 0;
				map_it tmpit = word_count.begin();
				//һ�����ٸ��������֣����Ч��
				std::string subwords(curr_words, ii, cn);
				size_t sub_wsize = subwords.size();

				

				std::string prefix_words(subwords, 0, sub_wsize - WORD_SIZE);
				std::string suffix_words(subwords, sub_wsize - WORD_SIZE, WORD_SIZE);
				while (SEG_word_map_find(tmpit, suffix_words, cnt)) {
					if (tmpit != word_count.end()) {
						std::string next_suffix_words(tmpit->first, 2, tmpit->first.size() - WORD_SIZE);
						int res = SEG_word_string_find(subwords, next_suffix_words);
						if (res == 1) {
							if (ii > WORD_INDEX && (sub_wsize == curr_wsize - WORD_SIZE)) {
								if (word_count[curr_words] < tmpit->second)
									erase_flag = 1;
							}
							if (word_count[curr_words] >= tmpit->second) {

								
								if (cnt >= word_count.size() - rcnt) {

									

									it = word_count.rbegin();
									rcnt = 1;
								}
								word_count.erase(tmpit++);
								continue;
							}
						}
						++tmpit;
					}
				}
				//ֻ�е���������������ʱ���ҵ�ǰ�ʵ�Ƶ�ʲ�С���Ӽ���Ƶ��ʱ�Ž����Ӽ�ɾ����
				if (curr_wsize >= DWORD_SIZE)
					if (word_count[curr_words] >= word_count[subwords] && sub_wsize != curr_wsize) {
						word_count.erase(subwords);

					

					}
				if (erase_flag == 1) {
					word_count.erase(curr_words);

					


				}
			}
		}
	}
}

void SEGarticiple::SEG_source_print()
{
	std::cout << "Origin essay:" << std::endl;
	std::cout << source_string.c_str() << std::endl;
}

void SEGarticiple::SEG_word_print()
{
	int c = 0;
	std::cout << "words type:" << word_count.size();
	std::cout << "\tthreshold:" << threshold << std::endl;
	for (map_it it = word_count.begin(); it != word_count.end(); ++it) {
		std::cout << c++ << ":" << it->first.c_str();
		std::cout << "--" << it->second << "\t";
		if (c % 4 == 0)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}

void SEGarticiple::SEG_word_wback()
{
	
	size_t cn = 0;
	std::string outstring = "";
	size_t length = source_string.size();

	for (size_t ii = 0; ii < length - 1;) {
		int punct_flag = 0;
		for (cn = words_length; cn >= DWORD_SIZE; cn -= WORD_SIZE) {
			std::string punct = "";
			std::string tmpstr = "";
			for (size_t index = ii; index < length - 1 && index - ii < cn; index += WORD_SIZE) {
				std::string word(source_string, index, WORD_SIZE);
				if (find(punctuation.begin(), punctuation.end(), word) == punctuation.end()) {
					tmpstr += word;
				}
				else {
					if (index == ii) {			//��һ���־��Ƿ���
						outstring += word;
						punct_flag = 1;
						ii += WORD_SIZE;
					}
					else
						punct = word;
					break;
				}
			}
			if (punct_flag == 1) break;
			if ((tmpstr.size() == cn) && (word_count.find(tmpstr) != word_count.end())) {
				if (punct.size() != 0) {
					outstring = outstring + tmpstr + punct;
					ii = ii + tmpstr.size() + WORD_SIZE;
				}
				else {
					outstring = outstring + tmpstr + " ";
					ii = ii + tmpstr.size();
				}
				break;
			}
		}

		if (cn == WORD_SIZE) {
			std::string word(source_string, ii, WORD_SIZE);
			outstring = outstring + word + " ";
			ii += WORD_SIZE;
		}
	}
	out << outstring.c_str();
}