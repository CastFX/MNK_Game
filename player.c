#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#define MAX 1
#define MIN 2


/*
Current situation: Runs smoothly 5x5x3, 4x4x4, 6x4x3 (no 4x6x3) (no 3x7x3)


Errors:  NOT FOUND.

Latest Improvements:
	
	MostWins now lets AI choose (if value == 0) the move with most possible wins based on (depth^2) to valorize wins with less moves done
	DeleteTree!
	
List to do: 
	
	//



*/
static unsigned int M = 0, N = 0, K = 0, C = 0, Z = 0; //Z is used only to determine if I created already a gametree (whenever I created it > z++)
static unsigned int tmpI = -1, tmpJ = -1;
static unsigned short **BOARD = NULL;


/*Node's structure*/
typedef struct NODE {

	
	int depth; //equals to M*N-C
	unsigned int i; //coordinate 1
	unsigned int j; //coordinate 2
	int player; //MAX or MIN, if I have to choose between maximizing moves or minimizing moves
	int value; //initialized with 2,-2 whether it is a MAX or a MIN node
	unsigned short *transboard; //BOARD as a monodimensional array, used to code better functions
	struct NODE *leftchild; // points to the first of the children list
	struct NODE *rightbrothers; //points to the next brother in the children list

}NODE;

static NODE *GameTree = NULL;


static int Max(int a, int b) {
	if (a >= b)
		return a;

	return b;
}
static int Min(int a, int b) {
	if (a <= b)
		return a;

	return b;
}



/*Given a father-node, it allocates the space for a child and the new node's values are set according the function input*/
static int AllocateChildren(NODE **T, int depth, unsigned int i, unsigned int j, int player, unsigned short *transboard) {
	NODE *tmp = NULL;

	if ((*T)->leftchild == NULL) { //if father doesn't have children, I allocate it as a leftchild...
		if ((tmp = (NODE*)malloc(sizeof(NODE))) == NULL) return 0;
		else {
			tmp->i = i;
			tmp->j = j;
			tmp->depth = depth-1; //if you pass father's depth, child's is decreased by one
			(player == MAX) ? (tmp->value = 2) : (tmp->value = -2); 
			tmp->transboard = transboard;

			tmp->leftchild = NULL;
			tmp->rightbrothers = NULL;

			(*T)->leftchild = tmp; //eventually I save him as a leftchild
		}
	}

	else { //otherwise, if it has already a child...
		NODE *scorri = (*T)->leftchild;
		while (scorri->rightbrothers != NULL) //i allocate it after the last child in the list
			scorri = scorri->rightbrothers;

		if ((tmp = (NODE*)malloc(sizeof(NODE))) == NULL) return 0; 
		else {
			tmp->i = i;
			tmp->j = j;
			tmp->depth = depth-1;
			(player == MAX) ? (tmp->value = 2) : (tmp->value = -2);
			tmp->player = player;
			tmp->transboard = transboard;

			tmp->leftchild = NULL;
			tmp->rightbrothers = NULL;
		}
		scorri->rightbrothers = tmp; //eventually I update the pointer of the list's last node


	}

	return 1;

}


/*transforms BOARD's matrix of player.c into an array, it's necessary to organize better some functions in a single cycle*/
static unsigned short *Trans() {
	unsigned short *v;
	unsigned int s, t;

	if ((v = (unsigned short*)malloc(N*M*sizeof(unsigned short))) == NULL) return NULL;

	for (s = 0; s < M; s++) {
		for (t = 0; t < N; t++) {
			v[N*s + t] = BOARD[s][t]; //having a matrix A = (M X N) transformed into a B = (M*N x 1), the element (i,j) of A goes into (N*i+J,1) of B, it's a mathematical property.
		}
	}


	return v; //returns v
}





