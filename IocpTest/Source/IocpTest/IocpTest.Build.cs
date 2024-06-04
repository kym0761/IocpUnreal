// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IocpTest : ModuleRules
{
	public IocpTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(
            new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Sockets", "Networking" ,"EnhancedInput" , "UMG" });

        //ProtobufCore 모듈 추가
        //언리얼 엔진 자체의 Protobuffer 모듈이 존재하긴 하지만 버전이 다르므로 맞는 버전을 연동함
        PrivateDependencyModuleNames.AddRange(new string[] { "ProtobufCore" });

        PrivateIncludePaths.AddRange(new string[]
        {
            "IocpTest/",
            "IocpTest/Network/",
            "IocpTest/Game/"
        });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
