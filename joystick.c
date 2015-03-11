#include "joystick.h"
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
int minv(int a, int b) {
        return a<b ? a : b;
}
int joystick_init(int id){
        SDL_Init(SDL_INIT_JOYSTICK);
        // Initialize the joystick subsystem
        signal(SIGINT,SIG_DFL);
        // Check for joystick
        if (SDL_NumJoysticks() > id) {
                // Open joystick
                jstick = SDL_JoystickOpen(id);

                if (jstick) {
                        printf("Opened Joystick 0\n");
                        printf("Name: %s\n", SDL_JoystickNameForIndex(id));
                        printf("Number of Axes: %d\n", SDL_JoystickNumAxes(jstick));
                        printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(jstick));
                        printf("Number of Balls: %d\n", SDL_JoystickNumBalls(jstick));
                } else {
                        printf("Couldn't open Joystick %i\n",id);
                        return -1;
                }
        }
        return 0;
}
int joystick_update(packet_t *ctl){
        int i;
        if(SDL_JoystickGetAttached(jstick) == SDL_FALSE)
                return -1;
        SDL_JoystickUpdate();
        //populate controller struct
        ctl->stickX = (SDL_JoystickGetAxis(jstick, 1)/256)+128;
        ctl->stickY = (SDL_JoystickGetAxis(jstick, 3)/256)+128;
        ctl->btnlo = 0;
        ctl->btnhi = 0;
        for(i=0; (i<minv(SDL_JoystickNumButtons(jstick), 15)); i++){
                if(i<8){
                        ctl->btnlo |= (SDL_JoystickGetButton(jstick,i) << i);
                } else if(i>=8 && i<15){
                        ctl->btnhi |= (SDL_JoystickGetButton(jstick,i) << (i-8));
                }
        }
        return 0;
}
int joystick_wait_safe(){
        int i;
        packet_t tmp;
        if(SDL_JoystickGetAttached(jstick) == SDL_FALSE)
                return -1;
        do{
                usleep(1E5);
                SDL_JoystickUpdate();
                //populate controller struct
                tmp.stickX = (SDL_JoystickGetAxis(jstick, 1)/256);
                tmp.stickY = (SDL_JoystickGetAxis(jstick, 3)/256);
                tmp.btnlo = 0;
                tmp.btnhi = 0;
                for(i=0; (i<minv(SDL_JoystickNumButtons(jstick), 15)); i++){
                        if(i<8){
                                tmp.btnlo |= (SDL_JoystickGetButton(jstick,i) << i);
                        } else if(i>=8 && i<15){
                                tmp.btnhi |= (SDL_JoystickGetButton(jstick,i) << (i-8));
                        }
                }
                printf("Waiting for safe stick position\n");
                printf("X: %i, Y: %i, buttons 0-7: %x, buttons 8+: %x\n", tmp.stickX, tmp.stickY, tmp.btnlo, tmp.btnhi);
        }while(abs(tmp.stickX) < 2 && abs(tmp.stickY) < 2 && tmp.btnlo == 0 && tmp.btnhi == 0);
        return 0;        

}
void joystick_release(){ 	 
        // Close if opened
        if (SDL_JoystickGetAttached(jstick)) {
                SDL_JoystickClose(jstick);
        }

}
