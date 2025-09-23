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

#ifndef LOTTIETURBO_GLY_PATH_BAG_H
#define LOTTIETURBO_GLY_PATH_BAG_H

#include "path.h"
#include "pen.h"
#include <unordered_map>
#include <vector>

namespace LottieTurbo::drawing {

class GlyPathBag {
private:
    int GetIndex(char ch);
    std::vector<drawing::Path> paths;
    std::unordered_map<char, int> data;
    drawing::Pen paint;

public:
    explicit GlyPathBag(drawing::Pen const &paint)
    {
        // Make indexed-by-one, to allow zero to represent non-cached
        paths.push_back(drawing::Path());
    }
    drawing::Path GetOrCreateAndCache(const char &ch, const std::string &current);
};

} // namespace LottieTurbo::drawing

#endif
