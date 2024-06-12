/*

░██╗░░░░░░░██╗███╗░░░███╗██████╗░
░██║░░██╗░░██║████╗░████║██╔══██╗
░╚██╗████╗██╔╝██╔████╔██║██████╔╝
░░████╔═████║░██║╚██╔╝██║██╔═══╝░
░░╚██╔╝░╚██╔╝░██║░╚═╝░██║██║░░░░░
░░░╚═╝░░░╚═╝░░╚═╝░░░░░╚═╝╚═╝░░░░░*/

#include "Includes.h" 
#include "Game.h" 
#include "Verification.h" 
#include "Main.h"

SYS_MODULE_INFO( OBMW2, 0, 1, 1);
SYS_MODULE_START( _OBMW2_prx_entry );

SYS_LIB_DECLARE_WITH_STUB( LIBNAME, SYS_LIB_AUTO_EXPORT, STUBNAME );
SYS_LIB_EXPORT( _OBMW2_export_function, LIBNAME );

extern "C" int _OBMW2_export_function(void)
{
    return CELL_OK;
}

sys_ppu_thread_t plugin;
extern "C" int _OBMW2_prx_entry(void)
{
    sys_ppu_thread_create(&plugin, PLUGIN_START, 0, 0x4AA, 0x5000, 0, "MW2 Base"); 
    return SYS_PRX_RESIDENT;
}
