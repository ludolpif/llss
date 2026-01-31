#pragma once
/*
 * This file is part of LLSS.
 *
 * LLSS is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with LLSS.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 ludolpif <ludolpif@gmail.com>
 */
#include "app.h"

// FLECS Reflection system boilerplate
#undef ECS_META_IMPL
#undef ONCE
#ifndef APP_COMPONENTS_COMPOSITION_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#define ONCE extern
#else
#define ONCE
#endif

APP_API void AppComponentsCompositionImport(ecs_world_t *world);

APP_API ECS_STRUCT(LayoutVariant, {
    int32_t aspect_ratio_num;  // AVRational aspect_ratio numerator
    int32_t aspect_ratio_den;  // AVRational framerate denominator
});

APP_API ONCE ECS_QUERY_DECLARE(ActivateLayoutVariantsQuery);

