#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <bits/stdc++.h>

#define OFF 0
#define ON 1
#define READ_END 0
#define WRITE_END 1

/* This structure contains the information about the *
 * CPU per-process.                                  */
struct CPU_info{
        unsigned long int utime_ticks;
        unsigned long int stime_ticks;
        long int cutime_ticks;
        long int cstime_ticks;
        unsigned long long int start_time;
        int processor;
	unsigned long int cpu_total_time;
};

/* This function is meant to return the process Id  *
 * given the process name.                          */
int getProcIdByName(std::string procName);

/* This function fetched the CPU information linked *
 * with the process using it PID.                   */
void getCPUInfoByPID(int PID,struct CPU_info *cpu_info);

/* It will list down the contents of the DIR given  *
 * the directory path.                              */
void listDirContents(std::string path,std::ofstream&);

/* The below function calculates the cpu_usage */
void cpu_usage(const struct CPU_info* cur_info, const struct CPU_info* last_info, double* ucpu_usage, double* scpu_usage);

/* This function will write values to the file. */
void writeToFile(pid_t process_id, const struct CPU_info* cpuInfo_curr, const double* ucpu_usage, const double* scpu_usage, std::ofstream&);

/* The function will write the directory content to the file. */
void writeToFile_dirContent(struct stat* stat, std::ofstream& fd);
