#include "crypt/crypt.h"

#include <string>
#include <cstdint>

#include "mbedtls/cipher.h"
#include "mbedtls/md.h"
#include "mbedtls/pkcs5.h"

namespace buried {

std::string AESCrypt::GetKey(const std::string& salt,
                             const std::string& password) {
    constexpr size_t kKeyLength = 32;
    
    constexpr std::uint32_t kIterations = 1000;

    unsigned char key[kKeyLength] = {0};

    mbedtls_md_context_t md_ctx;
    mbedtls_md_init(&md_ctx);

    const mbedtls_md_info_t* md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    mbedtls_md_setup(&md_ctx, md_info, 1);

    int ret = mbedtls_pkcs5_pbkdf2_hmac(
        &md_ctx,
        reinterpret_cast<const unsigned char*>(password.data()),
        password.size(),
        reinterpret_cast<const unsigned char*>(salt.data()),
        salt.size(),
        kIterations,
        kKeyLength,
        key);

    mbedtls_md_free(&md_ctx);

    if (ret != 0) {
        return "";
    }

    return std::string(reinterpret_cast<const char*>(key), kKeyLength);
}

} // namespace buried