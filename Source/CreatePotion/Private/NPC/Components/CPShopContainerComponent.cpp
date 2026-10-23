// CPShopContainerComponent.cpp

#include "NPC/Components/CPShopContainerComponent.h"
#include "GameInstance/Subsystem/CPTimeSubsystem.h"

UCPShopContainerComponent::UCPShopContainerComponent()
{
	ContainerType = EContainerType::Slot1D;
	MaxSlots = 20;
	TargetContext = EUITargetContext::Shop;
}

void UCPShopContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	// GridIndex를 배열 순서 그대로 자동 배정
	for (int32 i = 0; i < ContainerItems.Num(); ++i)
	{
		ContainerItems[i].GridIndex = i;
	}

	DefaultStock = ContainerItems;

	if (UGameInstance* GI = GetWorld()->GetGameInstance())
	{
		if (UCPTimeSubsystem* TimeSubsystem = GI->GetSubsystem<UCPTimeSubsystem>())
		{
			// 초기화 시 오늘 날짜 기록
			LastRestockDay = TimeSubsystem->GetCurrentDay();
			TimeSubsystem->OnTimePhaseChanged.AddDynamic(this, &UCPShopContainerComponent::HandleTimePhaseChanged);
		}
	}
}

void UCPShopContainerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 레벨이 바뀔 때 델리게이트 구독 해제
	if (UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr)
	{
		if (UCPTimeSubsystem* TimeSubsystem = GI->GetSubsystem<UCPTimeSubsystem>())
		{
			TimeSubsystem->OnTimePhaseChanged.RemoveDynamic(this, &UCPShopContainerComponent::HandleTimePhaseChanged);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UCPShopContainerComponent::HandleTimePhaseChanged(ECPTimePhase NewPhase)
{
	if (NewPhase != ECPTimePhase::Morning)
	{
		return;
	}

	UCPTimeSubsystem* TimeSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UCPTimeSubsystem>();
	if (!TimeSubsystem)
	{
		return;
	}

	// 날짜가 실제로 바뀌었을 때만 초기화 
	// 레벨 이동, 불러오기 등의 재알림은 무시하기 위해
	const int32 CurrentDay = TimeSubsystem->GetCurrentDay();
	if (CurrentDay <= LastRestockDay)
	{
		return;
	}

	LastRestockDay = CurrentDay;
	RestockShop();
}

void UCPShopContainerComponent::RestockShop()
{
	ContainerItems = DefaultStock;
	OnContainerUpdated.Broadcast(); // 상점 창이 열려 있어도 갱신될 수 있도록
}
