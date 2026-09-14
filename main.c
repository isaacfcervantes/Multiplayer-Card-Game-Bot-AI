// Wyatt Thomas and Isaac Cervantes
// 5/7/25
// This program simulates a game of Uno with some changed rules. It had dynamic memory allocation and has a customizable amount of users.
// It also prompts the user if they want to play a new game at the end.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// create the structs defined in the project rules
typedef struct card_t {
    char name; // '0'–'9' for number cards, 'A' for AND, 'O' for OR, 'N' for NOT, 'R' for Reverse
    char color; // 'R' for Red, 'Y' for Yellow, 'G' for Green, 'B' for Blue, 'S' for special cards
} card;
    
typedef struct player_t {
    char playerName[20];
    card* deck; // dynamic array (or linked list) of cards
    int decksize; // how many cards the player currently holds
} player;

// define each function because some functions use other ones
void printCard(card c);
int player_turn(int numPlayers,int turn);
void initializeDeck(card deck[], int deckSize);
void shuffleDeck(card deck[]);
void draw_card(player* p1, card** deck,int* num_deck);
int isValidCard (card topOfPile, card candidate);
int isScard(card given_card);
int comp_card(card temp1, card temp2);
void and_played(player* currentPlayer,player* nextPlayer, card* topOfPile,int index,player p[], int size, int* order,  card* deck,int* num_deck);
void or_played(player* currentPlayer,player* nextPlayer, card* topOfPile,int index,player p[], int size, int* order,  card* deck,int* num_deck);
int handleAND(player* currentPlayer, card* topOfPile, int user_choice);
int handleOR(player* currentPlayer, card* topOfPile, int user_choice);
void handleReverse(int reverseFlag, int *currentPlayerindex, int playerCount, player players[], int userInput);
int handleNot(int currentPlayerIndex,int playerCount);
void printPlayerHand(player p);
void print_sit(player p[],int size,card top);
void runTurn(player *p, card *topOfDeck, card *deck, int *deck_size, player players[], int numberPlayers, int *current_turn);
void remove_card(player* p1, int card_num);
void checkWinner(int deck_size, bool* win_check,player players[], int numPlayers);

//prints the card given
void printCard(card c){
    if (c.name == 'R') {
        printf("Reverse\n");
    }
    else if (c.name == 'A') {
        printf("AND\n");
    }
    else if (c.name == 'O') {
        printf("OR\n");
    }
    else if (c.name == 'N') {
        printf("NOT\n");
    }
    else {
        switch (c.color) {
            case 'R':
                printf("Red %c\n", c.name);
                break;
            case 'Y':
                printf("Yellow %c\n", c.name);
                break;
            case 'B':
                printf("Blue %c\n", c.name);
                break;
            case 'G':
                printf("Green %c\n", c.name);
                break;
            default:
                printf("Card error.\n");
        }
    }
    
}

// updates the player's turn and also resets it once it looks through all the players
int player_turn(int numPlayers,int turn){
    int num = 0;
    turn++;
    if(turn > numPlayers){
        turn = 0;
    }
    return turn;
}

// creates the deck with the proper amount of cards for each color, number, and specials
void initializeDeck(card deck[], int deckSize) {
    for(int i = 0;i < deckSize; i++) {
        if(i < 20){
            deck[i].name = i % 10 + '0';
            deck[i].color = 'R';
        }
        else if(i < 40){
            deck[i].name = i % 10 + '0';
            deck[i].color = 'Y';

        }
        else if(i < 60){
            deck[i].name = i % 10 + '0';
            deck[i].color = 'G';

        }
        else if(i < 80){
            deck[i].name = i % 10 + '0';
            deck[i].color = 'B';
        }
        else{
            deck[i].color = 'S';
            if(i < 85){
                deck[i].name = 'A';
            }
            else if(i < 90){
                deck[i].name = 'O';
            }
            else if(i < 95){
                deck[i].name = 'N';
            }
            else if(i < 100){
                deck[i].name = 'R';
            }
        }
        
    }
}