/*functions already present in the given prototype*/
static unsigned short **ushort_matrix_alloc(unsigned int row, unsigned int col) {
	unsigned short **m = NULL;
	unsigned int i;

	if ((m = (unsigned short **)calloc(row, sizeof(unsigned short *))) != NULL) {
		if ((m[0] = (unsigned short *)calloc(row*col, sizeof(unsigned short))) == NULL) {
			free(m);
			m = NULL;
		}
		else {
			for (i = 1; i<row; i++)
				m[i] = &m[i - 1][col];
		}
	}
	return m;
}


/*does the oppoisite of Trans(), returns a matrix M x N, needed to be evaluated by the functions check_win*/
static unsigned short **ReverseTrans(unsigned short *v) {
	unsigned int x;
	unsigned short **m;

	m = ushort_matrix_alloc(M, N);

	for (x = 0; x < M*N; x++)
		m[x / N][x - N*(x / N)] = v[x]; // x / N is an integer division, this is also a mathematical property

	return m;
}

/*functions already present in the given prototype*/
static void ushort_matrix_free(unsigned short **m) {
	free(m[0]);
	free(m);
}

/*functions already present in the given prototype*/
int setup_player(unsigned int m, unsigned int n, unsigned int k) {
	time_t seed = time(NULL);
	M = m;
	N = n;
	K = k;
	C = M*N;
	if ((BOARD = ushort_matrix_alloc(M, N)) == NULL) return 0;
	srand(seed);
	return 1;
}

/*functions already present in the given prototype*/
void free_player() {
	M = N = K = 0;
	ushort_matrix_free(BOARD);
	BOARD = NULL;
}

/*functions taken from board.c for convenience*/
static int check_win_row(unsigned int i, unsigned int j, int PLAYER, unsigned short **BOARD) {
	unsigned int k;
	if (j + K>N) return 0;
	for (k = 0; k<K; k++)
		if (BOARD[i][j + k] != PLAYER) return 0;
	return 1;
}

static int check_win_col(unsigned int i, unsigned int j, int PLAYER, unsigned short **BOARD) {
	unsigned int k;
	if (i + K>M) return 0;
	for (k = 0; k<K; k++)
		if (BOARD[i + k][j] != PLAYER) return 0;
	return 1;
}

static int check_win_diag(unsigned int i, unsigned int j, int PLAYER, unsigned short **BOARD) {
	unsigned int k;
	if (i + K>M || j + K>N) return 0;
	for (k = 0; k<K; k++)
		if (BOARD[i + k][j + k] != PLAYER) return 0;
	return 1;
}

static int check_win_antidiag(unsigned int i, unsigned int j, int PLAYER, unsigned short **BOARD) {
	unsigned int k;
	if (i + K>M || j<K - 1) return 0;
	for (k = 0; k<K; k++)
		if (BOARD[i + k][j - k] != PLAYER) return 0;
	return 1;
}

static int check_win(int PLAYER, unsigned short **BOARD) {
	unsigned int i, j;
	for (i = 0; i<M; i++)
		for (j = 0; j<N; j++) {
		if (check_win_row(i, j, PLAYER, BOARD))
			return 1;
		else if (check_win_col(i, j, PLAYER, BOARD))
			return 1;
		else if (check_win_diag(i, j, PLAYER, BOARD))
			return 1;
		else if (check_win_antidiag(i, j, PLAYER, BOARD))
			return 1;

		}
	return 0;
}



/*simply function that returns a copy of an array created dynamically with the library function memcpy*/
static unsigned short *copyv(unsigned short *v) {
	unsigned short *newv;
	if((newv = (unsigned short*)malloc(M*N*sizeof(unsigned short))) == NULL) exit(0);
	memcpy(newv, v, M*N*sizeof(unsigned short));
	return newv;
}

/*returns the state of a certain board, 1 in case of win of either player1 or player2, 0 in case of draw*/
static int GetWin(NODE *T) {

	return check_win(T->player == MAX ? 2 : 1, ReverseTrans(T->transboard));

}

