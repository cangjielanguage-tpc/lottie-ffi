/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lottie_utils.h"


namespace LottieTurbo {

extern "C" {

void safe_copy(void *dest, uint32_t dest_size, const void *src, uint32_t src_size)
{
    if (!dest || !src) {
        throw std::invalid_argument("safe_copy null pointer passed to safe_copy.");
    }
    if (dest_size < src_size) {
        throw std::runtime_error("safe_copy insufficient space in the target buffer.");
    }
    uint8_t *dest_ptr = static_cast<uint8_t *>(dest);
    const uint8_t *src_ptr = static_cast<const uint8_t *>(src);
    for (uint32_t i = 0; i < src_size; ++i) {
        dest_ptr[i] = src_ptr[i];
    }
}

CJBytesArray stringToCJBytesArray(const std::string &str)
{
    CJBytesArray result;
    if (str.empty()) {
        return {0, nullptr};
    }
    result.size = static_cast<int64_t>(str.size());
    result.buf = static_cast<uint8_t *>(malloc(result.size));
    if (result.buf == nullptr) {
        throw std::bad_alloc();
    }
    safe_copy(result.buf, static_cast<uint32_t>(result.size), str.data(), static_cast<uint32_t>(result.size));
    return result;
}

}
}