// shuffles the deck 10000 times for each of the 100 cards
void shuffleDeck(card deck[]) {
    
    int randomNumOne, randomNumTwo;

    card tempOne, tempTwo;

    // pick a random card, and switch it with the current one
    for (int j = 0; j < 10000; ++j) {
        for (int i = 0; i < 100; ++i) {
            randomNumOne = i;
            randomNumTwo = (rand() % 100) + 0;
    
            tempOne = deck[randomNumOne];
            tempTwo = deck[randomNumTwo];
    
            deck[randomNumOne] = tempTwo;
            deck[randomNumTwo] = tempOne;
        }
    }
}

// wyatt will explain this one
void draw_card(player* p1, card** deck_big, int* num_deck){
    p1->deck = realloc(p1->deck, (p1->decksize + 1) * sizeof(card));
    while(p1->deck==NULL){
        p1->deck = realloc(p1->deck, (p1->decksize + 1) * sizeof(card));
    }
    p1->deck[p1->decksize] = *deck_big[0];
    p1->decksize++;
    for(int i=1;i<*num_deck;i++){
        (*deck_big)[i-1]=(*deck_big)[i];
    }
    (*num_deck)--;
    *deck_big = realloc(*deck_big,sizeof(card)*(*num_deck));
    while(p1->deck==NULL){
        *deck_big = realloc(*deck_big,sizeof(card)*(*num_deck));
    }
} 

// wyatt will explain this one
void remove_card(player* p1, int card_num){
    for(int i=card_num;i<p1->decksize;i++){
        p1->deck[i]=p1->deck[i+1];
    }
    p1->deck=realloc(p1->deck,(p1->decksize-1)*sizeof(card));
    p1->decksize--;
}

// checks if card can be played based on the card at the top of the pile
int isValidCard (card topOfPile, card candidate) {
    if (candidate.color == 'S') {
        return 1;
    }
    else if (candidate.name == topOfPile.name) {
        return 1;
    }
    else if (candidate.color == topOfPile.color) {
        return 1;
    }
    else if (topOfPile.name == 'R') {
        return 1;
    }
    else if (topOfPile.name == 'N') {
        return 1;
    }
    else {
        return 0;
    }
}


int isScard(card given_card){
    if(given_card.color=='S'){
        return 0;
    }
    else{
        return 1;
    }
}

int comp_card(card temp1, card temp2){
    if(temp1.color==temp2.color&&temp1.name==temp2.name){
        return 1;
    }
    else{
        return 0;
    }

}

//WYATT WRITE A COMMENT YOU DUMB FUCK
void and_played(player* currentPlayer, player* nextPlayer, card* topOfPile,int index,player p[], int size, int* order,  card* deck,int* num_deck){
    int user_choice=0;
    int card_bool=0;
    while(card_bool == 0){
        printf("Choose the card to play with AND from ");
        for(int i = 0; i < currentPlayer->decksize; i++){
            if(i != index){
            printf("%d", i);
            }
            if(i + 1 != currentPlayer->decksize && i!=index){
                printf(", ");
            }

        }
        printf(": ");
        scanf("%d", &user_choice);
        card_bool = isScard((currentPlayer->deck[user_choice]));
        if(card_bool == 0 || user_choice == index){
            printf("Cannot play two special cards in one turn or cannot play AND again.\n");
        }
    }
    *topOfPile=currentPlayer->deck[user_choice];
    card second_card;
    second_card=currentPlayer->deck[user_choice];
    print_sit(p, size, *topOfPile);
    printf("with AND: ");
    card_bool=0;
    bool endAndDraw = true;
    for (int i = 0; i < nextPlayer->decksize; ++i) {
        if (handleAND(nextPlayer,topOfPile, i) == 1) {
            endAndDraw = false;
        }
    }

    if (endAndDraw == true) {
        printf("%s has no card that matches.\n",nextPlayer->playerName);
        printCard(second_card);
        draw_card(nextPlayer,&deck,num_deck);
        draw_card(nextPlayer,&deck,num_deck);
        draw_card(nextPlayer,&deck,num_deck);
        draw_card(nextPlayer,&deck,num_deck);
    }
    else {
        *order = player_turn(size,*order);
        printf("%s, enter which card to play from 0-%d: ", nextPlayer->playerName, nextPlayer->decksize - 1);
        scanf("%d",&user_choice);
        while(handleOR(nextPlayer,topOfPile,user_choice) != 1){
            printf("Incorrect Choice\n");
            printf("%s, enter which card to play from 0-%d: ", nextPlayer->playerName, nextPlayer->decksize - 1);
            scanf("%d",&user_choice);
        }
        printf("Card Matches, no AND penalty.\n");
        *topOfPile=nextPlayer->deck[user_choice];
        remove_card(nextPlayer,user_choice);
    
    }
}

