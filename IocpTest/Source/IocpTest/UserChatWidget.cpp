// Fill out your copyright notice in the Description page of Project Settings.


#include "UserChatWidget.h"
#include "IocpTest.h"
#include "Components/EditableText.h"
#include "ChatSlotWidget.h"
#include "Components/ScrollBox.h"


void UUserChatWidget::NativeConstruct()
{
	Super::NativeConstruct();


	EditableText_Chat->OnTextCommitted.AddDynamic(this, &UUserChatWidget::OnTextEntered);

}

void UUserChatWidget::AddChat(FString Str, Protocol::ChatType ChatType)
{
	assert(ChatSlotBP);

	auto chatSlot = CreateWidget<UChatSlotWidget>(this, ChatSlotBP);
	if (IsValid(chatSlot))
	{
		chatSlot->SetChatText(Str, ChatType);

		float prevOffset = ScrollBox_ChatScroll->GetScrollOffset();
		float prevOffsetOfEnd = ScrollBox_ChatScroll->GetScrollOffsetOfEnd();
		ScrollBox_ChatScroll->AddChild(chatSlot);

		//채팅 Slot 추가 전에 스크롤 위치가 맨 아래였다면, Slot 추가 후에도 스크롤이 맨 아래를 유지
		if (FMath::IsNearlyEqual(prevOffset, prevOffsetOfEnd))
		{
			ScrollBox_ChatScroll->ScrollToEnd();
		}
	}
	
}

void UUserChatWidget::OnTextEntered(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		//protobuf가 UTF-8만 지원하는 관계로 UTF-8로 변경 후 char packet을 만들어 전송
		FString fstr = Text.ToString();
		std::string str = TCHAR_TO_UTF8(*fstr);

		Protocol::C2S_CHAT chatPkt;
		
		{
			auto msg = chatPkt.mutable_msg();
			*msg = str;
		}

		SEND_PACKET(chatPkt);

		EditableText_Chat->SetText(FText::FromString(""));
	}

}
