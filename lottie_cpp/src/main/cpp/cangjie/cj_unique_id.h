/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_CJ_UNIQUE_ID_H
#define RLOTTIE510828_CJ_UNIQUE_ID_H

#include <atomic>

#include "lottie_type.h"
#include "lottie_utils.h"

namespace LottieTurbo {

extern "C" { // cangjie [get UniqueID] ffi

class UniqueID {
 public:
  static uint32_t Next();
};

CJBytesArray cj_LottieView_uniqueID();

}
}

#endif //RLOTTIE510828_CJ_UNIQUE_ID_H
