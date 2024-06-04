// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

//ProtoBufCore 모듈
public class ProtobufCore : ModuleRules
{
	public ProtobufCore(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		//경로 : google/~~~
		PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));

		//.lib 파일
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "Lib", "Win64", "libprotobuf.lib"));

		PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI=1");
	}
}
