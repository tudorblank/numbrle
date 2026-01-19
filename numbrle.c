#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/random.h>

#define RED     "\033[31m"
#define YELLOW  "\033[93m"
#define GREEN   "\033[32m"
#define BLUE    "\033[38;5;159m"
#define GRAY    "\033[90m"
#define RESET   "\033[0m"

int digitCheck(char n) {
    return (n >= '0' && n <= '9');
}

int* processInput(int attempt) {
    printf(BLUE "Attempt %d: " RESET, attempt);
    int* v = NULL;
    while(1){
        //input (buffer = string)
        char* buffer = NULL;
        size_t bufsize = 0;
        ssize_t n = getline(&buffer, &bufsize, stdin);

        //check memory error
        if (n <= 0 || buffer[0] == '\n') {
            free(buffer);
            buffer = NULL;
            continue;
        }
        //from buffer (string) to v (integer array)
        v = malloc(n * sizeof(int));
        int index = 0;

        for(int i = 0; i < n; i++) {
            if(digitCheck(buffer[i])) {
                int j = i;
                while(j < n && digitCheck(buffer[j])) j++;

                int nr = 0;
                for(int k = i; k < j; k++)
                    nr = nr * 10 + (buffer[k] - '0');

                v[index++] = nr;
                i = j - 1;
            }
        }
        int* temp = realloc(v, index * sizeof(int));
        if(!temp){
            free(buffer);
            buffer = NULL;
            free(v);
            v = NULL;
            continue;
        }
        v = temp;
        free(buffer);
        buffer = NULL;

        //check correct size = 5 ###
        if(index != 5){
            printf(RED "\nERROR. Insert exactly 5 digits.\n\n" RESET);
            printf(BLUE "Attempt %d: " RESET, attempt);
            free(v);
            v = NULL;
            continue;
        }
        
        //check digit validity ###
        int error = 0;
        for(int i = 0; i < 5; i++){
            if(v[i] < 0 || v[i] > 9){
                error = 1;
                break;
            }
        }
        if(error){
            printf(RED "\nERROR. Only NATURAL SINGLE digits allowed (0 ~ 9).\n\n" RESET);
            printf(BLUE "Attempt %d: " RESET, attempt);
            free(v);
            v = NULL;
            continue;    
        }
        break; // input is valid
    }
    return v;
}

void unique(void){
    //generate 5 unique random digits (0~9)
    unsigned int random[5] = {10, 10, 10, 10, 10};
    for(int i = 0; i < 5; i++){
        int validRand = 0;

        while(!validRand){
            validRand = 1;

            unsigned int x;
            getrandom(&x, sizeof(x), 0);
            int c = x % 10;

            for(int j = 0; j < i; j++){
                if(random[j] == c)
                    validRand = 0;
            }

            if(validRand)
                random[i] = c;
        }
    }

    //start program
    printf(YELLOW "Insert 5 unique positive single digits.\nOnly 6 attempts available.\n\n" RESET);
    int done = 0;
    unsigned int attempt = 0;

    while(!done && attempt < 6){
        attempt++;
        
        //insert digits
        int* input = processInput(attempt);
        
        //check unique
        int unique = 1;
        for(int i = 0; i < 4; i++){
            for(int j = i + 1; j < 5; j++){
                if(input[i] == input[j]){
                    unique = 0;
                    break;
                }
            }
            if(!unique) break;
        }
        if(!unique){
            printf(RED "\nERROR. Only UNIQUE digits allowed.\n\n" RESET);
            free(input);
            input = NULL;
            attempt--;
            done = 0;
            continue;
        }

        //check state
        int state[5]; //1 = red; 2 = yellow; 3 = green

        for(int i = 0; i < 5; i++){
            int exists = 0;

            for(int j = 0; j < 5; j++){
                if(input[i] == random[j]){
                    exists = 1;
                    break;
                }
            }

            //assign state
            if(input[i] == random[i])
                state[i] = 3; //green
            else if(exists)
                state[i] = 2; //yellow
            else state[i] = 1; //red
        }

        //print state
        printf("\n");
        for(int i = 0; i < 5; i++){
            if(state[i] == 3) printf(GREEN "%d " RESET, input[i]);
            else if(state[i] == 2) printf(YELLOW "%d " RESET, input[i]);
            else printf(RED "%d " RESET, input[i]);
        }
        free(input);
        printf("\n\n");

        //guessed?
        if(state[0] == 3 && state[1] == 3 && state[2] == 3 && state[3] == 3 && state[4] == 3){
            done = 1;
            break;
        }
    }

    if(done) printf(GREEN "GUESSED" RESET);
    else printf(RED "OUT OF MOVES" RESET);
}

