#include "std.h"
#include "interface.h"
#include "lpc_to_c.h"

extern interface_t LPCINFO_cmd_adm_nslookup;


interface_t *interface[] = {
    &LPCINFO_cmd_adm_nslookup,
    0
};
