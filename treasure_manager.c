#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct treasure{
    char treasureID[11];
    char username[20];
    float lat,lng;
    char clue[100];
    int value;
} treasure;

void add(char huntID[7]){
    char buff[200];
    treasure t;

    do{
        printf("\nTreasure format: treasureID username latitude longitude clue value\nInput new treasure:\n\n");
        fgets(buff,200,stdin);
    }while(sscanf(buff,"%s %s %f %f %s %d",&t.treasureID,&t.username,&t.lat,&t.lng,&t.clue,&t.value) != 6);

    char dirPath[13] = "Hunts/";
    char filePath[27];
    strcat(dirPath,huntID);
    strcpy(filePath,dirPath);
    strcat(filePath,"/Treasures.txt");

    mkdir(dirPath,0777);
    int file = open(filePath, O_WRONLY | O_CREAT);

    write(file,&t,sizeof(t));

    close(file);
}

int main(int argc, char* argv[]){
    int result = mkdir("Hunts",0777);

    if(result == -1) printf("Hunts directory already exists\n");
    else printf("Created Hunts directory\n");

    add("Hunt001");
}