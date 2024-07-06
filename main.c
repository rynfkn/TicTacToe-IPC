#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

int *boardMatrix;

void CheckWin(bool *Player_1_Status, bool *Player_2_Status){
    //column 0
    if((boardMatrix[0] == boardMatrix[1]) && (boardMatrix[1] == boardMatrix[2]) && boardMatrix[0] != -1){
        boardMatrix[0] == 1 ? (*(Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }
    if((boardMatrix[0] == boardMatrix[3]) && (boardMatrix[3] == boardMatrix[6]) && boardMatrix[0] != -1){
        boardMatrix[0] == 1 ? ((*Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }
    if((boardMatrix[0] == boardMatrix[4]) && (boardMatrix[4] == boardMatrix[8]) && boardMatrix[0] != -1){
        boardMatrix[0] == 1 ? (*Player_1_Status = true) : ((*Player_2_Status) = true);
        return;
    }

    if((boardMatrix[2] == boardMatrix[4]) && (boardMatrix[4] == boardMatrix[6]) && boardMatrix[2] != -1){
        boardMatrix[2] == 1 ? ((*Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }
    if((boardMatrix[2] == boardMatrix[5]) && (boardMatrix[5] == boardMatrix[8]) && boardMatrix[2] != -1){
        boardMatrix[2] == 1 ? ((*Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }

    if((boardMatrix[8] == boardMatrix[7]) && (boardMatrix[7] == boardMatrix[6]) && boardMatrix[8] != -1){
        boardMatrix[8] == 1 ? ((*Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }

    if((boardMatrix[1] == boardMatrix[4]) && (boardMatrix[4] == boardMatrix[7]) && boardMatrix[1] != -1){
        boardMatrix[1] == 1 ? ((*Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }
    if((boardMatrix[3] == boardMatrix[4]) && (boardMatrix[4] == boardMatrix[5]) && boardMatrix[3] != -1){
        boardMatrix[3] == 1 ? ((*Player_1_Status) = true) : ((*Player_2_Status) = true);
        return;
    }
}

bool CheckBoard(){
    for(int i = 0 ; i < 9 ; i++){
        if(boardMatrix[i] == -1){
            return true;
        }
    }
    return false;
}

void PrintBoard(){
    for(int i = 0 ; i < 9 ; i++){
        if(boardMatrix[i] == 1){
            printf(" X ");
        }
        else if(boardMatrix[i] == 2){
            printf(" O ");
        }
        else{
            printf(" %d ", i+1);
        }
        
        if((i + 1) % 3 == 0){
            printf("\n");
        }
    }
}

int main(){
    bool *players; 
    bool *PlayerTurn;
    bool *Player_Status;
    
    //shared memory player
    key_t key1 = 1234;
    int shm_id1 = shmget(key1, sizeof(int), IPC_CREAT | 0666);
    players = shmat(shm_id1, NULL, 0);
    memset(players, 0, sizeof(int));
    
    //shared memory board
    key_t key2 = 1235;
    int shm_id2 = shmget(key2, sizeof(int), IPC_CREAT | 0666);
    boardMatrix = shmat(shm_id2, NULL, 0);
    
    for(int i = 0 ; i < 9 ; i++){ //memset the matrixBoard
        boardMatrix[i] = -1;
    }

    //shared memory for switching
    key_t key3 = 1236;
    int shm_id3 = shmget(key3, sizeof(int), IPC_CREAT | 0666);
    PlayerTurn = shmat(shm_id3, NULL, 0);
    PlayerTurn[0] = true;
    PlayerTurn[1] = false;

    //shared memory for status
    key_t key4 = 1237;
    int shm_id4 = shmget(key4, sizeof(int), IPC_CREAT | 0666);
    Player_Status = shmat(shm_id4, NULL, 0);
    Player_Status[0] = false;
    Player_Status[1] = false;


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
    bool ok = false;
    while(players[0] == false || players[1] == false){
        if(!ok){
            printf("Waiting for other player to join\n");
            ok = true;
        }
    }

    printf("Game Started!\n");

    int BoardChoice;
    while(CheckBoard()){

        bool can = false;
        while(PlayerTurn[PlayerChoice - 1] == false){ // wait for player turn
            if(can == false){
                printf("Waiting for other player to turn\n");
                can = true;
            }
            if(Player_Status[0] == true || Player_Status[1] == true){
                break;
            }
        }
        PrintBoard(1);

        CheckWin(&Player_Status[0], &Player_Status[1]);
        if(Player_Status[0] == true || Player_Status[1] == true){
            break;
        }
        
        printf("Choose the availabe number on the board : ");
        scanf("%d", &BoardChoice);

        if(BoardChoice < 1 || BoardChoice > 9){
            printf("Invalid Input!");
            continue;
        }
        if(boardMatrix[BoardChoice - 1] != -1){
            printf("The Board not available!\n");
            continue;
        }

        printf("Player %d has playing\n", PlayerChoice);
        boardMatrix[BoardChoice - 1] = PlayerChoice;
        PrintBoard(1);


        PlayerTurn[PlayerChoice - 1] = 0;
        PlayerTurn[(PlayerChoice % 2)] = 1;

    }

    printf("Game End!\n");
    if(Player_Status[0] == true){
        printf("Player X has WON!\n");
    }
    else if(Player_Status[1] == true){
        printf("Player O has WON!\n");
    }

    //destroy
    shmdt(players);
    shmctl(shm_id1, IPC_RMID, NULL);
    
    shmdt(boardMatrix);
    shmctl(shm_id2, IPC_RMID, NULL);

    shmdt(PlayerTurn);
    shmctl(shm_id3, IPC_RMID, NULL);
    
    shmdt(Player_Status);
    shmctl(shm_id4, IPC_RMID, NULL);
}
