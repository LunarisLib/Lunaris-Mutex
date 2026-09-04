#include <Lunaris/Mutex/exception.h>


namespace Lunaris {
namespace Mutex {

    MutexException::MutexException(const std::string& msg) noexcept
        : std::runtime_error(msg)
    {
    }

    MutexException::MutexException(const char* msg) noexcept
        : std::runtime_error(msg)
    {
    }

    const char* MutexException::what() const noexcept {
        return std::runtime_error::what();
    }

} // namespace Mutex
} // namespace Lunaris