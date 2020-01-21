// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

/*
   - Kone Fanhatcha
   - S1803435
   - ALC
   -Systems Programming 
*/

// Main Libraries
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

//Additional libraries
#include <sys/utsname.h>

// thread functions (prototypes)
void showMenu();
void transferedToServer(char *, int);
void displayStudentInfo(int);
void displayFiveRandomNum(int);
void displayServerUnameInfo(int);
void getFiles(int);
void getFileCopyFromServer(int);
char* getFileName(int);


/*
  The main function is the entry point of the app, it allows the user to interact with the app via
  the menu. 
*/
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
     showMenu(); //Initialization of the function that will display the menu to user
     
     //Variable to get the user input
     char userInput;
     char valueEntered[10];

     //This loop will always run if the user choose values : [0 -6] 
    do{
         printf("Select Option > ");
         fgets(valueEntered, 10, stdin); //Getting the value entered by user
         userInput = valueEntered[0]; //Get the first character of the value entered by user
    
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

/************************************ 1 - start showMenu() *****************************************************/
//The aim of this function is to the menu to the user
void showMenu(){
printf("0-Display options \n");
printf("1-Display student name, id, and server IP address \n");
printf("2-Display 5 random numbers between 0 and 1000 \n");
printf("3-Display uname information of server with OS name, version, and release \n");
printf("4-Display regular files in server's \"upload\" directory \n");
printf("5-Get copy of file from server's \"upload\" directory \n");
printf("6-Exit \n");
}

/************************************ end showMenu()************************************************************/

/************************************ 2 - start transferedToServer() ********************************************/
//Each time this function is called is to transfered the value entered by the user to the server for further processing
void transferedToServer(char *valueEntered, int socket){
      size_t n = strlen(valueEntered) + 1;
      writen(socket, (unsigned char *) &n, sizeof(size_t));
      writen(socket, (unsigned char *) valueEntered, n);
}
/************************************ end transferedToServer() **************************************************/

/************************************ 3 - start displayStudentInfo() ********************************************/
//This aim of this function is to request the following user data (Name, ID, IP) from the server
void displayStudentInfo(int socket){
char studentInfoAndIp[256]; //data(string) expected from the server
    size_t n;

    //Reading the the string sent by the server
    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) studentInfoAndIp, n);

    //Printing out the data(student Info and IP) received to console
    printf("\nStudent Information and IP address: %s\n", studentInfoAndIp);
    printf("\nReceived: %zu bytes\n\n", n);
}
/************************************ end displayStudentInfo() **************************************************/

/************************************ 4 - start displayFiveRandomNum() ********************************************/
//This function aims to request each time it is called 5 random numbers from the server
void displayFiveRandomNum(int socket){
    char numbers[256];  //data(string) expected from the server
    size_t n;
    
    //Reading the the string sent by the server
    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) numbers, n);

    //Printing out the data (the 5 numbers) received to console
    printf("\nList of five (05) random numbers: %s\n", numbers);
    printf("\nReceived: %zu bytes\n\n", n);
}
/************************************ end displayFiveRandomNum() **************************************************/

/************************************ 5 - start displayServerUnameInfo() ********************************************/
//This function will request the OS details to the server
void displayServerUnameInfo(int socket){
    //Allocate memory to the struct
    struct utsname *uname = (struct utsname *) malloc(sizeof( struct utsname));
    
    size_t payload_length = sizeof(struct utsname);
    
   //Sending the utsname struct to server
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));  
    writen(socket, (unsigned char *) uname, payload_length);            

    //Receiving the modified version of the utsname struct
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));      
    readn(socket, (unsigned char *) uname, payload_length);

    //Printing out the data (Server uname information) received to console
    printf("\nServer uname information:\n");
    printf("-OS  name: %s\n", uname->sysname);
    printf("-OS version: %s\n", uname->version);
    printf("-OS release: %s\n", uname->release);
    
    printf("\nReceived: %zu bytes\n\n", payload_length);
    
    free(uname); // free the memory to avoid memory leaks
}
/************************************ end displayServerUnameInfo() **************************************************/

/************************************ 6 - start getFiles() ********************************************/
//This function aims to request a list of files present in the upload directory of the server
void getFiles(int socket){
    char fileNames[256]; //data(array of string) expected from the server
    size_t n;

    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) fileNames, n);
    
    //Checking if the file names are not empty | In case they are, throw file not found warning
    if(strlen(fileNames) != 0){
        
        const char delimiter[2] = ", "; //This comma will be used a delimiter to separate the file names
        char *currentFileName;
       
        currentFileName = strtok(fileNames, delimiter); //Splitting the data(string) received from server by delimiter

        printf("List of the files in server \"upload\" directory:\n");

        while( currentFileName != NULL ) {
            printf( " - %s\n", currentFileName ); // Print out the  file name during each iteration

            currentFileName = strtok(NULL, delimiter);
        }
    } else {
        printf("No files found in server \"upload\" directory\n"); 
    }
    
    printf("\nReceived: %zu bytes\n\n", n);
}

/************************************ end getFiles() **************************************************/

/************************************ 6 - start getFileCopyFromServer() ********************************************/
//This function aims to get the file name specified by user then transfer it to server
void getFileCopyFromServer(int socket){
    char* filename = getFileName(socket); //This helper function will be used to get the filename from the user

    unsigned char fileContent[BUFSIZ]; //Getting the content of the file
    size_t n;

    FILE *copyFile;
    char copyFileName[128] = "./"; //File received from the server will be stored in root of the client folder  
    strcat(copyFileName, filename);
    
    //Reading the file content
    readn(socket, (unsigned char *) &n, sizeof(size_t));    
    readn(socket, (unsigned char *) fileContent, n);

    //Checking in filename entered by user exists in server (upload directory)
    if(n == 1){
        printf("File not found \"upload\" directory");
    } else {
        //Writting a copy of the file name entered
        copyFile = fopen(copyFileName, "wb");
        fwrite(fileContent, sizeof(unsigned char), n, copyFile);
        fclose(copyFile);//closing the file
        printf("\nFile successfully copied, find it in your current directory. \nReceived: %zu bytes\n\n", n);
    }
}

/************************************ end getFileCopyFromServer() **************************************************/

/**
^^^  HELPER FUNCTION: this aim to get the filename from user then send it to server  ^^^
***/
char* getFileName(int socket){
    printf("Enter file name: \n");
    static char filename[64];
    fgets(filename, 64, stdin); // get the value from input
    filename[strcspn(filename, "\n")] = 0;

    //Transfering filename to server
    size_t n = strlen(filename) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));   
    writen(socket, (unsigned char *) filename, n);

	return filename;
}


