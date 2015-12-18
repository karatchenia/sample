/*Each of three numbers Rows, Columns, Side, is in the range[1..1000 * 1000 * 1000].
Rectangle Rows by Columns must be covered with square tiles Side by Side without cutting / breaking them.
Tiles can be rotated.
Find minimum number of tiles to cover the rectangle?

For example, to cover 15 * 16 area with 10 * 10 tiles, minimum 4 tiles are needed.
But if Rows = 11, Columns = 12, and Side = 10 as before, 3 tiles are enough:
- Use 1 tile for 10 * 10.
- Rotate 2 tiles by 45 degrees to cover two 2 by 12 areas.
*/

#include <utility>
#include <cmath>
#include <exception>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

template <typename Number = int,
  typename LongNumber = long long,
  typename DoubleNumber = double>
class Tiles final
{
  Tiles() = delete;

public:

  using CheckLimitsFunction = void(*)(const Number& number, const string& name);

  //Sides are parallel, no rotations.
  static LongNumber ParallelSides(
    const Number& rows, const Number& columns, const Number& side,
    CheckLimitsFunction checkFunction);

  //Assume sides don't have to be parallel.
  static LongNumber WithRotations(
    const Number& rows, const Number& columns, const Number& side,
    CheckLimitsFunction checkFunction);

private:

  static DoubleNumber CalcDiagonal(
    const Number& side, const Number& remainder);

  static void CalcRectangle(
    const Number& side, const DoubleNumber& diagonal,
    LongNumber& totalCount, DoubleNumber& actualSize);

  //Two diagonals are greater than side.
  static void TwoDiagonals(
    const Number& rows, const Number& columns, const Number& side,
    const DoubleNumber& diagonal1, const DoubleNumber& diagonal2,
    LongNumber& result);

  //Maybe the (reminder1 * reminder2) has already been covered?
  static bool CanDecreaseByOne(
    const Number& rows, const Number& columns, const Number& side,
    const DoubleNumber& actualSize1, const DoubleNumber& actualSize2);
};

struct TestCase final
{
  using Number = int;
  using BigNumber = long long int;

  string Name;
  Number Rows, Columns, Side;
  BigNumber ExpectedResult;

  TestCase(
    string&& name,
    Number rows, Number columns, Number side,
    BigNumber expectedResult);

  friend ostream& operator << (ostream& str, TestCase const& test);

  void Validate() const;
};

class TilesTests final
{
  TilesTests() = delete;

public:
  static void Test();

private:

  using DoubleNumber = double;

  static vector<TestCase> CreateTestCases(void);

  // x * y must have the same result as y * x.
  static void AddSymmetricTestCases(vector<TestCase>& testCases);

  static void RunTestCases(const vector<TestCase>& testCases);

  //Returns errorCount.
  static size_t RunTestCase(const TestCase& testCase);

  static void ReportResult(const size_t errorCount);
};

static void CheckLimits(const int& number, const string& name)
{
  const int minValue = 1;
  const int maxValue = 1000 * 1000 * 1000;

  const bool valid = minValue <= number && number <= maxValue;
  if (!valid)
  {
    ostringstream ss;
    ss << "The '" << name << "' value (" << number
      << ") must be between " << minValue << " and " << maxValue << ".";
    throw exception(ss.str().c_str());
  }
}

template <typename Number>
static Number Ceiling(
  const Number& dividend, const Number& divisor)
{
#ifdef _DEBUG
  if (Number(0) == divisor)
  {
    throw exception("The 'divisor' is zero in Ceiling.");
  }
#endif
  Number result = dividend / divisor;
  if (dividend % divisor)
  {
    ++result;
  }

  return result;
}

TestCase::TestCase(
  string&& name,
  Number rows, Number columns, Number side,
  BigNumber expectedResult)
  : Name(forward<string>(name)), Rows(rows), Columns(columns), Side(side),
  ExpectedResult(expectedResult)
{
}

ostream& operator<<(ostream& str, TestCase const& test)
{
  const string separator = ", ";
  const char stringSeparator = '\'';

  str << stringSeparator << test.Name << stringSeparator
    << separator << test.Rows
    << separator << test.Columns
    << separator << test.Side
    << separator << test.ExpectedResult;
  return str;
}

void TestCase::Validate() const
{
  CheckLimits(Rows, "Rows");
  CheckLimits(Columns, "Columns");
  CheckLimits(Side, "Side");
}

template <typename Number, typename LongNumber, typename DoubleNumber>
LongNumber Tiles<Number, LongNumber, DoubleNumber>::ParallelSides(
  const Number& rows, const Number& columns, const Number& side,
  CheckLimitsFunction checkFunction)
{
  if (nullptr == checkFunction)
  {
    throw exception("The 'checkFunction' must be not null.");
  }

  checkFunction(rows, "rows");
  checkFunction(columns, "columns");
  checkFunction(side, "side");

  Number side1 = Ceiling(rows, side);
  Number side2 = Ceiling(columns, side);

  const LongNumber result = LongNumber(side1) * LongNumber(side2);
  return result;
}

