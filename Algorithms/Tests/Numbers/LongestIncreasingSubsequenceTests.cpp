#include "LongestIncreasingSubsequence.h"
#include "LongestIncreasingSubsequenceTests.h"
#include "../PrintVectorUtilities.h"
#include "../Tests/TestUtilities.h"

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
        struct LongestIncreasingSubsequenceTest final
        {
          static void Test();

        private:

          using Number = int;
          using TContainer = vector<Number>;
          using Indexes = LongestIncreasingSubsequence<Number>::Indexes;

          class TestCase final : public BaseTestCase
          {
            TContainer _Data;
            Indexes _Expected;

          public:

            TestCase(
              string&& name,
              TContainer&& data,
              Indexes&& expected);

            inline const TContainer& get_Data() const { return _Data; }
            inline const Indexes& get_Expected() const { return _Expected; }

            void Print(ostream& str) const override;
          };

          static void GenerateTestCases(vector<TestCase>& testCases);

          static void RunTestCase(const TestCase& testCase);
        };

        void LongestIncreasingSubsequenceTest::GenerateTestCases(
          vector<TestCase>& testCases)
        {
          testCases.push_back({ "Increasing",{ 5, 10, 20, 500 },{ 0, 1, 2, 3 } });
          testCases.push_back({ "NonIncreasing",{ 50, 10 },{ 1 } });
          testCases.push_back({ "Trivial",{ -97864 },{ 0 } });
          testCases.push_back({ "Many",{ -50, 10, 90, 25, -3, 1000, 50, 500 },{ 0, 1, 3, 6, 7 } });
        }

        LongestIncreasingSubsequenceTest::TestCase::TestCase(
          string&& name, TContainer&& data, Indexes&& expected)
          : BaseTestCase(forward<string>(name)),
          _Data(forward<TContainer>(data)), _Expected(forward<Indexes>(expected))
        {
        }

        void LongestIncreasingSubsequenceTest::TestCase::Print(ostream& str) const
        {
          BaseTestCase::Print(str);

          AppendSeparator(str);
          PrintVectorUtilities::Print("Data", _Data, str);
          PrintVectorUtilities::Print("Expected", _Expected, str);
        }

        void LongestIncreasingSubsequenceTest::RunTestCase(const TestCase& testCase)
        {
          using TComparer = less<Number>;
          using Alg = Indexes(*)(const TContainer& source, TComparer);
          using NameAlg = pair<string, Alg>;

          const vector<NameAlg> subTests{
            { "Slow",LongestIncreasingSubsequence<Number>::Slow },
            { "Fast",LongestIncreasingSubsequence<Number>::Fast },
          };

          const string separator = "_";
          for (const auto& subTest : subTests)
          {
            const string name = testCase.get_Name() + separator + subTest.first;
            const auto actual = subTest.second(testCase.get_Data(), TComparer());
            Assert::AreEqual(testCase.get_Expected(), actual, name);
          }
        }

        void LongestIncreasingSubsequenceTest::Test()
        {
          TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
        }

        void LongestIncreasingSubsequenceTests(void)
        {
          LongestIncreasingSubsequenceTest::Test();
        }
      }
    }
  }
}