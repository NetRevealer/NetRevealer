#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#########################
typedef struct IpProtoSets{
    char ipset[10][20];
    int size;
} IpProtoSet ;

struct IpProtoSet *APP_IPs[7];

IpProtoSet youtube;
IpProtoSet instagram;
IpProtoSet googlemeet;
IpProtoSet skype;
IpProtoSet anghami; 
IpProtoSet twitch;
IpProtoSet others;

//#########################
void init_AppIps(){

    youtube = (IpProtoSet) {.size = 0};
    APP_IPs[0] = &youtube ;

    instagram = (IpProtoSet) {.size = 0};
    APP_IPs[1] = &instagram ;

    googlemeet = (IpProtoSet) {.size = 0};
    APP_IPs[2] = &googlemeet ;

    skype = (IpProtoSet) {.size = 0};
    APP_IPs[3] = &skype ;

    anghami = (IpProtoSet) {.size = 0};
    APP_IPs[4] = &anghami ;

    twitch = (IpProtoSet) {.size = 0};
    APP_IPs[5] = &twitch ;

    others = (IpProtoSet) {.size = 0};
    APP_IPs[6] = &others ;

}

void insert_IpProto(IpProtoSet *set, char **flowid){
    int i;
    int contains ;
    char *ipproto[20];

    contains = 0;

    char * token = strtok(flowid,"-");

    strcpy(ipproto,token);

    strcat(ipproto,"-");
    token = strtok(NULL,"-");

    strcat(ipproto,token);

    if(set->size == 20){
        printf("set is full");
        return;
    }

    for(i = 0; i < set->size; i++){

        if(strcmp(set->ipset[i],ipproto) == 0){
            contains = 1;
            break;
        }

    }

    if(contains == 0){
        strcpy(set->ipset[set->size],ipproto);
        set->size += 1;
    }
}

void insert(char *APP, char **flowid){

    int index = mapper(APP);
    IpProtoSet *set = APP_IPs[index];

    insert_IpProto(set,flowid);

}

int mapper(char *APP){
    int len = strlen(APP);

    switch(len){

        case 5:
            return 3;

        case 6:
            if(strcmp(APP,"Twitch") == 0){
                return 5;  
            }

            if(strcmp(APP,"Others") == 0){
                return 6; 
            }

        case 7:
            if(strcmp(APP,"Youtube") == 0){
                return 0;  
            }

            if(strcmp(APP,"Anghami") == 0){
                return 4;  
            }

        case 9:
            return 1;
            
        case 10:
            return 2;                     
    }

}
/*
void block_App(char *APP){
    int index = mapper(APP);
    IpProtoSet *set = APP_IPs[index];


}*/

void check_ipsets(){

    int i;
    printf("youtube size: %d\n", youtube.size);
    printf("twitch size: %d\n", twitch.size);
    printf("instagram size: %d\n", instagram.size);
    printf("skype size: %d\n", skype.size);
    printf("googlemeet size: %d\n", googlemeet.size);
    printf("instagram size: %d\n", instagram.size);
    printf("anghami size: %d\n", anghami.size);
    

    printf("YOUTUBE:\n");
    for(i = 0; i < youtube.size; i++){
        printf("%d  %s\n",i,youtube.ipset[i]);
    }
    printf("TWITCH:\n");
    for(i = 0; i < twitch.size; i++){
        printf("%d  %s\n",i,twitch.ipset[i]);
    }
    printf("INSTAGRAM:\n");
    for(i = 0; i < instagram.size; i++){
        printf("%d  %s\n",i,instagram.ipset[i]);
    }
    printf("ANGHAMI:\n");
    for(i = 0; i < anghami.size; i++){
        printf("%d  %s\n",i,anghami.ipset[i]);
    }
    printf("GOOGLEMEET:\n");
    for(i = 0; i < googlemeet.size; i++){
        printf("%d  %s\n",i,googlemeet.ipset[i]);
    }
    printf("SKYPE:\n");
    for(i = 0; i < skype.size; i++){
        printf("%d  %s\n",i,skype.ipset[i]);
    }
    printf("OTHERS:\n");
    for(i = 0; i < others.size; i++){
        printf("%d  %s\n",i,others.ipset[i]);
    }
    
}