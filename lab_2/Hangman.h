/* Hangman.h
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 10 / Apr / 2017
 *
 * Header file for Hangman.c and play_game.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct HangmanInfo {
	int total_guesses;
	int wrong_guesses;
	int correct_guesses;
	int word_len;
	int unknowns_left;
	char *word;
	char *wrongs;
	char *corrects;
	char *current_state;
} HangmanInfo;

// use to free game after done
void free_game(struct HangmanInfo *game);

// initialize a game with given word and number of guesses
struct HangmanInfo *init_game(char *w, int guesses);

// get the current state with unguesses letters as -'s
char *get_state(struct HangmanInfo *game);

// get wrong guesses
char *get_wrongs(struct HangmanInfo *game);

// get number of wrong guesses
int get_wrong_count(struct HangmanInfo *game);

// get guesses left
int guesses_left(struct HangmanInfo *game);

// guess a letter
// 3 game is won!!
// 2 if already guessed (no penalty)
// 1 if correct guess
// 0 if wrong guess (penalty, but still have guesses left)
// -1 if game over
int guess(struct HangmanInfo *game, char g);
