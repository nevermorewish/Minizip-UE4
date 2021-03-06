#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MiniZip/zip.h"
#include "MiniZip/unzip.h"
#include "ZipBlueprintFunctionLibrary.generated.h"

UCLASS()
class MINIZIP_API UZipBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Zip")
		static bool ZipDir(const FString& StrDir, const FString& StrZipFileName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Zip")
		static bool ZipFile(const FString& StrFileName, const FString& StrZipFileName);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Zip")
		static bool UnZipFile(const FString& StrZipFile, const FString& StrDstDir);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Zip")
		static bool GetZipAllFileCRC(const FString& StrZipFile, TMap<FString, FString>& MapFileCRC);
};