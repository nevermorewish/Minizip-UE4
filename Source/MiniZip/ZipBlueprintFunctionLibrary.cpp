#include "ZipBlueprintFunctionLibrary.h"
#include "unzip.h"
#include "HAL/UnrealMemory.h"
#include "HAL/FileManager.h"
#include "Paths.h"
static int DoExtractCurrentFile(unzFile uf, const FString& StrDstDir)
{
	char filename_inzip[256];
	FString StrFileNameInZip;
	FString StrFileNameWithOutPath;
	int err = UNZ_OK;

	unz_file_info64 file_info;
	uLong ratio = 0;
	err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
	StrFileNameInZip = UTF8_TO_TCHAR(filename_inzip);
	if (err != UNZ_OK)
	{
		UE_LOG(LogTemp, Display, TEXT("解压缩获取文件中信息失败%s"), *StrFileNameInZip);
		return err;
	}

	int64 SizeBuf = file_info.uncompressed_size;
	ANSICHAR* Buf = (ANSICHAR*)FMemory::Malloc(SizeBuf);
	FMemory::Memset(Buf, 0, SizeBuf);

	err = unzOpenCurrentFile(uf);
	if (err == UNZ_OK)
	{
		FString StrFileWrite = FPaths::Combine(StrDstDir, StrFileNameInZip);
		FArchive*  FileWrite = IFileManager::Get().CreateFileWriter(*StrFileWrite);
		if (FileWrite)
		{
			err = unzReadCurrentFile(uf, Buf, SizeBuf);
			FileWrite->Serialize(Buf, SizeBuf);
			FileWrite->Close();
		}
	}

	FMemory::Free(Buf);
	return UNZ_OK;
}

static int extractCurrentFile(unzFile uf, const FString& StrDstDir)
{
	int err;
	unz_global_info64 gi;
	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK)
	{
		UE_LOG(LogTemp, Display, TEXT("解压缩获取文件Gobal信息失败"));
		return err;
	}

	for (int i = 0; i < gi.number_entry; i++)
	{
		if (DoExtractCurrentFile(uf, StrDstDir)!= UNZ_OK)
		{
			break;
		}
		if ((i + 1) < gi.number_entry)
		 {
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK)
			{
				break;
			}
		}
	}
	return UNZ_OK;
}

int DoZipFile(zipFile zf, const FString& srcFile, const FString& StrSaveFileName)
{
	zip_fileinfo zi;
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
	zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
	
	int32 err = zipOpenNewFileInZip3_64(zf, TCHAR_TO_UTF8(*StrSaveFileName), &zi,
		NULL, 0, NULL, 0, NULL /* comment*/,
		Z_DEFLATED,
		Z_DEFAULT_COMPRESSION, 0,
		MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		NULL, 0, false);
	if (err == ZIP_OK)
	{
		UE_LOG(LogTemp, Display, TEXT("压缩文件：%s"), *StrSaveFileName);
		FArchive*  FileRead = IFileManager::Get().CreateFileReader(*srcFile);
		if (FileRead)
		{
			int32 BufSize = FileRead->TotalSize();
			ANSICHAR* Buf = (ANSICHAR*)FMemory::Malloc(BufSize);
			FMemory::Memset(Buf, 0, BufSize);
			FileRead->Serialize(Buf, BufSize);
			err = zipWriteInFileInZip(zf, Buf, BufSize);

			FMemory::Free(Buf);

			FileRead->Close();
		}

		err = zipCloseFileInZip(zf);
		return ZIP_OK;
	}
	return err;
}

