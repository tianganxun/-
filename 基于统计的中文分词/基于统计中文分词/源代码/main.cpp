#include "segment.h"

int main()
{
	int res = 0;
	int wlen = 4;

	

	SEGarticiple *SEG = new SEGarticiple("infile.txt", "outfile.txt", wlen);
	res = SEG->SEG_init_filestream();
	if (res) return 1;

	SEG->SEG_init_statistics();
	SEG->SEG_word_statistics();
	SEG->SEG_word_probability();

	std::cout << "统计中..." << std::endl;
	SEG->SEG_word_analyze();
	SEG->SEG_word_print();
	SEG->SEG_word_wback();

	SEG->SEG_end_filestream();
	std::cout << "消耗时间:" << clock() / CLOCKS_PER_SEC << "s." << std::endl;
	system("pause");

	return 0;
}