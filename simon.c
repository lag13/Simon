#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define RED    1
#define GREEN  2
#define YELLOW 3
#define BLUE   4
#define ALL    5

#define RED_COLOR    "\x1b[41m"
#define GREEN_COLOR  "\x1b[42m"
#define YELLOW_COLOR "\x1b[43m"
#define BLUE_COLOR   "\x1b[44m"
#define RESET        "\x1b[0m"

#define DISPLAY_SPEED 600000
#define REDRAW_ON_INPUT_SPEED 300000

#define INPUT_LEN 128

#define STARTING_SEQ_LEN 4
int sequence[128];

// Display the game.
// which_lit - determines which light is lit when printing the board. 0 Means
// no lights and 5 means all lights.
void printSimon(char which_lit)
{
    char *red = "";
    char *green = "";
    char *yellow = "";
    char *blue = "";
    printf("\x1b[H\x1b[J");
    switch(which_lit)
    {
        case RED:
            red = RED_COLOR;
            break;
        case GREEN:
            green = GREEN_COLOR;
            break;
        case YELLOW:
            yellow = YELLOW_COLOR;
            break;
        case BLUE:
            blue = BLUE_COLOR;
            break;
        case ALL:
            red = RED_COLOR;
            green = GREEN_COLOR;
            yellow = YELLOW_COLOR;
            blue = BLUE_COLOR;
            break;
    }
    printf("%s1%s%s2%s%s3%s%s4%s\n", red, RESET, green, RESET, yellow, RESET, blue, RESET);
}

// An opening before simon begins
void do_opening()
{
    for(int i = 0; i <= 4; i++)
    {
        printSimon(i);
        usleep(DISPLAY_SPEED);
    }
    for(int i = 0; i < 2; i++)
    {
        printSimon(ALL);
        usleep(150000);
        printSimon(0);
        usleep(150000);
    }
}

// Displays the sequence of moves
void display_sequence(int *sequence, int seq_len)
{
    for(int i = 0; i < seq_len; i++)
    {
        printSimon(sequence[i]);
        usleep(DISPLAY_SPEED);
        printSimon(0);
        usleep(200000);
    }
}

// Generates the sequence of moves
void generate_sequence(int *sequence, int seq_len, int start_idx)
{
    for(int i = start_idx; i < seq_len; i++)
    {
        sequence[i] = rand() % 4 + 1;
    }
}

// Play the game with the user
// Return 0 if user lost and 1 otherwise
int handle_input(char *input_buffer, int seq_len)
{
    int input;
    int i = 0;
    while(i < seq_len)
    {
        fgets(input_buffer, INPUT_LEN, stdin);
        if(sscanf(input_buffer, "%d", &input) && 1 <= input && input <= 4)
        {
            if(input == sequence[i])
            {
                printSimon(input);
                usleep(REDRAW_ON_INPUT_SPEED);
                printSimon(0);
                i++;
            }
            else
            {
                return 0;
            }
        }
    }
    usleep(500000);
    return 1;
}

// The main game loop
int game_loop()
{
    char input_buffer[INPUT_LEN];
    int generator_idx = 0;
    int seq_len = STARTING_SEQ_LEN;
    for(;;)
    {
        generate_sequence(sequence, seq_len, generator_idx);
        display_sequence(sequence, seq_len);
        if(handle_input(input_buffer, seq_len))
        {
            generator_idx = seq_len;
            seq_len += 1;
        }
        else
        {
            char keep_playing;
            printf("Sorry that is wrong. Would you like to play again? [y/n] ");
            fgets(input_buffer, INPUT_LEN, stdin);
            sscanf(input_buffer, "%c", &keep_playing);
            if(keep_playing != 'y')
            {
                return 0;
            }
            generator_idx = 0;
            seq_len = STARTING_SEQ_LEN;
        }
    }
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL);
    srand(time(NULL));
    do_opening();
    game_loop();
    printf("Thanks for playing!!\n");
    return 0;
}
