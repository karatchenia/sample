#pragma once

#include <string>
#include <ostream>

#include "../PrintUtilities.h"

namespace MyCompany
{
  namespace Algorithms
  {
    namespace Tests
    {
      class BaseTestCase
      {
        std::string Name;

      protected:

        explicit BaseTestCase(std::string&& name);

      public:

        virtual ~BaseTestCase() = default;

        BaseTestCase(const BaseTestCase&) = default;
        BaseTestCase& operator = (const BaseTestCase&) = default;

        BaseTestCase(BaseTestCase &&) = default;
        BaseTestCase& operator = (BaseTestCase &&) = default;

        inline const std::string& get_Name() const
        {
          return Name;
        }

        inline void set_Name(std::string&& name)
        {
          Name = std::forward<std::string>(name);
        }

        virtual void Validate() const;

        virtual void Print(std::ostream& str) const;

        friend std::ostream& operator << (
          std::ostream& str, BaseTestCase const & t);
      };
    }
  }
}