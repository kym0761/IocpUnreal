// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSlotWidget.h"
#include "Components/TextBlock.h"

void UChatSlotWidget::SetChatText(FString Str, Protocol::ChatType ChatType)
{
	TextBlock_Chat->SetText(FText::FromString(Str));

	switch (ChatType)
	{
	case Protocol::CHAT_PLAYER:
		{
		FSlateColor inColor(FLinearColor(1.0f, 1.0f, 1.0f));
		TextBlock_Chat->SetColorAndOpacity(inColor);
		}
		
		break;
	case Protocol::CHAT_SYSTEM:
	{
		FSlateColor inColor(FLinearColor(0.25f, 0.25f, 1.0f));
		TextBlock_Chat->SetColorAndOpacity(inColor);
		}
		
		break;
	default:
		break;
	}

}
