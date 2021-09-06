// Fill out your copyright notice in the Description page of Project Settings.


#include "FlibHDiffPatchHelper.h"

#include "HDiffPatch.h"
#include "Misc/FileHelper.h"

bool UFlibHDiffPatchHelper::CreateCompressedDiff(const TArray<uint8>& NewData, const TArray<uint8>& OldData, TArray<uint8>& OutPatch)
{
	return FHDiffPatchModule::Get().CreateDiff(NewData,OldData,OutPatch);
}

bool UFlibHDiffPatchHelper::PatchCompressedDiff(const TArray<uint8>& OldData, const TArray<uint8>& PatchData, TArray<uint8>& OutNewData)
{
	return FHDiffPatchModule::Get().PatchDiff(OldData,PatchData,OutNewData);
}

TArray<uint8> UFlibHDiffPatchHelper::LoadFileToArray(const FString& InPath)
{
	TArray<uint8> result;
	FFileHelper::LoadFileToArray(result,*InPath);
	return result;
}

bool UFlibHDiffPatchHelper::SaveArrayToFile(const TArray<uint8>& InBytes, const FString& InPath)
{
	return FFileHelper::SaveArrayToFile(InBytes,*InPath);
}
