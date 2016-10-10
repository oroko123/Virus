#include<iostream>
#include<set>
#include<tuple>
#include<string>
#include<list>
#include<algorithm>

//                          <year, value,    country,       name    ,    valueInput>
using Poststamp = std::tuple<int, double , std::string,  std::string,   std::string>;

struct PoststampCompare
{
    bool operator() (const Poststamp& lhs, const Poststamp& rhs) const
    {
        return std::get<0>(lhs) < std::get<0>(rhs);
    }
};

using PoststampStore = std::multiset<Poststamp, PoststampCompare>;
using PoststampStoreIter = std::multiset<Poststamp, PoststampCompare>::iterator;

namespace
{
  void PrintPoststamp(const Poststamp& poststamp)
  {
      std::cout<<
          std::get<0>(poststamp)<<" "<<
          std::get<4>(poststamp)<<" "<<
          std::get<1>(poststamp)<<" "<<
          std::get<3>(poststamp)<<"\n";
  }

  bool ProcessStamp(const PoststampStore store, const std::string& line)
  {
      return true;
  }

  void ProcessQuery(const PoststampStore& store, const std::string& line)
  {

  }

  Poststamp BuildPoststamp(int year, double value, std::string country,
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

  void RegisterPoststamp(PoststampStore& store, const Poststamp& poststamp)
  {
       store.insert(poststamp);
  }

  void QueryPoststamps(const PoststampStore& store, int yearFrom, int yearTo)
  {
      Poststamp begin = BuildPoststamp(yearFrom, 0, "", "", "");
      Poststamp end = BuildPoststamp(yearTo, 0, "", "", "");
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
}

int main()
{
    std::string line;
    PoststampStore store;
    bool readingStamps = true;
    while(std::getline(std::cin, line))
    {
      if(readingStamps)
        readingStamps = ProcessStamp(store, line);
      else
        ProcessQuery(store, line);
    }
}
