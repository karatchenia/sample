#pragma once

#include <string>
#include <vector>
#include <ostream>
#include "..\TestUtilities.h"

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Stack
    {
      namespace Tests
      {
        struct IsPushPopStackSequenceTests final
        {
          static void Test();
          
        private:

          IsPushPopStackSequenceTests() = delete;

          using Number = short;
          using Data = std::vector<Number>;

          struct TestCase final : BaseTestCase
          {
            Data Array1;
            Data Array2;
            bool Expected;

            TestCase(
              std::string&& name,
              const Data& array1,
              const Data& array2 = Data(),
              const bool expected = false);

            void Print(std::ostream& str) const override;
          };

          static void GenerateTestCases(std::vector<TestCase>& testCases);

          static void RunTestCase(const TestCase& testCase);
        };
      }
    }
  }
}