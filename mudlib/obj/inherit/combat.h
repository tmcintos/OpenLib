// combat.h: Prototypes for functions in combat.c

void init_combat();
void set_combating(int foo);
int query_combating();
void set_ac(int *new_ac);
int *query_ac();
void set_armour(int new_armours);
int query_armour();
void set_free_hands(int new_hands);
int query_free_hands();
void set_weapon_info(mixed *new_weapon_info);
mixed *query_weapon_info();
varargs int do_wield(object weapon, int hands_used, int make_noise);
varargs int do_unwield(object weapon, int make_noise);
varargs int do_wear(object armour, int make_noise);
void do_remove(object armour, int make_noise);
