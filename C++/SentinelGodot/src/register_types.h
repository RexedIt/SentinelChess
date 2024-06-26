#ifndef GDSENTINEL_REGISTER_TYPES_H
#define GDSENTINEL_REGISTER_TYPES_H

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level);
void uninitialize_example_module(ModuleInitializationLevel p_level);

void initialize_gdextension_types(godot::ModuleInitializationLevel p_level);
void uninitialize_gdextension_types(godot::ModuleInitializationLevel p_level);

#endif // GDSENTINEL_REGISTER_TYPES_H