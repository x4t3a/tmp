#include "common/hash.h"

#include <mbedtls/md5.h>

bool
calcMD5Hash(Hash hash, BufferView* buffer_view)
{
    bool view_error = checkBufferView(buffer_view);
    if (view_error)
    {
        return true;
    }

    mbedtls_md5_context hash_ctx = {0};
    int md5_error = mbedtls_md5_ret(buffer_view->view, buffer_view->size, hash);

    return md5_error;
}