#pragma once

#include<stdarg.h>
#include "../StreamUtilities.h"
#include "../ClassProblems\Point.h"

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Geometry
    {
      //Calculate the angle between 3 points.
      template <typename Coordinate = long long,
        typename Point = MyCompany::Algorithms::ClassProblems::Point2D<Coordinate>>
      inline Coordinate CalcAngle(
        const Point& p1,
        const Point& p2,
        const Point& p3)
      {
        // angle(p1, p2) = (y2 - y1)/(x2 - x1)
        // angle(p1, p3) = (y3 - y1)/(x3 - x1)
        // difference = [angle(p1, p3) - angle(p1, p2)] * (x3 - x1) * (x2 - x1) =
        //  = (y3 - y1)*(x2 - x1) - (y2 - y1)*(x3 - x1).

        const Coordinate result = (p3.Y - p1.Y)*(p2.X - p1.X)
          - (p2.Y - p1.Y)*(p3.X - p1.X);
        return result;
      }

      enum class PointPosition
      {
        //The point is strictly outside the region.
        Outside,
        //The point is strictly inside the region.
        Inside,
        //The point belongs to a side.
        AlongSide,
      };

      inline std::ostream& operator << (
        std::ostream& str, PointPosition const& position)
      {
        switch (position)
        {
        case PointPosition::Outside: str << "Outside"; break;
        case PointPosition::Inside: str << "Inside"; break;
        case PointPosition::AlongSide: str << "Along a side"; break;
#ifdef _DEBUG
        default :
        {
          std::ostringstream ss;
          ss << "Unknown value of PointPosition="
            << to_string(static_cast<int>(position));

          StreamUtilities::ThrowException<std::out_of_range>(ss);
        }
#endif
        }

        return str;
      }

      template <typename Coordinate, typename Point>
      static inline bool IsAlongSide(
        const Point& checkPoint,
        const Point& previous,
        const Point& current,
        bool& positive,
        bool& negative)
      {
        const auto angle = CalcAngle<Coordinate, Point>(checkPoint, previous, current);
        if (0 == angle)
        {
          return true;
        }

        if (0 < angle)
        {
          negative = false;
        }
        else
        {
          positive = false;
        }

        return false;
      }

      //Computes relative position of a point
      // to a convex region consisting of 3 or more points.
      //
      //The first point is to be checked whether it is inside a convex.
      //The remaining points are assumed to form a convex.
      //All the points must be distinct.
      //
      //When the "first" of the returned value is "AlongSide",
      // the "second" is which segment the "checkPoint" belongs to.
      template <typename Coordinate = long long,
        typename Point = MyCompany::Algorithms::ClassProblems::Point2D<Coordinate>>
        std::pair<PointPosition, size_t> ConvexToPointPosition(
          const Point& checkPoint,
          const size_t size,
          const Point ...)
      {
        constexpr size_t minSize = 3;
        if (size < minSize)
        {
          std::ostringstream ss;
          ss << "ConvexPosition: The size("
            << size << ") must be at least " << minSize << ".";
          StreamUtilities::ThrowException<std::out_of_range>(ss);
        }

        auto position = PointPosition::Outside;
        size_t index = 0;
        auto positive = true;
        auto negative = true;

        va_list args;
        va_start(args, size);

        const Point firstPoint = va_arg(args, Point);
        auto previous = firstPoint;

        for (;;)
        {
          const Point current = va_arg(args, Point);

          if (IsAlongSide<Coordinate, Point>(
            checkPoint, previous, current,
            positive, negative))
          {
            position = PointPosition::AlongSide;
            break;
          }

          if ((size - 1) //The first point is already taken.
            == ++index)
          {//Check last - first.
            if (IsAlongSide<Coordinate, Point>(
              checkPoint, current, firstPoint,
              positive, negative))
            {
              position = PointPosition::AlongSide;
            }

            break;
          }

          previous = current;
        }
        va_end(args);

        if (PointPosition::AlongSide == position)
        {
#ifdef _DEBUG
          if (!(positive || negative))
          {
            throw std::runtime_error(
              "The point is Outside and AlongSide - the convex might be concave.");
          }
#endif
          return { PointPosition::AlongSide, index };
        }

        if (positive || negative)
        {
          return{ PointPosition::Inside, 0 };
        }

        return{ PointPosition::Outside, 0 };
      }
    }
  }
}