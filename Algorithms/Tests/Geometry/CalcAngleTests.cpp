#include <vector>
#include "CalcAngleTests.h"
#include "../GeometryUtilities.h"
#include "../../Tests/TestUtilities.h"

using namespace std;
using namespace MyCompany::Algorithms::ClassProblems;
using namespace MyCompany::Algorithms::Geometry;
using namespace MyCompany::Algorithms;

namespace
{
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

  TPoint NegateX(const TPoint& point)
  {
    return{ -point.X, point.Y };
  }

  void GenerateTestCases(vector<TestCase>& testCases)
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

  void RunTestCase(
    const TestCase& testCase)
  {
    const auto actual = CalcAngle<Coordinate>(
      testCase.P1,
      testCase.P2,
      testCase.P3);

    Assert::AreEqual(testCase.Expected, actual, "Expected");
  }
}

void MyCompany::Algorithms::Geometry::Tests::CalcAngleTests()
{
  TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
}
