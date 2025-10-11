/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cj_unique_id.h"

namespace LottieTurbo {
extern "C" { // cangjie [get UniqueID] ffi

static constexpr uint32_t InvalidUniqueID = 0;

uint32_t UniqueID::Next() {
  static std::atomic<uint32_t> nextID{1};
  uint32_t id;
  do {
    id = nextID.fetch_add(1, std::memory_order_relaxed);
  } while (id == InvalidUniqueID);
  return id;
}

//------------------------------------------------------------------------------------------------------------

CJBytesArray cj_LottieView_uniqueID()
{
    std::string id = std::to_string(UniqueID::Next());
    CJBytesArray result = stringToCJBytesArray(id);
    return result;
}

}
}
