/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);

/**
 * BLACKJACK
 * Everything under here is written by Arif Jehda-Oh and Nils Falck
 * 2021-02-22
 **/
/** CONSTANTS **/
#ifndef DATA_H
#define DATA_H
/** Buttons **/ 
extern const int BTN1;
extern const int BTN2;
extern const int BTN3;
extern const int BTN4;
/** Switch **/ 
extern const int SW1;
extern const int SW2;
extern const int SW3;
extern const int SW4;

#endif
extern int playerState;
extern int player_score;
extern int cpu_score;

/** Functions **/
void init_buttons(void);
void setup_interrupt(void);
void display_score(int, char*, int);
int is_pressed(int);
int check_turn(void);
int check_score(void);
int compare_score(void);
void show_all_hands(void);
void reset_display(void);
void reset_game(void);
