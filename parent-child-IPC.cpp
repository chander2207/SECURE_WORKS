#include "utilityfunc.h"

using namespace std;
int main(){

        int descSwitch_1Parent = ON;
        int descSwitch_2Parent = ON;
        int descSwitchChild1 = ON;
        int descSwitchChild2 = ON;
	
	pid_t child1;
	pid_t child2;
	// Create 2 pipes. One for child 1 and another for child 2.
        int fd_child1[2];
	int fd_child2[2];
	
	if(pipe(fd_child1) == -1){
		cout << "Error in opening pipe for child 1. \nEXITING...........\n";
		return 1;
	}	
	if(pipe(fd_child2) == -1){
		cout << "Error in opening pipe for child 2. \nEXITING...........\n";
		return 1;
	}
	
	/* Open the file for writing the o/p content */
	ofstream file_child1;
	ofstream file_child2;
	file_child1.open ("child1_processInfo.txt");
	file_child2.open ("child2_directoryContent.txt");
	
	//cout << "child1: " <<file_child1 << "child2: " << file_child2 <<endl;	
	// Create only 2 child processes
	(child1 = fork()) && (child2 = fork());

	// For the 1st Child that would find the associated PID and the CPU info/
	if(child1 == 0){
		cout << "Child 1 with pid: " << getpid() << " and ppid: " << getppid() << endl;
                char *buff = NULL;
		char byte = 0;
                int count = 0;
		struct CPU_info cpuInfo_last;	
		struct CPU_info cpuInfo_curr;	
		/* This condition will prevent file descriptor end from closing again. */
		if(descSwitchChild1 != OFF){
                        close(fd_child1[WRITE_END]);
                        descSwitchChild1 = OFF;
                }
		
		/* Wait for reading while it gets the first character to read */
                while (read(fd_child1[READ_END], &byte, 1) == 1)
                {
                        cout << "byte: " << byte << endl;
			/* ioctl gives the immediate count for the bytes present in the pipe for reading */
                        if (ioctl(fd_child1[READ_END], FIONREAD, &count) != -1)
                        {
                                //fprintf(stdout,"Child: count = %d\n",count);

                                /* allocate space for the byte we just read + the rest *
                                 * of whatever is on the pipe.                         */
                                int capacity = count+2;
                                buff = (char *) malloc(capacity);
                                memset(buff,0,capacity);
                                buff[0] = byte;
                                buff[capacity] = '\0';
                                cout << "buff: "<< buff <<endl;
				
				/* Read the content of the pipe */
                                if (read(fd_child1[READ_END], buff+1, count) == count)
                                        //fprintf(stdout,"Child1: received \"%s\"\n", buff);
				string str(buff);
				
				/* Get the process_id using the process_name */
				pid_t process_id = getProcIdByName(str);
				cout << "process_id: " << process_id << endl;

				/* Get the CPU info for the process. This is called twice *
				 * with gap of 1 sec in order to calculate the CPU usage. */
				getCPUInfoByPID(process_id, &cpuInfo_last);
			        sleep(1); 
				getCPUInfoByPID(process_id, &cpuInfo_curr);

				/* Get CPU usage in percentage after 1 sec delay */
				double ucpu_usage, scpu_usage;
				cpu_usage(&cpuInfo_curr,&cpuInfo_last,&ucpu_usage,&scpu_usage);
				
				/* Write all the values to the file */
				writeToFile(process_id, &cpuInfo_curr, &ucpu_usage, &scpu_usage, file_child1);
				/* Free the buffer */	
				memset(&cpuInfo_curr,0,sizeof(cpuInfo_curr));
				memset(&cpuInfo_last,0,sizeof(cpuInfo_last));
                                free(buff);
                        }
                        else
                        {   /* could not read in-size */
                                perror("Failed to read input size.");
                        }
                }
                close(fd_child1[READ_END]);
                fprintf(stdout,"Child: Shutting down.\n");
	}
	/* For the 2nd Child that will list the content of the given directory path */
	else if(child2 == 0){
		cout << "Child 2 with pid: " << getpid() << " and ppid: " << getppid() << endl;
                char *buff = NULL;
                char byte = 0;
                int count = 0;

		/* This condition will prevent file descriptor end from closing again. */
                if(descSwitchChild2 != OFF){
                        close(fd_child2[WRITE_END]);
                        descSwitchChild2 = OFF;
                }

		/* Wait for reading while it gets the first character to read */
                while (read(fd_child2[READ_END], &byte, 1) == 1)
                {
                        cout << "byte: " << byte << endl;
			/* ioctl gives the immediate count for the bytes present in the pipe for reading */
                        if (ioctl(fd_child2[READ_END], FIONREAD, &count) != -1)
                        {
                                fprintf(stdout,"Child: count = %d\n",count);

                                /* allocate space for the byte we just read + the rest *
                                 * of whatever is on the pipe.                         */
                                int capacity = count+2;
                                buff = (char *) malloc(capacity);
                                memset(buff,0,capacity);
                                buff[0] = byte;
                                buff[capacity] = '\0';
                                //cout << "buff: "<< buff <<endl;
                                
				/* Read the content of the pipe */
				if (read(fd_child2[READ_END], buff+1, count) == count)
                                        //fprintf(stdout,"Child2: received \"%s\"\n", buff);
                                string str(buff);
                		
				/* List all the content of the directory */
				listDirContents(str,file_child2);
                               
			        /* Free the buffer */	
				free(buff);
                        }
                        else
                        {   /* could not read in-size */
                                perror("Failed to read input size.");
                        }
                }
                close(fd_child1[READ_END]);
                fprintf(stdout,"Child: Shutting down.\n");

	}
	/* Parent process that will take value from the user input. */
	else{
		//cout << "Process ID: " << getpid() << endl;

		string str,keystring;
		int key = -1;
		int count = 0;
		while(1){
			getline(cin,str);
			istringstream stream(str);
		        
			// Count the number of the values entered	
			while(getline(stream, keystring, ' ')){
				count++;
			}

			// fetch the value of the input string
			if(count >= 1 && count <= 2){
				istringstream stream(str);
				stream >> key >> keystring; 
				/* Key == 0 represents that user entered process_name */
				if(key == 0){
                                        cout << "Key: " << key << " and Entered process_name: " << keystring << endl;
                                        /* Write to the File */
					file_child1 <<"Process_name: " << keystring << endl;
					if(descSwitch_1Parent != OFF){
                                                if(close(fd_child1[0]) == -1){
                                                        perror("closing Error");
                                                }
                                                descSwitch_1Parent = OFF;
                                        }

					/* Write the process name to the pipe. */
                                        if(write(fd_child1[1], keystring.c_str(), keystring.size()) < 0)
                                                perror("writing message");
                                }
				/* Key == 1 represents that the directory path has been entered. */
                                else if(key == 1){
                                        cout << "Key: " << key << " and Entered path: " << keystring << endl;
					file_child2 <<"Directory_path: " << keystring << endl;
                                        if(descSwitch_2Parent != OFF){
                                                if(close(fd_child2[0]) == -1){
                                                        perror("closing Error");
                                                }
                                                descSwitch_2Parent = OFF;
                                        }

					/* Write the directory path string to the pipe. */
                                        if(write(fd_child2[1], keystring.c_str(), keystring.size()) < 0)
                                                perror("writing message");
                                }
				/* If 3 is entered as the input by the user, it will terminate the program. */
                                else if(key == 3){
                                        cout << "Exiting the program\n";
                                        break;
                                }
			}
			else{
				cout << "USAGE: The value should be entered in following ways\n\t 1. <0> <process_name>\n\t 2. <1> <directory_path>\n\t 3. Enter \'3\' to exit the program\n";
			}

			count = 0;
			//cout << "key: " << key <<endl;
			//cout << "String entered: " << keystring << endl;
		}
		close(fd_child1[WRITE_END]);
                close(fd_child2[WRITE_END]);
	}
	
	/* Close the file before exiting */
	file_child1.close();
	file_child2.close();
	//cout << "Ending the program" <<  endl;
	return 0;
}

