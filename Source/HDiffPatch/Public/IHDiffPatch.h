#pragma once
#include "Features/IModularFeature.h"

#ifdef HOTPATCHER_PLUGIN
#include "BinariesPatchFeature.h"
struct IHDiffPatch: public IBinariesDiffPatchFeature
#else
#define BINARIES_DIFF_PATCH_FEATURE_NAME TEXT("BinariesDiffPatchFeatures")
struct IHDiffPatch: public IModularFeature
#endif
{
	virtual ~IHDiffPatch(){};
	virtual bool CreateDiff(const TArray<uint8>& NewData, const TArray<uint8>& OldData, TArray<uint8>& OutPatch) = 0;
	virtual bool PatchDiff(const TArray<uint8>& OldData, const TArray<uint8>& PatchData, TArray<uint8>& OutNewData) = 0;
	virtual FString GetFeatureName()const = 0;
};
