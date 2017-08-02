#ifndef GDT_BASIC_H
#define GDT_BASIC_H

#include <string>
#include <vector>

class g_name
{
public:

private:
	std::string               surname;
	std::vector <std::string> given_names;
	std::vector <std::string> nicknames;
};

class g_date
{
public:

private:

	// Start of permissible date range

	int start_day;
	int start_month;
	int start_year;

	int tolerance_stop_day;
	int tolerance_stop_month;
	int tolerance_stop_year;

	// End of permissible date range

	int end_day;
	int end_month;
	int end_year;

	int tolerance_start_day;
	int tolerance_start_month;
	int tolerance_start_year;


};


#endif