bool UZipBlueprintFunctionLibrary::ZipDir(const FString& StrDir, const FString& StrZipFileName)
{
	bool bSuccess = true;
	int OpenMode = APPEND_STATUS_CREATE;
	zipFile zf;
	zf = zipOpen64(TCHAR_TO_UTF8(*StrZipFileName), OpenMode);
	if (zf == NULL)
	{
		UE_LOG(LogTemp, Display, TEXT("压缩打开文件失败%s"), *StrZipFileName);
	}
	if (zf)
	{
		TArray<FString> ArrStrFile;
		IFileManager::Get().FindFilesRecursive(ArrStrFile, *StrDir, TEXT("*.*"), true, false, false);
		for (auto StrFile : ArrStrFile)
		{
			FString StrSaveFileName = StrFile;
		    FPaths::MakePathRelativeTo(StrSaveFileName, *StrDir);
			if (DoZipFile(zf, StrFile, StrSaveFileName) != ZIP_OK)
			{
				bSuccess = false;
			}
		}
	}
	zipClose(zf, NULL);
	return bSuccess;
}

bool UZipBlueprintFunctionLibrary::ZipFile(const FString& StrFileName, const FString& StrZipFileName)
{
	int OpenMode = APPEND_STATUS_CREATE;
	zipFile zf;
	zf = zipOpen64(TCHAR_TO_UTF8(*StrZipFileName), OpenMode);
	if (zf == NULL)
	{
		UE_LOG(LogTemp, Display, TEXT("压缩打开文件失败%s"), *StrZipFileName);
	}
	FString StrSaveFileName = FPaths::GetCleanFilename(StrFileName);
	bool bSuccess = DoZipFile(zf, StrFileName, StrSaveFileName) == ZIP_OK;
	zipClose(zf, NULL);
	return bSuccess;
}

bool UZipBlueprintFunctionLibrary::UnZipFile(const FString& StrZipFile, const FString& StrDstDir)
{
	unzFile uf = NULL;
	if (StrZipFile.IsEmpty())
	{
		return false;
		
	}

	uf = unzOpen64(TCHAR_TO_UTF8(*StrZipFile));
	if (uf == NULL)
	{
		UE_LOG(LogTemp, Display, TEXT("解压缩打开文件失败%s"), *StrZipFile);
	}
	int status = 0;
	status = unzGoToFirstFile(uf);
	if (status != UNZ_OK) 
	{
		unzClose(uf);
		return false;
	}

	
	status = extractCurrentFile(uf, StrDstDir);
	if (status != UNZ_OK)
	{
		unzClose(uf);
		return false;
	}
	unzClose(uf);
	return true;
}

bool UZipBlueprintFunctionLibrary::GetZipAllFileCRC(const FString& StrZipFile, TMap<FString, FString>& MapFileCRC)
{
	unzFile uf = NULL;
	if (StrZipFile.IsEmpty())
	{
		return false;

	}

	uf = unzOpen64(TCHAR_TO_UTF8(*StrZipFile));
	if (uf == NULL)
	{
		UE_LOG(LogTemp, Display, TEXT("获取压缩包文件失败%s"), *StrZipFile);
	}
	int status = 0;
	status = unzGoToFirstFile(uf);
	if (status != UNZ_OK)
	{
		unzClose(uf);
		return false;
	}

	unz_global_info64 gi;
	status = unzGetGlobalInfo64(uf, &gi);
	if (status != UNZ_OK)
	{
		UE_LOG(LogTemp, Display, TEXT("解压缩获取文件Gobal信息失败"));
		return false;
	}

	char filename_inzip[256];
	for (int i = 0; i < gi.number_entry; i++)
	{
		unz_file_info64 file_info;
		status = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
		if (status != UNZ_OK)
		{
			break;
		}
		MapFileCRC.Add(UTF8_TO_TCHAR(filename_inzip), FString::Printf(TEXT("%ld"), file_info.crc));
		if ((i + 1) < gi.number_entry)
		{
			status = unzGoToNextFile(uf);
			if (status != UNZ_OK)
			{
				break;
			}
		}
	}

	if (status != UNZ_OK)
	{
		unzClose(uf);
		return false;
	}
	unzClose(uf);
	return true;
}