#pragma once
#include <vector>
#include <stdexcept>
#include "../StreamUtilities.h"

namespace MyCompany
{
	namespace Algorithms
	{
		namespace Trees
		{
			//Fenwick tree running time is O(log(N)), where N is the input size.
			//It is ideal when 1) adding a number to a slot,
			// and then 2) calculating the sum between consecutive slots i to j, where i <= j.
			//Internally, a node stores the value of itself plus its left subtree.
			//Linear space is required.
			//
			//Note: The indexes start from 1.
			template <typename Number>
			class BinaryIndexedTree final
			{
				std::vector<Number> _Data;

			public:

				static constexpr size_t InitialIndex = 1;

				explicit BinaryIndexedTree(size_t initialSize);

				//Return the maximum supported index.
				inline size_t max_index() const
				{
					return _Data.size() - InitialIndex;
				}

				//When "leftInclusive" is ether 0 or 1,
				// the sum is taken from the beginning to the "rightInclusive".
				//Otherwise, the returned sum is taken between indexes inclusively.
				Number get(size_t leftInclusive, size_t rightInclusive) const;
				
				//Return the sum from 1 to the "index".
				Number get(size_t index) const;

				//Return the scalar value at "index",
				// which is semantically equivalent to get(index, index),
				// but can run faster.
				Number value_at(size_t index) const;

				void add(size_t index, const Number& increment = Number(1));

			private:

				void check_index(const size_t index) const;
			};

			template <typename Number>
			BinaryIndexedTree<Number>::BinaryIndexedTree(size_t size)
				: _Data(size <= 1
					? 2 //Use 2 to generate the proper exception in the "check_index()".
					: size + InitialIndex)
			{
			}

			template <typename Number>
			Number BinaryIndexedTree<Number>::get(
			  size_t leftInclusive, size_t rightInclusive) const
			{
#ifdef _DEBUG
				if (rightInclusive < leftInclusive)
				{
					std::ostringstream ss;
					ss << "The rightInclusive (" << rightInclusive
						<< ") cannot be smaller than leftInclusive (" << leftInclusive
						<< "), size=" << (_Data.size()) << ".";
					StreamUtilities::ThrowException<std::out_of_range>(ss);
				}
#endif
				const auto result = leftInclusive <= InitialIndex
					? get(rightInclusive)
					: static_cast<Number>(get(rightInclusive) - get(leftInclusive - InitialIndex));
				return result;
			}

			template <typename Number>
			Number BinaryIndexedTree<Number>::get(size_t index) const
			{
				check_index(index);

				Number result {};
				do
				{
					result += _Data[index];
					index &= index - 1; //Remove the right-most 1-bit.
				} while (0 != index);

				return result;
			}

			template <typename Number>
			Number BinaryIndexedTree<Number>::value_at(size_t index) const
			{
				check_index(index);

				Number result = _Data[index];

				const auto stopIndex = index & (index - 1); //Remove the right-most 1-bit.
				--index;

				while (stopIndex != index)
				{
					result -= _Data[index];
					index &= index - 1;
				}

				return result;
			}

			template <typename Number>
			void BinaryIndexedTree<Number>::add(size_t index, const Number& increment)
			{
				check_index(index);

				const auto size = _Data.size();
				do
				{
					_Data[index] += increment;
					index += index& (0 - index); //Add the right-most 1-bit.
				} while (index < size);
			}

			template <typename Number>
			void BinaryIndexedTree<Number>::check_index(const size_t index) const
			{
				if (0 == index || _Data.size() <= index)
				{
					std::ostringstream ss;
					ss << "The index (" << index
						<< ") must be between " << InitialIndex
						<< " and " << (_Data.size() - InitialIndex) << ".";
					StreamUtilities::ThrowException<std::out_of_range>(ss);
				}
			}
		}
	}
}