//WYATT WRITE A COMMENT YOU DUMB FUCK
void or_played(player* currentPlayer,player* nextPlayer, card* topOfPile,int index,player p[], int size, int* order,  card* deck,int* num_deck){
    int user_choice=0;
    int card_bool=0;
    while(card_bool == 0){
        printf("Choose the card to play with OR from ");
        for(int i = 0; i < currentPlayer->decksize; i++){
            if(i != index){
            printf("%d", i);
            }
            if(i + 1 != currentPlayer->decksize && i!=index){
                printf(", ");
            }
            else{
             printf(": ");
            }
        }
        scanf("%d", &user_choice);
        card_bool = isScard((currentPlayer->deck[user_choice]));
        if(card_bool == 0 || user_choice == index){
            printf("Cannot play two special cards in one turn or cannot play AND again.\n");
        }
    }
    *topOfPile=currentPlayer->deck[user_choice];
    card second_card;
    second_card=currentPlayer->deck[user_choice];
    print_sit(p, size, *topOfPile);
    printf("with OR: ");

    card_bool=0;
    bool endAndDraw = true;
    for (int i = 0; i < nextPlayer->decksize; ++i) {
        if (handleOR(nextPlayer,topOfPile,i) == 1) {
            endAndDraw = false;
        }
    }

    if (endAndDraw == true) {
        printf("%s has no card that matches.\n",nextPlayer->playerName);
        printCard(second_card);
        draw_card(nextPlayer,&deck,num_deck);
        draw_card(nextPlayer,&deck,num_deck);
        draw_card(nextPlayer,&deck,num_deck);
        draw_card(nextPlayer,&deck,num_deck);
    }
    else {
        *order = player_turn(size,*order);
        printf("%s, enter which card to play from 0-%d: ", nextPlayer->playerName, nextPlayer->decksize - 1);
        scanf("%d",&user_choice);
        while(handleOR(nextPlayer,topOfPile,user_choice) != 1){
            printf("Incorrect Choice\n");
            printf("%s, enter which card to play from 0-%d: ", nextPlayer->playerName, nextPlayer->decksize - 1);
            scanf("%d",&user_choice);
        }
        printf("Card Matches, no OR penalty.\n");
        *topOfPile=nextPlayer->deck[user_choice];
        remove_card(nextPlayer,user_choice);
    
    }

}

int handleAND(player* currentPlayer, card* topOfPile, int user_choice) {
    int card_bool = 0;
    card cur_card = *topOfPile;
    if (cur_card.name == currentPlayer->deck[user_choice].name && cur_card.color == currentPlayer->deck[user_choice].color) {
        return 1;
    }
    else{
        return 0;
    }

}

int handleOR(player* currentPlayer, card* topOfPile, int user_choice) {
    int card_bool = 0;
    card cur_card = *topOfPile;
    if (cur_card.name == currentPlayer->deck[user_choice].name || cur_card.color == currentPlayer->deck[user_choice].color) {
        return 1;
    }
    else{
        return 0;
    }

}
int handleNot(int currentPlayerIndex,int playerCount){
    if(currentPlayerIndex=playerCount-1){
        currentPlayerIndex=0;
    }
    else{
        currentPlayerIndex++;
    }
    return currentPlayerIndex;
}
void handleReverse(int reverseFlag, int *currentPlayerindex, int playerCount, player players[], int userInput) {
    if (playerCount == 2) {
        if(*currentPlayerindex = playerCount - 1){
            *currentPlayerindex = 0;
        }
        else {
            (*currentPlayerindex)++;
        }
    }
    else {
        player tempPlayer = players[*currentPlayerindex];
        player tempPlayerTwo;
        int firstIndex = 0;
        int last = playerCount - 1;
    
        while (firstIndex < last) {
            tempPlayerTwo= players[firstIndex];
            players[firstIndex] = players[last];
            players[last] = tempPlayerTwo;
    
            firstIndex++;
            last--;
        }

        for (int i = 0; i < playerCount; ++i) {
            if (players[i].playerName == tempPlayer.playerName) {
                *currentPlayerindex = i;
            }
        }
    }
    void remove_card(&players[*currentPlayerindex], userInput); 
}