/*alphabeta pruning,  player.c's core, accurate description is inside the paper*/
static int  alphabeta(NODE **T, int depth, int a, int b, int maximizingPlayer) {


	if (depth == 0 || GetWin(*T)) { //if leaf...

		((*T)->player == MAX) ? ((*T)->value = -GetWin(*T)) : ((*T)->value = GetWin(*T)); //I assign a value to the node, -1 if opponent's win, +1 if AI win
		return (*T)->value;
	}

	if (maximizingPlayer == MAX) { //only if minimizing node

		NODE *tmp;
		unsigned short *v = NULL;
		unsigned int x, count;


		if (!AllocateChildren(T, depth, 0, 0, MIN, NULL)) { //first I allocate 1 child without giving any value, I do this to keep everything in one for-cycle

			printf("\nError");
			exit(0);
		}

		tmp = (*T)->leftchild;
		v = copyv((*T)->transboard); //I take the node's board and...
		for (x = 0, count = 0; x < M*N; x++) { //...I check every possibile move from that certain board state



			if (v[x] != 0) //if it's already occupied it's not an acceptable move so I go on
				continue;

			else {
				v[x] = maximizingPlayer; //I temporarily make a move on the free box (then it'll become empty once again)
				if (count == 0) { //means that I'm working with the first child, hence its address comes from the father and not from the brothers
					tmp->i = x / N; //saving the current move in the node
					tmp->j = x - N*(x / N);
					tmp->transboard = copyv(v); //and also the board
					count++; //increasing count in order not to belong to this case (first child) again
				}

				else { //if I already handled the first child

					if (!AllocateChildren(T, depth, x / N, x - N*(x / N), MIN, copyv(v))) { //I allocate the space for the brothers and their current values (board, i,j etc)

						printf("\nError");
						exit(0);
					}

					tmp = tmp->rightbrothers;
					count++;
				}

				tmp->value = Max(tmp->value, alphabeta(&tmp, depth - 1, a, b, maximizingPlayer == 1 ? 2 : 1)); //recursive call that goes to the other part (minimizing part) of the function (if father is maximizing then the children are minimizing because between father and children the player changes) 

				a = Max(a, tmp->value); //a is the best possible move for player1, so I have to choose the one with highest value
				v[x] = 0;
				if (a >= b) //pruning: if I see that the best possible move for player 1 has the same value of the best possibile move of player 2 I don't need to waste time to find another move nor to allocate more brothers
					break;



			}


		}

		return a;
	}




	else if (maximizingPlayer == MIN) { //same as above, just we're working with minimizing nodes

		NODE *tmp;
		unsigned short *v = NULL;
		unsigned int x, count;


		if (!AllocateChildren(T, depth, 0, 0, MAX, NULL)) {

			printf("\nError");
			exit(0);
		}

		tmp = (*T)->leftchild;
		v = copyv((*T)->transboard);
		for (x = 0, count = 0; x < M*N; x++) { //for every possible move...



			if (v[x] != 0)
				continue;

			else {
				v[x] = maximizingPlayer;
				if (count == 0) {
					tmp->i = x / N;
					tmp->j = x - N*(x / N);
					tmp->transboard = copyv(v);
					count++;
				}

				else {

					if (!AllocateChildren(T, depth, x / N, x - N*(x / N), MAX, copyv(v))) {

						printf("\nError");
						exit(0);
					}

					tmp = tmp->rightbrothers;
					count++;
				}

				tmp->value = Min(tmp->value, alphabeta(&tmp, depth - 1, a, b, maximizingPlayer == 1 ? 2 : 1)); //again, here the recursive call inverts minimizing with maximizing

				b = Min(b, tmp->value); //b instead chooses the minimum value because it has to pick the move that helps player1 less (=best move for player2 = optimal strategy)
				v[x] = 0;
				if (a >= b) //as before
					break;



			}


		}

		return b;
	}


}

