#include "std.h"
#include "interface.h"
#include "lpc_to_c.h"

static void LPCINIT_cmd_adm_nslookup() {
C_AGGREGATE_ASSOC(0);
C_LVALUE(&current_object->variables[variable_index_offset + 1]);
c_void_assign();
c_return_zero();
return;
}
static void LPC_cmd_adm_nslookup__main() {
C_LOCAL(0);
c_not();
C_LOR(goto label000);
C_LOCAL(0);
C_STRING(6);
f_eq();
label000:;
C_BRANCH_WHEN_ZERO(goto label001);
C_STRING(7);
CHECK_TYPES(sp - 0, 68, 1, 131);
st_num_arg = 1;
f_notify_fail();
push_number(0);
c_return();
return;
label001:;
C_LOCAL(0);
C_STRING(8);
CHECK_TYPES(sp - 1, 4, 1, 304);
CHECK_TYPES(sp - 0, 4, 2, 304);
st_num_arg = 2;
f_resolve();
push_number(0);
CHECK_TYPES(sp - 0, 2, 1, 113);
st_num_arg = 1;
f_this_player();
C_AGGREGATE_ASSOC(2);
C_LVALUE(&current_object->variables[variable_index_offset + 1]);
c_add_eq(1);
C_STRING(9);
call_simul_efun(0, (lpc_int = 1 + num_varargs, num_varargs = 0, lpc_int));
pop_stack();
push_number(1);
c_return();
return;
}

static void LPC_cmd_adm_nslookup__callback() {
C_LOCAL(2);
C_GLOBAL(1);
c_index();
C_STRING(10);
C_LOCAL(1);
C_LOCAL(0);
CHECK_TYPES(sp - 2, 4, 1, 285);
CHECK_TYPES(sp - 1, 1022, 2, 285);
CHECK_TYPES(sp - 0, 1022, 3, 285);
st_num_arg = 3 + num_varargs;
num_varargs = 0;
f_sprintf();
call_simul_efun(20, (lpc_int = 2 + num_varargs, num_varargs = 0, lpc_int));
pop_stack();
c_return_zero();
return;
}



static void (*functions[])() = {
LPC_cmd_adm_nslookup__main,
LPC_cmd_adm_nslookup__callback,
LPCINIT_cmd_adm_nslookup,
0
};

interface_t LPCINFO_cmd_adm_nslookup = {
    "cmd/adm/nslookup",
    functions,
    0
};
