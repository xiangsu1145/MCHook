#pragma once

#include "Packet.h"
#include <cstdint>
#include "src/utils/Utils.h"
#include <vector>

enum class InputMode : int {
    Undefined = 0x0,
    Mouse = 0x1,
    Touch = 0x2,
    GamePad = 0x3,
    MotionController = 0x4,
};

enum class PlayerAuthActionType : int {
    Unknown = -1,
    StartDestroyBlock = 0,
    AbortDestroyBlock = 1,
    StopDestroyBlock = 2,
    GetUpdatedBlock = 3,
    DropItem = 4,
    StartSleeping = 5,
    StopSleeping = 6,
    Respawn = 7,
    StartJump = 8,
    StartSprinting = 9,
    StopSprinting = 10,
    StartSneaking = 11,
    StopSneaking = 12,
    CreativeDestroyBlock = 13,
    ChangeDimensionAck = 14,
    StartGliding = 15,
    StopGliding = 16,
    DenyDestroyBlock = 17,
    CrackBlock = 18,
    ChangeSkin = 19,
    DeprecatedUpdatedEnchantingSeed = 20,
    StartSwimming = 21,
    StopSwimming = 22,
    StartSpinAttack = 23,
    StopSpinAttack = 24,
    InteractWithBlock = 25,
    PredictDestroyBlock = 26,
    ContinueDestroyBlock = 27,
    StartItemUseOn = 28,
    StopItemUseOn = 29,
    HandledTeleport = 30,
    MissedSwing = 31,
    StartCrawling = 32,
    StopCrawling = 33,
    StartFlying = 34,
    StopFlying = 35,
    DeprecatedClientAckServerData = 36,
    StartUsingItem = 37,
    Count = 38,
};

enum class InputData : uint64_t {
    Ascend = 1ULL << 0x0,
    Descend = 1ULL << 0x1,
    NorthJump_Deprecated = 1ULL << 0x2,
    JumpDown = 1ULL << 0x3,
    SprintDown = 1ULL << 0x4,
    ChangeHeight = 1ULL << 0x5,
    Jumping = 1ULL << 0x6,
    AutoJumpingInWater = 1ULL << 0x7,
    Sneaking = 1ULL << 0x8,
    SneakDown = 1ULL << 0x9,
    Up = 1ULL << 0xA,
    Down = 1ULL << 0xB,
    Left = 1ULL << 0xC,
    Right = 1ULL << 0xD,
    UpLeft = 1ULL << 0xE,
    UpRight = 1ULL << 0xF,
    WantUp = 1ULL << 0x10,
    WantDown = 1ULL << 0x11,
    WantDownSlow = 1ULL << 0x12,
    WantUpSlow = 1ULL << 0x13,
    Sprinting = 1ULL << 0x14,
    AscendBlock = 1ULL << 0x15,
    DescendBlock = 1ULL << 0x16,
    SneakToggleDown = 1ULL << 0x17,
    PersistSneak = 1ULL << 0x18,
    StartSprinting = 1ULL << 0x19,
    StopSprinting = 1ULL << 0x1A,
    StartSneaking = 1ULL << 0x1B,
    StopSneaking = 1ULL << 0x1C,
    StartSwimming = 1ULL << 0x1D,
    StopSwimming = 1ULL << 0x1E,
    StartJumping = 1ULL << 0x1F,
    StartGliding = 1ULL << 0x20,
    StopGliding = 1ULL << 0x21,
    PerformItemInteraction = 1ULL << 0x22,
    PerformBlockActions = 1ULL << 0x23,
    PerformItemStackRequest = 1ULL << 0x24,
    HandledTeleport = 1ULL << 0x25,
    Emoting = 1ULL << 0x26,
    MissedSwing = 1ULL << 0x27,
    StartCrawling = 1ULL << 0x28,
    StopCrawling = 1ULL << 0x29,
    StartFlying = 1ULL << 0x2A,
    StopFlying = 1ULL << 0x2B,
    ReceivedServerData = 1ULL << 0x2C,
    InClientPredictedInVehicle = 1ULL << 0x2D,
    PaddlingLeft = 1ULL << 0x2E,
    PaddlingRight = 1ULL << 0x2F,
    BlockBreakingDelayEnabled = 1ULL << 0x30,
    Input_Num = 1ULL << 0x31,
};

inline InputData operator|(InputData a, InputData b) {
    return static_cast<InputData>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
}

inline InputData operator&(InputData a, InputData b) {
    return static_cast<InputData>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b));
}

inline InputData operator^(InputData a, InputData b) {
    return static_cast<InputData>(static_cast<uint64_t>(a) ^ static_cast<uint64_t>(b));
}

inline InputData operator~(InputData a) {
    return static_cast<InputData>(~static_cast<uint64_t>(a));
}

inline void operator|=(InputData& a, InputData b) {
    a = a | b;
}

inline void operator&=(InputData& a, InputData b) {
    a = a & b;
}

inline void operator^=(InputData& a, InputData b) {
    a = a ^ b;
}

inline bool operator!(InputData a) {
    return static_cast<uint64_t>(a) == 0;
}

enum class NewInteractionModel : int {
    Touch = 0,
    Crosshair = 1,
    Classic = 2,
    Count = 3,
};

enum class ClientPlayMode : unsigned int {
    Normal = 0,
    Teaser = 1,
    Screen = 2,
    Viewer = 3,
    Reality = 4,
    Placement = 5,
    LivingRoom = 6,
    ExitLevel = 7,
    ExitLevelLivingRoom = 8,
    NumModes = 9,
};


struct PlayerBlockActionData {
    PlayerAuthActionType action = PlayerAuthActionType::StartDestroyBlock;
    BlockPos position;
    int32_t facing = 0;
    PlayerBlockActionData() = default;
    PlayerBlockActionData(PlayerAuthActionType type, const BlockPos& pos, int32_t face)
        : action(type), position(pos), facing(face) {}

};


class PlayerBlockActions {
public:
    std::vector<PlayerBlockActionData> actions;
    void clear() { actions.clear(); }
    bool empty() const { return actions.empty(); }
    size_t size() const { return actions.size(); }

    void add(PlayerAuthActionType type, const BlockPos& pos, int32_t facing) {
        actions.emplace_back(type, pos, facing);
    }

};

class PlayerAuthInputPacket : public Packet{

public:
    Vec2 mRot;
    Vec3 mPos;
    float mYHeadRot;
    Vec3 mPosDelta;
    bool mIsDepart;
    bool mUnknown_flag1;
    char mFiller2_1[2];
    Vec2 mUnknown_vec2_1;
    bool mHasfalldamage;
    bool mOnGround;
    bool mUnknown_flag4;
    char mFiller2_2[1];
    // move (96-119)
    Vec2 mVehicleRotation;
    Vec2 mMove;
    Vec2 mAnalogMoveVector;
    Vec3 mGazeDir;      // 120, size=12, end132
    char mFiller3[16];   // 132, end148
    InputData mInputFlags; // 152
    InputMode mInputMode;
    ClientPlayMode mClientPlayMode;
    NewInteractionModel mInteractionModel;
    uint32_t mFiller4;
    uint64_t mTicksAlive;
    void* mDataPtr1;
    void* mDataPtr2;
    PlayerBlockActions mPlayerBlockActions;
    // Actor ID
    uint64_t mActorUniqueID;
public:

};