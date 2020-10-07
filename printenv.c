#include  "sh.h"

void printenv(char ** environment, char *arguments){
    
    if(environment == NULL){ 
            printf("Environment null");       
            return;
    }else if(arguments[1]== NULL){
        int index = 0;
        while(environment[index]){
            printf("%s\n", environment[index]);
            //printf("%s=%s\n", environment[index], getenv(environment[index]));
            index++;
        }

        return;
    }
    else if(arguments[2]== NULL){
        getenv(3);
        return;
    }
    else{
        printf("ERROR");
        return;
    }
}