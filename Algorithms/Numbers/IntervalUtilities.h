#pragma once
#include <utility>
#include <vector>

#include "Interval.h"

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Numbers
    {
      //Split the [Finish] intervals into [Start, Finish].
      //It is assumed there are no duplicates in the input.
      // Weights must not decrease.
      //The returned flag will indicate whether the intervals are valid.
      template<
        typename Distance,
        typename Weight,
        typename TInputIterator,
        typename TInput = FinishWeightInterval<Distance, Weight>,
        typename TOutput = WeightedInterval<Distance, Weight>
      >
        static std::pair<bool, std::vector<TOutput>> to_full_intervals(
          TInputIterator inputBegin, TInputIterator inputEnd,
          //The beginning of all intervals.
          const Distance startValue = {},
          const bool shallJoinZeroWeightIntervals = true)
      {
        auto isValid = true;
        std::vector<TOutput> intervals;

        auto hasPrevious = false;
        TInput previous;
        for (auto it = inputBegin; it != inputEnd; ++it)
        {
          const auto& current = *it;
          if (hasPrevious)
          {
            if (previous.Finish == current.Finish
              || current.Weight < previous.Weight)
            {
              isValid = false;
              break;
            }
            //Here: prev.Finish < curr.Finish
            // prev.Weight <= curr.Weight

            if (shallJoinZeroWeightIntervals && 0 == current.Weight)
            {//Join with the previous.
              intervals[0] = TOutput(startValue, current.Finish, current.Weight);
            }
            else
            {
              const auto delta = current.Weight - previous.Weight;

              auto& lastInterval = intervals[intervals.size() - 1];

              const auto shallMerge = shallJoinZeroWeightIntervals
                && 0 == delta && 0 == lastInterval.Weight;
              if (shallMerge)
              {
                lastInterval = TOutput(lastInterval.Start, current.Finish, delta);
              }
              else
              {
                intervals.push_back(TOutput(previous.Finish + 1, current.Finish, delta));
              }
            }
          }
          else
          {//The first interval.
            hasPrevious = true;
            intervals.push_back(TOutput(startValue, current.Finish, current.Weight));
          }

          previous = current;
        }

        return{ isValid, intervals };
      }
    }
  }
}