//*********************************************************
//
// Ty Donnelly
// Operating Systems
// Writing Your Own Shell: hfsh24
//
//*********************************************************


//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <string>
#include "csapp.h"
#include <iostream>
#include <pthread.h>
#define STR_EXIT "exit"


//*********************************************************
//
// Extern Declarations
//
//*********************************************************
// Buffer state. This is used to parse string in memory...
// Leave this alone.
extern "C"{
    extern char **gettoks();
    typedef struct yy_buffer_state * YY_BUFFER_STATE;
    extern YY_BUFFER_STATE yy_scan_string(const char * str);
    extern YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
    extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
    extern void yy_switch_to_buffer(YY_BUFFER_STATE buffer);
    
}


//*********************************************************
//
// Namespace
//
// If you are new to C++, uncommment the `using namespace std` line.
// It will make your life a little easier.  However, all true
// C++ developers will leave this commented.  ;^)
//
//*********************************************************
using namespace std;


//Global Variables
const char error_message[30] = "An error has occurred\n";


//*********************************************************
//
// Function Prototypes
//
//*********************************************************

struct threadArgs {
    vector<string> cmdArgsThread;
    vector<string> paths;
};



//This function basically converts the vector to char** which exec takes, its a little more complicated because i thought envp had to be something else
void prepareExecArgs(vector<string> &toks, char** &argv, char** &envp, const char* cmd){ 
    int tokCount = toks.size();
    int argCount;

    argCount = (tokCount - 1);

    argv = new char*[argCount]; 

    char* newCMD = const_cast<char*>(cmd);

    for (int i = 0; i <= argCount; ++i) {
        argv[i] = new char[toks[i].length() + 1]; 
        strcpy(argv[i], toks[i].c_str());
        
    }
    
    
    argv[argCount + 1] = NULL; 

    
    envp = NULL; 
}
//Deals with executing commands using fork and execve
void executeCommand(vector<string> toks, vector<string> paths){ 
    vector<string> args1;
                    
    vector<string> left;
    vector<string> right;

    int index;                
    bool redirect = false;
    for(index=0; index != toks.size(); index++){
        args1.push_back(toks[index]);
        
        if(args1.at(index) == ">"){
            redirect = true;
            continue;
                            
        }
        if(args1.at(index) == "!"){ // Takes ! character and appends it to previous token

            args1.pop_back();
            string back = args1.back();
            back.append("!");
            args1.pop_back();
            args1.push_back(back);
            
        }
        

        if(redirect){
            right.push_back(args1[index]); //Fill right side if > is detected
        }
        else{
            left.push_back(args1[index]); // ELse fill left
        }
       
    }
    //Three below check for errors when redirect symbol is present
    if(right.empty() && redirect){ // Make sure it isnt empty after redirect
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
    else if(right.size() > 1 && redirect){ // Make sure only 1 arg is after redirect
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
    else if(left.size() == 0 && redirect){ //Make sure something is before redirect
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }


    int success = 0;
    for (string& path : paths) { // 
        string cmdPath = path + "/" + left[0];
        const char* cmd = cmdPath.c_str();
        
            if(access(cmd, X_OK) == 0){
                success = 1;
                pid_t child = Fork();
                if(child == 0){
                    
                    char **argv = nullptr;
                    char **envp = nullptr;
                    
                    if(redirect){
                        prepareExecArgs(left, argv, envp, cmd); //IF redirect only use left side
                    }
                    else{
                        prepareExecArgs(args1, argv, envp, cmd); //Else use entire arg
                    }

                    
                    int ii;
                    

                    int fd;
                    if(redirect){ // If redirect change stdout and stderr to whatever is redirected
                        const char* file = right[0].c_str();
                        fd = open(file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
                        if(fd < 0){
                            write(STDERR_FILENO, error_message, strlen(error_message));
                        }
                        else{
                            dup2(fd, STDOUT_FILENO); 
                            dup2(fd, STDERR_FILENO);
                        }
                    }
                     
                    Execve(cmd, argv, envp);

                    if(fd > 0){
                        close(fd);
                    } 
                }
                else{
                    //Waits for child process
                    int status;
                    pid_t pid;
                    pid = Waitpid(child, &status, 0);
                    }
                            
            }
            
        }
        if(!success){ //If commands cant be found in specified paths then print error
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
}
//Prints prompt if argc is there or not
void printPrompt(int argc){
    if(argc == 1){
        fprintf(stdout, "hfsh24> ");

    }
    else{
        return;
    }
}

//Is used when parallel commands are invoked
void* executeCommandInThread(void* arg) { 
    threadArgs* data = static_cast<threadArgs*>(arg);
    vector<string> cmdArgs = data->cmdArgsThread;
    vector<string> paths = data->paths;

    
    executeCommand(cmdArgs, paths);

    delete data; 
    return NULL;
}


//*********************************************************
//
// Main Function
//
//*********************************************************
int main(int argc, char *argv[])
{
    /* local variables */
    int ii;
    char **toks;
    int retval;
    char linestr[256];
    vector<string> paths;
    paths.push_back("/bin"); //Set default path


    YY_BUFFER_STATE buffer;
    /* initialize local variables */
    ii = 0;
    toks = NULL;
    retval = 0;
    if(argc > 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    FILE *input;
    if(argc == 2){ // If shell is launched with file open it
        input = fopen(argv[1], "r");
        if(input == NULL){
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }

    }
    else{ //Else use stdinput
        input = stdin;
        fprintf(stdout, "hfsh24> ");
    }
    /* main loop */
    while(fgets(linestr, 256, input)){
        
        // make sure line has a '\n' at the end of it
        if(!strstr(linestr, "\n"))
            strcat(linestr, "\n");

        /* get arguments */
        buffer = yy_scan_string(linestr);
        yy_switch_to_buffer(buffer);
        toks = gettoks();
        yy_delete_buffer(buffer);

        if(toks[0] != NULL){
            if(strcmp(toks[0],"exit") == 0){
                if(toks[1] != NULL){ //Make sure exit doesnt have args
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            else if(strcmp(toks[0],"cd") == 0){
                if(toks[1] == NULL){ //Make sure cd has atleast 1 arg
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else if(toks[2] != NULL){ //Make sure cd doesnt have 2 args
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else{
                    chdir(toks[1]);
                }

            }
            else if(strcmp(toks[0],"path") == 0){
                paths.clear(); //Remove previous paths
                if(toks[1] != NULL){ //if a path is specified add it, if none are specified then paths is left empty
                    int count = 1;
                    for(count=1; toks[count] != NULL; count++){
                        paths.push_back(toks[count]);
                    }
                }
                
            }
            else{
                if(!paths.empty()){ // Do external commands if path isnt empty
                    vector<string> cmdArgs;
                    int index;
                    int childCount = 1;
                    vector<vector<string>> cmdsToRun; 
                    for(index=0; toks[index] != NULL; index++){
                        
                        if(!strcmp(toks[index], "&")){
                            if(!strcmp(toks[0], "&")){ //Checks to make sure & isnt first item in tokens
                                childCount = 0;
                                continue;
                            }
                            else if(toks[index + 1] == NULL){ //Checks if index after is null
                                continue;
                            }

                            else{ //if it is valid then increase number of threads to create
                                childCount++;
                                vector<string> runArgs = cmdArgs;
                                cmdsToRun.push_back(runArgs); //Adds vector to vector of vectors
                                
                                
                                cmdArgs.clear();
                                continue;
                            }
                            
                                            
                        }
                        else{
                            cmdArgs.push_back(toks[index]);
                        }
                        
                        

                        
                       
                    }
                    if(childCount != 0){ //If there are no & commands then take singular command
                        cmdsToRun.push_back(cmdArgs);

                    }

                    vector<pthread_t> threads(cmdsToRun.size());

                    for (int i = 0; i < cmdsToRun.size(); ++i) { //Creates a thread for each cmd in cmdsToRun

                        threadArgs* data = new threadArgs();
                        data->cmdArgsThread = cmdsToRun[i];
                        data->paths = paths;

                        pthread_create(&threads[i], NULL, executeCommandInThread, data);
                    }

                    for (int i = 0; i < threads.size(); ++i) { //Waits for threads
                        pthread_join(threads[i], NULL);
                    }
                    
                    
                  
                }
                else{ //If paths is empty and they try to do a command throw an error
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            
            if(!strcmp(toks[0], STR_EXIT)){
                break;

            }
            printPrompt(argc); // Prints prompt if shell is in interactive mode
            
        }

    }

    /* return to calling environment */
    return( retval );
}



