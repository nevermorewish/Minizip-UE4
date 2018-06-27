extern "C"
{
#include "IOUe4Api.h"
}
#include "CoreMinimal.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/PlatformFile.h"
#include "GenericPlatformFile.h"
extern "C"
{
voidpf call_zopen64(const zlib_filefunc64_32_def* pfilefunc, const void*filename, int mode)
{
	if (pfilefunc->zfile_func64.zopen64_file != NULL)
		return (*(pfilefunc->zfile_func64.zopen64_file)) (pfilefunc->zfile_func64.opaque, filename, mode);
	else
	{
		return (*(pfilefunc->zopen32_file))(pfilefunc->zfile_func64.opaque, (const char*)filename, mode);
	}
}

long call_zseek64(const zlib_filefunc64_32_def* pfilefunc, voidpf filestream, ZPOS64_T offset, int origin)
{
	if (pfilefunc->zfile_func64.zseek64_file != NULL)
		return (*(pfilefunc->zfile_func64.zseek64_file)) (pfilefunc->zfile_func64.opaque, filestream, offset, origin);
	else
	{
		uLong offsetTruncated = (uLong)offset;
		if (offsetTruncated != offset)
			return -1;
		else
			return (*(pfilefunc->zseek32_file))(pfilefunc->zfile_func64.opaque, filestream, offsetTruncated, origin);
	}
}

ZPOS64_T call_ztell64(const zlib_filefunc64_32_def* pfilefunc, voidpf filestream)
{
	if (pfilefunc->zfile_func64.zseek64_file != NULL)
		return (*(pfilefunc->zfile_func64.ztell64_file)) (pfilefunc->zfile_func64.opaque, filestream);
	else
	{
		uLong tell_uLong = (*(pfilefunc->ztell32_file))(pfilefunc->zfile_func64.opaque, filestream);
		if ((tell_uLong) == MAXU32)
			return (ZPOS64_T)-1;
		else
			return tell_uLong;
	}
}

void fill_zlib_filefunc64_32_def_from_filefunc32(zlib_filefunc64_32_def* p_filefunc64_32, const zlib_filefunc_def* p_filefunc32)
{
	p_filefunc64_32->zfile_func64.zopen64_file = NULL;
	p_filefunc64_32->zopen32_file = p_filefunc32->zopen_file;
	p_filefunc64_32->zfile_func64.zerror_file = p_filefunc32->zerror_file;
	p_filefunc64_32->zfile_func64.zread_file = p_filefunc32->zread_file;
	p_filefunc64_32->zfile_func64.zwrite_file = p_filefunc32->zwrite_file;
	p_filefunc64_32->zfile_func64.ztell64_file = NULL;
	p_filefunc64_32->zfile_func64.zseek64_file = NULL;
	p_filefunc64_32->zfile_func64.zclose_file = p_filefunc32->zclose_file;
	p_filefunc64_32->zfile_func64.zerror_file = p_filefunc32->zerror_file;
	p_filefunc64_32->zfile_func64.opaque = p_filefunc32->opaque;
	p_filefunc64_32->zseek32_file = p_filefunc32->zseek_file;
	p_filefunc64_32->ztell32_file = p_filefunc32->ztell_file;
}



static voidpf  ZCALLBACK fopen_file_func OF((voidpf opaque, const char* filename, int mode));
static uLong   ZCALLBACK fread_file_func OF((voidpf opaque, voidpf stream, void* buf, uLong size));
static uLong   ZCALLBACK fwrite_file_func OF((voidpf opaque, voidpf stream, const void* buf, uLong size));
static ZPOS64_T ZCALLBACK ftell64_file_func OF((voidpf opaque, voidpf stream));
static long    ZCALLBACK fseek64_file_func OF((voidpf opaque, voidpf stream, ZPOS64_T offset, int origin));
static int     ZCALLBACK fclose_file_func OF((voidpf opaque, voidpf stream));
static int     ZCALLBACK ferror_file_func OF((voidpf opaque, voidpf stream));

static voidpf ZCALLBACK fopen_file_func(voidpf opaque, const char* filename, int mode)
{
	voidpf file = NULL;
	const char* mode_fopen = NULL;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
		mode_fopen = "rb";
	else
		if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
			mode_fopen = "r+b";
		else
			if (mode & ZLIB_FILEFUNC_MODE_CREATE)
				mode_fopen = "wb";

	if ((filename != NULL) && (mode_fopen != NULL))
		file = (voidpf)FPlatformFileManager::Get().GetPlatformFile().OpenRead(UTF8_TO_TCHAR(filename));
	//file = fopen(filename, mode_fopen);
	return file;
}

static voidpf ZCALLBACK fopen64_file_func(voidpf opaque, const void* filename, int mode)
{
	voidpf file = NULL;
	const char* mode_fopen = NULL;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
		mode_fopen = "rb";
	else
		if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
			mode_fopen = "r+b";
		else
			if (mode & ZLIB_FILEFUNC_MODE_CREATE)
				mode_fopen = "wb";

	if ((filename != NULL) && (mode_fopen != NULL))
		file = (voidpf)FPlatformFileManager::Get().GetPlatformFile().OpenRead(UTF8_TO_TCHAR(filename));
	//file = FOPEN_FUNC((const char*)filename, mode_fopen);
	return file;
}


static uLong ZCALLBACK fread_file_func(voidpf opaque, voidpf stream, void* buf, uLong size)
{
	IFileHandle* FileHandle = (IFileHandle*)stream;
	uLong ret = 0;
	if (FileHandle)
	{
		ret = FileHandle->Read((uint8*)buf, size);
	}
	//ret = (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
	if (ret)
	{
		return size;
	}
	return ret;
}

static uLong ZCALLBACK fwrite_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size)
{
	IFileHandle* FileHandle = (IFileHandle*)stream;
	uLong ret = 0;
	if (FileHandle)
	{
		ret = FileHandle->Write((uint8*)buf, size);
	}
	//ret = (uLong)fwrite(buf, 1, (size_t)size, (FILE *)stream);
	if (ret)
	{
		return size;
	}
	return ret;
}

