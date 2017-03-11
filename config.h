/*
 * File:   config.h
 * Author: Syed Tahmid Mahbub
 * Modified by: Bruce Land 
 * Modified again by: Manish Patel
 * Created on October 10, 2014
 * Mod: 24Sept2015
 * Mod2: March 2017
 */

#ifndef CONFIG_H
#define	CONFIG_H
#define _SUPPRESS_PLIB_WARNING 1
#include <plib.h>
// serial stuff
#include <stdio.h>

//=============================================================
// 40 MHz
#pragma config FNOSC = FRCPLL, POSCMOD = OFF, OSCIOFNC = OFF
#pragma config FPLLIDIV = DIV_2, FPLLMUL = MUL_20 //40 MHz
#pragma config FPBDIV = DIV_1, FPLLODIV = DIV_2 // PB 40 MHz
#pragma config FWDTEN = OFF,  FSOSCEN = OFF, JTAGEN = OFF

//==============================================================
// Protothreads configure

// IF use_vref_debug IS defined, pin 25 is Vref output
//#define use_vref_debug

// IF use_uart_serial IS defined, pin 21 and pin 22 are used by the uart
//#define use_uart_serial
#define BAUDRATE 9600 // must match PC terminal emulator setting

// enable UART2 - see PT_setup in pt_cornell_1_2_1.h
#define use_uart_serial 

/////////////////////////////////
// set up clock parameters
// system cpu clock
#define sys_clock 40000000

// sys_clock/FPBDIV
#define pb_clock sys_clock // divide by one in this case

#endif	/* CONFIG_H */

