#include "register_types.h"

#include "sentinelcommon.h"
#include "sentinelchess.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_example_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
    ClassDB::register_class<ChessCoord>();
    ClassDB::register_class<ChessMove>();
    ClassDB::register_class<SentinelChess>();
    ClassDB::register_class<ChessPlayer>();
    ClassDB::register_class<ChessEvent>();
    ClassDB::register_class<ChessBoard>();
    ClassDB::register_class<ChessClock>();
    ClassDB::register_class<ChessMeta>();
}

void uninitialize_example_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
        return;
}

extern "C"
{
    // Initialization.
    GDExtensionBool GDE_EXPORT sentinel_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
        init_obj.register_initializer(initialize_example_module);
        init_obj.register_terminator(uninitialize_example_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
        return init_obj.init();
    }
}

// For Types
void initialize_gdextension_types(godot::ModuleInitializationLevel p_level)
{
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_CORE)
    {
        return;
    }
}

void uninitialize_gdextension_types(godot::ModuleInitializationLevel p_level)
{
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_CORE)
    {
        return;
    }
}