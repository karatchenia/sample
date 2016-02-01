#include <vector>
#include "CalcAngleTests.h"
#include "../GeometryUtilities.h"
#include "../../Tests/TestUtilities.h"

using namespace std;
using namespace MyCompany::Algorithms::ClassProblems;
using namespace MyCompany::Algorithms;

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Geometry
    {
      namespace Tests
      {
        class CalcAngleTest final
        {
          CalcAngleTest() = delete;

        public:

          static void Test();

        private:

          using Coordinate = int;
          using TPoint = Point2D<Coordinate>;
          
          struct TestCase final : BaseTestCase
          {
            TPoint P1, P2, P3;
            Coordinate Expected;

            TestCase(string&& name,
              const TPoint& p1,
              const TPoint& p2,
              const TPoint& p3,
              Coordinate expected)
              : BaseTestCase(forward<string>(name)),
              P1(p1), P2(p2), P3(p3), Expected(expected)
            {
            }

            void Print(ostream& str) const override
            {
              BaseTestCase::Print(str);

              str << " P1=" << P1
                << ", P2=" << P2
                << ", P3=" << P3
                << ", Expected=" << Expected;
            }
          };

          static TPoint NegateX(const TPoint& point);

          static void RunTestCase(const TestCase& testCase);

          static void GenerateTestCases(vector<TestCase>& testCases);
        };

        void CalcAngleTest::GenerateTestCases(vector<TestCase>& testCases)
        {
          const TestCase simple = { "Simple",{ 1,2 },{ 10, 30 },{ 7, 50 }, 264 };

          //Changing the sign of X coordinates must negate the result.
          const TestCase simpleNegated = { "SimpleNegated",
            NegateX(simple.P1),
            NegateX(simple.P2),
            NegateX(simple.P3),
            -simple.Expected };

          testCases.insert(testCases.end(),
          {
            simple,
            simpleNegated,
            { "ZeroTrivial",{ 1,1 },{ 2,2 },{ 5,5 }, 0 },
            { "ZeroTwo",{ 1,1 },{ 2, 4 },{ 5, 13 }, 0 },
          });
        }

        CalcAngleTest::TPoint CalcAngleTest::NegateX(const TPoint& point)
        {
          return{ -point.X, point.Y };
        }

        void CalcAngleTest::RunTestCase(
          const TestCase& testCase)
        {
          const auto actual = CalcAngle<Coordinate>(
            testCase.P1,
            testCase.P2,
            testCase.P3);

          Assert::AreEqual(testCase.Expected, actual, testCase.get_Name());
        }

        void CalcAngleTest::Test()
        {
          TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
        }

        void CalcAngleTests(void)
        {
          CalcAngleTest::Test();
        }
      }
    }
  }
}