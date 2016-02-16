#include <algorithm> // is_sorted
#include "../VectorUtilities.h"
#include "../PrintUtilities.h"
#include "../Tests/TestUtilities.h"
#include "MinimumMovesToSort.h"
#include "MinimumMovesToSortTests.h"

using namespace std;
using namespace MyCompany::Algorithms::Numbers;
using namespace MyCompany::Algorithms;

namespace
{
  //A function pointer (instead of std::less) is being tested.
  template<typename T>
  inline bool Less(const T& a, const T& b)
  {
    return a < b;
  }

  template<typename T>
  inline bool LessEqual(const T& a, const T& b)
  {
    return a <= b;
  }

  using Number = int;
  using Number_list = vector<Number>;

  class TestCase final : public BaseTestCase
  {
    Number_list _Data;
    size_t _Expected;

  public:

    TestCase(
      string&& name,
      Number_list&& data,
      const size_t expected)
      : BaseTestCase(forward<string>(name)),
      _Data(forward<Number_list>(data)),
      _Expected(expected)
    {
    }

    inline const Number_list& get_Data() const { return _Data; }
    inline const size_t& get_Expected() const { return _Expected; }

    void Validate() const override
    {
      BaseTestCase::Validate();

      const auto data = get_Data();
      const auto isSorted = is_sorted(data.begin(), data.end());
      if (isSorted)
      {
        Assert::AreEqual(size_t(0), get_Expected(), "Expected_" + get_Name());
      }
      else
      {
        Assert::Greater(get_Expected(), size_t(0), "Expected_" + get_Name());
      }
    }

    void Print(ostream& str) const override
    {
      BaseTestCase::Print(str);

      ::Print(", Data=", _Data, str);

      str << " Expected=" << _Expected;
    }
  };

  void GenerateTestCases(vector<TestCase>& testCases)
  {
    testCases.push_back(TestCase{ "One move",{ 4, 1, 2, 3, }, 1 });
    testCases.push_back(TestCase{ "No move",{ 1, 2, 3, 4, }, 0 });
    testCases.push_back(TestCase{ "Two moves",{ 6, 3, 1, 2, 4, 5 }, 2 });
    testCases.push_back(TestCase{ "One move with repetitions",
    { 4, 1,1,1, 2,2, 3,3,3,3,3, 4 },
      1 });
  }

  void RunTestCase(const TestCase& testCase)
  {
    using Alg = bool (*)(const Number&, const Number&);
    using NameAlg = pair<string, Alg>;

    const auto data = testCase.get_Data();
    const auto hasUniqueValues = VectorUtilities::HasUniqueValues(data);

    const vector<NameAlg> subTests = [](bool isStrict)
    {
      vector<NameAlg> subTestsTemp{ { "LessEqual",&LessEqual<Number> } };
      if (isStrict)
      {
        subTestsTemp.push_back({ "Less",&Less<Number> });
      }
      return subTestsTemp;
    }(hasUniqueValues);

    const string separator = "_";

    for (const auto& subTest : subTests)
    {
			const auto name = testCase.get_Name() + separator + subTest.first;
			const auto actual = MinimumMovesToSort<Number, Alg>(data, subTest.second);
	    Assert::AreEqual(testCase.get_Expected(), actual, name);
    }
  }
}

void MyCompany::Algorithms::Numbers::Tests::MinimumMovesToSortTests(void)
{
  TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
}
