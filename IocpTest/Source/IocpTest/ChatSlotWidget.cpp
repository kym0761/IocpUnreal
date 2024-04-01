// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSlotWidget.h"
#include "Components/TextBlock.h"

void UChatSlotWidget::SetChatText(FString Str)
{
	TextBlock_Chat->SetText(FText::FromString(Str));
}
