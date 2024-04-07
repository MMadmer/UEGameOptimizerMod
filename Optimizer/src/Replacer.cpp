#include "Replacer.hpp"
#include <UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <GameplayStatics.hpp>

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
    Unreal::TArray<AActor*> MeshActors;
    UGameplayStatics::GetAllActorsOfClass(ContextActor, MeshActorClass, MeshActors);

    for (const auto& MeshActor : MeshActors)
    {
        const Unreal::TArray<UObject*>& MeshComponents = MeshActor->GetComponentsByClass(MeshCompClass);
        if (!MeshComponents.IsValidIndex(0)) continue;

        UObject* StaticMesh = *MeshComponents[0]->GetValuePtrByPropertyNameInChain<UObject*>(STR("StaticMesh"));
        if (!StaticMesh) continue;

        if (!MainMeshActor) MainMeshActor = MeshActor;

        // Find unique meshes
        FUniqueMesh UniqueMesh{StaticMesh, Unreal::TArray<UObject*>()};
        bool IsValidMaterials = true;
        for (const auto& Material : *StaticMesh->GetValuePtrByPropertyName<Unreal::TArray<GUI::Dumpers::FStaticMaterial_420AndBelow>>(STR("StaticMaterials")))
        {
            if (Material.MaterialInterface)
            {
                FGetBaseMaterial BaseMaterialArgs{};
                UFunctionUtils::TryCallUFunction(Material.MaterialInterface, STR("GetBaseMaterial"), &BaseMaterialArgs);
                if (BaseMaterialArgs.ReturnValue)
                {
                    if (!static_cast<bool>(*BaseMaterialArgs.ReturnValue->GetValuePtrByPropertyName<uint8>(STR("bUsedWithInstancedStaticMeshes"))))
                    {
                        IsValidMaterials = false;
                        break;
                    }
                }
            }
            UniqueMesh.Materials.Add(Material.MaterialInterface);
        }
        if (!IsValidMaterials) continue;

        UniqueMeshes.AddUnique(UniqueMesh);
        Output::send<LogLevel::Default>(STR("[Optimizer] UniqueMeshes: {}\n"), UniqueMeshes.Num());
    }

    MeshActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(ContextActor, MeshActorClass, MeshActors);

    for (const auto& MeshActor : MeshActors)
    {
        const Unreal::TArray<UObject*>& MeshComponents = MeshActor->GetComponentsByClass(MeshCompClass);
        if (!MeshComponents.IsValidIndex(0)) continue;

        UObject* StaticMesh = *MeshComponents[0]->GetValuePtrByPropertyNameInChain<UObject*>(STR("StaticMesh"));
        if (!StaticMesh) continue;

        // Create new HISM and add to main mesh actor
        FAddComponentByClass ComponentArgs{HismClass, false, FTransform(), false};
        ComponentArgs.Transform.GetScale3D() = FVector(1.0f, 1.0f, 1.0f);
        UFunctionUtils::TryCallUFunction(MainMeshActor, STR("AddComponentByClass"), &ComponentArgs);
        UActorComponent* NewHism = ComponentArgs.NewComponent;

        FSetWorldScale3D Scale3D{FVector(1.0f, 1.0f, 1.0f)};
        UFunctionUtils::TryCallUFunction(NewHism, STR("SetWorldScale3D"), &Scale3D);

        FSetMesh Params{StaticMesh};
        UFunctionUtils::TryCallUFunction(NewHism, STR("SetStaticMesh"), &Params);

        const FTransform InstanceTransform = MeshActor->GetTransform();
        FAddInstance InstanceArgs{InstanceTransform};
        UFunctionUtils::TryCallUFunction(NewHism, STR("AddInstanceWorldSpace"), &InstanceArgs);

        if (MeshActor != MainMeshActor) MeshActor->K2_DestroyActor();
    }

    // Check last static mesh actors
    MeshActors.Empty();
    UGameplayStatics::GetAllActorsOfClass(ContextActor, MeshActorClass, MeshActors);
    Output::send<LogLevel::Default>(STR("[Optimizer] Mesh actors count: {}\n"), MeshActors.Num());
}
