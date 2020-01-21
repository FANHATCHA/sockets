// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "rdwrn.h"

#include <sys/utsname.h>


void showMenu();
void transferedToServer(char *, int);
void displayStudentInfo(int);
void displayFiveRandomNum(int);
void displayServerUnameInfo(int);
void getFiles(int);
void getFileCopyFromServer(int);
char* getFileName(int);



int main(void)
{
    // *** this code down to the next "// ***" does not need to be changed except the port number
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Error - could not create socket");
    exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50031);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
    perror("Error - connect failed");
    exit(1);
    } else
       printf("Connected to server...\n");

    // ***
    // your own application code will go here and replace what is below... 
    // i.e. your menu etc.
     showMenu();
     
     char userInput;
     char valueEntered[10];
    do{
         printf("Select Option > ");
         fgets(valueEntered, 10, stdin);
         userInput = valueEntered[0];

        
        switch(userInput){
            case '0':
               showMenu();
                break;
            case '1': 
                transferedToServer(valueEntered, sockfd);
                displayStudentInfo(sockfd);
                break;
            case '2':
                transferedToServer(valueEntered, sockfd);
                displayFiveRandomNum(sockfd);
                break;
            case '3':
                transferedToServer(valueEntered, sockfd);
                displayServerUnameInfo(sockfd);
                break;
            case '4':
                transferedToServer(valueEntered, sockfd);
                getFiles(sockfd);
                break;
            case '5':
                transferedToServer(valueEntered, sockfd);
                getFileCopyFromServer(sockfd);
                break;
            case '6':
               printf("End of program ! Good Bye !");
                break;
            default:
                printf("Not a valid option, display menu by entering 0\n");
                break;
        }

    } while(userInput != 6);

       

    // *** make sure sockets are cleaned up

    close(sockfd);

    exit(EXIT_SUCCESS);
} // end main()

void showMenu(){

printf("0-Display options \n");
printf("1-Display student name, id, and server IP address \n");
printf("2-Display 5 random numbers between 0 and 1000 \n");
printf("3-Display uname information of server with OS name, version, and release \n");
printf("4-Display regular files in server's \"upload\" directory \n");
printf("5-Get copy of file from server's \"upload\" directory \n");
printf("6-Exit \n");

}

void transferedToServer(char *valueEntered, int socket){
      size_t n = strlen(valueEntered) + 1;
      writen(socket, (unsigned char *) &n, sizeof(size_t));
      writen(socket, (unsigned char *) valueEntered, n);
}

void displayStudentInfo(int socket){

char studentInfoAndIp[256];
    size_t n;

    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) studentInfoAndIp, n);

    printf("\nStudent Information and IP address: %s\n", studentInfoAndIp);
    printf("\nReceived: %zu bytes\n\n", n);
}

void displayFiveRandomNum(int socket){
    char numbers[256];
    size_t n;

    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) numbers, n);

    printf("\nList of five (05) random numbers: %s\n", numbers);
    printf("\nReceived: %zu bytes\n\n", n);
}

void displayServerUnameInfo(int socket){
      struct utsname *uname = (struct utsname *) malloc(sizeof( struct utsname));
    
    size_t payload_length = sizeof(struct utsname);
    

    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));  
    writen(socket, (unsigned char *) uname, payload_length);            

    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));      
    readn(socket, (unsigned char *) uname, payload_length);

    printf("\nServer uname information:\n");
    printf("-OS  name: %s\n", uname->sysname);
    printf("-OS version: %s\n", uname->version);
    printf("-OS release: %s\n", uname->release);
    
    printf("\nReceived: %zu bytes\n\n", payload_length);
    
    free(uname);
}

void getFiles(int socket){
    char fileNames[256];
    size_t n;

    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) fileNames, n);
    
    if(strlen(fileNames) != 0){
        
        const char delimiter[2] = ", ";
        char *currentFileName;
       
        currentFileName = strtok(fileNames, delimiter);

        printf("List of the files in server \"upload\" directory:\n");
        while( currentFileName != NULL ) {
            printf( " - %s\n", currentFileName );

            currentFileName = strtok(NULL, delimiter);
        }
    } else {
        printf("No files found in server \"upload\" directory\n");
    }
    
    printf("\nReceived: %zu bytes\n\n", n);
}

void getFileCopyFromServer(int socket){
    char* filename = getFileName(socket);

    unsigned char fileContent[BUFSIZ];
    size_t n;

    FILE *copyFile;
    char copyFileName[128] = "./";
    strcat(copyFileName, filename);

    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) fileContent, n);
    if(n == 1){
        printf("File not found \"upload\" directory");
    } else {
        copyFile = fopen(copyFileName, "wb");
        fwrite(fileContent, sizeof(unsigned char), n, copyFile);
        fclose(copyFile);
        printf("\nFile successfully copied, find it in your current directory. \nReceived: %zu bytes\n\n", n);
    }
}

char* getFileName(int socket){
    printf("Enter file name: \n");
    static char filename[64];
    fgets(filename, 64, stdin); // get the value from input
    filename[strcspn(filename, "\n")] = 0;

    size_t n = strlen(filename) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));   
    writen(socket, (unsigned char *) filename, n);

	return filename;
}


