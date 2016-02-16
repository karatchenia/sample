#include <algorithm>
#include <vector>
#include <tuple>
#include "../BinaryIndexedTree.h"
#include "../../Assert.h"
#include "BinaryIndexedTreeTests.h"

using namespace std;
using namespace MyCompany::Algorithms::Trees;
using namespace MyCompany::Algorithms;

namespace
{
  using Number = short;
  using Tree = BinaryIndexedTree<Number>;

  //leftIndex, rightIndex, sum.
  using Request = tuple<size_t, size_t, Number>;
  using Request_vector = vector<Request>;

  Tree CreateTree()
  {
    const vector<size_t> indexesToAdd{ 1, 2, 4, 1, 7, 3, 12 };
    const auto maxIndex = *max_element(indexesToAdd.cbegin(), indexesToAdd.cend());

    Tree result(maxIndex);
    for (const auto& index : indexesToAdd)
    {
      result.add(index);
    }

    return result;
  }

  void CheckIndexOutOfRange(const Tree& tree)
  {
    const auto maxIndex = tree.max_index();
    const auto tooLargeIndex = 123456789;
    const string expectedMessage = "The index (123456789) must be between 1 and "
      + to_string(maxIndex) + ".";
    const string message = "CheckIndexOutOfRange";

    Assert::ExpectException<out_of_range>(
      [&](void) -> void { tree.get(tooLargeIndex, tooLargeIndex); },
      expectedMessage, message);
  }

  Request_vector GetRequests1()
  {
    //11, 2, 3, 4, 7, 12
    const Request_vector result{
      Request{ 0, 1, 2 }, Request{ 1, 1, 2 },
      Request{ 1, 12, 7 },
      Request{ 2, 3, 2 }, Request{ 2, 6, 3 },
      Request{ 5, 5, 0 }, Request{ 5, 6, 0 }, Request{ 6, 6, 0 },
      Request{ 7, 7, 1 },
    };
    return result;
  }

  void CheckRequest(const Tree& tree, const Request& request,
    const string& stepName)
  {
    const auto left = get<0>(request);
    const auto right = get<1>(request);
    const auto actual = tree.get(left, right);

    const string separator = ", ";
    const auto name = to_string(left) + separator + to_string(right) + separator + stepName;

    Assert::AreEqual(get<2>(request), actual, name);
  }

  void CheckRequests(
    const Tree& tree, const Request_vector& requests, const string& stepName)
  {
    Assert::NotEmpty(requests, stepName);

    for (const auto& request : requests)
    {
      CheckRequest(tree, request, stepName);
    }
  }

  void MakeChanges(Tree& tree)
  {
    tree.add(1, 30);
    tree.add(3, 500);
    tree.add(6, 7000);
  }

  Request_vector GetRequests2()
  {
    const Request_vector result{
      Request{ 0, 1, 32 }, Request{ 1, 1, 32 },
      Request{ 1, 12, 7537 },
      Request{ 2, 3, 502 }, Request{ 2, 6, 7503 },
      Request{ 5, 5, 0 }, Request{ 5, 6, 7000 }, Request{ 6, 6, 7000 },
      Request{ 7, 7, 1 },
    };
    return result;
  }

  void TestValueAt(const Tree& tree, const string& stepName)
  {
    const auto max_index = tree.max_index();
    Assert::Greater(max_index, BinaryIndexedTree<Number>::InitialIndex, "max_index");

    const auto prefix = stepName + "_at_";

    for (size_t i = BinaryIndexedTree<Number>::InitialIndex; i <= max_index; ++i)
    {
      const auto expected = tree.value_at(i);
      const auto actual = tree.get(i, i);
      const auto name = prefix + to_string(i);
      Assert::AreEqual(expected, actual, name);
    }
  }
}

void MyCompany::Algorithms::Trees::Tests::BinaryIndexedTreeTests(void)
{
  auto tree = CreateTree();
  CheckIndexOutOfRange(tree);
  {
    const auto requests = GetRequests1();
    CheckRequests(tree, requests, "step1");
  }
  MakeChanges(tree);
  {
    const auto requests = GetRequests2();
    CheckRequests(tree, requests, "step2");
  }
}
