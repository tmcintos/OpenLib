// ucache.c:  I3 'ucache' implementation
//
// Created 04.12.96 by Tim@Dysfunctional Mud

#include <net/daemons.h>

#define SERVICE_UCACHE

void
eventReceiveUcacheUpdate(mixed* packet)
{
  UCACHE_D->cache(packet[2], packet[6], packet[7], packet[8]);
}
