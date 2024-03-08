// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatSlotWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class IOCPTEST_API UChatSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TextBlock_Chat;

};