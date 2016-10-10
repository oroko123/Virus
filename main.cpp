#include <iostream>
#include <set>
#include <tuple>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stdexcept> 

//                          <year, value,    country,       name    ,    valueInput>
using Poststamp = std::tuple<int, double , std::string,  std::string,   std::string>;

struct Poststamp_Compare
{
	bool operator() (const Poststamp& lhs, const Poststamp& rhs) const
	{
		return std::get<0>(lhs) < std::get<0>(rhs);
	}
};

using Poststamp_Store = std::multiset<Poststamp, Poststamp_Compare>;
using Poststamp_Store_Iter = std::multiset<Poststamp, Poststamp_Compare>::iterator;

/** H E L P E R S **/

namespace
{
	void split(const std::string& s, char delim, std::vector<std::string> &elems) 
	{
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			if (!item.empty()) {
				elems.push_back(item);
			}
		} 
	}

	std::vector<std::string> split(const std::string &s, char delim) 
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	int format_date(std::string& s) 
	{
		if (s.length() != 4) {
			throw std::invalid_argument("Not a date");
		}
		int date = std::stoi(s);
		if (date < 1800 || date > 3000) {
			throw std::invalid_argument("Not a date");
		}
		return date;

	}

	void debug_poststamp(const Poststamp& poststamp)
	{
		std::cout<<
		"\nRok: " << 
		std::get<0>(poststamp)<<" "<<
		"\nPoczta: " <<
		std::get<2>(poststamp)<<" "<<
		"\nCena: " <<
		std::get<1>(poststamp)<<" "<<
		"\nNazwa: " << 
		std::get<3>(poststamp)<<"\n";
	}

/** END OF HELPERS **/

	void print_poststamp(const Poststamp& poststamp)
	{
		std::cout<<
		std::get<0>(poststamp)<<" "<<
		std::get<2>(poststamp)<<" "<<
		std::get<1>(poststamp)<<" "<<
		std::get<3>(poststamp)<<"\n";

	}

	Poststamp build_poststamp(int year, double value, std::string country,
		std::string name, std::string valueInput)
	{
		Poststamp poststamp;
		std::get<0>(poststamp) = year;
		std::get<1>(poststamp) = value;
		std::get<2>(poststamp) = country;
		std::get<3>(poststamp) = name;
		std::get<4>(poststamp) = valueInput;
		return poststamp;
	}

	void register_poststamp(Poststamp_Store& store, const Poststamp& poststamp)
	{
		store.insert(poststamp);
	}

	void process_query(const Poststamp_Store store, const std::string& line)
	{
	}

	bool process_stamp(const Poststamp_Store store, const std::string& line)
	{
		std::vector<std::string> tokens = split(line, ' ');
		int date_position = 0;
		int iterator = -1;
		int converted_date = 0;
		double converted_price = 0.00;
		std::string parsed_price = "";
		for (auto token : tokens) {
			std::cout << token << " ";
			iterator++;
			try {
				converted_date = format_date(token);
				date_position = iterator;
			} catch (const std::invalid_argument& ia) {
			}
			try {
				converted_price = std::stod(token);
			} catch (const std::invalid_argument& ia) {
			}
		}
		std::string parsed_stamp_name = "";
		std::string parsed_post_name = "";
		for (int i = 0; i < date_position - 1; i++) {
			parsed_stamp_name += tokens.at(i);
			parsed_stamp_name += " ";
		}
		for (int i = date_position + 1; i < tokens.size(); i++) {
			parsed_post_name += tokens.at(i);
			parsed_post_name += " ";
		}
		Poststamp stamp = build_poststamp(converted_date, converted_price, parsed_post_name, parsed_stamp_name, parsed_price);
		debug_poststamp(stamp);
	}

	void query_poststamps(const Poststamp_Store& store, int yearFrom, int yearTo)
	{
		Poststamp begin = build_poststamp(yearFrom, 0, "", "", "");
		Poststamp end = build_poststamp(yearTo, 0, "", "", "");
		Poststamp_Store_Iter fromIter = store.lower_bound(begin);
		Poststamp_Store_Iter toIter = store.upper_bound(end);

		std::list<Poststamp> result(fromIter, toIter);
		result.sort();
		for(std::list<Poststamp>::iterator it = result.begin();
			it != result.end(); it ++)
		{
			print_poststamp(*it);
		}
	}
}

int main()
{
	std::string line;
	Poststamp_Store store;
	bool reading_stamps = true;
	while(std::getline(std::cin, line))
	{
		if (reading_stamps)
			reading_stamps = process_stamp(store, line);
		else
			process_query(store, line);
	}
}

