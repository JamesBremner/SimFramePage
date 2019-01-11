/*
This program simulates a few of the steps an operating system takes when managing system memory and allocating it to processes by using paging and virtual addresses.

Processes will enter the system requesting a fixed amount of memory. The simulator will allocate the appropriate number of physical memory frames to the process, create a page table for that process, and update its internal system tables tracking free frames and allocated frames. If insufficient memory is available, our simulator will simply deny the process the chance to run and display a short message to that effect.

Processes in the system will make memory references using logical addresses. The simulator will use the page table to look up a logical address and display both the logical and physical addresses for that request. If a logical address is not valid for the process, the simulator will report a memory violation.

The simulator will read all of its data from a single text input file, and output its work to the screen/console. Here is the format of the text file:

RAM nnnn

Always the first line of the file. nnnn will be the number of MEGABYTES of memory available in the system

PAGESIZE nnnn

Always the second line of the file. nnnn is the page size in MEGABYTES

The remainder of the file can be a mix of any of the following, one per line:

NEW p nnnn

A new process, numbered p, wants to start and needs nnnn BYTES of memory

REF p nnnn

Process p is running and wants to access its virtual memory address nnnn

END p

Process p exits the system and returns all its allocated memory

PT p

Prints the page table for process p. Each line will have the page number followed by the frame number for that page.

PM

Prints the list of all memory frames. Each line will have the frame number followed by the process number owning it, or "free" if unallocated.

*/
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

#include "cMMSim.h"

using namespace std;

/** Construct the simulator */
cMMSim SIM;

int main(int argc, char** argv )
{
    try
    {
        SIM.ReadInputFile( argv[1] );
    }
    catch( std::runtime_error& e )
    {
        cout << e.what() << "\n";
    }

    return 0;
}
