#include <EIPlatform.h>
#include <EILog.h>
#include <cstring.h>

#include "file.h"

#if defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
#include <kernel/os/os.h>
#include <fs/fatfs/ff.h>
cstring_t * readFile(const char *pathname)
{
    unsigned int ret = 0;
    FRESULT res;
    cstring_t * cs = NULL;

    FIL *fp = malloc(sizeof(FIL));
    if (NULL == fp)
        goto malloc_out;

    memset(&fp, 0, sizeof(fp));
    res = f_open(fp, pathname, FA_READ | FA_OPEN_EXISTING);
    if (res != FR_OK) {
        LOG(EERROR, "open read file %s failed, return %d", pathname, res);
        goto open_out;
    }
    if (!fp) return NULL;

    size_t len = f_size(fp);

    cstring_new_len(str, len);
    size_t total = 0;
    while (total != len) {
        res = f_read(fp, str->str, len*1, &ret);
        if (res != FR_OK) {
            LOG(EERROR, "read file failed: %d\n", res);
            goto read_out;
        }
    }
    str->len += ret;
    cs = str;

    f_close(fp);
    free(fp);
    return cs;

read_out:
    cstring_del(str);
open_out:
    f_close(fp);
malloc_out:
    free(fp);

    return NULL;
}

size_t writeFile(const char *pathname, void *data, int len)
{
    unsigned int ret = 0;
    FRESULT res;
    cstring_t * cs = NULL;

    FIL *fp = malloc(sizeof(FIL));
    if (NULL == fp)
        goto malloc_out;

    res = f_open(fp, pathname, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        LOG(EERROR, "open read file %s failed, return %d", pathname, res);
        goto open_out;
    }

    res = f_write(fp, data, len * 1, &ret);
    if (res != FR_OK) {
        LOG(EERROR, "write file failed: %d\n", res);
        ret = -res;
    }

open_out:
    f_close(fp);
malloc_out:
    free(fp);

    return ret;
}

size_t appendFile(const char *pathname, void *data, int len)
{
    unsigned int ret = 0;
    FRESULT res;
    cstring_t * cs = NULL;

    FIL *fp = malloc(sizeof(FIL));
    if (NULL == fp)
        goto malloc_out;

    res = f_open(fp, pathname, FA_OPEN_APPEND | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        LOG(EERROR, "open read file %s failed, return %d", pathname, res);
        goto open_out;
    }

    res = f_write(fp, data, len * 1, &ret);
    if (res != FR_OK) {
        LOG(EERROR, "write file failed: %d\n", res);
        ret = -res;
    }

open_out:
    f_close(fp);
malloc_out:
    free(fp);

    return ret;
}

int deleteFile(const char *pathname)
{
    return f_unlink(pathname);
}

#elif defined(_WIN32) || defined(__unix__)
#include <stdio.h>
cstring_t * readFile(const char *pathname)
{
    cstring_t * cs = NULL;
    FILE *fp = fopen(pathname, "rb");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    cstring_new_len(str, len);
    size_t total = 0;
    while (total != len) {
        size_t size = fread(str->str, len, 1, fp);
        if (size <= 0) break;
        str->len += size*len;
    }
    
    fclose(fp);

    cs = str;

    return cs;
}

size_t writeFile(const char *pathname, void *data, int len)
{
    cstring_t * cs = NULL;
    FILE *fp = fopen(pathname, "wb+");
    if (!fp) return 0;

    fwrite(data, len, 1, fp);
    fclose(fp);

    return len;
}

size_t appendFile(const char *pathname, void *data, int len)
{
    cstring_t * cs = NULL;
    FILE *fp = fopen(pathname, "ab");
    if (!fp) return 0;

    fwrite(data, len, 1, fp);
    fclose(fp);

    return len;
}

int deleteFile(const char *pathname)
{
    return unlink(pathname);
}

#else
cstring_t * readFile(const char *pathname)
{
    
}

size_t writeFile(const char *pathname, void *data, int len)
{

}

size_t appendFile(const char *pathname, void *data, int len)
{

}

int deleteFile(const char *pathname)
{

}
#endif