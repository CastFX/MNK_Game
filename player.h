#ifndef PLAYER_H
#define PLAYER_H

/*
 * Inizializza il player.
 *
 * Restituisce 1 se l'inizializzazione e' andata a buon fine, 0 altrimenti. 
 */
int  setup_player(unsigned int m, unsigned int n, unsigned int k);

/*
 * Salva la mossa dell'avversario.
 *
 * Restituisce 1 se l'operazione e' andata a buon fine, 0 altrimenti.
 */
int  set_opponent_move(unsigned int i, unsigned int j);

/*
 * Seleziona la mossa successiva.
 *
 * Restituisce 1 se l'operazione e' andata a buon fine, 0 altrimenti.
 */
int  get_next_move(unsigned int *i, unsigned int *j);

/*
 * Distrugge e resetta le stutture interne.
 */
void free_player();

#endif
