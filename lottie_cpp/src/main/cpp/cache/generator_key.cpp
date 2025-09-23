/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cache/generator_key.h"
#include "CryptoArchitectureKit/crypto_common.h"
#include "CryptoArchitectureKit/crypto_digest.h"
#include <iomanip>
#include <sstream>

namespace LottieTurbo {
std::string GeneratorKey(std::string fileName)
{
    OH_Crypto_ErrCode errCode;
    OH_CryptoDigest *ctx = nullptr;
    Crypto_DataBlob in = {.data = (uint8_t *)fileName.c_str(), .len = fileName.size()};
    Crypto_DataBlob out = {.data = nullptr, .len = 0};
    std::string key = "";
    do {
        errCode = OH_CryptoDigest_Create("SHA256", &ctx);
        if (errCode != CRYPTO_SUCCESS) {
            break;
        }
        errCode = OH_CryptoDigest_Update(ctx, &in);
        if (errCode != CRYPTO_SUCCESS) {
            break;
        }
        errCode = OH_CryptoDigest_Final(ctx, &out);
    } while (0);
    if (errCode == CRYPTO_SUCCESS) {
        const size_t defaultW = 2;
        std::ostringstream oss;
        for (int i = 0; i < out.len; ++i) {
            oss << std::setw(defaultW) << std::setfill('0') << std::hex << static_cast<int>(out.data[i]);
        }
        key = oss.str();
    }
    OH_Crypto_FreeDataBlob(&out);
    OH_DigestCrypto_Destroy(ctx);
    ctx = nullptr;
    return key;
}
}