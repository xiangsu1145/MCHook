#ifndef SYNCSOULS_ACTORUNIQUEID_HPP
#define SYNCSOULS_ACTORUNIQUEID_HPP

#include <cstdint>

// 检测C++20支持
#if __cplusplus >= 202002L
#define SYNCSOULS_HAS_CPP20 1
    #include <compare>
#else
#define SYNCSOULS_HAS_CPP20 0
#endif

struct ActorUniqueID {
public:
    int64_t rawID;

    [[nodiscard]] constexpr bool operator==(ActorUniqueID const &other) const noexcept {
        return rawID == other.rawID;
    }

#if SYNCSOULS_HAS_CPP20
    [[nodiscard]] constexpr auto operator<=>(ActorUniqueID const &other) const noexcept {
        return rawID <=> other.rawID;
    }
#else
    // C++17及以下的手动实现
    [[nodiscard]] constexpr bool operator!=(ActorUniqueID const &other) const noexcept {
        return rawID != other.rawID;
    }

    [[nodiscard]] constexpr bool operator<(ActorUniqueID const &other) const noexcept {
        return rawID < other.rawID;
    }

    [[nodiscard]] constexpr bool operator<=(ActorUniqueID const &other) const noexcept {
        return rawID <= other.rawID;
    }

    [[nodiscard]] constexpr bool operator>(ActorUniqueID const &other) const noexcept {
        return rawID > other.rawID;
    }

    [[nodiscard]] constexpr bool operator>=(ActorUniqueID const &other) const noexcept {
        return rawID >= other.rawID;
    }
#endif
};

#endif // SYNCSOULS_ACTORUNIQUEID_HPP