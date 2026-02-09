#pragma once
#include "Packet.h"
#include "../../../Utils/Maths.h"
#include <bitset>

enum class InputData : uint64_t {
    Ascend = 1 << 0,
    Descend = 1 << 1,
    NorthJump = 1 << 2,
    JumpDown = 1 << 3,
    SprintDown = 1 << 4,
    ChangeHeight = 1 << 5,
    Jumping = 1 << 6,
    AutoJumpingInWater = 1 << 7,
    Sneaking = 1 << 8,
    SneakDown = 1 << 9,
    Up = 1 << 10,
    Down = 1 << 11,
    Left = 1 << 12,
    Right = 1 << 13,
    UpLeft = 1 << 14,
    UpRight = 1 << 15,
    WantUp = 1 << 16,
    WantDown = 1 << 17,
    WantDownSlow = 1 << 18,
    WantUpSlow = 1 << 19,
    Sprinting = 1 << 20,
    AscendBlock = 1 << 21,
    DescendBlock = 1 << 22,
    SneakToggleDown = 1 << 23,
    PersistSneak = 1 << 24,
    StartSprinting = 1 << 25,
    StopSprinting = 1 << 26,
    StartSneaking = 1 << 27,
    StopSneaking = 1 << 28,
    StartSwimming = 1 << 29,
    StopSwimming = 1 << 30,
    StartJumping = 1 << 31,
    StartGliding = 1ll << 32,
    StopGliding = 1ll << 33,
    PerformItemInteraction = 1ll << 34,
    PerformBlockActions = 1ll << 35,
    PerformItemStackRequest = 1ll << 36,
    HandledTeleport = 1ll << 37,
    Emoting = 1ll << 38,
    MissedSwing = 1ll << 39,
    StartCrawling = 1ll << 40,
    StopCrawling = 1ll << 41,
    StartFlying = 1ll << 42,
    StopFlying = 1ll << 43,
    ReceivedServerData = 1ll << 44,
    InClientPredictedInVehicle = 1ll << 45,
    PaddlingLeft = 1ll << 46,
    PaddlingRight = 1ll << 47,
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

enum class InputModeAuth : int {
    Undefined = 0x0,
    Mouse = 0x1,
    Touch = 0x2,
    GamePad = 0x3,
    MotionController = 0x4,
};

enum class ClientPlayMode : int {
    Normal = 0x0,
    Teaser = 0x1,
    Screen = 0x2,
    Viewer = 0x3,
    Reality = 0x4,
    Placement = 0x5,
    LivingRoom = 0x6,
    ExitLevel = 0x7,
    ExitLevelLivingRoom = 0x8,
    NumModes = 0x9,
};



class PackedItemuseLegacyInventoryTransaction;
struct ItemStackRequestData;

class PlayerAuthInputPacket : public Packet {
public:
	Vec2<float> rotation;
	Vec3<float> position;
	float headYaw;
	Vec3<float> velocity;
	Vec2<float> mAnalogMoveVector;
	Vec2<float> mVehicleRotation;
	Vec2<float> mMove;
	Vec3<float> mGazeDir;
	//std::bitset<39> mInputData;
    InputData                                                  mInputData;
	int mInputMode;
	int mClientPlayMode;
	int mNewInteractionModel;
	uint64_t mClientTick;
	std::unique_ptr<PackedItemuseLegacyInventoryTransaction> mItemUseTransaction;
	std::unique_ptr<ItemStackRequestData> mItemStackRequest;
	int playerBlockActions;
	uint64_t mPredictedVehicle;
};
