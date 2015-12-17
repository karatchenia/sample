#include <exception>
#include "BaseTestCase.h"

using namespace std;
using namespace MyCompany::Algorithms::Tests;

BaseTestCase::BaseTestCase(std::string&& name)
  : Name(forward<std::string>(name))
{
}

void BaseTestCase::Validate() const
{
  if (this->Name.empty())
  {
    throw exception("Test case Name is empty.");
  }
}

ostream& MyCompany::Algorithms::Tests::operator<<(
  ostream& str, BaseTestCase const& t)
{
  t.Print(str);
  return str;
}

void BaseTestCase::Print(ostream& str) const
{
  PrintValue(str, "Name", Name);
}