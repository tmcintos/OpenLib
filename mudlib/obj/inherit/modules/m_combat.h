// combat.h:  Functions for objects which do combat
// Tim 24 Apr 96 (created from old combat.c)
#ifndef _M_COMBAT_H
#define _M_COMBAT_H

//
// You must also define the functions found in race.h at some point when
// using this module. (Normally inherit a race module)
//
#include <daemons.h> 
#include <weapon_types.h>
#include <race.h>

void initialize();
void low_setup();
void set_combating(int);
int query_num_attacks();
int query_combating();
void set_ac(int*);
int* query_ac();
void set_armour(int);
int query_armour();
void set_free_hands(int);
int query_free_hands();
mixed* query_weapon_info();
void set_weapon_info(mixed*);
varargs int do_unwield(object weapon, int make_noise);
varargs int do_wield(object weapon, int hands_used, int make_noise);
varargs int do_wear(object armour, int make_noise);
void do_remove(object armour, int make_noise);

#endif /* _M_COMBAT_H */
