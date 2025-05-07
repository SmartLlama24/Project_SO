#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>  

typedef struct treasure{
    int ID;
    char username[20];
    float lat,lng;
    char clue[100];
    int value;
} treasure;

void add(char huntID[7]){
    char* buf = calloc(1,sizeof(treasure));
    treasure t;

    do{
        printf("\nTreasure format: treasureID username latitude longitude clue value\nInput new treasure:\n\n");
        fgets(buf,200,stdin);
    }while(sscanf(buf,"%d %s %f %f %s %d",&t.ID,&t.username,&t.lat,&t.lng,&t.clue,&t.value) != 6);

    free(buf);

    char dirPath[13] = "Hunts/";
    char filePath[27] = "";
    char logPath[29] = "";
    char linkPath[34] = "logged_hunt_";

    strcpy(logPath,dirPath);
    strcat(logPath,huntID);
    strcat(logPath,"/logged_hunt.txt");
    strcat(linkPath,huntID);
    strcat(linkPath,".txt");
    strcat(dirPath,huntID);

    int check = mkdir(dirPath,0777);
    int log = open(logPath, O_WRONLY | O_CREAT | O_APPEND, 0666);

    if(check == 0){
        int x = symlink(logPath,linkPath);
        write(log,"Created hunt",sizeof("Created hunt"));
    }
    else {
        write(log,"Added treasure",sizeof("Added treasure"));
    }

    close(log);

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
    char logPath[30] = "";
    struct stat st;

    strcat(filePath,huntID);
    strcpy(logPath,filePath);
    strcat(logPath,"/logged_hunt.txt");
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
        printf("\nTreasure ID: %d\nUsername: %s\nLatitude: %f\nLongitude: %f\nClue: %s\nValue: %d\n\n",t.ID,t.username,t.lat,t.lng,t.clue,t.value);
    }

    int log = open(logPath, O_WRONLY | O_APPEND);
    write(log,"Listed hunt",sizeof("Listed hunt"));
    close(log);

    close(file);
}

void view(char huntID[7],int treasureID){
    char filePath[27] = "Hunts/";
    char logPath[30] = "";

    strcat(filePath,huntID);
    strcpy(logPath,filePath);
    strcat(logPath,"/logged_hunt.txt");
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
        printf("\nTreasure ID: %d\nUsername: %s\nLatitudeL %f\nLongitude: %f\nClue: %s\nValue: %d\n\n",t.ID,t.username,t.lat,t.lng,t.clue,t.value);

        int log =open(logPath, O_WRONLY | O_APPEND);
        write(log,"Viewed treasure",sizeof("Viewed treasure"));
        close(log);
    } 
    else {
        printf("Hunt does not contain specified treasure\n");
    }

    close(file);
}

void remove_treasure(char huntID[7],int treasureID){
    int trigger = 0;
    char filePath[27] = "Hunts/";
    char logPath[30] = "";

    strcat(filePath,huntID);
    strcpy(logPath,filePath);
    strcat(logPath,"/logged_hunt.txt");

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
            if(trigger == 1) t.ID--;
            addTreasure(newFilePath,t);
        }
        else trigger = 1;
    }

    unlink(filePath);
    rename(newFilePath,filePath);

    if(trigger){ 
        printf("Removed treasure\n");

        int log = open(logPath, O_WRONLY | O_APPEND);
        write(log,"Removed treasure",sizeof("Removed treasure"));
        close(log);
    }
    else printf("Treasure does not exist in hunt\n");
}

void remove_hunt(char huntID[7]){
    char dirPath[13] = "Hunts/";
    char buf[100] = "                   ";
    char linkPath[100] = "";

    strcat(linkPath,"logged_hunt_");
    strcat(dirPath,huntID);
    strcat(linkPath,huntID);
    strcat(linkPath,".txt");

    DIR *dir = opendir(dirPath);
    if(!dir){
        printf("Hunt does not exist\n");
        return;
    }

    unlink(linkPath);

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

    if(argc == 1) {
        printf("Program functions:\n--add <hunt_id>\n--list <hunt_id>\n--view <hunt_id> <treasure_id>\n--remove_treasure <hunt_id> <treasure_id>\n--remove_hunt <hunt_id>\nHunt ID template: HuntXXX\n\n");
    }
    else {
        if(strcmp(argv[1],"--add") == 0){
            if(argc < 3) printf("Missing arguments\n\n");
            else add(argv[2]);
        }
    
        if(strcmp(argv[1],"--list") == 0){
            if(argc < 3) printf("Missing arguments\n\n");
            else list(argv[2]);
        }
    
        if(strcmp(argv[1],"--view") == 0){
            if(argc < 4) printf("Missing arguments\n\n");
            else view(argv[2],atoi(argv[3]));
        }
    
        if(strcmp(argv[1],"--remove_treasure") == 0){
            if(argc < 4) printf("Missing arguments\n\n");
            else remove_treasure(argv[2],atoi(argv[3]));
        }
    
        if(strcmp(argv[1],"--remove_hunt") == 0){
            if(argc < 3) printf("Missing arguments\n\n");
            else remove_hunt(argv[2]);
        }

    }

}