/*simply function that controls if a certain move is present among all the children of a certain node (since alphabeta doesn't allocate all the possible combination it's legit asking if a certain move is present or not)*/
static unsigned int check_ij(NODE *T, unsigned int i, unsigned int j) { //checks all the brothers of a certain father, T is the first brother
	NODE *tmp = T;
	while (tmp != NULL) {

		if (tmp->i == i && tmp->j == j)
			return 1; //found

		tmp = tmp->rightbrothers;
	}

	return 0;
}




/*"simply" function that allows, given a starting node, to destroy itself and all his children and grandsons... etc freeing the memory*/
static void DeleteTree(NODE **T){

	if ((*T) != NULL) { //if T is NULL I can't free anything
		NODE *tmp, *succ;
		
	
		for (tmp = (*T)->leftchild; tmp != NULL; ) { 
			succ = tmp->rightbrothers; 
			DeleteTree(&tmp); 
			tmp = succ; 
		}
		


		/*node's freeing and setting to NULL*/
		free(*T);
		(*T) = NULL;
		
	
	
	}
		
	return;
}

/*Adaption of CountLeaves suggested by prof Di Lena, in this case I just count the winning leaves for player1*/
static unsigned int CountWins(NODE *T) {
	if (T == NULL)
		return 0;

	else {
		unsigned int n = 0;
		if (T->leftchild == NULL) { //if it doesnt have children it's a leaf.
			if (check_win(1, ReverseTrans(T->transboard)) == 1) //I also check if it's a winning leaf
				n = (T->depth)*(T->depth); //n is a value that better reflects winning leaves after less moves to help the algorithm go towards a path statistically better (winning-wise)
		}								   //fluctuates as a parabola

		else {
			NODE *tmp;
			for (tmp = T->leftchild; tmp != NULL; tmp = tmp->rightbrothers) {
				n = n + CountWins(tmp); //recursive call that sums the obtained values
			}							//Anyways I should fina the perfect formula to determine statistically how much depth should weigh in the decision. As for now depth^2 works pretty well I must say
		}

		return n;


	}


}

/*A problem found in alphabeta pruning is that once I get a 0 value for a certain MAX node (guaranteed draw) if any of the next nodes has a value of -1 alphabeta allocates it as a 0 value node. Although these children don't really affect the true value of father's alphabeta (as max node if there's a 0, it can't get less) it can happen that I have to count the number of winnning leaves and naturally I can't choose a path that seems 0 (draw) while it is indeed -1 (lose) so that I developed this function that returns the TRUE value of a certain node that normally is allocated as 0 but in reality can also be -1*/
static int TrueValue(NODE *T) {
	NODE *tmpSubTree; //temporarily to this function
	int valore;
	if ((tmpSubTree = (NODE*)malloc(sizeof(NODE))) == NULL) exit(0);

	tmpSubTree->depth = T->depth;
	tmpSubTree->i = T->i;
	tmpSubTree->j = T->j;
	tmpSubTree->leftchild = NULL;
	tmpSubTree->rightbrothers = NULL;
	tmpSubTree->player = T->player;
	tmpSubTree->value == (T->player == MAX) ? (-2) : (2);
	tmpSubTree->transboard = copyv(T->transboard);


	valore = alphabeta(&tmpSubTree, tmpSubTree->depth, -1, 1, tmpSubTree->player); //simply I recreate a sub-tree with the technique used to create the whole tree so that I get the TRUE value and I store it in the real tree
	DeleteTree(&tmpSubTree); //eventually I free tmpSubTree

	return (valore); //returns the TRUE value of that child
}

