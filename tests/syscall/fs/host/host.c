// Copyright (c) Open Enclave SDK contributors.
// Licensed under the MIT License.

#if defined(_WIN32)
#include <windows.h>
#endif
#include <openenclave/host.h>
#include <openenclave/internal/syscall/host.h>
#include <openenclave/internal/tests.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "fs_u.h"

#define SKIP_RETURN_CODE 2

int rmdir(const char* path);

int test_fs_posix(
    const char* enclave_path,
    const char* src_dir,
    const char* tmp_dir)
{
    const uint32_t flags = oe_get_create_flags();
    if ((flags & OE_ENCLAVE_FLAG_SIMULATE))
    {
        printf("=== Skipped unsupported test in simulation mode (sealKey)\n");
        return SKIP_RETURN_CODE;
    }

    rmdir(tmp_dir);

    const oe_enclave_type_t type = OE_ENCLAVE_TYPE_SGX;
    oe_enclave_t* enclave = NULL;
    oe_result_t r =
        oe_create_fs_enclave(enclave_path, type, flags, NULL, 0, &enclave);
    OE_TEST(r == OE_OK);

    r = test_fs(enclave, src_dir, tmp_dir);
    OE_TEST(r == OE_OK);

    r = oe_terminate_enclave(enclave);
    OE_TEST(r == OE_OK);

    printf("=== passed all tests (hostfs)\n");

    return 0;
}

#if defined(_WIN32)
int wmain(int argc, wchar_t* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %ls ENCLAVE_PATH SRC_DIR BIN_DIR\n", argv[0]);
        return 1;
    }

    /* create_enclave takes an ANSI path instead of a Unicode path, so we have
     * to try to convert here */
    char enclave_path[MAX_PATH];
    if (WideCharToMultiByte(
            CP_ACP,
            0,
            argv[1],
            -1,
            enclave_path,
            sizeof(enclave_path),
            NULL,
            NULL) == 0)
    {
        fprintf(stderr, "Invalid enclave path\n");
        return 1;
    }
    char* src_dir = oe_win_path_to_posix((PCWSTR)argv[2]);
    char* tmp_dir = oe_win_path_to_posix((PCWSTR)argv[3]);

    // Windows does not support umask.
    // Please set up the right permission to the parent directory.

    int ret = test_fs_posix(enclave_path, src_dir, tmp_dir);

    free(src_dir);
    free(tmp_dir);

    return ret;
}

#else /* !_WIN32 */

int main(int argc, const char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s ENCLAVE_PATH SRC_DIR BIN_DIR\n", argv[0]);
        return 1;
    }

    umask(0022);

    return test_fs_posix(argv[1], argv[2], argv[3]);
}
#endif