void printPlayerHand(player p) {
    printf("%s's hand:\n\n", p.playerName);
    for (int i = 0; i < p.decksize; ++i) {
        card card = p.deck[i];
        printCard(card);
    }
}

void print_sit(player p[],int size,card top){
    for(int i = 0; i < size; i++){
        printPlayerHand(p[i]);
        printf("\n\n");
    }
    if((&top) != NULL){
        printf("Top of card pile is: ");
        printCard(top);
    }
    else{
        printf("Card pile is empty.\n\n");
    }


}

void runTurn(player *p, card *topOfDeck, card *deck, int *deck_size, player players[], int numberPlayers, int *current_turn) {
    int userInput = 100;
    bool endAndDraw = true;

    for (int i = 0; i < p->decksize; ++i) {
        if (isValidCard(*topOfDeck, p->deck[i]) == 1) {
            endAndDraw = false;
        }
    }

    if (endAndDraw == true) {
        draw_card(p, &deck, deck_size);
    }
    else {
        while (userInput > p->decksize - 1 || userInput < 0) {
            printf("%s, enter which card to play from 0 to %d: ", p->playerName, p->decksize - 1);
            scanf("%d", &userInput);
            if (userInput > p->decksize - 1) {
                printf("Invalid choice, %s does not have %d cards.\n", p->playerName, userInput);
            }
        }
    
    
        if (p->deck[userInput].color == 'S') {
            topOfDeck->name = p->deck[userInput].name;
            topOfDeck->color = p->deck[userInput].color;
            if (p->deck[userInput].name == 'R') {
                handleReverse(0, current_turn, numberPlayers, players, userInput);
            }
            else if (p->deck[userInput].name == 'A') {
                if (*current_turn == numberPlayers-1) {
                    and_played(p, &players[0], topOfDeck, userInput, players, numberPlayers, current_turn, deck, deck_size);
                }
                else {
                    and_played(p, &players[(*current_turn)+1], topOfDeck, userInput, players, numberPlayers, current_turn, deck, deck_size);
                }
            }
            else if (p->deck[userInput].name == 'O') {
                if (*current_turn == numberPlayers - 1) {
                    or_played(p, &players[0], topOfDeck, userInput, players, numberPlayers, current_turn, deck, deck_size);
                }
                else {
                    or_played(p, &players[(*current_turn) + 1], topOfDeck, userInput, players, numberPlayers, current_turn, deck, deck_size);
                }
            }
            else if (p->deck[userInput].name == 'N') {
                *current_turn = handleNot(*current_turn, numberPlayers, userInput);
            }
            remove_card(p, userInput);
        }
        else { 
            if (isValidCard(*topOfDeck, p->deck[userInput]) == 1) {
                topOfDeck->name = p->deck[userInput].name;
                topOfDeck->color = p->deck[userInput].color;
                remove_card(p, userInput);
            }
            else {
                while (isValidCard(*topOfDeck, p->deck[userInput]) == 0) {
                    printf("Invalid choice, number or color has to match.\n");
                    printf("%s, enter which card to play from 0 to %d: ", p->playerName, p->decksize - 1);
                    scanf("%d", &userInput);
                }
                topOfDeck->name = p->deck[userInput].name;
                topOfDeck->color = p->deck[userInput].color;
                remove_card(p, userInput);
            }
        }
    }
    
    if(*current_turn==numberPlayers){
        *deck_size+=1;
    }
        
}

void checkWinner(int deck_size, bool* win_check, player players[], int numPlayers) {
    if (deck_size == 0) {
        *win_check == true;
    }
    for (int i = 0; i > numPlayers; ++i) {
        if (players[i].decksize == 0) {
            *win_check == true;
        }
    }
}

