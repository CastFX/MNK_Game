#include <stdio.h>
#include <stdlib.h>
#include "board.h"

#define PLYR1  0
#define PLYR2  1
#define PLYR1w 2
#define PLYR2w 3
#define EMPTY  4
#define LIMIT  26


#define TAG(n) (n==PLYR1?"X":(n==PLYR2?"O":(n==PLYR1w?"X":(n==PLYR2w?"O":" ")))) 
#define BOLDRED  "\033[1;31m" /* 1 -> bold ;  31 -> red */
#define BLINKRED "\033[5;31m" /* 5 -> blink ;  31 -> red */
#define NONE     "\033[0m"

static unsigned int M=0,N=0,K=0;
static unsigned short **BOARD=NULL;

static unsigned short **ushort_matrix_alloc(unsigned int row, unsigned int col) {
  unsigned short **m=NULL;
  unsigned int i;

  if((m=(unsigned short **)calloc(row,sizeof(unsigned short *)))!=NULL) {
		if((m[0]=(unsigned short *)calloc(row*col,sizeof(unsigned short)))==NULL) {
			free(m);
			m=NULL;
		} else {
			for(i=1; i<row; i++)
				m[i]=&m[i-1][col];
		} 
  }
  return m;
}

static void ushort_matrix_free(unsigned short **m) {
	free(m[0]);
	free(m);
}

static int setup_board(unsigned int m, unsigned int n) {
	unsigned int i,j;
	if((BOARD=ushort_matrix_alloc(m,n))==NULL)
		return 0;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			BOARD[i][j]=EMPTY;
	return 1;
}

static void print_top_index() {
	unsigned int i;
	printf("  ");
	for(i=0; i<N; i++)
		printf("  %c ",BASE_INDEX+i);
	printf("\n");
}

static void print_border() {
	unsigned int i;
	printf("  +");
	for(i=0; i<N; i++)
		printf("---+");
	printf("\n");
}

static void print_row(unsigned int i) {
	unsigned int j;
	printf("%c |",BASE_INDEX+i);
	for(j=0; j<N; j++) 
		printf(" %s |",TAG(BOARD[i][j]));
	printf("\n");
}

static int check_win_row(unsigned int i, unsigned int j, int PLAYER) {
	unsigned int k;
	if(j+K>N) return 0;
	for(k=0; k<K; k++)
		if(BOARD[i][j+k]!=PLAYER) return 0;
	return 1;
}

static int check_win_col(unsigned int i, unsigned int j, int PLAYER) {
	unsigned int k;
	if(i+K>M) return 0;
	for(k=0; k<K; k++)
		if(BOARD[i+k][j]!=PLAYER) return 0;
	return 1;
}

static int check_win_diag(unsigned int i, unsigned int j, int PLAYER) {
	unsigned int k;
	if(i+K>M || j+K>N) return 0;
	for(k=0; k<K; k++)
		if(BOARD[i+k][j+k]!=PLAYER) return 0;
	return 1;
}

static int check_win_antidiag(unsigned int i, unsigned int j, int PLAYER) {
	unsigned int k;
	if(i+K>M || j<K-1) return 0;
	for(k=0; k<K; k++)
		if(BOARD[i+k][j-k]!=PLAYER) return 0;
	return 1;
}

static void setup_win_row(unsigned int i, unsigned int j) {
	unsigned int k;
	for(k=0; k<K; k++)
    BOARD[i][j+k]+=2;
}

static void setup_win_col(unsigned int i, unsigned int j) {
  unsigned int k;
  for(k=0; k<K; k++)
    BOARD[i+k][j]+=2;
}

static void setup_win_diag(unsigned int i, unsigned int j) {
  unsigned int k;
  for(k=0; k<K; k++)
    BOARD[i+k][j+k]+=2;
}

static void setup_win_antidiag(unsigned int i, unsigned int j) {
	unsigned int k;
  for(k=0; k<K; k++)
    BOARD[i+k][j-k]+=2;
}


static int check_win(int PLAYER) {
	unsigned int i, j;
	for(i=0; i<M; i++)
		for(j=0; j<N; j++) {
			if(check_win_row(i,j,PLAYER)) {
				setup_win_row(i,j);
				return 1;
			} else if(check_win_col(i,j,PLAYER)) {
				setup_win_col(i,j);
				return 1;
			} else if(check_win_diag(i,j,PLAYER)) {
				setup_win_diag(i,j);
				return 1;
			} else if(check_win_antidiag(i,j,PLAYER)) {
				setup_win_antidiag(i,j);
				return 1;
			}
		}
	return 0;
}

static int set_move_player(unsigned int i, unsigned int j, int PLAYER) {
	if(i>=M || j>=N || BOARD[i][j]!=EMPTY) return 0;
	BOARD[i][j]=PLAYER;
	return 1;
}


/*
 * PUBLIC FUNCTIONS
 */

int setup_game(unsigned int m, unsigned int n, unsigned int k) {
	M=m;
	N=n;
	K=k;
	return (M<=LIMIT && N<=LIMIT && setup_board(M,N));
}

void free_game() {
	M=N=K=0;
	ushort_matrix_free(BOARD);
	BOARD=NULL;
}

void print_board() {
	unsigned int i;
	if(BOARD!=NULL) {
		print_top_index();
		print_border();
		for(i=0; i<M; i++) {
			print_row(i);
			print_border();
		}
	}
}


void print_win_board(int PLAYER) {
	unsigned int i,j;
	int direction=-1;

	for(i=0; i<M && direction==-1; i++)
		for(j=0; j<N && direction==-1; j++)
			if(check_win_row(i,j,PLAYER)) direction=0;
			else if(check_win_col(i,j,PLAYER)) direction=1;
			else if(check_win_diag(i,j,PLAYER)) direction=2;
			else if(check_win_antidiag(i,j,PLAYER)) direction=3;

	if(BOARD!=NULL) {
		print_top_index();
		print_border();
		for(i=0; i<M; i++) {
			print_row(i);
			print_border();
		}
	}
}

int set_move_player1(unsigned int i, unsigned int j) {
  return set_move_player(i,j,PLYR1);
}

int set_move_player2(unsigned int i, unsigned int j) {
  return set_move_player(i,j,PLYR2);
}

int check_win_player1() {
	return check_win(PLYR1);
}

int check_win_player2() {
  return check_win(PLYR2);
}