/*Modified version of a function already present*/
int set_opponent_move(unsigned int i, unsigned int j) {
	if (i >= M || j >= N) return 0;


	BOARD[i][j] = 2; //= player2 move
	tmpI = i; //saving opponents move in two global variables
	tmpJ = j;

	if (GameTree != NULL) { //once done I have to free the memory of all the other possible moves (and their sub-trees) that he didn't choose but are still in the tree


		/*As said before the move may or may not be inside the tree*/
		
		if (check_ij(GameTree->leftchild, tmpI, tmpJ) == 1) { //iin case it is...
			
			/*Deletion of the part of tree that I don't need*/
			
			NODE *tmp = GameTree->leftchild, *prev = NULL;
			while(tmp != NULL) {
				if (i != tmp->i || j != tmp->j) { //if it isn't the move chosen by player 2...
					
					if (prev == NULL) { 
						GameTree->leftchild = tmp->rightbrothers; //moving the pointer from the first child to the following brother
						DeleteTree(&tmp); //freeing the subtree
						tmp = GameTree->leftchild; 
					}

					else { 
						prev->rightbrothers = tmp->rightbrothers;
						DeleteTree(&tmp);
						tmp = prev->rightbrothers; 
					}
				}

				else { //if it's the move chosen by player2...
					prev = tmp; 
					tmp = tmp->rightbrothers; 
				}
			}

			GameTree = GameTree->leftchild; //eventually I just change tree's root to the only child left (the one with the move chosen by player2, because all other have been deleted)
			
		}

		else { // IF there's already a gametree and the move isn't found, I assume the opponent didn't make the optimal move, good news then: I just create another subtree from this move hoping that it'll lead to the victory
			NODE *tmp = GameTree, *NewGameTree = NULL;

			DeleteTree(&tmp);//in this case I can just delete the whole tree and create a new one

			if ((NewGameTree = (NODE*)malloc(sizeof(NODE)) ) == NULL) { //allocation...
				printf("\nError");
				exit(0);
			}
			NewGameTree->depth = C - 1;
			NewGameTree->i = tmpI;
			NewGameTree->j = tmpJ;
			NewGameTree->player = MAX;
			NewGameTree->rightbrothers = NULL;
			NewGameTree->leftchild = NULL;
			NewGameTree->transboard = Trans();
			NewGameTree->value = alphabeta(&NewGameTree, NewGameTree->depth, -1, 1, NewGameTree->player); //...e and creation...
			GameTree = NewGameTree; //hence the new tree's root is player2 move
		}



	}
	C--;
	return 1;
}




static unsigned int firstmove() { //to avoid MANY calculations and to limit sensibly the number of allocated nodes I set a certain initial move that doesn't preclude any chance of winning and that doesn't necessarily lead me to a certain lose

	srand(time(NULL));

	
	if (M == 2 && N >= 4) 
		return N / 2;

	else if (M >= 4 && N == 2) 
		return N / 2;
	
	else if (M == 3 && N == 4 && K == 3) 
		return 5;

	else if (M == 4 && N == 3 && K == 3) 
		return 5;

	else //in every other case i set a starting move the first box because it helps considerably the algorithm alphabeta (that allocates the nodes sequentially), discarding immediately the first box I can save much memory focusing on the second box and so
		return 0;



}



static unsigned int secondmove() { //in bigger boards I need also a secondmove to be able to play
	
		if (M == 5 && N == 5) { //as the firstmove I have to avoid a certain lose

			if (BOARD[1][1] != 0)
				return 1;
			else if (BOARD[0][1] != 0)
				return N + 1;
			else if (BOARD[0][2] != 0)
				return N + 1;
			else if (BOARD[0][3] != 0)
				return N + 1;
			else if (BOARD[0][4] != 0)
				return N;
			else if (BOARD[1][2] != 0)
				return 2 * N;
			else if (BOARD[1][3] != 0)
				return N;
			else if (BOARD[2][1] != 0)
				return 2;
			else if (BOARD[2][2] != 0)
				return 2;
			else if (BOARD[2][4] != 0)
				return N;
			else
				return 1;
		}


		if (M == 6 && N == 4) { //as above
			if (BOARD[0][1] != 0)
				return N;
			else if (BOARD[0][2] != 0)
				return N;
			else if (BOARD[0][3] != 0)
				return N;
			else if (BOARD[1][2] != 0)
				return N;


			else 
				return 1;
			
		}
			 

		if (BOARD[0][1] == 0) //this for 4x4x4
			return 1;
		else
			return 2;

}








