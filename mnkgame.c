#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "player.h"

#define PLAYER1 1
#define PLAYER2 2
#define DRAW    0

int M,N,K;

void mypause() {
	printf("\n\n -- PRESS A KEY TO CONTINUE --");
	getchar();
	fflush(stdin); /* Win: fflush(stdin);  */
}

/*
 * Stampa un messaggio di termine gioco.
 */
void end_game(int status, int HUMAN) {
	if(status==DRAW)
		printf("\n\n  GAME ENDED: Draw!!\n");
	else printf("\n\n  GAME ENDED: %s wins!!\n",status==HUMAN?"HUMAN":"COMPUTER");
}


/*
 * Legge i parametri di gioco.
 */
int read_parameters(char *argv[]) {
	if((M=atoi(argv[1]))<=1) {
		fprintf(stderr,"Error: the board must have at least 2 rows\n");
		return 0;
	}
	if((N=atoi(argv[2]))<=1) {
		fprintf(stderr,"Error: the board must have at least 2 columns\n");
		return 0;
	}
	if((K=atoi(argv[3]))<=1) {
		fprintf(stderr,"Error: the number K must be at least 2\n");
		return 0;
  }
	if(K>M && K>N) {
		fprintf(stderr,"Error: the number K must be no greather than both M=%d and N=%d\n",M,N);
		return 0;
	}
	return 1;
}

/*
 * Legge la mossa successiva del giocatore COMPUTER.
 *
 * Restituisce 1 se l'operazione e' andata a buon fine, 0 altrimenti.
 */
int read_move_computer_player(int PLAYER) {
	unsigned int i,j;
	printf("\nPlayer%d - COMPUTER (%c): ",PLAYER,PLAYER==1?'X':'O');
	if(!get_next_move(&i,&j))
		return 0;
	printf("%c%c\n",(char)i+BASE_INDEX,(char)j+BASE_INDEX);
	if(PLAYER==1)
		return set_move_player1(i,j);
	else
		return set_move_player2(i,j);
}

/*
 * Legge la mossa successiva del giocatore HUMAN.
 *
 * Restituisce 1 se l'operazione e' andata a buon fine, 0 altrimenti.
 */
int read_move_human_player(int PLAYER) {
  char A,B;
	int check;
	unsigned int i,j;
  printf("\nPlayer%d - HUMAN (%c): ",PLAYER,PLAYER==1?'X':'O');
  A=getchar();
  B=getchar();
  fflush(stdin);
	if(A-BASE_INDEX<0 || B-BASE_INDEX<0) return 0;
	i=A-BASE_INDEX;
	j=B-BASE_INDEX;
	if(PLAYER==1)
		check=set_move_player1(i,j);
	else
		check=set_move_player2(i,j);
	if(check)
		set_opponent_move(i,j);
	return check;	
}


/*
 * Legge la mossa successiva. Nel caso in cui il giocatore computer
 * non esegua una mossa corretta, termina il gioco.
 * 
 * Restutuisce 1 se la partita e' vinta da uno dei giocatori, 0 altrimenti.
 */
int read_move(int PLAYER, int HUMAN) {
	system("cls");
	print_board();
	if(PLAYER==HUMAN) 
		while(!read_move_human_player(PLAYER))
      printf("Not a valid move!\n\n");
	else
		while(!read_move_computer_player(PLAYER)) {
			printf("Not a valid move!\n\n");
			exit(1);
		}
	mypause();
	if((PLAYER==1 && check_win_player1()) || (PLAYER==2 && check_win_player2())) return PLAYER;
	return 0;
}



int main(int argc, char *argv[]) {
	int CONTINUE,WINNER,HUMAN,COMPUTER;	

	if(argc!=4) {
		fprintf(stderr,"Usage: mnkgame <row> <col> <k>\n");
		return 1;
	}
	if(!read_parameters(argv)) return 1;

	if(!setup_game(M,N,K)) {
		fprintf(stderr,"Error: the board is too big!\n");
		return 1;
	}

	COMPUTER=1;
	HUMAN=2;
	
	setup_player(M,N,K);
	printf("Computer Player ready\n");
	mypause();

	CONTINUE=N*M;
  WINNER=DRAW;

	while(CONTINUE>0) {
		if(read_move(PLAYER1,HUMAN)) {
			WINNER=PLAYER1;
			CONTINUE=0;
		}
		if(--CONTINUE>0) {

			if(read_move(PLAYER2,HUMAN)) {
				WINNER=PLAYER2;
				CONTINUE=0;
			}
			--CONTINUE;
		}
	}
	system("cls");
	print_board();
	end_game(WINNER,HUMAN);	
	

	free_player();
	free_game();

	return 0;
}
