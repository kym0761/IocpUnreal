// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserChatWidget.generated.h"

class UScrollBox;
class UEditableText;

/**
 * 
 */
UCLASS()
class IOCPTEST_API UUserChatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(Meta = (BindWidget))
	UScrollBox* ScrollBox_ChatScroll;

	UPROPERTY(Meta = (BindWidget))
	UEditableText* EditableText_Chat;

	UFUNCTION()
	void OnTextEntered(const FText& Text, ETextCommit::Type CommitMethod);

};
