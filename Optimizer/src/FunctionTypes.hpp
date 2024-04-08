#pragma once


/** UActorComponent* AActor::AddComponentByClass(TSubclassOf<UActorComponent> Class, bool bManualAttachment, const FTransform& RelativeTransform, bool bDeferredFinish)*/
struct FAddComponentByClass
{
    UClass* Class;
    bool bManualAttachment;
    Unreal::FTransform Transform;
    bool bDeferredFinish;
    Unreal::UActorComponent* ReturnValue{};
};

/** virtual bool USceneComponent::IsVisible() const*/
struct FIsVisible
{
    bool ReturnValue{};
};

/** void USceneComponent::SetVisibility(bool bNewVisibility, bool bPropagateToChildren=false)*/
struct FSetVisibility
{
    bool bNewVisibility;
    bool bPropagateToChildren{false};
};

/** FName USceneComponent::GetCollisionProfileName() const*/
struct FGetCollisionProfileName
{
    FName ReturnValue{};
};

/** virtual void USceneComponent::SetCollisionProfileName(FName InCollisionProfileName, bool bUpdateOverlaps=true)*/
struct FSetCollisionProfileName
{
    FName InCollisionProfileName;
    bool bUpdateOverlaps{true};
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

/** virtual TArray<class UMaterialInterface*> GetMaterials() const*/
struct FGetMaterials
{
    TArray<UObject*> ReturnValue{};
};

/** virtual void UPrimitiveComponent::SetMaterial(int32 ElementIndex, class UMaterialInterface* Material);*/
struct FSetMaterial
{
    Unreal::int32 ElementIndex;
    UObject* Material;
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
