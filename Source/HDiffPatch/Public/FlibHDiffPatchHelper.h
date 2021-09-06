// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlibHDiffPatchHelper.generated.h"

/**
 * 
 */
UCLASS()
class HDIFFPATCH_API UFlibHDiffPatchHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static bool CreateCompressedDiff(const TArray<uint8>& NewData, const TArray<uint8>& OldData, TArray<uint8>& OutPatch);
	UFUNCTION(BlueprintCallable)
	static bool PatchCompressedDiff(const TArray<uint8>& OldData, const TArray<uint8>& PatchData, TArray<uint8>& OutNewData);
	UFUNCTION(BlueprintCallable)
	static TArray<uint8> LoadFileToArray(const FString& InPath);
	UFUNCTION(BlueprintCallable)
	static bool SaveArrayToFile(const TArray<uint8>& InBytes ,const FString& InPath);
};
