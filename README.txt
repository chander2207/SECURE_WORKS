# Instructions to compile and execute the commands

        # To compile code
        make

        # To execute code
        make run

        # To clean the project
        make clean

# Valid input for the program
	# The code accepts three types of input.
		--> Chlid 1 process is supposed to find the pid and cpu_info given its name.
		    Therefore, it accepts input as: 
			0 process_name
		    where '0' denotes the identifiable key which tells the parent process 
		    that process_name is entered.
		--> List directory: Chlid 2 process is supposed to find the directory 
		    content with properties given the process_id.
		    Therefore, it accepts input as: 
			1 directory_path
		    where '1' denotes the identifiable key which tells the parent process 
		    that directory_path is entered.
		--> Exit code: To exit code, user need to input '3'.

# To deal with accepting multiple request, The input values can be queued in the buffer
  of type queue. The parent would check the type of the input entered by the user and 
  based on that it will enter the respective content for the child processes in their
  respective queue. We can set the limit of the queue to a certain level to hold maximum
  possible values.
  Here I am considering separate queues in order to avoid extra checks by each child pro-
  cess and many splitting and analysis of the input text. Otherwise, it would lead to 
  the multiple checks.
  
# Assumption
	# In case of the cpu information for the process Id. I considered fetching per-
	  process associated cpu information present in the stat file.
	# The file properties are assumed to be fetched from the stat data structure.
	# For writing the content from both the child separate files have been used
	  "child1_processInfo.txt" and "child2_directoryContent.txt". This assumption
	  was important considering the writing to the single file might would have run
	  into race condition without proper synchronize method. This would have taken
	  some time and hence decided to use the separate file in order to prevent the 
	  time.
	# The pipes are used over other IPC mechanism for faster delivery of the content
	  to the child process.

# Measures
	# The condition checks has been used in the program in order to prevent it from
	  unnecessary crashing. One such example in case of wrong input, the usage will 
	  be displayed to the user until user explicitly exits the code.
	# The pipe has been written considering it doesn't lose data and the child must
	  wait before parent writes anything to its respective pipe.
	# The open end of the pipe has been closed before ending the program.
 
# Code Description
	# This code is meant to show the communication between parent and child process.
	  In this program, parent has 2 child processes.
	  To establish the communication channel between two, pipe has been used. This
	  pipe allows parent to write the write-end of the pipe while blocking the read-
	  end.

	# The role of the parent is to take user input from the command line. Therefore
	  parent process uses getline() func and waits for the user to provide the valid
	  content. User can input the content one after the other. The parent will read 
	  all the value from standard input and based on the input type it will forward
	  to the respective child. 
	  If key value is 0, then key String is provided to child1
	  If key value is 1, then key String is provided to child2. 
	  If key value is 3, then the program will exit.

	# The role of first child "child1" is to fetch the pid and cpuInfo by process_name.
	  To find the process_id, I have used the content of the /proc file system. Once
	  the /proc directory is accessed, while reading through each directory, cmdline
	  file has been used to match the given process_name. Once the process_name matches
	  the pid is returned to the main function. In the cmdline file, present at 
	  "/proc/[pid]/cmdline", shows the exceution commands which is stripped to fetch
	  the process_name.
	  Once the process_id is retrieved, it is passed as an argument to another function
	  "getCPUInfoByPID". This function will fetch the CPU info from "/proc/[pid]/stat"
	  file and "/proc/stat" for total_cpu_time. This function would calculate usage of
	  cpu in percentage along with other parameters such as processor(the process is executed at), 
	  utime_ticks, stime_ticks, cutime_ticks, cstime_ticks, start_time.

	# The role of the second child "child2" is to list content of given directory.
	  To achieve this, the input directory is opened and the stat() function is used
	  for each sub-folder or file to extract the properties of it. The properties that
	  are displayed in the code are inode_number,fileType, hard_links, user_id and few 
	  other fields of the "strct stat" structure.

	# The file has been written in the form of JSON format manually. Here I intitally  
	  tried to include the json object, however, it was throwing undefined reference 
	  error despite of loading and installing the library file. Therefore, due to time
	  constraint, I decided to write the content in the file manually with JSON format
	  style.
