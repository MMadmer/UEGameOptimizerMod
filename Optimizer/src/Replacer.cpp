#include "Replacer.hpp"
#include <UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <GameplayStatics.hpp>
#include <Constructs/Views/EnumerateView.hpp>

#include "BPMacros.hpp"
#include "UActorComponent.hpp"
#include "UFunctionUtils.hpp"
#include "FunctionTypes.hpp"

using namespace ::RC::Unreal;

void Replacer::ClearTrashMeshes()
{
    UClass* MeshActorClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/Engine.StaticMeshActor"));
    UClass* MeshCompClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/Engine.StaticMeshComponent"));

    const UObject* ContextActor = Unreal::UObjectGlobals::FindFirstOf(STR("Actor"));
    if (!ContextActor)
    {
        Output::send<LogLevel::Warning>(STR("[Optimizer] Can't clear empty static mesh actors. Actors not found!\n"));
        return;
    }

    Unreal::TArray<AActor*> MeshActors;
    UGameplayStatics::GetAllActorsOfClass(ContextActor, MeshActorClass, MeshActors);

    int32 InvalidMeshes = 0;
    for (const auto& MeshActor : MeshActors)
    {
        const Unreal::TArray<UObject*>& MeshComponents = MeshActor->GetComponentsByClass(MeshCompClass);
        if (!MeshComponents.IsValidIndex(0))
        {
            MeshActor->K2_DestroyActor();
            InvalidMeshes++;
            continue;
        }

        // Clear empty meshes
        const UObject* StaticMesh = *MeshComponents[0]->GetValuePtrByPropertyNameInChain<UObject*>(STR("StaticMesh"));
        if (!StaticMesh)
        {
            MeshActor->K2_DestroyActor();
            InvalidMeshes++;
        }
    }

    Output::send<LogLevel::Default>(STR("[Optimizer] Destroyed empty static mesh actors: {}\n"), InvalidMeshes);
}

