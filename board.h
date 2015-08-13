#ifndef BOARD_H
#define BOARD_H

#define BASE_INDEX 97

/*
 * Inizializza il gioco.
 */
int setup_game(unsigned int, unsigned int, unsigned int);

/*
 * Libera le strutture dati del gioco. 
 */
void free_game();

/*
 * Stampa la scacchiera.
 */
void print_board();

/*
 * Setta la mossa del giocatore 1.
 */
int set_move_player1(unsigned int, unsigned int);

/*
 * Setta la mossa del giocatore 2.
 */
int set_move_player2(unsigned int, unsigned int);

/*
 * Restutisce 1 se il giocatore 1 ha vinto, 0 altrimenti. 
 */
int check_win_player1();

/*
 * Restutisce 1 se il giocatore 2 ha vinto, 0 altrimenti. 
 */
int check_win_player2();

#endif
