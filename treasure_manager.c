#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>

typedef struct treasure{
    int ID;
    char username[20];
    float lat,lng;
    char clue[100];
    int value;
} treasure;

void add(char huntID[7]){
    char* buff = calloc(1,sizeof(treasure));
    treasure t;

    do{
        printf("\nTreasure format: treasureID username latitude longitude clue value\nInput new treasure:\n\n");
        fgets(buff,200,stdin);
    }while(sscanf(buff,"%d %s %f %f %s %d",&t.ID,&t.username,&t.lat,&t.lng,&t.clue,&t.value) != 6);

    char dirPath[13] = "Hunts/";
    char filePath[27] = "";

    strcat(dirPath,huntID);
    mkdir(dirPath,0777);

    strcpy(filePath,dirPath);
    strcat(filePath,"/Treasures.bin");

    addTreasure(filePath,t);
}

void addTreasure(char* filePath,treasure t){

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
    if(file == -1){
        printf("Hunt does not exist or does not contain treasures\n");
        return;
    }

    stat(filePath,&st);

    printf("\n%s\nSize: %ld bytes\nLast modified: %s\n",huntID,st.st_size,ctime(&st.st_mtime));

    treasure t;

    while(read(file,&t,sizeof(treasure)) != 0){
        printf("%d %s %f %f %s %d\n",t.ID,t.username,t.lat,t.lng,t.clue,t.value);
    }

    close(file);
}

void view(char huntID[7],int treasureID){
    char filePath[27] = "Hunts/";

    strcat(filePath,huntID);
    strcat(filePath,"/Treasures.bin");

    int file = open(filePath, O_RDONLY);
    if(file == -1){
        printf("Hunt does not exist or does not contain treasures\n");
        return;
    }

    treasure t;

    while(read(file,&t,sizeof(treasure)) != 0){
        if(t.ID == treasureID) break;
    }

    if(t.ID == treasureID){
        printf("%d %s %f %f %s %d\n",t.ID,t.username,t.lat,t.lng,t.clue,t.value);
    } 
    else {
        printf("Hunt does not contain specified treasure\n");
    }

    close(file);
}

void remove_treasure(char huntID[7],int treasureID){
    char filePath[27] = "Hunts/";

    strcat(filePath,huntID);
    char newFilePath [30];
    strcpy(newFilePath,filePath);
    strcat(filePath,"/Treasures.bin");
    strcat(newFilePath,"/TreasuresNew.bin");

    int fileOld = open(filePath, O_RDONLY);
    if(fileOld == -1){
        printf("Hunt does not exist or does not contain treasures\n");
        return;
    }

    int fileNew = open(newFilePath, O_CREAT | O_WRONLY, 0666);

    treasure t;

    while(read(fileOld,&t,sizeof(treasure)) != 0){
        if(t.ID != treasureID){
            addTreasure(newFilePath,t);
        }
    }

    unlink(filePath);
    rename(newFilePath,filePath);
}

void remove_hunt(char huntID[7]){
    char dirPath[13] = "Hunts/";
    strcat(dirPath,huntID);

    DIR *dir = opendir(dirPath);
    if(!dir){
        printf("Hunt does not exist\n");
        return;
    }

    struct dirent *entry;
    char filePath[100];

    while((entry = readdir(dir)) != NULL){
        sprintf(filePath,"%s/%s",dirPath,entry->d_name);
        unlink(filePath);
        strcpy(filePath,"");
    }

    closedir(dir);
    int x = rmdir(dirPath);
    if(x == -1) printf("Error deleting hunt\n");
    else printf("Hunt removed\n");
}

int main(int argc, char* argv[]){
    mkdir("Hunts",0777);

    

}