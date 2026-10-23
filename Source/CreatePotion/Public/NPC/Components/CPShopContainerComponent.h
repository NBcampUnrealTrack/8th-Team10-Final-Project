// CPShopContainerComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/CPItemContainerComponent.h"
#include "CPShopContainerComponent.generated.h"

enum class ECPTimePhase : uint8;

UCLASS(ClassGroup = (CPContainer), meta = (BlueprintSpawnableComponent))
class CREATEPOTION_API UCPShopContainerComponent : public UCPItemContainerComponent
{
	GENERATED_BODY()

public:
	UCPShopContainerComponent();

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RestockShop();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleTimePhaseChanged(ECPTimePhase NewPhase);

private:
	// 복원 재고
	UPROPERTY()
	TArray<FContainerItem> DefaultStock;

	// 아침 알림이 여러 번 와도 한 번만 초기화할 수 있도록 날짜 구분
	int32 LastRestockDay = -1;
};
