// TODO: Use correct inlucde file.
#include <sequoia/env/export.h>
// #include <sequoia/interface/public/nr_plugin_interface.h>

#ifdef SEQUOIA_SHARED_LIBS
SEQUOIA_EXTERN_C_BEGIN
/*
void NRPluginLoad_Sequoia(NRInterfaces* interfaces);
void NRPluginUnload_Sequoia();

void NR_INTERFACE_EXPORT NR_INTERFACE_API
NRPluginLoad(NRInterfaces* interfaces) {
	NRPluginLoad_Sequoia(interfaces);
}
void NR_INTERFACE_EXPORT NR_INTERFACE_API
NRPluginUnload() {
	NRPluginUnload_Sequoia();
}
*/
SEQUOIA_EXTERN_C_END
#endif
