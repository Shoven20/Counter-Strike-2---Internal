#include "memory.h"

struct Vector2 {
public:
    float x, y;
};
struct Vector3
{
public:
    float x, y, z;
};
struct BONEDATA
{
    Vector3 Pos;
    char pad[0x14];
};
bool Skeleton = true;
ImVec4 enemyespcolor(153.f / 255.f, 0, 0, 1.f);
ImVec4 teamespcolor(0, 153.f / 255.f, 76.f / 255.f, 1.f);
struct playerstruct {
    ImVec4 EspColor;
    BONEDATA BoneArray[30];
};
std::vector<playerstruct> player_prefabs;


struct view_matrix_t {
    float* operator[ ](int index) {
        return matrix[index];
    }
    float matrix[4][4];
};
view_matrix_t viewmatrix;

Vector2 screensize;
bool w2s(const Vector3& pos, Vector3& out) {
    out.x = viewmatrix[0][0] * pos.x + viewmatrix[0][1] * pos.y + viewmatrix[0][2] * pos.z + viewmatrix[0][3];
    out.y = viewmatrix[1][0] * pos.x + viewmatrix[1][1] * pos.y + viewmatrix[1][2] * pos.z + viewmatrix[1][3];
    float w = viewmatrix[3][0] * pos.x + viewmatrix[3][1] * pos.y + viewmatrix[3][2] * pos.z + viewmatrix[3][3];
    if (w < 0.01f || w == 0) return false;
    float inv_w = 1.f / w; out.x *= inv_w; out.y *= inv_w;
    float x = screensize.x * .5f, y = screensize.y * .5f;
    x += 0.5f * out.x * screensize.x + 0.5f; y -= 0.5f * out.y * screensize.y + 0.5f;
    out.x = x; out.y = y;
    return true;
}
 

uintptr_t entitylists = 0;
class GameSceneNode
{
public:
    uintptr_t BoneArrayAddress()
    {
        uintptr_t BoneArrayaddress = shovenmemory.read<uintptr_t>((uintptr_t)(this) + s_bonearray);
        return BoneArrayaddress;
    }
};
class CSPlayerPawn
{
public:
    static CSPlayerPawn* PlayerPawn(std::uint32_t uHandle)
    {
        std::uintptr_t ListEntry = shovenmemory.read<std::uintptr_t>(entitylists + uintptr_t(0x8 * ((uHandle & 0x7FFF) >> 9) + 16));
        if (!ListEntry)
            return 0;

        CSPlayerPawn* pCSPlayerPawn = shovenmemory.read<CSPlayerPawn*>(ListEntry + uintptr_t(120) * uintptr_t(uHandle & 0x1FF));

        return pCSPlayerPawn;
    }
    GameSceneNode* GameScenenode()
    {
        GameSceneNode* GameScenenode = shovenmemory.read<GameSceneNode*>((uintptr_t)(this) + s_gamescene);
        return GameScenenode;
    }
};
class CCSPlayerController
{
public:
    static CCSPlayerController* GetBaseEntity(int nIdx)
    {
        std::uintptr_t ListEntry = shovenmemory.read<uintptr_t>(entitylists + uintptr_t((8 * (nIdx & 0x7FFF) >> 9) + 16));
        if (!ListEntry)
            return 0;
        CCSPlayerController* ListEntry2 = shovenmemory.read<CCSPlayerController*>((ListEntry + uintptr_t(120) * uintptr_t(nIdx & 0x1FF)));
        return ListEntry2;
    }
    bool PawnIsAlive()
    {
        bool bok = shovenmemory.read<bool>(reinterpret_cast<uintptr_t>(this) + s_IsAlive);
        return bok;
    }
    int Teamnum()
    {
        int team = shovenmemory.read<int>((uintptr_t)(this) + s_teamnuOffset);
        return team;
    }
    bool bIsLocalPlayerController()
    {
        bool bok = shovenmemory.read<bool>((uintptr_t)(this) + s_bIsLocalPlayerController_Offset);
        return bok;
    }
    CSPlayerPawn* PlayerPawn()
    {
        std::uint32_t playerpawn = shovenmemory.read<std::uint32_t>((uintptr_t)(this) + s_dwPlayerPawn_Offset);
        return CSPlayerPawn::PlayerPawn(playerpawn);
    }
};

void HileVoid()
{
    shovenmemory.readsize(client + viewmatrix_Offset, viewmatrix, 64);
    CCSPlayerController* localplayer = shovenmemory.read<CCSPlayerController*>(client + localplayer_Offset);
    entitylists = shovenmemory.read<uintptr_t>(client + s_dwEntityList_Offset);
    player_prefabs.clear();
    for (int i = 0; i < 64; i++)
    {
        playerstruct PlayerThisPrefab;
         CCSPlayerController* pPlayerController = CCSPlayerController::GetBaseEntity(i);
        if (pPlayerController->bIsLocalPlayerController()) continue;
        if (pPlayerController->PawnIsAlive() <= 0) continue;
        Vector3 pos, head;
        uintptr_t BoneArrayAddress = pPlayerController->PlayerPawn()->GameScenenode()->BoneArrayAddress(); if (!BoneArrayAddress) continue;
        shovenmemory.readsize(BoneArrayAddress, PlayerThisPrefab.BoneArray, 30 * sizeof(BONEDATA)); if (!PlayerThisPrefab.BoneArray[0].pad) continue;
        PlayerThisPrefab.EspColor = pPlayerController->Teamnum() != localplayer->Teamnum() ? enemyespcolor : teamespcolor;
        player_prefabs.push_back(PlayerThisPrefab);
    }   
}