//#include "BL144002.h"
//#include "LCD.h"
#include "COMMUNICATION.h"

/******************************************************
 * º¯ÊýÃû£º
 * ÃèÊö  £º
 * ÊäÈë  : void
 * Êä³ö  £ºvoid
 * ×¢Òâ  £º    £¨£©²âÊÔ
*********************************************************/  


int controled_turn;
int controled_turn_time = 0;
int CONTROLED_TURN(int turn){
    if(controled_turn_time > 0){
        controled_turn_time--;
        return controled_turn;
    }else{
        return turn;
    }
}
void CONTROL_TURN(int turn, int time){
    controled_turn = turn;
    controled_turn_time = time;
}






int controled_motor;
int controled_motor_time = 0;
int CONTROLED_MOTOR(int motor){
    if(controled_motor_time > 0){
        controled_motor_time--;
        return controled_motor;
    }else{
        return motor;
    }
}
void CONTROL_MOTOR(int motor, int time){
    controled_motor = motor;
    controled_motor_time = time;
}