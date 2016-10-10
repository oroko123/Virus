#include <iostream>
#include <set>
#include <tuple>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <sstream>
#include <stdexcept> 

//                          <year, value,    country,       name    >
using Poststamp = std::tuple<int, double , std::string,  std::string>;

struct PoststampCompare
{
    bool operator() (const Poststamp& lhs, const Poststamp& rhs) const
    {
        return std::get<0>(lhs) < std::get<0>(rhs);
    }
};

using PoststampStore = std::multiset<Poststamp, PoststampCompare>;
using PoststampStoreIter = std::multiset<Poststamp, PoststampCompare>::iterator;

/** H E L P E R S **/

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
		throw std::invalid_argument("Not date");
	}
	int date = std::stoi(s);
	if (date < 1800 || date > 3000) {
		throw std::invalid_argument("Not date");
	}
	return date;
	
}

void debug_poststamp(const Poststamp& poststamp)
{
    std::cout<<
		"\nRok: " << 
        std::get<0>(poststamp)<<" "<<
        "\nCena: " <<
        std::get<2>(poststamp)<<" "<<
        "\nPocta: " <<
        std::get<1>(poststamp)<<" "<<
        "\nNazwa: " << 
        std::get<3>(poststamp)<<"\n";
}

/** END OF HELPERS **/

void PrintPoststamp(const Poststamp& poststamp)
{
    std::cout<<
        std::get<0>(poststamp)<<" "<<
        std::get<2>(poststamp)<<" "<<
        std::get<1>(poststamp)<<" "<<
        std::get<3>(poststamp)<<"\n";
}

Poststamp BuildPoststamp(int year, double value, std::string country, 
    std::string name)
{
    Poststamp poststamp;
    std::get<0>(poststamp) = year;
    std::get<1>(poststamp) = value;
    std::get<2>(poststamp) = country;
    std::get<3>(poststamp) = name;
    return poststamp;
}

void RegisterPoststamp(PoststampStore& store, const Poststamp& poststamp)
{
     store.insert(poststamp);
}

void ProcessLine(const PoststampStore store, const std::string& line)
{
	std::vector<std::string> tokens = split(line, ' ');
	///for (auto token : tokens) {
	///	std::cout << token << " ";
	///}
	int date_position = 0;
	int iterator = -1;
	int converted_date = 0;
	double converted_price = 0.00;
	for (auto token : tokens) {
		std::cout << token << " ";
		iterator++;
		try {
			converted_date = format_date(token);
			date_position = iterator;
			///std::cout << converted << " ";
		} catch (const std::invalid_argument& ia) {
			///std::cout << token << " nie jest liczbą! ";
		}
		try {
			converted_price = std::stod(token);
			///std::cout << converted << " ";
		} catch (const std::invalid_argument& ia) {
			///std::cout << token << " nie jest liczbą! ";
		}
	}
	///std::cout << "Nazwa znaczka: ";
	std::string parsed_stamp_name = "";
	std::string parsed_post_name = "";
	for (int i = 0; i < date_position - 1; i++) {
	///	std::cout << tokens.at(i) << " ";
		parsed_stamp_name += tokens.at(i);
		parsed_stamp_name += " ";
	}
	///std::cout << "\n Cena: " << tokens.at(date_position - 1);
	///std::cout << "\n Data: " << tokens.at(date_position);
	///std::cout << "\n Nazwa poczty: ";
	for (int i = date_position + 1; i < tokens.size(); i++) {
	///	std::cout << tokens.at(i) << " ";
		parsed_post_name += tokens.at(i);
		parsed_post_name += " ";
	}
	//TODO - if line was valid
	Poststamp stamp = BuildPoststamp(converted_date, converted_price, parsed_post_name, parsed_stamp_name);
	debug_poststamp(stamp);
}

void QueryPoststamps(const PoststampStore& store, int yearFrom, int yearTo)
{
    Poststamp begin = BuildPoststamp(yearFrom, 0, "", "");
    Poststamp end = BuildPoststamp(yearTo, 0, "", "");
    PoststampStoreIter fromIter = store.lower_bound(begin);
    PoststampStoreIter toIter = store.upper_bound(end);
    
    std::list<Poststamp> result(fromIter, toIter);  
    result.sort();
    for(std::list<Poststamp>::iterator it = result.begin();
        it != result.end(); it ++)
    {
        PrintPoststamp(*it);
    }
}

int main()
{
    std::string line;
    PoststampStore store;
    while(std::getline(std::cin, line))
    {
        ProcessLine(store, line);
    }
}

