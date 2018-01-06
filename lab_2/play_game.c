/* play_game.c
 * Group 15: Jacky Wang, Denis Jivaikin, Novin Changizi
 * 10 / Apr / 2017
 *
 * Hangman game interaction with LCD
 */

#include "master.h"
#include "Hangman.h"
#include <stdbool.h>

bool fl = true;

// Returns a word size and stores a random word from a dictionary into
// char* word input.
int pick_word(char* word) {
	FILE* f = fopen("words.txt", "r+");
	srand(time(NULL));
	int random_val = rand();
	int N = 998;
	int rand_capped = random_val % N;

	int i;
	for (i = 0; i < rand_capped; i++ ) {
		fgets(word, 12, f);
	}

	fclose(f);
	int word_size = size_str(word);
	word[word_size - 1] = '\0';
	return word_size;
}

// Updates the game's statistics
char *format_game(HangmanInfo *game) {
	char *game_state = get_state(game);
	int num_left = guesses_left(game);
	char *wrongs = get_wrongs(game);
	printf("%s", wrongs);
	char *formatted = (char *) malloc(32 * sizeof(char));
	int i = 0;
	while (i < strlen(game_state)) {
		formatted[i] = game_state[i];
		i++;
	}

	while (i < 14) {
		formatted[i] = ' ';
		i++;
	}
	char s[2];
	sprintf(s, "%d", num_left);
	while (i - 14 < strlen(s)) {
		formatted[i] = s[i - 14];
		i++;
	}
	while (i < 16) {
		formatted[i] = ' ';
		i++;
	}
	int init = i;
	while (i < 32) {
		formatted[i] = wrongs[i - init];
		i++;
	}
	return formatted;
}

int main() {
	HangmanInfo* game;
	int hm_guesses;
	char* random_word;
	int guess_feedback;
	while (1) {
		random_word = (char*) malloc(sizeof(char) * 12);
		int word_size = pick_word(random_word);
		write_fifo("    Welcome!    Hangman the game\n");
		usleep(3000000);
		write_fifo("Pick amount of  guesses.(16 max)\n");
		scanf("%d",&hm_guesses);
		if (hm_guesses > 16) {
			write_fifo("Max guesses set to 16.\n");
			hm_guesses = 16;
		} else if (hm_guesses < word_size) {
			hm_guesses = word_size;
		}
		game = init_game(random_word, hm_guesses);
		usleep(100000);
		write_fifo("                                \n");
		usleep(500000);
		// display game init
		char *state = format_game(game);
		write_fifo(state);
		bool game_done = false;
		char ui;
		while (!game_done) {
			// display game progress
			fprintf(stdout, "\nEnter character: ");
			fflush(stdout);
			if (fl) {
				flush_stdin();
				fl = false;
			}
			ui = getchar();
			flush_stdin();
			guess_feedback = guess(game, ui);
			if (guess_feedback == -1) {
				game_done = true;
				usleep(2500000);
				write_fifo("YOU DEFEATED\n");
				usleep(2000000);
			} else if(guess_feedback == 3) {
				game_done = true;
				write_fifo("VICTORY ACHIEVED\n");
				usleep(2000000);
			} else if (guess_feedback == 0) {
				printf("wrong guess\n");
				state = format_game(game);
				write_fifo(state);
				usleep(500000);
			} else if (guess_feedback == 1){
				printf("correct guess!\n");
				state = format_game(game);
				write_fifo(state);
				usleep(500000);
			} else if (guess_feedback == 2) {
				printf("already guessed\n");
				write_fifo(state);
			}
		}
		//Repeat game
		free(random_word);
		free_game(game);
		write_fifo("PLAY AGAIN?(Y/N)\n");
		ui = getchar();
		flush_stdin();
		if (ui == 'N') {
			write_fifo("Goodbye!\n");
			break;
		}
	}
	// Close the pipe
	unlink(myfifo);
	return 0;
}
