
#ifndef LE_SERVER_H
#define LE_SERVER_H

void le_server_init();

void le_server_start();

void le_server_stop();

double le_server_get_latest_latitude();

double le_server_get_latest_longitude();

double le_server_get_latest_speed_mps();

#endif