template <typename Number, typename LongNumber, typename DoubleNumber>
LongNumber Tiles<Number, LongNumber, DoubleNumber>::WithRotations(
  const Number& rows, const Number& columns, const Number& side,
  CheckLimitsFunction checkFunction)
{
  LongNumber result = ParallelSides(rows, columns, side, checkFunction);

  const Number zero = Number(0);
  const Number remainder1 = rows % side;
  const Number remainder2 = columns % side;

  if (zero == remainder1 && zero == remainder2)
  {//Cannot beat full match.
    return result;
  }

  DoubleNumber diagonal1 = CalcDiagonal(side, remainder1);
  DoubleNumber diagonal2 = CalcDiagonal(side, remainder2);

  if (diagonal1 <= side && diagonal2 <= side)
  {//There is no sense to try smaller sides.
    return result;
  }

  LongNumber diagonalResult = LongNumber(0);

  if (zero == remainder1 || zero == remainder2)
  {
    const DoubleNumber& diagonal = zero == remainder1 ? diagonal2 : diagonal1;
    if (diagonal <= side)
    {//Skip smaller side.
      return result;
    }

    const Number& side2 = zero == remainder1 ? rows : columns;

    DoubleNumber actualSize;
    CalcRectangle(side2, diagonal, diagonalResult, actualSize);
  }
  else
  {//Areas: [remainder1 * columns] + [rows * remainder2] - [remainder1 * remainder2].
    if (side < diagonal1 && side < diagonal2)
    {
      TwoDiagonals(rows, columns, side, diagonal1, diagonal2, diagonalResult);
    }
    else
    {
      const pair<Number, Number> p = side < diagonal1
        ? pair<Number, Number>(rows, columns) : pair<Number, Number>(columns, rows);
      const DoubleNumber& diagonal = side < diagonal1 ? diagonal1 : diagonal2;

      diagonalResult += (p.first / side);

      DoubleNumber actualSize;
      CalcRectangle(p.second, diagonal, diagonalResult, actualSize);
    }
  }

  diagonalResult += LongNumber(rows / side) * LongNumber(columns / side);

  if (diagonalResult < result)
  {
    result = diagonalResult;
  }

  return result;
}

template <typename Number, typename LongNumber, typename DoubleNumber>
DoubleNumber Tiles<Number, LongNumber, DoubleNumber>::CalcDiagonal(
  const Number& side, const Number& remainder)
{
  const DoubleNumber root = sqrt(DoubleNumber(2.0));
  DoubleNumber result = DoubleNumber(side) * root - DoubleNumber(remainder);
  return result;
}

//The "totalCount" is changed.
template <typename Number, typename LongNumber, typename DoubleNumber>
void Tiles<Number, LongNumber, DoubleNumber>::CalcRectangle(
  const Number& side, const DoubleNumber& diagonal,
  LongNumber& totalCount, DoubleNumber& actualSize)
{
  const DoubleNumber countD = DoubleNumber(side) / diagonal;
  const LongNumber count = LongNumber(countD);
  totalCount += count;

  actualSize = diagonal * DoubleNumber(count);
  const LongNumber actualSizeI = LongNumber(actualSize);

  if (actualSizeI < side)//Have fractional part?
  {
    ++totalCount;
  }
}

template <typename Number, typename LongNumber, typename DoubleNumber>
bool Tiles<Number, LongNumber, DoubleNumber>::CanDecreaseByOne(
  const Number& rows, const Number& columns, const Number& side,
  const DoubleNumber& actualSize1, const DoubleNumber& actualSize2)
{
  const LongNumber sideL = LongNumber(side);
  const LongNumber yFull = LongNumber(columns / side) * sideL;
  const LongNumber xFull = LongNumber(rows / side) * sideL;

  const bool result = yFull <= actualSize2
    || xFull <= actualSize1;
  return result;
}

template <typename Number, typename LongNumber, typename DoubleNumber>
void Tiles<Number, LongNumber, DoubleNumber>::TwoDiagonals(
  const Number& rows, const Number& columns, const Number& side,
  const DoubleNumber& diagonal1, const DoubleNumber& diagonal2,
  LongNumber& result)
{
  DoubleNumber actualSize1;
  CalcRectangle(rows, diagonal2, result, actualSize1);

  DoubleNumber actualSize2;
  CalcRectangle(columns, diagonal1, result, actualSize2);

  if (CanDecreaseByOne(rows, columns, side, actualSize1, actualSize2))
  {
    --result;
  }
}

