#include "Resource/Actor/CPFarmTileActor.h"
#include "Character/CPCarryComponent.h"
#include "Components/CPHandItemContainerComponent.h"
#include "Data/CPFarmCropDefinition.h"
#include "Lab/Actor/CPAlchemyProp.h"
#include "Resource/System/CPFarmSubsystem.h"

ACPFarmTileActor::ACPFarmTileActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACPFarmTileActor::OnInteract_Implementation(AActor* Interactor)
{
	ECPFarmInteractionType Type = GetInteractionType();
	
	if (Type == ECPFarmInteractionType::Plant) Plant(Interactor);
	if (Type == ECPFarmInteractionType::Water) Water(Interactor);
	if (Type == ECPFarmInteractionType::Harvest) Harvest(Interactor);
}

FText ACPFarmTileActor::GetInteractionPrompt_Implementation()
{
	ECPFarmInteractionType Type = GetInteractionType();
	
	if (Type == ECPFarmInteractionType::Plant)
	{
		return FText::FromString(TEXT("심기"));
	}
	
	if (Type == ECPFarmInteractionType::Water)
	{
		return FText::FromString(TEXT("물주기"));
	}
	
	if (Type == ECPFarmInteractionType::Harvest)
	{
		return FText::FromString(TEXT("수확하기"));
	}
	
	return FText::GetEmpty();
}

FName ACPFarmTileActor::GetInteractionName_Implementation()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return NAME_None;
	
	UCPFarmSubsystem* FarmSubsystem = GI->GetSubsystem<UCPFarmSubsystem>();
	if (!FarmSubsystem) return NAME_None;
	
	const FCPFarmTileState* State = FarmSubsystem->FindTileState(GetTileKey());
	if (!State) return TEXT("농지");
	
	return FName(*State->Crop.Definition->HarvestData.HarvestedItem->DisplayName.ToString());
}

bool ACPFarmTileActor::CanInteract_Implementation(AActor* Interactor)
{
	if (!Interactor) return false;
	
	ECPFarmInteractionType Type = GetInteractionType();
	
	if (Type == ECPFarmInteractionType::Plant)
	{
		return GetSeedData(Interactor) != nullptr;
	}
	
	if (Type == ECPFarmInteractionType::Water) return true;
	
	if (Type == ECPFarmInteractionType::Harvest) return true;
	
	return false;
}

float ACPFarmTileActor::GetInteractionDuration_Implementation(AActor* Interactor)
{
	return ICPTimedInteractable::GetInteractionDuration_Implementation(Interactor);
}

void ACPFarmTileActor::BeginPlay()
{
	Super::BeginPlay();
	
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	
	UCPFarmSubsystem* FarmSubsystem = GI->GetSubsystem<UCPFarmSubsystem>();
	if (!FarmSubsystem) return;
	
	FarmSubsystem->RegisterTile(GetTileKey());
	
	RefreshVisual();
}

FCPFarmTileKey ACPFarmTileActor::GetTileKey() const
{
	FCPFarmTileKey Key;
	Key.FarmId = FarmId;
	Key.Coordinate = GridCoordinate;
	
	return Key;
}

void ACPFarmTileActor::RefreshVisual()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	
	UCPFarmSubsystem* FarmSubsystem = GI->GetSubsystem<UCPFarmSubsystem>();
	if (!FarmSubsystem) return;
	
	const FCPFarmTileState* TileState = FarmSubsystem->FindTileState(GetTileKey());
	if (!TileState) return;
	
	if (!TileState->HasCrop())
	{
		CropMesh->SetStaticMesh(nullptr);
		return;
	}
	
	const FCPFarmCropInstance& Crop = TileState->Crop;
	ECPCropGrowthStage Stage = Crop.GetGrowthStage();
	
	if (Stage == ECPCropGrowthStage::Seed)
	{
		CropMesh->SetStaticMesh(Crop.Definition->SeedMesh.LoadSynchronous());
	}
	
	else if (Stage == ECPCropGrowthStage::Intermediate)
	{
		CropMesh->SetStaticMesh(Crop.Definition->IntermediateMesh.LoadSynchronous());
	}
	
	else if (Stage == ECPCropGrowthStage::Mature)
	{
		CropMesh->SetStaticMesh(Crop.Definition->MatureMesh.LoadSynchronous());
	}
}

ECPFarmInteractionType ACPFarmTileActor::GetInteractionType() const
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return ECPFarmInteractionType::None;
	
	UCPFarmSubsystem* FarmSubsystem = GI->GetSubsystem<UCPFarmSubsystem>();
	if (!FarmSubsystem) return ECPFarmInteractionType::None;
	
	const FCPFarmTileState* State = FarmSubsystem->FindTileState(GetTileKey());
	
	if (!State) return ECPFarmInteractionType::None;
	
	if (!State->HasCrop()) return ECPFarmInteractionType::Plant;
	
	if (State->Crop.IsMature()) return ECPFarmInteractionType::Harvest;
	
	return ECPFarmInteractionType::Water;
}

UCPForageableItemData* ACPFarmTileActor::GetSeedData(AActor* Interactor) const
{
	if (!Interactor) return nullptr;
	
	UCPCarryComponent* CarryComponent = Interactor->GetComponentByClass<UCPCarryComponent>();
	if (!CarryComponent) return nullptr;
	
	ACPAlchemyProp* HeldProp = Cast<ACPAlchemyProp>(CarryComponent->GetHeldProp());
	if (!HeldProp) return nullptr;
	
	return HeldProp->GetWorkingIngredient().SourceItemData;
}

void ACPFarmTileActor::Plant(AActor* Interactor)
{
	if (!Interactor) return;
	
	UCPCarryComponent* CarryComponent = Interactor->GetComponentByClass<UCPCarryComponent>();
	if (!CarryComponent) return;
	
	UCPForageableItemData* SeedData = GetSeedData(Interactor);
}

void ACPFarmTileActor::Water(AActor* Interactor)
{
	
}

void ACPFarmTileActor::Harvest(AActor* Interactor)
{
	
}

