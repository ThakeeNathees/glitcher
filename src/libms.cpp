/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#include "common.h"
#include "types.h"

#include "ms_api.h"
#include "ms_editor.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options * o) {
    godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options * o) {
    godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void* handle) {
    godot::Godot::nativescript_init(handle);

    godot::register_class<MiniScript>();
    godot::register_class<_ErrorInfo>();
    godot::register_class<MSEditor>();
}
