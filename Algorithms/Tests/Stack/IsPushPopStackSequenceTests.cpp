#include "../../PrintVectorUtilities.h"
#include "../../Stack/IsPushPopStackSequence.h"
#include "..\TestUtilities.h"
#include "IsPushPopStackSequenceTests.h"

using namespace std;
using namespace MyCompany::Algorithms::Stack;
using namespace MyCompany::Algorithms;

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Stack
    {
      namespace Tests
      {
        struct IsPushPopStackSequenceTest final
        {
          static void Test();

        private:

          IsPushPopStackSequenceTest() = delete;

          using Number = short;
          using Data = vector<Number>;

          class TestCase final : public BaseTestCase
          {
            Data _Array1;
            Data _Array2;
            bool _Expected;

          public:

            TestCase(
              string&& name,
              Data&& array1,
              Data&& array2,
              const bool expected);

            inline const Data& get_Array1() const { return _Array1; }
            inline const Data& get_Array2() const { return _Array2; }
            inline bool get_Expected() const { return _Expected; }

            void Print(ostream& str) const override;
          };

          static void GenerateTestCases(vector<TestCase>& testCases);

          static void RunTestCase(const TestCase& testCase);
        };

        void IsPushPopStackSequenceTest::GenerateTestCases(vector<TestCase>& testCases)
        {
          testCases.push_back({ "Equal",{ 1, 2, 3 },{ 1, 2, 3 }, true });
          testCases.push_back({ "Reversed",{ 1, 2, 3 },{ 3, 2, 1 }, true });
          testCases.push_back({ "213",{ 1, 2, 3 }, { 2, 1, 3 }, true });

          testCases.push_back({ "different last item", { 1, 2, 3 }, { 2, 1, 2 }, false });
          testCases.push_back({ "smaller size", { 1, 2, 3 }, { 2, 1 }, false });
          testCases.push_back({ "greater size", { 1, 2, 3 }, { 2, 1, 3, 4 }, false });
          testCases.push_back({ "all same items", { 1, 2, 3 }, { 2, 2, 2 }, false });
          testCases.push_back({ "empty2", { 1, 2, 3 }, {}, {} });
          testCases.push_back({ "impossible 312", { 1, 2, 3 }, { 3, 1, 2}, false });
        }

        IsPushPopStackSequenceTest::TestCase::TestCase(
          string&& name,
          Data&& array1, Data&& array2, const bool expected)
          : BaseTestCase(forward<string>(name)),
          _Array1(forward<Data>(array1)), _Array2(forward<Data>(array2)), _Expected(expected)
        {
        }

        void IsPushPopStackSequenceTest::TestCase::Print(ostream& str) const
        {
          BaseTestCase::Print(str);

          AppendSeparator(str);
          PrintVectorUtilities::Print("Array1", _Array1, str);
          PrintVectorUtilities::Print("Array2", _Array2, str);
          PrintValue(str, "Expected", _Expected);
        }

        void IsPushPopStackSequenceTest::RunTestCase(const TestCase& testCase)
        {
          const auto actual = IsPushPopStackSequence<Number>(
            testCase.get_Array1(), testCase.get_Array2());

          Assert::AreEqual(testCase.get_Expected(), actual, testCase.get_Name());
        }

        void IsPushPopStackSequenceTest::Test()
        {
          TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
        }

        void IsPushPopStackSequenceTests()
        {
          IsPushPopStackSequenceTest::Test();
        }
      }
    }
  }
}