vector<TestCase> TilesTests::CreateTestCases(void)
{
  vector<TestCase> testCases;

  //Simple
  testCases.push_back(TestCase("One", 10, 10, 10, 1));
  testCases.push_back(TestCase("Two", 10, 20, 10, 2));
  testCases.push_back(TestCase("Four", 20, 20, 10, 4));

  //Some remainders.
  testCases.push_back(TestCase("1 remainder big1", 1000 * 1000, 1000 * 1000 + 1, 10, 10000076092LL));
  testCases.push_back(TestCase("1 remainder big2", 1000 * 1000, 1000 + 1, 1000, 1708));
  testCases.push_back(TestCase("1 remainder", 10, 21, 10, 3));
  testCases.push_back(TestCase("Long line", 102, 1005, 10, 1093));
  testCases.push_back(TestCase("Thin long line", 2, 1005, 10, 83));
  testCases.push_back(TestCase("Long line2", 103, 1005, 10, 1101));
  testCases.push_back(TestCase("Thin long line2", 3, 1005, 10, 91));

  //Both sides have greater diagonals.
  testCases.push_back(TestCase("RotateBigNumbers", 10 * 1000 * 1000 + 1, 100 * 1000 * 1000 + 2, 10, 10000008432693LL));
  testCases.push_back(TestCase("RotateBigNumbers2", 1000 * 1000 * 1000 - 9, 1000 * 1000 * 1000 - 8, 1000, 1000000000000LL));
  testCases.push_back(TestCase("RotateBigNumbers3", 1000 * 1000 * 1000 - 9, 1000 * 1000 * 1000 - 8, 10, 9999999958448969LL));
  testCases.push_back(TestCase("Rotate12", 11, 12, 10, 3));
  testCases.push_back(TestCase("Rotate1", 11, 11, 10, 3));
  testCases.push_back(TestCase("Rotate2", 12, 12, 10, 3));
  testCases.push_back(TestCase("No Rotate", 12, 13, 10, 4));
  testCases.push_back(TestCase("Rectangle", 13, 13, 10, 4));

  //Subtract 1.

  // Simple solution: 24 * 503 = 12072.
  //
  // Use rotations: 
  // 230/10 * 5020/10 = 11546 completely covered tiles.
  //
  // What remains is [1 * 5022] + [2 * 231] - [1 * 2].
  //
  // diag1 = 10*sqrt(2) - 1 = 13.14213562373095;
  // diag2 = 10*sqrt(2) - 2 = 12.14213562373095;
  //
  // 19 tiles of width 2 along x,
  //  and 382 tiles of width 1 along y.
  //
  // 19 * diag2 = 230.7005768508881;
  // 382 * diag1 = 5020.295808265223;
  //
  // # of tiles is 11546 + 19 + 382 = 11947;
  //
  // What remains is an area, which is slightly smaller than [1 * 2].
  // It can be covered with 1 more tile.
  // Total # of tiles is 11948.
  // Compared with simple version, 12072 - 11948 = 124 tiles (about 1 per cent) are saved.
  //
  // Interesting question: can it be solved even better?
  testCases.push_back(TestCase("Rotate close", 231, 5022, 10, 11948));

  return testCases;
}

void TilesTests::AddSymmetricTestCases(vector<TestCase>& testCases)
{
  const string suffix = "_Symmetric";

  vector<TestCase> testCasesNew = testCases;
  for (TestCase& testCase : testCasesNew)
  {
    //TODO: p3. Ignore test cases when (testCase.Columns == testCase.Rows).
    testCase.Name += suffix;
    swap(testCase.Columns, testCase.Rows);
  }

  testCases.reserve(testCases.size() << 1);
  testCases.insert(testCases.end(), testCasesNew.begin(), testCasesNew.end());
}

void TilesTests::RunTestCases(const vector<TestCase>& testCases)
{
  size_t errorCount = 0;

  for (const TestCase& testCase : testCases)
  {
    errorCount += RunTestCase(testCase);
  }

  ReportResult(errorCount);
}

size_t TilesTests::RunTestCase(const TestCase& testCase)
{
  const TestCase::BigNumber actualBest = 
    Tiles<TestCase::Number, TestCase::BigNumber, DoubleNumber>::WithRotations(
    testCase.Rows, testCase.Columns, testCase.Side,
    CheckLimits);
  if (actualBest != testCase.ExpectedResult)
  {
    cout << "Got " << actualBest
      << " for test case: " << testCase << "." << endl;
    return 1;
  }

  return 0;
}

void TilesTests::ReportResult(const size_t errorCount)
{
  if (errorCount)
  {
    cout << "There is(are) " << errorCount << " error(s)." << endl;
  }
  else
  {
    cout << "All tests passed." << endl;
  }
}

void TilesTests::Test()
{
  vector<TestCase> testCases = CreateTestCases();
  AddSymmetricTestCases(testCases);
  RunTestCases(testCases);
}

int main()
{
  try
  {
    TilesTests::Test();
  }
  catch (const exception& e)
  {
    cout << "Error: " << e.what() << endl;
    return 1;
  }
  catch (...)
  {
    cout << "Unknown error." << endl;
    return 2;
  }

  return 0;
}