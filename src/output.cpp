#include <iostream>
#include <iomanip>

static constexpr auto COL = "    ||    ";
static constexpr auto DIV_1 =
	"|                         ||                             ||                            |";
static constexpr auto DIV_2 =
	"|_________________________||_____________________________||____________________________|";
static constexpr auto DIV_3 =
	" ______________________________________________________________________________________";
void table_header_string()
{
	std::cout << DIV_3 << "\n" << DIV_1 << std::endl;
	std::cout << "|  "
			  << "        TWINS FOUND" << COL << "           TIME TAKEN" << COL
			  << "          TOTAL TWINS"
			  << "   |" << std::endl;
	std::cout << DIV_2 << std::endl;
}

void table_line_string(unsigned long long twins, double time, unsigned long long tot_twins)
{
	std::cout << DIV_1 << std::endl;
	std::cout << "| " << std::setw(20) << twins << COL << std::setw(20) << time << "s" << COL
			  << std::setw(20) << tot_twins << "    |" << std::endl;
	std::cout << DIV_2 << std::endl;
}

