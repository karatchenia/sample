#include "../../PrintVectorUtilities.h"
#include "../../Stack/IsPushPopStackSequence.h"
#include "IsPushPopStackSequenceTests.h"

using namespace std;
using namespace MyCompany::Algorithms::Stack::Tests;
using namespace MyCompany::Algorithms::Stack;
using namespace MyCompany::Algorithms;

void IsPushPopStackSequenceTests::GenerateTestCases(vector<TestCase>& testCases)
{
  const vector<Number> simple = { 1, 2, 3 };

  testCases.push_back({ "Equal", simple, simple, true });
  testCases.push_back({ "Reversed", simple, { 3, 2, 1 }, true });
  testCases.push_back({ "213", simple, { 2, 1, 3 }, true });

  testCases.push_back({ "different last item", simple, { 2, 1, 2 }, false });
  testCases.push_back({ "smaller size", simple, { 2, 1 }, false });
  testCases.push_back({ "greater size", simple, { 2, 1, 3, 4 }, false });
  testCases.push_back({ "all same items", simple, { 2, 2, 2 }, false });
  testCases.push_back({ "empty2", simple });
  testCases.push_back({ "impossible 312", simple, { 3, 1, 2}, false });
}

IsPushPopStackSequenceTests::TestCase::TestCase(
  string&& name,
  const Data& array1, const Data& array2, const bool expected)
  : BaseTestCase(forward<string>(name)),
  Array1(array1), Array2(array2), Expected(expected)
{
}

void IsPushPopStackSequenceTests::TestCase::Print(ostream& str) const
{
  BaseTestCase::Print(str);

  AppendSeparator(str);
  PrintVectorUtilities::Print("Array1", Array1, str);
  PrintVectorUtilities::Print("Array2", Array2, str);
  PrintValue(str, "Expected", Expected);
}

void IsPushPopStackSequenceTests::RunTestCase(const TestCase& testCase)
{
  const bool actual = IsPushPopStackSequence<Number>(
    testCase.Array1, testCase.Array2);

  Assert::AreEqual(testCase.Expected, actual, testCase.GetName());
}

void IsPushPopStackSequenceTests::Test()
{
  TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
}