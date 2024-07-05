#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int boardMatrix[9];

bool CheckBoard(){
    for(int i = 0 ; i < 9 ; i++){
        if(boardMatrix[i] == -1){
            return true;
        }
    }

    return false;
}

void PrintBoard(int pos){
    if(pos == -1){
        printf("\n");
        printf("  1 | 2 | 3  \n");
        printf("- - - - - - -\n");
        printf("  4 | 5 | 6  \n");
        printf("- - - - - - -\n");
        printf("  7 | 8 | 9  \n");
        printf("\n");
    }
    else{
        printf("To be continue");
    }
}

int main(){
    bool *players; 
    
    //shared memory
    key_t key = 1234;
    int shm_id = shmget(key, sizeof(int), IPC_CREAT | 0666);
    players = shmat(shm_id, NULL, 0);
    memset(players, 0, sizeof(int));


    int PlayerChoice;
    while(true){
        printf("Welcome to the game! \n1. Player 1 (O)\n2. Player 2 (X)\n");
        printf("Choose the player : ");
        scanf("%d", &PlayerChoice);

        if(players[PlayerChoice - 1] == true){
            printf("Player %d has taken\n", PlayerChoice);
            continue;
        }
        else{
            players[PlayerChoice - 1] = true;
            break;
        }
    }

    //wait for other player to join
    while(true){
        if(players[0] == true && players[1] == true){
            break;
        }
    }

    printf("Game Started!\n");
    PrintBoard(-1);

    int BoardChoice;
    memset(boardMatrix, -1, sizeof(boardMatrix));
    while(CheckBoard()){
        printf("Choose the availabe number on the board : ");
        scanf("%d", &BoardChoice);

        if(BoardChoice < 1 || BoardChoice > 9){
            printf("Invalid Input!");
            continue;
        }
        if(boardMatrix[BoardChoice] != -1){
            printf("The Board not available!\n");
            continue;
        }

        printf("Player %d has playing\n", PlayerChoice);
        boardMatrix[BoardChoice] = PlayerChoice;
    }

    //destroy
    shmdt(players);
    shmctl(shm_id, IPC_RMID, NULL);
}