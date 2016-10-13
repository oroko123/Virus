#include <iostream>
#include <set>
#include <tuple>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <regex>

//                          <year,  country,   value,      name    ,    valueInput>
using Poststamp = std::tuple<int, std::string, float , std::string,   std::string>;

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

	std::vector<std::string> split_whitespace(const std::string& s, char delim)
	{
		std::vector<std::string> elems;
		std::string copy_s = s;
		std::replace(copy_s.begin(), copy_s.end(), '\t', ' ');;
		split(copy_s, delim, elems);
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

/** END OF HELPERS **/

	void print_poststamp(const Poststamp& poststamp)
	{
		std::cout<<
		std::get<0>(poststamp)<<" "<<		
		std::get<1>(poststamp)<<" "<<
		std::get<4>(poststamp)<<" "<<
		std::get<3>(poststamp)<<"\n";

	}

	Poststamp build_poststamp(int year, float value, std::string country,
		std::string name, std::string valueInput)
	{
		Poststamp poststamp;
		std::get<0>(poststamp) = year;
		std::get<1>(poststamp) = country;
		std::get<2>(poststamp) = value;
		std::get<3>(poststamp) = name;
		std::get<4>(poststamp) = valueInput;
		return poststamp;
	}

	void register_poststamp(Poststamp_Store& store, const Poststamp& poststamp)
	{
		store.insert(poststamp);
	}

	void query_poststamps(const Poststamp_Store& store, int yearFrom, int yearTo)
	{
		Poststamp begin = build_poststamp(yearFrom, 0, "", "", "");
		Poststamp end = build_poststamp(yearTo, 0, "", "", "");
		Poststamp_Store_Iter fromIter = store.lower_bound(begin);
		Poststamp_Store_Iter toIter = store.upper_bound(end);

		std::list<Poststamp> result(fromIter, toIter);
		result.sort();
		for (std::list<Poststamp>::iterator it = result.begin();
			it != result.end(); it ++)
		{
			print_poststamp(*it);
		}
	}

	bool process_query(const Poststamp_Store& store, const std::string& line)
	{
		bool invalid = false;
		int from_date = 0;
		int to_date = 0;
		std::vector<std::string> tokens = split_whitespace(line, ' ');
		if (tokens.size() > 1) {
			try {
				from_date = format_date(tokens.at(0));
			} catch (const std::invalid_argument& ia) {
			}
			try {
				to_date = format_date(tokens.at(1));
			} catch (const std::invalid_argument& ia) {
			}
		}
		if (from_date < 3001 && to_date < 3001 && from_date > 1800 && to_date > 1800 && from_date <= to_date) {
			query_poststamps(store, from_date, to_date);
		} else {
			invalid = true;
		}
		return invalid;
	}

	bool process_stamp(Poststamp_Store& store, const std::string& line)
	{
		int date_position = 0;
		int iterator = -1;

		int converted_date = 0;
		float converted_price = 0.00;
		std::string parsed_stamp_name = "";
		std::string parsed_post_name = "";
		std::string parsed_price = "";

		std::vector<std::string> tokens = split_whitespace(line, ' ');

		bool valid_stamp = true;
		bool valid_post = true;
		bool valid_date = true;
		bool valid_price = true;
		bool valid = true;

		for (auto token : tokens) {
			iterator++;
			try {
				converted_date = format_date(token);
				date_position = iterator;
			} catch (const std::invalid_argument& ia) {
			}
		}

		if (date_position != 0 && date_position != tokens.size() - 1) {
			for (int i = 0; i < date_position - 1; i++) {
				parsed_stamp_name += tokens.at(i);
				if (i != date_position - 2)
					parsed_stamp_name += " ";
			}
			for (int i = date_position + 1; i < tokens.size(); i++) {
				parsed_post_name += tokens.at(i);
				if (i != tokens.size() - 1)
					parsed_post_name += " ";
			}
			try {
				parsed_price = tokens.at(date_position - 1);
				std::string copy_parsed_price  = parsed_price;
				std::replace(copy_parsed_price.begin(), copy_parsed_price.end(), ',', '.');
				converted_price = std::atof(copy_parsed_price.c_str());
			} catch (const std::invalid_argument& ia) {
			}
		}

		valid_date = converted_date > 0;
		valid_price = converted_price > 0;
		valid_post = parsed_post_name.length() > 0;
		valid_stamp = parsed_stamp_name.length() > 0;

		if (valid_date && valid_price && valid_post && valid_stamp) {
			Poststamp stamp = build_poststamp(converted_date, converted_price, parsed_post_name, parsed_stamp_name, parsed_price);
			store.insert(stamp);
		} else {
			Poststamp stamp = build_poststamp(converted_date, converted_price, parsed_post_name, parsed_stamp_name, parsed_price);
			valid = false;
		}
		return valid;
	}
}

int main()
{
	std::string line;
	Poststamp_Store store;
	bool reading_stamps = true;
	bool only_queries = false;
	int line_number = 0;
	int not_parsed = 0;
	while (std::getline(std::cin, line))
	{
		line_number++;
		if (reading_stamps && !only_queries) {
			reading_stamps = process_stamp(store, line);
		}
		if (!reading_stamps || only_queries) {
			not_parsed++;
			reading_stamps = process_query(store, line);
			if (!reading_stamps) {
				only_queries = true;
			} else {
				not_parsed++;
			}
		}
		/** nie sparsowano stampa ani query **/
		if (not_parsed == 2) {
			std::cerr << "Error in line "<< line_number << ":" << line << "\n";
		}
		not_parsed = 0;
	}
}