int main(){
    srand(time(NULL));//sets rand() seed to random value
    int numberPlayers;
    int current_turn = 1;
    card topOfDeck;

    bool continueOrNot = true;

    while (continueOrNot == true) {
        card* deck=NULL;
        int deck_size=100;
        deck = malloc(deck_size * sizeof(card));//allocates memory for 100 cards
        initializeDeck(deck, 100);//creats 100 cards in the deck
        shuffleDeck(deck);//shuffles deck
        
    
        printf("Enter number of players: ");
        scanf("%d", &numberPlayers);
    
        while (numberPlayers > 6 || numberPlayers < 0) {
            printf("Invalid player count, must be between 2-6.\n");
            printf("Enter number of players: ");
            scanf("%d", &numberPlayers);
        }
    
        player* players = (player*)malloc(numberPlayers*sizeof(player));//allocates memeory the amount of players
        
        for (int i = 0; i < numberPlayers; ++i) {
            printf("Enter player %d's name (no spaces): ", i + 1);
            scanf("%s", players[i].playerName);
            (players[i].deck)=NULL;
            players[i].decksize = 0;
            for (int j = 0; j < 7; ++j) {
                draw_card(&players[i], &deck ,&deck_size);
            }
        }
        
        for (int i = 0; i < numberPlayers; ++i) {
            printPlayerHand(players[i]);
            printf("\n");
        }
        printf("\n");
        
        printf("Card pile is empty.\n\n");
    
        int userInput = 100;
    
        while (userInput > players[0].decksize - 1 || userInput < 0) {
            printf("%s, enter which card to play from 0 to %d: ", players[0].playerName, players[0].decksize - 1);
            scanf("%d", &userInput);
            if (userInput > players[0].decksize - 1 || userInput < 0) {
                printf("Invalid choice, %s does not have %d cards.\n", players[0].playerName, userInput);
            }
        }
    
        if (players[0].deck[userInput].color == 'S') {
            topOfDeck.name = players[0].deck[userInput].name;
            topOfDeck.color = players[0].deck[userInput].color;
            if (players[0].deck[userInput].name == 'R') {
                handleReverse(0, &current_turn, numberPlayers, players, userInput);
            }
            else if (players[0].deck[userInput].name == 'A') {
                and_played(&players[0], &players[1], &topOfDeck, userInput, players, numberPlayers, &current_turn, deck, &deck_size);
            }
            else if (players[0].deck[userInput].name == 'O') {
                or_played(&players[0], &players[1], &topOfDeck, userInput, players, numberPlayers, &current_turn, deck, &deck_size);
            }
            else if (players[0].deck[userInput].name == 'N') {
                current_turn = handleNot(current_turn, numberPlayers);
            }
            remove_card(&players[0], userInput);
        }
        else {
            topOfDeck.name = players[0].deck[userInput].name;
            topOfDeck.color = players[0].deck[userInput].color;
            remove_card(&players[0], userInput);
        }
    
        // start gameplay loop
        bool winnerDeclared = false;
    
        while (deck_size > 0 && winnerDeclared == false) {
            print_sit(players, numberPlayers, topOfDeck);
            printf("%d\n",deck_size);
            runTurn(&players[current_turn], &topOfDeck, deck, &deck_size,players,numberPlayers,&current_turn);
            checkWinner(deck_size, &winnerDeclared, players, numberPlayers);
            if(winnerDeclared==true){
                int min=1000;
                int count;
                for(int i=0;i<numberPlayers;i++){
                    if(players[i].decksize<min){
                        min=players[i].decksize;
                        count = i;
                        
                    }
                    else if(players[i].decksize==0){
                        printf("%s is the winner!",players[i].playerName);
                    }
                    printf("%s is the winner!",players[count].playerName);
                }
            }
            current_turn = player_turn(numberPlayers, current_turn);
        }
        
        free(players);

        char continOrNot;
        printf("Do you want to play again? n/N for no and anything else for yes: ");
        scanf(" %c", &continOrNot);

        if (continOrNot == 'n' || continOrNot == 'N') {
            continueOrNot = false;
        }
        else {
            continueOrNot = true;
        }
    }

    return 0;
}