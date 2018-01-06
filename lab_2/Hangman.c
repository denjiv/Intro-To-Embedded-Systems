/* Hangman.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 10 / Apr / 2017
 *
 * Hangman game algorithm
 */

#include "Hangman.h"

// hangman game logic

void free_game(struct HangmanInfo *game) {
	free(game->wrongs);
	free(game->corrects);
	free(game->current_state);
	free(game);
}

struct HangmanInfo *init_game(char *w, int guesses) {
	struct HangmanInfo *hm = (struct HangmanInfo *) malloc(sizeof(struct HangmanInfo));
	hm->total_guesses = guesses;
	hm->wrong_guesses = 0;
	hm->correct_guesses = 0;
	hm->word_len = strlen(w);
	hm->unknowns_left = hm->word_len;
	hm->word = w;
	hm->wrongs = (char *) malloc(hm->total_guesses * sizeof(char));
	hm->wrongs[0] = '\0';
	hm->corrects = (char *) malloc(30 * sizeof(char));
	hm->current_state = (char *) malloc((hm->word_len+1) * sizeof(char));
	for (int i = 0; i < hm->word_len; i++) {
		hm->current_state[i] = '_';
	}
	hm->current_state[hm->word_len] = '\0';
	return hm;
}

char *get_state(struct HangmanInfo *game) {
	return game->current_state;
}

char *get_wrongs(struct HangmanInfo *game) {
	return game->wrongs;
}

int get_wrong_count(struct HangmanInfo *game) {
	return game->wrong_guesses;
}

int guesses_left(struct HangmanInfo *game) {
	return game->total_guesses - game->wrong_guesses;
}

// 1 if is in
// 0 otherwise
int is_in(char g, char *arr, int len) {
	int i = 0;
	while (i < len) {
		if (g == arr[i]) {
			return 1;
		}
		i++;
	}
	return 0;
}


int guess(struct HangmanInfo *game, char g) {
	//check if repeat
	if (is_in(g, game->wrongs, game->wrong_guesses) || is_in(g, game->corrects, game->correct_guesses)) {
		return 2;
	}
	int i = 0;
	int num_hits = 0;
	while (i < game->word_len) {
		char curr = game->word[i];
		if (g == curr) {
			game->current_state[i] = g;
			num_hits++;
		}
		i++;
	}

	if (num_hits > 0) {
		// correct guess
		game->unknowns_left -= num_hits;
		game->corrects[game->correct_guesses++] = g;
		// update screen with g where hits is 1
		if (game->unknowns_left == 0) {
			return 3;
		} else {
			return 1;
		}
	} else {
		game->wrongs[game->wrong_guesses++] = g;
		game->wrongs[game->wrong_guesses] = '\0';
		// add g to the screen where we display wrong guesses
		if (game->wrong_guesses >= game->total_guesses) {
			// game over!!!
			return -1;
		} else {
			return 0;
		}
	}
}

// example usage
/*
int main() {
  struct HangmanInfo *game = init_game("hello", 3);
  printf("%s\n", get_state(game));
  int n = guess(game, 'l');
  printf("correct %d   %s\n", n, get_state(game));
  n = guess(game, 'a');
  printf("wrong %d   %s\n", n, get_state(game));
  n = guess(game, 'h');
  printf("correct %d   %s\n", n, get_state(game));
  n = guess(game, 'e');
  printf("correct %d   %s\n", n, get_state(game));
  //n = guess(game, 'o');
  //printf("win! %d   %s\n", n, get_state(game));
  n = guess(game, 'x');
  printf("wrong %d   %s\n", n, get_state(game));
  n = guess(game, 'x');
  printf("repeat wrong %d   %s\n", n, get_state(game));
  n = guess(game, 'l');
  printf("repeat correct %d   %s\n", n, get_state(game));
  n = guess(game, 'z');
  printf("game over %d   %s\n", n, get_state(game));
  free_game(game);
}*/
