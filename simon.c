#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define NONE_LIT -1
#define RED      0
#define GREEN    1
#define YELLOW   2
#define BLUE     3
#define ALL      4

#define RED_COLOR    "\x1b[41m"
#define GREEN_COLOR  "\x1b[42m"
#define YELLOW_COLOR "\x1b[43m"
#define BLUE_COLOR   "\x1b[44m"
#define RESET        "\x1b[0m"

#define DISPLAY_SPEED 600000
#define REDRAW_ON_INPUT_SPEED 300000

#define INPUT_LEN 128

#define STARTING_SEQ_LEN 4
#define MAX_SEQ_LEN 16
#define BITS_PER_SEQ_ITEM 2
#define MAX_SHIFT 6

#define YOU_WON_CODE 0

// Display the game.
// which_lit - determines which light is lit when printing the board. -1 Means
// no lights and 4 means all lights.
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
    for(int i = -1; i <= 3; i++)
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

void add_move_seq_item(uint8_t *move_sequence, uint8_t virt_idx, uint8_t value)
{
    uint8_t idx = virt_idx / 4;
    uint8_t seq_item_idx = virt_idx % 4;
    move_sequence[idx] = move_sequence[idx] | (value << (MAX_SHIFT - seq_item_idx));
}

uint8_t get_move_seq_item(uint8_t *move_sequence, uint8_t virt_idx)
{
    uint8_t idx = virt_idx / 4;
    uint8_t seq_item_idx = virt_idx % 4;
    // Note: 192 is the number when the high 2 bits are both 1
    return (move_sequence[idx] & (192 >> seq_item_idx)) >> (MAX_SHIFT - seq_item_idx);
}

// Displays the sequence of moves
void display_sequence(uint8_t *move_sequence, int seq_len)
{
    for(int i = 0; i < seq_len; i++)
    {
        printSimon(get_move_seq_item(move_sequence, i));
        usleep(DISPLAY_SPEED);
        printSimon(NONE_LIT);
        usleep(200000);
    }
}

// Generates the sequence of moves
void generate_sequence(uint8_t *move_sequence, int seq_len, int start_idx)
{
    for(int i = start_idx; i < seq_len; i++)
    {
        add_move_seq_item(move_sequence, i, rand() % 4);
    }
}

// Play the game with the user
// Return 0 if user lost and 1 otherwise
int handle_input(uint8_t *move_sequence, char *input_buffer, int seq_len)
{
    int input;
    int i = 0;
    while(i < seq_len)
    {
        fgets(input_buffer, INPUT_LEN, stdin);
        if(sscanf(input_buffer, "%d", &input) && 1 <= input && input <= 4)
        {
            // Adjust input to match how the data is stored
            input--;
            if(input == get_move_seq_item(move_sequence, i))
            {
                printSimon(input);
                usleep(REDRAW_ON_INPUT_SPEED);
                printSimon(NONE_LIT);
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
int game_loop(uint8_t *move_sequence)
{
    char input_buffer[INPUT_LEN];
    int generator_idx = 0;
    int seq_len = STARTING_SEQ_LEN;
    for(;;)
    {
        if(seq_len > MAX_SEQ_LEN)
        {
            return YOU_WON_CODE;
        }

        generate_sequence(move_sequence, seq_len, generator_idx);
        display_sequence(move_sequence, seq_len);
        if(handle_input(move_sequence, input_buffer, seq_len))
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
                return 1;
            }
            generator_idx = 0;
            seq_len = STARTING_SEQ_LEN;
        }
    }
}

int main(int argc, char **argv)
{
    uint8_t move_sequence[MAX_SEQ_LEN / BITS_PER_SEQ_ITEM] = {0};
    setbuf(stdout, NULL);
    srand(time(NULL));

    do_opening();
    if(game_loop(move_sequence) == YOU_WON_CODE)
    {
        printf("You beat the game!!\n");
    }
    else
    {
        printf("Thanks for playing!!\n");
    }

    return 0;
}
