#include "utilityfunc.h"

using namespace std;

int getProcIdByName(string procName){
	int pid = -1;

	// Open the /proc directory
	DIR *dp = opendir("/proc");
	if (dp != NULL)
	{
		// Enumerate all entries in directory until process found
		struct dirent *dirp;
		while (pid < 0 && (dirp = readdir(dp)))
		{
			// Skip non-numeric entries
			int id = atoi(dirp->d_name);
			if (id > 0)
			{
				// Read contents of virtual /proc/{pid}/cmdline file
				string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
				ifstream cmdFile(cmdPath.c_str());
				string cmdLine;
				getline(cmdFile, cmdLine);
				if (!cmdLine.empty())
				{
					// Keep first cmdline item which contains the program path
					size_t pos = cmdLine.find('\0');
					if (pos != string::npos)
						cmdLine = cmdLine.substr(0, pos);
					// Keep program name only, removing the path
					pos = cmdLine.rfind('/');
					if (pos != string::npos)
						cmdLine = cmdLine.substr(pos + 1);
					// Compare against requested process name
					if (procName == cmdLine)
						pid = id;

				}
			}
		}
	}
	closedir(dp);
	return pid;
}

void getCPUInfoByPID(int PID, 
		     struct CPU_info *cpu_info){
	string filePath = "/proc/"+to_string(PID)+"/stat";
	//cout << "file_name: " << filePath << endl;
	ifstream process_stat_file(filePath.c_str());
	string process_stat;
	getline(process_stat_file,process_stat);
	if(!process_stat.empty()){
		//cout << "process status: " << process_stat << endl;
		sscanf(process_stat.c_str(), " %*d, %*s, %*c, %*d, %*d, %*d, %*d, %*d, %*u, %*u, %*u, %*u, %*u,	%lu, %lu, %ld, %ld, %*d, %*d, %*d, %*d, %llu, %*u, %*d, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*u, %*d, %d", &cpu_info->utime_ticks, &cpu_info->stime_ticks, &cpu_info->cutime_ticks, &cpu_info->cstime_ticks, &cpu_info->start_time, &cpu_info->processor);
	}

	/* code for fetching the total CPU time */
	unsigned long int cpu_time[10];
	ifstream statFile("/proc/stat");
	string stat;
	getline(statFile, stat);
	if(!stat.empty()){
		sscanf(stat.c_str(),"%*s, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu",&cpu_time[0], &cpu_time[1],	&cpu_time[2], &cpu_time[3], &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7], &cpu_time[8], &cpu_time[9]);
	}
	for(int i=0; i<10; i++){
		cpu_info->cpu_total_time += cpu_time[i];	
	}

}

void cpu_usage(const struct CPU_info* cur_info, 
	       const struct CPU_info* last_info, 
	       double* ucpu_usage, double* scpu_usage){
	const long unsigned int total_time_diff = cur_info->cpu_total_time -
                                              last_info->cpu_total_time;

	*ucpu_usage = 100 * (((cur_info->utime_ticks + cur_info->cutime_ticks)
                           - (last_info->utime_ticks + last_info->cutime_ticks))
                           / (double) total_time_diff);

	*scpu_usage = 100 * (((cur_info->stime_ticks + cur_info->cstime_ticks)
                           - (last_info->stime_ticks + last_info->cstime_ticks)) 
                           / (double) total_time_diff);
}

void writeToFile(pid_t pid, 
		 const struct CPU_info* cpuInfo_curr, 
		 const double* ucpu_usage, 
		 const double* scpu_usage, ofstream& fd){
	fd << "{" << endl;
	fd << "\t\"process_id\": " <<"\"" << pid <<"\"," << endl;
	fd << "\t\"utime_ticks\": " <<"\"" << cpuInfo_curr->utime_ticks <<"\"," << endl;
	fd << "\t\"stime_ticks\": " <<"\"" << cpuInfo_curr->stime_ticks <<"\"," << endl;
	fd << "\t\"cutime_ticks\": " <<"\"" << cpuInfo_curr->cutime_ticks <<"\"," << endl;
	fd << "\t\"cstime_ticks\": " <<"\"" << cpuInfo_curr->cstime_ticks <<"\"," << endl;
	fd << "\t\"start_time\": " <<"\"" << cpuInfo_curr->start_time <<"\"," << endl;
	fd << "\t\"processor\": " <<"\"" << cpuInfo_curr->processor <<"\"," << endl;
	fd << "\t\"cpu_total_time\": " <<"\"" << cpuInfo_curr->cpu_total_time <<"\"," << endl;
	fd << "\t\"ucpu_usage\": " <<"\"" << *ucpu_usage <<"\"," << endl;
	fd << "\t\"scpu_usage\": " <<"\"" << *scpu_usage <<"\"" << endl;
	fd << "}" << endl;
}

void listDirContents(string path, ofstream& fd){
	DIR *mydir;
	struct dirent *myfile;
	struct stat mystat;

	string buf;
	mydir = opendir(path.c_str());
	while((myfile = readdir(mydir)) != NULL)
	{
		buf = path + "/" + myfile->d_name;
		stat(buf.c_str(), &mystat);
		fd << "{" << endl << "\t\"file_name\": " <<"\"" <<myfile->d_name <<"\","<< endl;
		writeToFile_dirContent(&mystat, fd);
	}
	closedir(mydir);
}

void writeToFile_dirContent(struct stat* stat, ofstream& fd){
	fd << "\t\"inode_number\": " <<"\"" << stat->st_ino <<"\"," << endl;
	fd << "\t\"fileType\": " <<"\"" << stat->st_mode <<"\"," << endl;
	fd << "\t\"hard_links\": " <<"\"" << stat->st_nlink <<"\"," << endl;
	fd << "\t\"user_id\": " <<"\"" << stat->st_uid <<"\"," << endl;
	fd << "\t\"group_id\": " <<"\"" << stat->st_gid <<"\"," << endl;
	fd << "\t\"device_id\": " <<"\"" << stat->st_rdev <<"\"," << endl;
	fd << "\t\"totalSize\": " <<"\"" << stat->st_size <<"\"," << endl;
	fd << "\t\"blockSize\": " <<"\"" << stat->st_blksize <<"\"" << endl;
	fd << "}" << endl;
}

