#include <algorithm> // is_sorted
#include "../VectorUtilities.h"
#include "../PrintVectorUtilities.h"
#include "../Tests/TestUtilities.h"
#include "MinimumMovesToSort.h"
#include "MinimumMovesToSortTests.h"

using namespace std;
using namespace MyCompany::Algorithms::Numbers;
using namespace MyCompany::Algorithms;

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Numbers
    {
      namespace Tests
      {
        class MinimumMovesToSortTest final
        {
          MinimumMovesToSortTest() = delete;

        public:

          static void Test(void);

        private:

          using Number = int;
          using Number_list = vector<Number>;

          static inline bool Less(const Number& a, const Number& b)
          {
            return a < b;
          }

          static inline bool LessEqual(const Number& a, const Number& b)
          {
            return a <= b;
          }

          class TestCase final : public BaseTestCase
          {
            Number_list _Data;
            size_t _Expected;

          public:

            TestCase(
              string&& name,
              Number_list&& data,
              const size_t expected);

            inline const Number_list& get_Data() const { return _Data; }
            inline const size_t& get_Expected() const { return _Expected; }

            void Validate() const override;

            void Print(ostream& str) const override;
          };

          static void GenerateTestCases(vector<TestCase>& testCases);

          static void RunTestCase(const TestCase& testCase);
        };

        void MinimumMovesToSortTest::GenerateTestCases(vector<TestCase>& testCases)
        {
          testCases.push_back(TestCase{ "One move",{ 4, 1, 2, 3, }, 1 });
          testCases.push_back(TestCase{ "No move",{ 1, 2, 3, 4, }, 0 });
          testCases.push_back(TestCase{ "Two moves",{ 6, 3, 1, 2, 4, 5 }, 2 });
          testCases.push_back(TestCase{ "One move with repetitions",
          { 4, 1,1,1, 2,2, 3,3,3,3,3, 4 },
            1 });
        }

        MinimumMovesToSortTest::TestCase::TestCase(
          string&& name,
          Number_list&& data,
          const size_t expected)
          : BaseTestCase(forward<string>(name)),
          _Data(forward<Number_list>(data)),
          _Expected(expected)
        {
        }

        void MinimumMovesToSortTest::TestCase::Validate() const
        {
          BaseTestCase::Validate();

          const auto data = get_Data();
          const bool isSorted = is_sorted(data.begin(), data.end());
          if (isSorted)
          {
            Assert::AreEqual(size_t(0), get_Expected(), "Expected_" + get_Name());
          }
          else
          {
            Assert::Greater(get_Expected(), size_t(0), "Expected_" + get_Name());
          }
        }

        void MinimumMovesToSortTest::TestCase::Print(ostream& str) const
        {
          BaseTestCase::Print(str);

          PrintVectorUtilities::Print(", Data=", _Data, str);

          str << " Expected=" << _Expected;
        }

        void MinimumMovesToSortTest::RunTestCase(const TestCase& testCase)
        {
          using Alg = bool (*)(const Number&, const Number&);
          using NameAlg = pair<string, Alg>;

          const auto data = testCase.get_Data();
          const auto hasUniqueValues = VectorUtilities::HasUniqueValues(data);

          const vector<NameAlg> subTests = [](bool isStrict)
          {
            vector<NameAlg> subTestsTemp{ { "LessEqual",&LessEqual } };
            if (isStrict)
            {
              subTestsTemp.push_back({ "Less",&Less });
            }
            return subTestsTemp;
          }(hasUniqueValues);

          const string separator = "_";

          for (const auto& subTest : subTests)
          {
						const string name = testCase.get_Name() + separator + subTest.first;
						const auto actual = MinimumMovesToSort<Number, Alg>(data, subTest.second);
	          Assert::AreEqual(testCase.get_Expected(), actual, name);
          }
        }

        void MinimumMovesToSortTest::Test()
        {
          TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
        }

        void MinimumMovesToSortTests()
        {
          MinimumMovesToSortTest::Test();
        }
      }
    }
  }
}