static long ZCALLBACK ftell_file_func(voidpf opaque, voidpf stream)
{
	uLong ret = 0;
	IFileHandle* FileHandle = (IFileHandle*)stream;
	if (FileHandle)
	{
		ret = FileHandle->Tell();
	}
	//ret = ftell((FILE *)stream);
	return ret;
}


static ZPOS64_T ZCALLBACK ftell64_file_func(voidpf opaque, voidpf stream)
{
	ZPOS64_T ret = 0;
	IFileHandle* FileHandle = (IFileHandle*)stream;
	if (FileHandle)
	{
		ret = FileHandle->Tell();
	}
	//ret = FTELLO_FUNC((FILE *)stream);
	return ret;
}

static long ZCALLBACK fseek_file_func(voidpf  opaque, voidpf stream, uLong offset, int origin)
{
	int fseek_origin = 0;
	long ret;
	switch (origin)
	{
	case ZLIB_FILEFUNC_SEEK_CUR:
		fseek_origin = SEEK_CUR;
		break;
	case ZLIB_FILEFUNC_SEEK_END:
		fseek_origin = SEEK_END;
		break;
	case ZLIB_FILEFUNC_SEEK_SET:
		fseek_origin = SEEK_SET;
		break;
	default: return -1;
	}
	ret = 0;
	IFileHandle* FileHandle = (IFileHandle*)stream;
	if (FileHandle)
	{
		switch (origin)
		{
		case ZLIB_FILEFUNC_SEEK_CUR:
		{
			int64 Pos = FileHandle->Tell();
			ret = !FileHandle->Seek(Pos + offset);
		}
		break;
		case ZLIB_FILEFUNC_SEEK_SET:
		{
			ret = !FileHandle->Seek(offset);
		}
		break;
		case ZLIB_FILEFUNC_SEEK_END:
		{
			ret = !FileHandle->SeekFromEnd(offset);
		}
		break;
		}
		//if (fseek((FILE *)stream, offset, fseek_origin) != 0)
		//    ret = -1;
		
	}
	return ret;
}

	static long ZCALLBACK fseek64_file_func(voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin)
	{
		int fseek_origin = 0;
		long ret;
		switch (origin)
		{
		case ZLIB_FILEFUNC_SEEK_CUR:
			fseek_origin = SEEK_CUR;
			break;
		case ZLIB_FILEFUNC_SEEK_END:
			fseek_origin = SEEK_END;
			break;
		case ZLIB_FILEFUNC_SEEK_SET:
			fseek_origin = SEEK_SET;
			break;
		default: return -1;
		}
		ret = 0;
		IFileHandle* FileHandle = (IFileHandle*)stream;
		if (FileHandle)
		{
			switch (origin)
			{
			case ZLIB_FILEFUNC_SEEK_CUR:
			{
				int64 Pos = FileHandle->Tell();
				ret = !FileHandle->Seek(Pos + offset);
			}
			break;
			case ZLIB_FILEFUNC_SEEK_SET:
			{
				ret = !FileHandle->Seek(offset);
			}
			break;
			case ZLIB_FILEFUNC_SEEK_END:
			{
				ret = !FileHandle->SeekFromEnd(offset);
			}
			break;
			}
		}
		//if(FSEEKO_FUNC((FILE *)stream, offset, fseek_origin) != 0)
		//                    ret = -1;

		return ret;
	}


	static int ZCALLBACK fclose_file_func(voidpf opaque, voidpf stream)
	{
		int ret = 0;
		IFileHandle* FileHandle = (IFileHandle*)stream;
		//ret = fclose((FILE *)stream);
		if (FileHandle)
		{

			FileHandle->~IFileHandle();
		}
		return ret;
	}

	static int ZCALLBACK ferror_file_func(voidpf opaque, voidpf stream)
	{
		int ret = 0;
		//ret = ferror((FILE *)stream);
		return ret;
	}

	void fill_fopen_filefunc(zlib_filefunc_def* pzlib_filefunc_def)
	{
		pzlib_filefunc_def->zopen_file = fopen_file_func;
		pzlib_filefunc_def->zread_file = fread_file_func;
		pzlib_filefunc_def->zwrite_file = fwrite_file_func;
		pzlib_filefunc_def->ztell_file = ftell_file_func;
		pzlib_filefunc_def->zseek_file = fseek_file_func;
		pzlib_filefunc_def->zclose_file = fclose_file_func;
		pzlib_filefunc_def->zerror_file = ferror_file_func;
		pzlib_filefunc_def->opaque = NULL;
	}

	void fill_fopen64_filefunc(zlib_filefunc64_def*  pzlib_filefunc_def)
	{
		pzlib_filefunc_def->zopen64_file = fopen64_file_func;
		pzlib_filefunc_def->zread_file = fread_file_func;
		pzlib_filefunc_def->zwrite_file = fwrite_file_func;
		pzlib_filefunc_def->ztell64_file = ftell64_file_func;
		pzlib_filefunc_def->zseek64_file = fseek64_file_func;
		pzlib_filefunc_def->zclose_file = fclose_file_func;
		pzlib_filefunc_def->zerror_file = ferror_file_func;
		pzlib_filefunc_def->opaque = NULL;
	}

}