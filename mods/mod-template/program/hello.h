#pragma once
/*
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 Author <author-contact@email-domain.tld>
 */
#include "app.h"

// FLECS Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef MOD_TEMPLATE_UI_STATE_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

void ModTemplateHelloImport(ecs_world_t *world);

// Components
MOD_API ECS_STRUCT(ModTemplateUIState, {
    bool show_template_window;
});
