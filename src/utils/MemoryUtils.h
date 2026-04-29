#pragma once
#include <cstdint>
#include <memory>
#include <type_traits>
#include <stdexcept>

class MemoryUtils {
private:
    template<typename T>
    struct is_shared_ptr : std::false_type {};
    template<typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
    template<typename T>
    static constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

    template<typename T>
    struct is_raw_pointer : std::false_type {};
    template<typename T>
    struct is_raw_pointer<T*> : std::true_type {};
    template<typename T>
    static constexpr bool is_raw_pointer_v = is_raw_pointer<T>::value;

public:
    template<typename T>
    static std::enable_if_t<!is_raw_pointer_v<T> && !is_shared_ptr_v<T>, T&>
        member_at(void* base, uintptr_t offset) {
        if (!base) throw std::runtime_error("member_at: base is null!");
        return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
    }

    template<typename T>
    static std::enable_if_t<is_raw_pointer_v<T> || is_shared_ptr_v<T>, T>
        member_at(void* base, uintptr_t offset) {
        if (!base) return nullptr;
        uintptr_t addr = reinterpret_cast<uintptr_t>(base) + offset;
        if constexpr (is_shared_ptr_v<T>) {
            return *reinterpret_cast<T*>(addr);
        }
        else {
            return reinterpret_cast<T>(*reinterpret_cast<uintptr_t*>(addr));
        }
    }

    template<typename T>
    static auto get(void* base, uintptr_t offset) {
        return member_at<T>(base, offset);
    }
};