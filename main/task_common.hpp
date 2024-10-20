#ifndef MY_TASK_COMMON_HPP
#define MY_TASK_COMMON_HPP

#include <cstddef>
#include <cstdint>

namespace TaskCommon
{
    constexpr size_t k_systemTaskStackSize{8192};
    constexpr uint8_t k_systemTaskPriority{6};
    constexpr uint8_t k_systemTaskCoreID{0};

    constexpr size_t k_appTaskStackSize{16384};
    constexpr uint8_t k_appTaskPriority{5};
    constexpr uint8_t k_appTaskCoreID{0};

    constexpr size_t k_managerTaskStackSize{4096};
    constexpr uint8_t k_managerTaskPriority{6};
    constexpr uint8_t k_managerTaskCoreID{0};

    constexpr size_t k_lvglTaskStackSize{8192};
    constexpr uint8_t k_lvglTaskPriority{2};
    constexpr uint8_t k_lvglTaskCoreID{0};
};

#endif
