#ifndef HELPERS_H
#define HELPERS_H

#include <chrono>
#include <iomanip>
#include <libgen.h>
#include <omnetpp.h>
#include <string>
#include <sys/stat.h>
#include <thread>


using namespace std::chrono;
using namespace omnetpp;

/** Run command with Popen */
int run_command(std::string cmd);

/** Run command with Popen, return the output */
std::string return_command(std::string cmd);

/** Run command with Popen, return elapsed time for the command */
float command_time(std::string cmd);

/** Print elapsed time */
void print_time(simtime_t checkpoint, simtime_t database);

/** Return file size in bytes */
long GetFileSize(std::string filename);

/** Calculate percentage as string */
std::string percentage(long a, long b);

/** Remove '\n' from string */
std::string removeNewLine(std::string s);

/** Return day as %m%d */
std::string getDay();

/** Return time as %HH-%MM */
std::string getTime();

//void createFolder(std::string path);

/** Get current results path */
std::string createPath();

/** Create a full directory tree */
int createDirTreeRecursive(std::string path);

#endif