// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserChatWidget.generated.h"

class UScrollBox;
class UEditableText;

class UChatSlotWidget;

/**
 * 
 */
UCLASS()
class IOCPTEST_API UUserChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	UScrollBox* ScrollBox_ChatScroll;

	UPROPERTY(Meta = (BindWidget))
	UEditableText* EditableText_Chat;

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UChatSlotWidget> ChatSlotBP;

public:

	void AddChat(FString Str);

public:

	UFUNCTION()
	void OnTextEntered(const FText& Text, ETextCommit::Type CommitMethod);




};
