// Fill out your copyright notice in the Description page of Project Settings.


#include "UserChatWidget.h"
#include "IocpTest.h"
#include "Components/EditableText.h"

void UUserChatWidget::NativeConstruct()
{
	Super::NativeConstruct();


	EditableText_Chat->OnTextCommitted.AddDynamic(this, &UUserChatWidget::OnTextEntered);

}
void UUserChatWidget::OnTextEntered(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString fstr = Text.ToString();
		std::string str = TCHAR_TO_UTF8(*fstr);

		Protocol::C_CHAT chatPkt;
		
		{
			std::string* msg = chatPkt.mutable_msg();
			*msg = str;
		}

		SEND_PACKET(chatPkt);

		EditableText_Chat->SetText(FText::FromString(""));
	}

}
