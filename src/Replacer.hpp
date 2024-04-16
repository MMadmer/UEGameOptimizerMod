#pragma once

#include <Mod/CppUserModBase.hpp>

#include "UnrealDef.hpp"

namespace RC::GUI::Dumpers
{
    using UMaterialInterface = UObject;

    enum
    {
        MAX_TEXCOORDS = 4,
        MAX_STATIC_TEXCOORDS = 8
    };

    struct FMeshUVChannelInfo
    {
        bool bInitialized;

        /** Whether this values was set manually or is auto generated. */
        bool bOverrideDensities;

        /**
         * The UV density in the mesh, before any transform scaling, in world unit per UV.
         * This value represents the length taken to cover a full UV unit.
         */
        float LocalUVDensities[MAX_TEXCOORDS];
    };

    struct FStaticMaterial_420AndBelow
    {
        using UMaterialInterface = Unreal::UObject;
        UMaterialInterface* MaterialInterface;

        /*This name should be use by the gameplay to avoid error if the skeletal mesh Materials array topology change*/
        FName MaterialSlotName;

        /*This name should be use when we re-import a skeletal mesh so we can order the Materials array like it should be*/
        FName ImportedMaterialSlotName;

        /** Data used for texture streaming relative to each UV channels. */
        FMeshUVChannelInfo UVChannelData;
    };
}

struct FUniqueMesh
{
    using UStaticMesh = UObject;
    using UMaterialInterface = GUI::Dumpers::UMaterialInterface;

    UStaticMesh* StaticMesh;
    TArray<UMaterialInterface*> Materials;

    bool operator==(const FUniqueMesh& Other) const
    {
        return StaticMesh == Other.StaticMesh && Materials == Other.Materials;
    }
};

class Replacer final : public CppUserModBase
{
public:
    Replacer() = delete;

    /** Clear all loaded static mesh actors with empty static mesh in the world. */
    static void ClearTrashMeshes();

    /** Replace all static mesh actors with HISM. HISM will be attached to the GameModeBase actor. */
    static void ConvertMeshToHism();
};
