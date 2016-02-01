#pragma once

#include <sstream>
#include "../StreamUtilities.h"

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Numbers
    {
      template <typename Distance = int,
        typename TWeight = int>
      struct WeightedInterval
      {
        Distance Start;
        Distance Finish;
        TWeight Weight;

        WeightedInterval(
          const Distance& start = {},
          const Distance& finish = {},
          const TWeight& weight = {})
          : Start(start), Finish(finish), Weight(weight)
        {
        }

        virtual ~WeightedInterval() = default;

        WeightedInterval(const WeightedInterval&) = default;
        WeightedInterval& operator = (const WeightedInterval&) = default;

        WeightedInterval(WeightedInterval &&) = default;
        WeightedInterval& operator = (WeightedInterval &&) = default;

        inline bool operator <(const WeightedInterval& b) const
        {
          auto result = Start < b.Start
            || Start == b.Start && (Finish < b.Finish
              || Finish == b.Finish && Weight < b.Weight);
          return result;
        }

        virtual void Validate() const
        {
          if (Finish < Start)
          {
            std::ostringstream ss;
            ss << "Finish (" << Finish
              << ") is smaller than Start (" << Start
              << "), Weight=" << Weight
              << ".";
            StreamUtilities::ThrowException(ss);
          }
        }
      };

      //Here the "Start" is implicit - it is the previous Finish.
      template <typename Distance = int,
        typename TWeight = int>
      struct FinishWeightInterval final
      {
        Distance Finish;
        TWeight Weight;

        FinishWeightInterval(Distance finish = {}, TWeight weight = {})
          : Finish(finish), Weight(weight)
        {
        }

        inline bool operator <(const FinishWeightInterval& b) const
        {
          auto result = Finish < b.Finish
            || Finish == b.Finish && Weight < b.Weight;
          return result;
        }
      };

      //The [7, 7] interval has 1 item, [5,7] - 3 items.
      template<typename Interval>
      inline auto items_in_interval(const Interval& interval)
      {
        const auto result = interval.Finish - interval.Start + 1;
        return result;
      }

      template<typename Interval>
      inline bool is_too_wide_interval(const Interval& interval)
      {
        const auto canDo = items_in_interval(interval);
        const auto result = canDo < interval.Weight;
        return result;
      }

      template<typename Interval>
      inline bool shall_include_all_interval_items(const Interval& interval)
      {
        const auto canDo = items_in_interval(interval);
        const auto result = canDo == interval.Weight;
        return result;
      }

      template<typename Distance,
        typename TWeight,
        typename Interval>
        inline bool is_enough_remaining_intervals(
          const Distance& rightMostPosition,
          const TWeight& maxWeight,
          const TWeight& currentWeightSum,
          const Interval& interval)
      {
        const auto canDo = rightMostPosition - interval.Finish;
        const auto mustBe = maxWeight - currentWeightSum;
        const auto result = mustBe <= canDo;
        return result;
      }
    }
  }
}