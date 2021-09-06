// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IHDiffPatch.h"
#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"

class FHDiffPatchModule : public IHDiffPatch,public IModuleInterface
{
public:
	static FHDiffPatchModule& Get()
	{
		static FHDiffPatchModule Module;
		return Module;
	}

	
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool CreateDiff(const TArray<uint8>& NewData, const TArray<uint8>& OldData, TArray<uint8>& OutPatch) override;
	virtual bool PatchDiff(const TArray<uint8>& OldData, const TArray<uint8>& PatchData, TArray<uint8>& OutNewData) override;
	virtual FString GetFeatureName()const { return TEXT("HDIFFPATCH"); }
};