void Replacer::ConvertMeshToHism()
{
    UClass* MeshActorClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/Engine.StaticMeshActor"));
    UClass* MeshCompClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, STR("/Script/Engine.StaticMeshComponent"));
    UClass* HismClass = Unreal::UObjectGlobals::StaticFindObject<UClass*>(nullptr,
                                                                          nullptr,
                                                                          STR("/Script/Engine.HierarchicalInstancedStaticMeshComponent"));

    const AActor* ContextActor = Cast<AActor>(Unreal::UObjectGlobals::FindFirstOf(STR("Actor")));
    if (!ContextActor)
    {
        Output::send<LogLevel::Warning>(STR("[Optimizer] Can't convert static mesh actors to HISM. Actors not found!\n"));
        return;
    }

    AActor* MainMeshActor = nullptr;
    Unreal::TArray<FUniqueMesh> UniqueMeshes;
    Unreal::TArray<UActorComponent*> UniqueHismArray;
    int32 HismInstances = 0;
    Unreal::TArray<AActor*> MeshActors;
    UGameplayStatics::GetAllActorsOfClass(ContextActor, MeshActorClass, MeshActors);

    Output::send<LogLevel::Default>(STR("[Optimizer] Static mesh actors before optimizing: {}\n"), MeshActors.Num());

    for (const auto& MeshActor : MeshActors)
    {
        const Unreal::TArray<UObject*>& MeshComponents = MeshActor->GetComponentsByClass(MeshCompClass);
        if (!MeshComponents.IsValidIndex(0)) continue;

        UObject* StaticMesh = *MeshComponents[0]->GetValuePtrByPropertyNameInChain<UObject*>(STR("StaticMesh"));
        if (!StaticMesh) continue;

        if (!MainMeshActor) MainMeshActor = MeshActor;
        if (MeshActor == MainMeshActor) continue;

        // Find unique mesh
        FUniqueMesh UniqueMesh{StaticMesh, Unreal::TArray<UObject*>()};
        FGetMaterials GetMaterialsArgs{};
        UFunctionUtils::TryCallUFunction(MeshComponents[0], STR("GetMaterials"), &GetMaterialsArgs);
        bool IsValidMaterials = true;
        for (const auto& Material : GetMaterialsArgs.ReturnValue)
        {
            if (Material)
            {
                FGetBaseMaterial BaseMaterialArgs{};
                UFunctionUtils::TryCallUFunction(Material, STR("GetBaseMaterial"), &BaseMaterialArgs);
                if (BaseMaterialArgs.ReturnValue)
                {
                    if (!static_cast<bool>(*BaseMaterialArgs.ReturnValue->GetValuePtrByPropertyNameInChain<uint8>(STR("bUsedWithInstancedStaticMeshes"))))
                    {
                        IsValidMaterials = false;
                        break;
                    }
                }
            }
            UniqueMesh.Materials.Add(Material);
        }
        if (!IsValidMaterials) continue;

        // Add unique mesh and unique HISM to arrays
        if (UniqueMeshes.Find(UniqueMesh) < 0)
        {
            // Create new unique HISM
            FAddComponentByClass ComponentArgs{HismClass, false, FTransform(), false};
            ComponentArgs.Transform.GetScale3D() = FVector(1.0f, 1.0f, 1.0f);
            UFunctionUtils::TryCallUFunction(MainMeshActor, STR("AddComponentByClass"), &ComponentArgs);

            // Set normal component scale
            FSetWorldScale3D Scale3D{FVector(1.0f, 1.0f, 1.0f)};
            UFunctionUtils::TryCallUFunction(ComponentArgs.ReturnValue, STR("SetWorldScale3D"), &Scale3D);

            // Set component static mesh
            FSetMesh Params{StaticMesh};
            UFunctionUtils::TryCallUFunction(ComponentArgs.ReturnValue, STR("SetStaticMesh"), &Params);

            // Set component materials
            for (const auto& [Material, MaterialIndex] : UniqueMesh.Materials | views::enumerate)
            {
                FSetMaterial SetMaterialArgs{static_cast<int32>(MaterialIndex), Material};
                UFunctionUtils::TryCallUFunction(ComponentArgs.ReturnValue, STR("SetMaterial"), &SetMaterialArgs);
            }

            // Apply visibility
            FIsVisible IsVisibleArgs{};
            UFunctionUtils::TryCallUFunction(MeshComponents[0], STR("IsVisible"), &IsVisibleArgs);

            if (!IsVisibleArgs.ReturnValue || static_cast<bool>(*MeshActor->GetValuePtrByPropertyNameInChain<uint8>(STR("bHidden"))))
            {
                FSetVisibility SetVisibilityArgs{false, true};
                UFunctionUtils::TryCallUFunction(ComponentArgs.ReturnValue, STR("SetVisibility"), &SetVisibilityArgs);
            }

            // Apply collision profile
            FGetCollisionProfileName GetCollisionProfileNameArgs{};
            UFunctionUtils::TryCallUFunction(MeshComponents[0], STR("GetCollisionProfileName"), &GetCollisionProfileNameArgs);
            FSetCollisionProfileName SetCollisionProfileNameArgs{GetCollisionProfileNameArgs.ReturnValue};
            UFunctionUtils::TryCallUFunction(MeshComponents[0], STR("SetCollisionProfileName"), &SetCollisionProfileNameArgs);

            // Add new unique HISM to array
            UniqueHismArray.Add(ComponentArgs.ReturnValue);

            // Add unique mesh to unique meshes
            UniqueMeshes.Add(UniqueMesh);
        }

        // Find HISM with current static mesh actor's mesh and materials
        UActorComponent* CurrentHism{};
        for (const auto& UniqueHism : UniqueHismArray)
        {
            FGetMaterials GetCurrentMaterialsArgs{};
            UFunctionUtils::TryCallUFunction(UniqueHism, STR("GetMaterials"), &GetCurrentMaterialsArgs);
            if (StaticMesh == *UniqueHism->GetValuePtrByPropertyNameInChain<UObject*>(STR("StaticMesh")) && UniqueMesh.Materials == GetCurrentMaterialsArgs.
                ReturnValue)
            {
                CurrentHism = UniqueHism;
                break;
            }
        }
        if (!CurrentHism)
        {
            Output::send<LogLevel::Warning>(STR("[Optimizer] Current HISM not found!\n"));
            break;
        }

        const FTransform InstanceTransform = MeshActor->GetTransform();
        FAddInstance InstanceArgs{InstanceTransform};
        UFunctionUtils::TryCallUFunction(CurrentHism, STR("AddInstanceWorldSpace"), &InstanceArgs);
        HismInstances++;

        if (MeshActor != MainMeshActor) MeshActor->K2_DestroyActor();
    }

    // Check unique HISM
    Output::send<LogLevel::Default>(STR("[Optimizer] Unique HISM: {}\n"), UniqueHismArray.Num());
    Output::send<LogLevel::Default>(STR("[Optimizer] Total HISM instances: {}\n"), HismInstances);

    // Check last static mesh actors
    MeshActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(ContextActor, MeshActorClass, MeshActors);
    Output::send<LogLevel::Default>(STR("[Optimizer] Static mesh actors after optimizing: {}\n"), MeshActors.Num());
}
