#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

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
    char filePath[27] = "";

    strcat(dirPath,huntID);
    mkdir(dirPath,0777);

    strcpy(filePath,dirPath);
    strcat(filePath,"/Treasures.bin");

    int file = open(filePath, O_WRONLY | O_APPEND | O_CREAT, 0666);

    write(file,&t,sizeof(t));

    close(file);
}

void list(char huntID[7]){
    char filePath[27] = "Hunts/";
    struct stat st;

    strcat(filePath,huntID);
    strcat(filePath,"/Treasures.bin");

    int file = open(filePath, O_RDONLY);
    stat(filePath,&st);

    printf("\n%s\nSize: %ld bytes\nLast modified: %s\n",huntID,st.st_size,ctime(&st.st_mtime));

    treasure t;

    while(read(file,&t,sizeof(treasure)) != 0){
        printf("%s %f %f %s %d\n",t.treasureID,t.lat,t.lng,t.clue,t.value);
    }
}

int main(int argc, char* argv[]){
    int result = mkdir("Hunts",0666);

    if(result == -1) printf("Hunts directory already exists\n");
    else printf("Created Hunts directory\n");

    //add("Hunt001");
    list("Hunt001");
}