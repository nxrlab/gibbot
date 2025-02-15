/*
 * File:   swing_control.h
 * Author: harntson
 *
 * Created on March 2, 2015, 12:47 AM
 */

#ifndef SWING_CONTROL_H
#define	SWING_CONTROL_H

#define N_MAX 170   //maximum number of states
#define MESSAGE_MAX 50

//Arguments to pass to robot, define as struct args[n], with n being
//the number of distinct states
struct args {
    int torque;
    int t_swing;    //milliseconds
    int t_flight;   //milliseconds
    int mag_state;  // 00=both off, 01=top on, 10=bottom on, 11=both on
};

void write_swing(int n, struct args* control_args);
void read_swing(int n, struct args* control_args);

#endif	/* SWING_CONTROL_H */