/*contains the call to alphabeta and the creation of the gametree*/
int get_next_move(unsigned int *i, unsigned int *j) {
		unsigned int x;

		/*Initially I consider the special cases*/
		if (C == M*N) {

			x = firstmove();
			*i = x / N;
			*j = x - N*(x / N);
			BOARD[*i][*j] = 1;


			C--;
			return 1;
		}

		if ((M == 4 && N == 4 && K == 4) && M*N - C == 2) { //4x4x4 and if M*N-C == 2 means that there have been only 2 total moves and this is currently the secondmove of player1

			x = secondmove();
			*i = x / N;
			*j = x - N*(x / N);
			BOARD[*i][*j] = 1;
			C--;
			return 1;
		}

		
		if ((M == 6 && N == 4 && K == 3) && M*N - C == 2) { //same with 6x4x3

			x = secondmove();
			*i = x / N;
			*j = x - N*(x / N);
			BOARD[*i][*j] = 1;
			C--;
			return 1;
		}
		
		if ((M == 5 && N == 5 && K == 3) && M*N - C == 2) { //same with 5x5x3

			x = secondmove();
			*i = x / N;
			*j = x - N*(x / N);
			BOARD[*i][*j] = 1;
			C--;
			return 1;
		}




		if (Z == 0) { 

			if ((GameTree = (NODE*)malloc(sizeof(NODE))) == NULL) { 
				printf("\nError");
				exit(0);
			}

			GameTree->depth = C;
			GameTree->i = tmpI;
			GameTree->j = tmpJ; 
			GameTree->player = MAX;
			GameTree->transboard = Trans();
			GameTree->leftchild = NULL;
			GameTree->rightbrothers = NULL;

			Z++;


			GameTree->value = alphabeta(&GameTree, GameTree->depth, -1, 1, GameTree->player); //I let alphabeta create all the game tree



	
			{ //right after being created
				NODE *scorri;


				if (GameTree->value == 1) { //if I fould the value to be 1 I can just follow the 1 path and it'll lead me to certain win
					for (scorri = GameTree->leftchild; scorri->value != GameTree->value; scorri = scorri->rightbrothers) //hence I'll just search among the children until I find the 1 value
						; 
					*i = scorri->i; 
					*j = scorri->j;
					C--; //decreasing the number of total moves left 
					BOARD[*i][*j] = 1; // updating BOARD...
				}
				
				
				else { //case value = 0 (it can't be -1 because since we're starting first there's no way, having the perfect strategy that we can lose, at least we can draw)
					NODE *tmp1;
					unsigned int maxwins = 0;
					for (tmp1 = GameTree->leftchild; tmp1 != NULL; tmp1 = tmp1->rightbrothers) { //among all children I check for the one with the highest win-coefficient (see explanation in CountWins), it'll be called MaxWins
						if (TrueValue(tmp1) == 0); //if value = 0(it can't be 1 otherwise Gametree's value wouldn't be 0) I also don't want to choose a losing path, even if it has the highest win coefficient
							maxwins = Max(maxwins, CountWins(tmp1)); 
					}


					if (maxwins != 0) { //se esistono vittorie possibili...
						for (tmp1 = GameTree->leftchild; maxwins != CountWins(tmp1) ||TrueValue(tmp1) != 0; tmp1 = tmp1->rightbrothers) 
							; 

					
					}

					else { //if instead there is no possible win (but I still can draw)
						for (tmp1 = GameTree->leftchild; TrueValue(tmp1) != 0; tmp1 = tmp1->rightbrothers)
							; //i just pick the first TrueValue = 0
					}
					*i = tmp1->i; //and i update the AI move
					*j = tmp1->j;
					C--;
					BOARD[*i][*j] = 1;
				
				}



				

			
		

				/*Deleting the part of tree that I won't use as before in set_opponent_move.*/

					NODE *canc = GameTree->leftchild, *prev = NULL;
					
					while (canc != NULL) {
						
						
						if (canc->i != *i || canc->j != *j) {
							if (prev == NULL) {
								GameTree->leftchild = canc->rightbrothers;
								DeleteTree(&canc);
								canc = GameTree->leftchild;
							}

							else {
								prev->rightbrothers = canc->rightbrothers;
								DeleteTree(&canc);
								canc = prev->rightbrothers;

							}
						}

						else {
							prev = canc;
							canc = canc->rightbrothers;
						}
					}

					GameTree = GameTree->leftchild;
				
				
			}
		}

		
		
		else { //If we already generated a GameTree
			NODE *tmp = GameTree;
			NODE *scorri;

		
			if (GameTree->value == 1) { //as before, if value = 1 then I can just pick the first child with value 1 and it'll lead me to certain win
				for (scorri = tmp->leftchild; scorri->value != 1; scorri = scorri->rightbrothers) {
					
				}
				/*Updating AI move*/
				*i = scorri->i;
				*j = scorri->j;
			}


			
			else if (GameTree->value == 0 && TrueValue(GameTree) == 1) { //it can happen that the original value of the gametree changes during the game (ex: non-optimal moves of the opponents) so that I always have to follow first a path that will lead to certain victory, if there is
				NODE *newGameTree, *scorri;
				
				
				if ((newGameTree = (NODE*)malloc(sizeof(NODE))) == NULL) exit(0); //creating a new winning gametree
				
				newGameTree->depth = GameTree->depth;
				newGameTree->i = GameTree->i;
				newGameTree->j = GameTree->j;
				newGameTree->leftchild = NULL;
				newGameTree->player = GameTree->player;
				newGameTree->transboard = copyv(GameTree->transboard);
				newGameTree->rightbrothers = NULL;

				DeleteTree(&GameTree);

				newGameTree->value = alphabeta(&newGameTree, newGameTree->depth, -1, 1, newGameTree->player); 
				GameTree = newGameTree;
				for (scorri = GameTree->leftchild; scorri->value != GameTree->value; scorri = scorri->rightbrothers) 
					;

				/*Updating the move*/
				*i = scorri->i;
				*j = scorri->j;

			}

				
			else { //as before if Gametree value = 0
					
				NODE *tmp;
				unsigned int maxwins = 0;

				for (tmp = GameTree->leftchild; tmp != NULL; tmp = tmp->rightbrothers) { //searching for the child with highest win-coefficient whose value isn't -1
					if (TrueValue(tmp) == 0)
						maxwins = Max(maxwins, CountWins(tmp));
				}



				if (maxwins != 0) { //if there are possible wins
					for (tmp = GameTree->leftchild; maxwins != CountWins(tmp) || TrueValue(tmp) != 0; tmp = tmp->rightbrothers)
						;
				}
				
				else { //if there aren't possible wins
					for (tmp = GameTree->leftchild; TrueValue(tmp) != 0; tmp = tmp->rightbrothers)
						; //i'll just take the first node with 0 value
				}
				/*updating AI move*/
				*i = tmp->i;
				*j = tmp->j;
				


			}

			/*deleting useless tree part*/
			
				NODE *canc = GameTree->leftchild, *prev = NULL;


				while (canc != NULL) {

					if (canc->i != *i || canc->j != *j) {
						if (prev == NULL) {
							GameTree->leftchild = canc->rightbrothers;
							DeleteTree(&canc);
							canc = GameTree->leftchild;
						}

						else {
							prev->rightbrothers = canc->rightbrothers;
							DeleteTree(&canc);
							canc = prev->rightbrothers;

						}
					}

					else {
						prev = canc;
						canc = canc->rightbrothers;
					}
				}

				GameTree = GameTree->leftchild; 
				
				
			C--;
			BOARD[*i][*j] = 1;
			



		}

		if (C == 0) //when we're finished I just free the whole memory used in the tree
			DeleteTree(&GameTree);

		return 1;
	}