void nonUnique(void){
    //generate 5 non-unique random digits (0~9)
    unsigned int random[5];
    for(int i = 0; i < 5; i++){
        unsigned int x;
        getrandom(&x, sizeof(x), 0);
        random[i] = x % 10;
    }

    //start program
    printf(YELLOW "Insert 5 positive single digits.\nOnly 6 attempts available.\n\n" RESET);
    int done = 0;
    int attempt = 0;
    while(!done && attempt < 6){
        attempt++;

        //frequency of random
        int freqRand[10] = {0};
        for(int i = 0; i < 5; i++){
            freqRand[random[i]]++;
        }
        
        //insert digits
        int* input = processInput(attempt);

        //check state
        int state[5]; //1 = red; 2 = yellow; 3 = green

        //check greens first, rest are unassigned
        for(int i = 0; i < 5; i++){
            if(input[i] == random[i]){
                state[i] = 3; //green
                freqRand[input[i]]--;
            }
            else state[i] = 0; //unassigned
        }

        //assign yellow and red
        for(int i = 0; i < 5; i++){
            if(state[i] == 0){
                if(freqRand[input[i]] > 0){
                    state[i] = 2;
                    freqRand[input[i]]--;
                }
                else state[i] = 1;
            }
        }

        //print state
        printf("\n");
        for(int i = 0; i < 5; i++){
            if(state[i] == 3) printf(GREEN "%d " RESET, input[i]);
            else if(state[i] == 2) printf(YELLOW "%d " RESET, input[i]);
            else printf(RED "%d " RESET, input[i]);
        }
        free(input);
        printf("\n\n");

        //guessed?
        if(state[0] == 3 && state[1] == 3 && state[2] == 3 && state[3] == 3 && state[4] == 3){
            done = 1;
            break;
        }
    }

    if(done) printf(GREEN "GUESSED" RESET);
    else printf(RED "OUT OF MOVES" RESET);
}

int main(void) {
    int done = 0;

    while(!done){
        printf(GREEN "NUMBRLE by Tudor Blank" RESET BLUE "\n\nSelect game mode:" RESET "\n\n1) Unique " GRAY "[Easy]\n" RESET "2) Non-unique " GRAY "[Classic]\n" RESET "3) Exit\n\n");
    
        //select option
        int option = 0;
        scanf("%d", &option);
        
        //check input validity
        while(option < 1 || option > 3){
            printf("\n" RED "ERROR. Select between options 1, 2 or 3" RESET "\n\n1) Unique " GRAY "[Easy]\n" RESET "2) Non-unique " GRAY "[Classic]\n" RESET "3) Exit\n\n");
            
            scanf("%d", &option);
        }

        //run option
        printf("\n");
        if(option == 1) unique();
        else if(option == 2) nonUnique();
        else break;

        //end. retry?
        char retry;
        printf("\n\n" YELLOW "Program finished. Try again? Y/N\n" RESET);
        scanf(" %c", &retry);
        
        //check retry validity
        while(retry != 'y' && retry != 'Y' && retry != 'n' && retry != 'N'){
            printf("\n" RED "ERROR. Select a valid option" RESET "\n\nTry again? Y/N\n");
            scanf(" %c", &retry);
        }

        //run retry option
        printf("\n");
        if(retry == 'n' || retry == 'N') done = 1;
        else done = 0;
    }
    
    return 0;
}