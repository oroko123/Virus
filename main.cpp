#include<iostream>
#include<set>
#include<tuple>
#include<string>

//                          <year,  country,     value,       name     >
using Poststamp = std::tuple<int, std::string, std::string, std::string>;

struct PoststampCompare
{
    bool operator() (const Poststamp& lhs, const Poststamp& rhs) const
    {
        return std::get<0>(lhs) < std::get<0>(rhs);
    }
};

using PoststampStore = std::multiset<Poststamp, PoststampCompare>;

void PrintPoststamp(const Poststamp& poststamp)
{
    std::cout<<
        std::get<0>(poststamp)<<" "<<
        std::get<1>(poststamp)<<" "<<
        std::get<2>(poststamp)<<" "<<
        std::get<3>(poststamp)<<"\n";
}

void ProcessLine(const PoststampStore& dict, const std::string& line)
{
    //TODO
    Poststamp poststamp;
    std::get<0>(poststamp) = 0;
    std::get<1>(poststamp) = "cambio";
    std::get<2>(poststamp) = "dolor";
    std::get<3>(poststamp) = "oreiro";
    PrintPoststamp(poststamp); 
}


void RegisterPoststamp(const PoststampStore& dict, const Poststamp& poststamp)
{
    //TODO
}

void QueryPoststamps(const PoststampStore& dict, int yearFrom, int yearTo)
{
    //TODO
}

int main()
{
    std::string line;
    PoststampStore dict;
    while(std::getline(std::cin, line))
    {
        ProcessLine(dict, line);
    }
}
