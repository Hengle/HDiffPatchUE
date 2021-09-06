// Copyright Epic Games, Inc. All Rights Reserved.

#include "HDiffPatch.h"
#include "libHDiffPatch/HDiff/diff.h"
#include "libHDiffPatch/HPatch/patch.h"

#define LOCTEXT_NAMESPACE "FHDiffPatchModule"

DECLARE_LOG_CATEGORY_CLASS(LogHDiffPatch,All,All);


void FHDiffPatchModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(BINARIES_DIFF_PATCH_FEATURE_NAME,this);
}

void FHDiffPatchModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(BINARIES_DIFF_PATCH_FEATURE_NAME,this);
}

bool FHDiffPatchModule::CreateDiff(const TArray<uint8>& NewData, const TArray<uint8>& OldData, TArray<uint8>& OutPatch)
{
	SCOPED_NAMED_EVENT(HDIFF_CreateCompressedDiff,FColor::Yellow);
	
	std::vector<unsigned char> out_diff;
	const unsigned char* Begin = NewData.GetData();
	const unsigned char* End = NewData.GetData() + NewData.Num();
	create_single_compressed_diff(Begin,End,
					OldData.GetData(),OldData.GetData() + OldData.Num(),out_diff);
	OutPatch.Reserve(out_diff.size());
	for (const auto& diff_byte: out_diff)
	{
		OutPatch.Add(diff_byte);
	}
	return true;
}

#define kPatchCacheSize_min      (hpatch_kStreamCacheSize*8)
#define kPatchCacheSize_bestmin  ((size_t)1<<21)
#define kPatchCacheSize_default  ((size_t)1<<26)
#define kPatchCacheSize_bestmax  ((size_t)1<<30)


static unsigned char* getPatchMemCache(hpatch_BOOL isLoadOldAll,size_t patchCacheSize,size_t mustAppendMemSize,
							hpatch_StreamPos_t oldDataSize,size_t* out_memCacheSize){
	unsigned char* temp_cache=0;
	size_t temp_cache_size;
	if (isLoadOldAll){
		size_t addSize=kPatchCacheSize_bestmin;
		if (addSize>oldDataSize+kPatchCacheSize_min)
			addSize=(size_t)(oldDataSize+kPatchCacheSize_min);
		assert(patchCacheSize==0);
		temp_cache_size=(size_t)(oldDataSize+addSize);
		if (temp_cache_size!=oldDataSize+addSize)
			temp_cache_size=kPatchCacheSize_bestmax;//can not load all,load part
	}else{
		if (patchCacheSize<kPatchCacheSize_min)
			patchCacheSize=kPatchCacheSize_min;
		temp_cache_size=patchCacheSize;
		if (temp_cache_size>oldDataSize+kPatchCacheSize_bestmin)
			temp_cache_size=(size_t)(oldDataSize+kPatchCacheSize_bestmin);
	}
	while (!temp_cache) {
		temp_cache=(unsigned char*)malloc(mustAppendMemSize+temp_cache_size);
		if ((!temp_cache)&&(temp_cache_size>=kPatchCacheSize_min*2))
			temp_cache_size>>=1;
	}
	*out_memCacheSize=(temp_cache)?(mustAppendMemSize+temp_cache_size):0;
	return temp_cache;
}

bool FHDiffPatchModule::PatchDiff(const TArray<uint8>& OldData, const TArray<uint8>& PatchData, TArray<uint8>& OutNewData)
{
	SCOPED_NAMED_EVENT(HDIFF_Patch_Compressed_Diff,FColor::Yellow);
	bool result = false;
	size_t  patchCacheSize =  kPatchCacheSize_default;
	hpatch_BOOL isSingleStreamDiff=hpatch_FALSE;
	unsigned char* temp_cache = 0;
	size_t temp_cache_size = 0;
	
	hpatch_singleCompressedDiffInfo sdiffInfo;
	hpatch_compressedDiffInfo diffInfo;

	unsigned char* PatchData_begin = (unsigned char*)PatchData.GetData();
	uint32 Num = PatchData.Num();
	unsigned char* PatchData_end = (unsigned char*)PatchData.GetData() + Num;
	if (getSingleCompressedDiffInfo_mem(&sdiffInfo,PatchData_begin,PatchData_end))
	{
		memcpy(diffInfo.compressType,sdiffInfo.compressType,strlen(sdiffInfo.compressType)+1);
		diffInfo.compressedCount=(sdiffInfo.compressType[0]!='\0')?1:0;
		diffInfo.newDataSize=sdiffInfo.newDataSize;
		diffInfo.oldDataSize=sdiffInfo.oldDataSize;
		patchCacheSize+=(size_t)sdiffInfo.stepMemSize;
		if (patchCacheSize<sdiffInfo.stepMemSize+hpatch_kStreamCacheSize*3)
		{
			patchCacheSize=(size_t)sdiffInfo.stepMemSize+hpatch_kStreamCacheSize*3;
		}
		isSingleStreamDiff=hpatch_TRUE;
		printf("patch single compressed diffData!\n");
		OutNewData.Reserve(diffInfo.newDataSize);
		OutNewData.SetNumZeroed(diffInfo.newDataSize);
		unsigned char* OutNewData_begin = (unsigned char*)OutNewData.GetData();
		unsigned char* OutNewData_end = (unsigned char*)OutNewData.GetData() + OutNewData.Num();

		unsigned char* OldData_begin = (unsigned char*)OldData.GetData();
		unsigned char* OldData_end = (unsigned char*)OldData.GetData() + OldData.Num();


		hpatch_TDecompress*  decompressPlugin=0;
		hpatch_TStreamOutput    newData;
		hpatch_TStreamInput     diffData;
		hpatch_TStreamInput     oldData;
	
		mem_as_hStreamOutput(&newData,OutNewData_begin,OutNewData_end);
		mem_as_hStreamInput(&oldData,OldData_begin,OldData_end);
		mem_as_hStreamInput(&diffData,PatchData_begin,PatchData_end);
		
		temp_cache = getPatchMemCache(true,patchCacheSize,0,oldData.streamSize, &temp_cache_size);
		
		result = (bool)patch_single_compressed_diff(&newData,&oldData,&diffData,sdiffInfo.diffDataPos,
												sdiffInfo.uncompressedSize,sdiffInfo.compressedSize,decompressPlugin,
												sdiffInfo.coverCount,(size_t)sdiffInfo.stepMemSize,temp_cache,temp_cache+temp_cache_size,0);
	}
	return result;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHDiffPatchModule, HDiffPatch)