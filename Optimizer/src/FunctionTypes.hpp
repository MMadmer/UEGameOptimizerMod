#pragma once


/** UActorComponent* AActor::AddComponentByClass(TSubclassOf<UActorComponent> Class, bool bManualAttachment, const FTransform& RelativeTransform, bool bDeferredFinish)*/
struct FAddComponentByClass
{
    UClass* Class;
    bool bManualAttachment;
    Unreal::FTransform Transform;
    bool bDeferredFinish;
    Unreal::UActorComponent* NewComponent{};
};

/** FTransform USceneComponent::K2_GetComponentToWorld() const*/
struct FK2_GetComponentToWorld
{
    Unreal::FTransform ReturnValue{};
};

/** void USceneComponent::SetWorldScale3D(FVector NewScale)*/
struct FSetWorldScale3D
{
    Unreal::FVector NewScale;
};

/** UStaticMeshComponent::SetStaticMesh(UStaticMesh* NewMesh)*/
struct FSetMesh
{
    UObject* StaticMesh;
    bool ReturnResult{};
};

/**
 * UInstancedStaticMeshComponent::AddInstance(const FTransform& InstanceTransform)
 * UInstancedStaticMeshComponent::AddInstanceWorldSpace(const FTransform& WorldTransform)
 */
struct FAddInstance
{
    Unreal::FTransform Transform;
    Unreal::int32 ReturnValue{};
};

/** bool GetInstanceTransform(int32 InstanceIndex, FTransform& OutInstanceTransform, bool bWorldSpace = false) const*/
struct FGetInstanceTransform
{
    Unreal::int32 InstanceIndex;
    Unreal::FTransform OutInstanceTransform{};
    bool bWorldSpace{false};
    bool ReturnResult{};
};

/**
 * virtual bool UpdateInstanceTransform(
 * int32 InstanceIndex, const FTransform& NewInstanceTransform, bool bWorldSpace=false, bool bMarkRenderStateDirty=false, bool bTeleport=false)*/
struct FUpdateInstanceTransform
{
    Unreal::int32 InstanceIndex;
    Unreal::FTransform NewInstanceTransform{};
    bool bWorldSpace{false};
    bool bMarkRenderStateDirty{false};
    bool bTeleport{false};
    bool ReturnResult{};
};

/** ENGINE_API UMaterial* MaterialInterface::GetBaseMaterial()*/
struct FGetBaseMaterial
{
    UObject* ReturnValue{};
};
