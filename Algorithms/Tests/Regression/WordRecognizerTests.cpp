#include "../../Tests/TestUtilities.h"
#include "../../PrintUtilities.h"
#include "..\..\Regression\WordRecognizer.h"
#include "WordRecognizerTests.h"

using namespace std;
using namespace MyCompany::Algorithms::Regression;
using namespace MyCompany::Algorithms;

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Regression
    {
      namespace Tests
      {
        class WordRecognizerTest final
        {
          WordRecognizerTest() = delete;

        public:

          static void Test(void);

        private:

          using TSize = unsigned int;
          using TWeight = int;
          using TDictionary = unordered_set<string>;
          using WordPosition = WordPosition<TSize>;
          using WordRecognizer = WordRecognizer<TSize, TWeight, TDictionary>;

          struct TestCase final : BaseTestCase
          {
            string Text;
            TDictionary Words;

            TWeight ExpectedCost;
            vector<WordPosition> ExpectedPositions;

            TestCase(string&& name,
              string&& text,
              TDictionary&& words,
              const TWeight expectedCost,
              vector<WordPosition>&& expectedPositions
              )
              : BaseTestCase(forward<string>(name)),
              Text(forward<string>(text)),
              Words(forward<TDictionary>(words)),
              ExpectedCost(expectedCost),
              ExpectedPositions(forward<vector<WordPosition>>(expectedPositions))
            {
            }

            void Print(ostream& str) const override
            {
              BaseTestCase::Print(str);

              //TODO: p3. Remove "AppendSeparator".
              AppendSeparator(str);
              ::PrintValue(str, "Text", Text);

              AppendSeparator(str);
              ::Print("Words", Words.cbegin(), Words.cend(), str);

              str << "ExpectedCost" << ExpectedCost;

              AppendSeparator(str);
              ::Print("ExpectedPositions", ExpectedPositions, str);
            }

            void Validate() const override;

          private:

            void ValidateExpectedPositions() const;
            void CheckPosition(const size_t index) const;
            void CheckLastPosition() const;
          };

          static void RunTestCase(const TestCase& testCase);

          static void GenerateTestCases(vector<TestCase>& testCases);
        };

        void WordRecognizerTest::GenerateTestCases(vector<TestCase>& testCases)
        {
          {
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(3)
              + WordRecognizer::EvaluateWeight(10);

            testCases.push_back({ "Simple1",
              //"the blackboard", lengths are 3 and 10.
              std::string("the") + "black" + "board",
              { "black", "blackboard", "board", "lack", "the" },
              expectedCost,
              { { 0, 3 },{ 3, 10 } } });
          }
          {
            //they outh event == 16 + 16 + 25 == 57 == not best.
            //the youth event == 59
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(3)
              + (WordRecognizer::EvaluateWeight(5) * 2);

            testCases.push_back({ "Several combinations",
              std::string("the") + "youth" + "event",
              { "the", "they", "youth", "outh", "event", "vent" },
              expectedCost,
              { { 0, 3 },{ 3, 5 },{ 8, 5 } } });
          }
          {
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(1);

            testCases.push_back({ "One letter",
              "a",
              { "a" },
              expectedCost,
              { { 0, 1 } } });
          }
          {
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(1) * 2;

            testCases.push_back({ "Two letters - two words",
              "ab",
              { "a", "b" },
              expectedCost,
              { { 0, 1 },{ 1, 1 } } });
          }
          {
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(2);

            testCases.push_back({ "Two letters - one word",
              "ab",
              { "a", "b", "ab" },
              expectedCost,
              { { 0, 2 } } });
          }
          {
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(4)
              + (WordRecognizer::EvaluateWeight(2) * 2)
              - (WordRecognizer::EvaluateWeight(1) * 3);

            testCases.push_back({ "Some not-existing words",
              //a h is this it ?
              "ahisthisit?",
              { "is", "it", "this" },
              expectedCost,{ { 0, 1 },{ 1, 1 },
              { 2, 2 },{ 4, 4 },{ 8, 2 },{ 10, 1 } } });
          }
          {
            constexpr auto expectedCost = -WordRecognizer::EvaluateWeight(1) * 4;

            testCases.push_back({ "All not-existing words",
              "ABBA",
              { "Mamma", "Mia", "!" },
              expectedCost,
              { { 0, 1 },{ 1, 1 },{ 2, 1 },{ 3, 1 } } });
          }
          {
            constexpr auto expectedCost = WordRecognizer::EvaluateWeight(9)
              + 2 * WordRecognizer::EvaluateWeight(4)
              - 2 * WordRecognizer::EvaluateWeight(1);

            testCases.push_back({ "Long text",
              "extremely long text",
              { "text", "tex", "re", "long", "extremely", "extreme", "ext", "ex" },
              expectedCost,
              { { 0, 9 },{ 9, 1 },{ 10, 4 },{ 14, 1 },{ 15, 4 } } });
          }
        }

        void WordRecognizerTest::TestCase::Validate() const
        {
          BaseTestCase::Validate();
          ThrowIfEmpty(Text, "Text");
          ThrowIfEmpty(Words, "Words");
          ValidateExpectedPositions();
        }

        void WordRecognizerTest::TestCase::ValidateExpectedPositions() const
        {
          ThrowIfEmpty(ExpectedPositions, "ExpectedPositions");
          CheckPosition(0);

          const auto length = ExpectedPositions.size();
          for (size_t i = 1; i < length; ++i)
          {
            const auto& previous = ExpectedPositions[i - 1];
            const auto& current = ExpectedPositions[i];

            CheckPosition(i);

            if (previous.get_Offset() + previous.get_Length() != current.get_Offset())
            {
              ostringstream ss;
              ss << "Bad Offset at index=" << i;
              StreamUtilities::ThrowException(ss);
            }
          }

          CheckLastPosition();
        }

        void WordRecognizerTest::TestCase::CheckPosition(const size_t index) const
        {
          const auto indexAsString = to_string(index);
          Assert::Greater(ExpectedPositions.size(), index, "Too large index=" + indexAsString);

          const auto textSize = Text.size();
          const auto offsetName = "Offset at index=" + indexAsString;
          const auto& position = ExpectedPositions[index];
          if (0 == index)
          {
            Assert::AreEqual(TSize(0), position.get_Offset(), offsetName);
          }
          else
          {
            Assert::Greater(position.get_Offset(), TSize(0), offsetName);
            Assert::GreaterOrEqual(textSize, position.get_Offset(), offsetName);
          }

          const auto lengthName = "Length at index=" + indexAsString;
          Assert::Greater(position.get_Length(), TSize(0), lengthName);
          Assert::GreaterOrEqual(textSize, position.get_Length(), lengthName);

          const auto sum = position.get_Offset() + position.get_Length();
          Assert::GreaterOrEqual(textSize, sum, "(Offset + Length) at index=" + indexAsString);
        }
        
        void WordRecognizerTest::TestCase::CheckLastPosition() const
        {
          const auto& last = ExpectedPositions[ExpectedPositions.size() - 1];
          const auto sum = last.get_Offset() + last.get_Length();
          Assert::AreEqual(Text.size(), sum, "Last position (Offset + Length)");
        }

        void WordRecognizerTest::RunTestCase(const TestCase& testCase)
        {
          const auto costPositions = WordRecognizer::Recognize(testCase.Text, testCase.Words);
          Assert::AreEqual(testCase.ExpectedCost, costPositions.first, "Cost");
          Assert::AreEqual(testCase.ExpectedPositions, costPositions.second, "Positions");
        }

        void WordRecognizerTest::Test()
        {
          TestUtilities<TestCase>::Test(RunTestCase, GenerateTestCases);
        }

        void WordRecognizerTests(void)
        {
          WordRecognizerTest::Test();
        }
      }
    }
  }
}
