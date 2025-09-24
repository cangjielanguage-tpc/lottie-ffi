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

#include "point.h"

namespace LottieTurbo::drawing {

Point::Point(float x, float y) : point_(OH_Drawing_PointCreate(x, y), OH_Drawing_PointDestroy)
{
}

Point::Point(const Point &other) : Point()
{
    float x = 0.0f;
    float y = 0.0f;
    OH_Drawing_PointGetX(other.get(), &x);
    OH_Drawing_PointGetY(other.get(), &y);
    OH_Drawing_PointSet(point_.get(), x, y);
}

Point &Point::operator=(const Point &other)
{
    if (this != &other) {
        float x = 0.0f;
        float y = 0.0f;
        OH_Drawing_PointGetX(other.get(), &x);
        OH_Drawing_PointGetY(other.get(), &y);
        OH_Drawing_PointSet(point_.get(), x, y);
    }
    return *this;
}

float Point::GetX() const
{
    float x = 0.0f;
    OH_Drawing_PointGetX(point_.get(), &x);
    return x;
}

float Point::GetY() const
{
    float y = 0.0f;
    OH_Drawing_PointGetY(point_.get(), &y);
    return y;
}

void Point::Set(float x, float y)
{
    OH_Drawing_PointSet(point_.get(), x, y);
}

} // namespace LottieTurbo::drawing