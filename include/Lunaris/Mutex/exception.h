#pragma once

#include <stdexcept>

namespace Lunaris {
namespace Mutex {

    class MutexException : public std::runtime_error {
    public:
        explicit MutexException(const std::string&) noexcept;
        explicit MutexException(const char*) noexcept;

        const char* what() const noexcept;
    };

} // namespace Mutex
} // namespace Lunaris