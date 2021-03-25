/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */

void *stdin, *stdout, *stderr;

/* Declare a helper function which is local to this file */
static void num32asc(char *s, int);

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc)
{
  int i;
  for (i = cyc; i > 0; i--)
    ;
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick(unsigned int *timep)
{
  /* Get current value, store locally */
  register unsigned int t = *timep;
  t += 1; /* Increment local copy */

  /* If result was not a valid BCD-coded time, adjust now */

  if ((t & 0x0000000f) >= 0x0000000a)
    t += 0x00000006;
  if ((t & 0x000000f0) >= 0x00000060)
    t += 0x000000a0;
  /* Seconds are now OK */

  if ((t & 0x00000f00) >= 0x00000a00)
    t += 0x00000600;
  if ((t & 0x0000f000) >= 0x00006000)
    t += 0x0000a000;
  /* Minutes are now OK */

  if ((t & 0x000f0000) >= 0x000a0000)
    t += 0x00060000;
  if ((t & 0x00ff0000) >= 0x00240000)
    t += 0x00dc0000;
  /* Hours are now OK */

  if ((t & 0x0f000000) >= 0x0a000000)
    t += 0x06000000;
  if ((t & 0xf0000000) >= 0xa0000000)
    t = 0;
  /* Days are now OK */

  *timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug(volatile int *const addr)
{
  display_string(1, "Addr");
  display_string(2, "Data");
  num32asc(&textbuffer[1][6], (int)addr);
  num32asc(&textbuffer[2][6], *addr);
  display_update();
}

uint8_t spi_send_recv(uint8_t data)
{
  while (!(SPI2STAT & 0x08))
    ;
  SPI2BUF = data;
  while (!(SPI2STAT & 1))
    ;
  return SPI2BUF;
}

void display_init(void)
{
  DISPLAY_CHANGE_TO_COMMAND_MODE;
  quicksleep(10);
  DISPLAY_ACTIVATE_VDD;
  quicksleep(1000000);

  spi_send_recv(0xAE);
  DISPLAY_ACTIVATE_RESET;
  quicksleep(10);
  DISPLAY_DO_NOT_RESET;
  quicksleep(10);

  spi_send_recv(0x8D);
  spi_send_recv(0x14);

  spi_send_recv(0xD9);
  spi_send_recv(0xF1);

  DISPLAY_ACTIVATE_VBAT;
  quicksleep(10000000);

  spi_send_recv(0xA1);
  spi_send_recv(0xC8);

  spi_send_recv(0xDA);
  spi_send_recv(0x20);

  spi_send_recv(0xAF);
}

void display_string(int line, char *s)
{
  int i;
  if (line < 0 || line >= 4)
    return;
  if (!s)
    return;

  for (i = 0; i < 16; i++)
    if (*s)
    {
      textbuffer[line][i] = *s;
      s++;
    }
    else
      textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data)
{
  int i, j;

  for (i = 0; i < 4; i++)
  {
    DISPLAY_CHANGE_TO_COMMAND_MODE;

    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(x & 0xF);
    spi_send_recv(0x10 | ((x >> 4) & 0xF));

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < 32; j++)
      spi_send_recv(~data[i * 32 + j]);
  }
}

void display_update(void)
{
  int i, j, k;
  int c;
  for (i = 0; i < 4; i++)
  {
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);
    spi_send_recv(i);

    spi_send_recv(0x0);
    spi_send_recv(0x10);

    DISPLAY_CHANGE_TO_DATA_MODE;

    for (j = 0; j < 16; j++)
    {
      c = textbuffer[i][j];
      if (c & 0x80)
        continue;

      for (k = 0; k < 8; k++)
        spi_send_recv(font[c * 8 + k]);
    }
  }
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc(char *s, int n)
{
  int i;
  for (i = 28; i >= 0; i -= 4)
    *s++ = "0123456789ABCDEF"[(n >> i) & 15];
}

/*
 * nextprime
 * 
 * Return the first prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE 0 /* Constant to help readability. */
#define PRIME_TRUE 1  /* Constant to help readability. */
int nextprime(int inval)
{
  register int perhapsprime = 0; /* Holds a tentative prime while we check it. */
  register int testfactor;       /* Holds various factors for which we test perhapsprime. */
  register int found;            /* Flag, false until we find a prime. */

  if (inval < 3) /* Initial sanity check of parameter. */
  {
    if (inval <= 0)
      return (1); /* Return 1 for zero or negative input. */
    if (inval == 1)
      return (2); /* Easy special case. */
    if (inval == 2)
      return (3); /* Easy special case. */
  }
  else
  {
    /* Testing an even number for primeness is pointless, since
      * all even numbers are divisible by 2. Therefore, we make sure
      * that perhapsprime is larger than the parameter, and odd. */
    perhapsprime = (inval + 1) | 1;
  }
  /* While prime not found, loop. */
  for (found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2)
  {
    /* Check factors from 3 up to perhapsprime/2. */
    for (testfactor = 3; testfactor <= (perhapsprime >> 1) + 1; testfactor += 1)
    {
      found = PRIME_TRUE;                   /* Assume we will find a prime. */
      if ((perhapsprime % testfactor) == 0) /* If testfactor divides perhapsprime... */
      {
        found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
        goto check_next_prime; /* Break the inner loop, go test a new perhapsprime. */
      }
    }
  check_next_prime:;         /* This label is used to break the inner loop. */
    if (found == PRIME_TRUE) /* If the loop ended normally, we found a prime. */
    {
      return (perhapsprime); /* Return the prime we found. */
    }
  }
  return (perhapsprime); /* When the loop ends, perhapsprime is a real prime. */
}

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ (24)
char *itoaconv(int num)
{
  register int i, sign;
  static char itoa_buffer[ITOA_BUFSIZ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ITOA_BUFSIZ - 1] = 0; /* Insert the end-of-string marker. */
  sign = num;                       /* Save sign. */
  if (num < 0 && num - 1 > 0)       /* Check for most negative integer */
  {
    for (i = 0; i < sizeof(maxneg); i += 1)
      itoa_buffer[i + 1] = maxneg[i];
    i = 0;
  }
  else
  {
    if (num < 0)
      num = -num;        /* Make number positive. */
    i = ITOA_BUFSIZ - 2; /* Location for first ASCII digit. */
    do
    {
      itoa_buffer[i] = num % 10 + '0'; /* Insert next digit. */
      num = num / 10;                  /* Remove digit from number. */
      i -= 1;                          /* Move index to next empty position. */
    } while (num > 0);
    if (sign < 0)
    {
      itoa_buffer[i] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return (&itoa_buffer[i + 1]);
}

/**
 * BLACKJACK
 * Everything under here is written by Arif Jehda-Oh and Nils Falck
 * 2021-02-22
 **/

/** VARIABLES **/
int player_score = 0;
int player_state = 1;
int player_draw;
char player_hand[260][9]; // Hold 52 card, 1 card = 5 characters.

int dealer_draw;
int dealer_score = 0;

uint8_t decks[520] = {0};
char *player_card[10];
/**
 * RANDOM GENERATED SEED 
 * Generated from ChipKIT's TMR2 multiplied with the hardware rand value.
 **/
int generate_seed(void)
{
  return TMR2 * (TMR2 * ((2021421421421 % rand()) | srand(rand() % srand())));
}

/** RESETING TRISX
 * For Button AND SLIDE SWITCH 
 **/
void reset(void)
{
  TRISD = 0x0000;
  TRISF = 0x0000;
  return;
}

/** RESET DISPLAY **/
void reset_display(void)
{
  display_string(0, "");
  display_string(1, "");
  display_string(2, "");
  display_string(3, "");
}

/** RESET GAME **/
void reset_game(void)
{
  player_score = 0;
  player_state = 1;
  player_draw = 0;

  dealer_score = 0;
  dealer_draw = 0;

  return;
}

/** INITIALIZE **/
void init_buttons(void)
{
  reset();
  TRISD |= BTN4 | BTN3 | BTN2;
  TRISD |= SW1 | SW2 | SW3 | SW4;
  TRISF |= BTN1;
  return;
}

/** SETUP INTERRUPT **/
void setup_interrupt(void)
{
  PR2 = (80000000 / 256) / 10;
  T2CON = 0x00;
  T2CONSET = 0x70;
  TMR2 = 0;
  T2CONSET = 0x8000;
  IPC(2) |= 0x10;
  IEC(0) = 0x100;

  enable_interrupt();
  return;
}

/** Button pressed **/
int is_pressed(const int button)
{
  return (button & (PORTD | PORTF)) ? 1 : 0;
}

/* GET SUITE */
char get_suite(int index)
{
  float value = (float)index;
  value = value / 13;

  if (value <= 1)
  {
    return 'H';
  }
  else if (value <= 2)
  {
    return 'R';
  }
  else if (value <= 3)
  {
    return 'K';
  }
  else if (value <= 4)
  {
    return 'S';
  }
  return '\0';
}

char get_card_string(const int card_value)
{
  if (card_value == 11)
  {
    return 'J';
  }
  else if (card_value == 12)
  {
    return 'Q';
  }
  else if (card_value == 13)
  {
    return 'K';
  }
  return '\0';
}

void store_card_in_hand(int index, int value)
{
  char card_text[9];
  card_text[0] = get_suite(index);
  card_text[1] = get_card_string(value);
  value = value > 10 ? 10 : value;
  strcat(card_text, itoaconv(value));
  strcat(player_hand[player_draw], card_text);
  //strcat(player_hand[player_draw], ",");
}


int card_value(const int score)
{
  reset_display();
  int i, j, value;
  while (1)
  {
    int index = rand() % 52;
    value = decks[index];

    if (value != 0)
    {
      if (player_state == 1)
      {
        store_card_in_hand(index, value);
      }
      decks[index] = 0;
      value = value > 10 ? 10 : value;
      if (value == 1 && score < 11)
      {
        return 11;
      }
      else if (value == 1 && score >= 11)
      {
        return 1;
      }
      break;
    }
  }
  return value;
}
/** DRAW CARD **/
void draw_card(int player)
{
  if (player == PLAYER && player_state == 1)
  {
    player_draw++;
    player_score += card_value(player_score);
  }
  if (player == DEALER)
  {
    dealer_draw++;
    dealer_score += card_value(dealer_score);
  }
}

/** CHECK HAND SCORE
 * If true = game over,
 * If false = continue **/
int is_game_over(void)
{
  if (player_score == BLACKJACK || dealer_score == BLACKJACK || player_state == 0 || player_score > BLACKJACK)
  {
    return 1;
  }
  return 0;
}

/** Compare Score**/
int compare_score(void)
{
  if (player_score == BLACKJACK || dealer_score > BLACKJACK)
  {
    return 1;
  }
  if (player_score > dealer_score && player_score < BLACKJACK)
  {
    return 1;
  }

  return 0;
}

/** DISPLAY WITH SCORE
 * Build upon display_score(int, char*) **/
void display_score(int line, char *s, int score)
{
  int i;
  char *k = itoaconv(score);
  if (line < 0 || line >= 4)
    return;
  if (!s)
    return;

  for (i = 0; i < 16; i++)
    if (*s)
    {
      textbuffer[line][i] = *s;
      s++;
    }
    else
    {
      if (*k)
      {
        textbuffer[line][i] = *k;
        k++;
      }
      else
      {
        textbuffer[line][i] = ' ';
      }
    }
}

/** SHOW HAND **/
void display_all_hands(void)
{
    reset_display();
  // Show recent 2 cards
  char recent_drawn_cards[9];
  strcpy(recent_drawn_cards, player_hand[player_draw - 1]);
  char* notZero = player_hand[player_draw];
  if(notZero != "0" || *notZero != '\0'){
  strcat(recent_drawn_cards,",");
  strcat(recent_drawn_cards, player_hand[player_draw]);
  }

  display_score(3, "", player_draw);
  display_string(1, recent_drawn_cards); // Visar spelaren dragna kort
  display_score(0, DISPLAY_PLAYER_NAME, player_score);
  //display_score(1, DISPLAY_DRAWN, player_draw);
  //display_score(2, DISPLAY_DEALER_NAME, dealer_score);
  //display_score(3, DISPLAY_DRAWN, dealer_draw);
  display_update();
  return;
}

/* DISPLAY WINNER */
void display_winner(void)
{
  int result = compare_score();
  if (result == DEALER)
  {
    display_score(0, DISPLAY_DEALER_WON, dealer_score);
  }
  else if (result == PLAYER)
  {
    display_score(0, DISPLAY_PLAYER_WON, player_score);
  }
  else
  {
    display_string(0, DISPLAY_DRAW_GAME);
  }
  display_update();
  return;
}

/* GENERATE DECK */

void generate_deck(void) // void ==> n value
{
  int card;
  int suite = 0;
  int value = 1;
  int size = 52; // 520 cards = 10 decks, 52 for 1 deck
  for (card = 0; card < size; card++) // 
  {
    decks[card] = value;
    suite++;
    if (suite == 4) // 4 for 1 deck, 40 deck for 10 deck
    {
      suite = 0;
      value++;
    }
  }
  return;
}
