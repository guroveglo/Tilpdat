
#include <stdio.h>
#include <stdlib.h>
#include "hardware.h"
#include "logic.h"
#include "door.h"
#include "order.h"

int stop_pressed=0;

void start_condition() {
    reset_lights();
    while (read_floor()!=0) {
       hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
       floor_indicator();
    }
    save_dir=dir;   
    dir = HARDWARE_MOVEMENT_STOP;
    hardware_command_movement(dir);
}

int read_floor() {
    for (int i = 0; i < NUMB_FLOORS; i++) {
        if (hardware_read_floor_sensor(i)) {
            return i;
        }
    }
    return -1;
}

void floor_indicator(){
    int floor = read_floor();
    if (floor!=-1){
        hardware_command_floor_indicator_on(floor);
    }
}

void reset_lights(){
    for (int i = 0; i < NUMB_FLOORS; i++){  
            elev_queue(i, HARDWARE_ORDER_UP,0);
            elev_queue(i, HARDWARE_ORDER_DOWN,0);
            elev_queue(i, HARDWARE_ORDER_INSIDE,0);
        } 
}

void check_and_stop_elevator(){	
    if(hardware_read_stop_signal()){
        reset_lights();   
        dir = HARDWARE_MOVEMENT_STOP;   
        hardware_command_movement(dir);
        while(hardware_read_stop_signal()){
            hardware_command_stop_light(1);
            stopbutton_door_open();
        }        
        hardware_command_stop_light(0);
        stop_pressed = 1;
    }
}

void door_delay(){
	time_t start_time = time(NULL);
	time_t current_time = time(NULL);
    double time_used = difftime(current_time,start_time);
     
    while (time_used<TIME_DELAY){
		check_buttons_update_floor();

		if(hardware_read_obstruction_signal()||hardware_read_stop_signal()){
			start_time = time(NULL);
		}
		current_time = time(NULL);
		time_used = difftime(current_time,start_time);
		
		if (!hardware_read_stop_signal()){
			hardware_command_stop_light(0);
		}
	}
}
