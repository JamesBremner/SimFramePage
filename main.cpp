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

using namespace std;


class cProcess
{
public:
    cProcess( int id, int bytes );

    string Text()
    {
        stringstream ss;
        ss << myID <<" "<< myBytes;
        return ss.str();
    }

    string PageTable();

    int ID()
    {
        return myID;
    }
private:
    int myID;
    int myBytes;
};

class cTasks
{
public:
    void Add( const cProcess& p )
    {
        vTasks.push_back( p );
    }
    string Text()
    {
        stringstream ss;
        for( auto& t : vTasks )
            ss << t.Text() << "\n";
        return ss.str();
    }
    string PageTable();

private:
    vector< cProcess > vTasks;
};

class cFrames
{
public:
    void Resize( int RAM, int PAGESIZE )
    {
        myRAM = RAM;
        myPAGESIZE = PAGESIZE;
        myFrame.resize( RAM / PAGESIZE, -1 );
    }

    void Allocate( int id, int bytes );

    vector<int> Allocated( int id );

    string Text();

private:
    int myRAM;
    int myPAGESIZE;
    vector< int > myFrame;
};

int RAM;
int PAGESIZE;
cTasks Tasks;
cFrames Frames;

cProcess::cProcess( int id, int bytes )
    : myID( id )
    , myBytes( bytes )
{
    Frames.Allocate( myID, myBytes );
}

void cFrames::Allocate( int id, int bytes )
{
    int pages = ceil( (float)(bytes+1) / ( 1000 * myPAGESIZE) );

    //cout << "allocate " << pages << "\n";

    for( int& a : myFrame )
    {
        if( a < 0 )
        {
            a = id;
            pages--;
            if( ! pages )
                break;
        }
    }
}

string cFrames::Text()
{
    stringstream ss;
    ss << "\nFrames\n";
    ss << "Frame\tProcess\n";
    int k = 0;
    for( int a : myFrame )
    {
        ss << k << "\t";
        k++;
        if( a < 0 )
            ss <<"free\n";
        else
            ss << a << "\n";
    }
    ss << "\n";
    return ss.str();
}
    vector<int> cFrames::Allocated( int id )
    {
        vector< int > vf;
        int kf = 0;
        for( int a : myFrame )
        {
            if( a == id )
            {
                vf.push_back( kf );
            }
            kf++;
        }
        return vf;
    }

string cProcess::PageTable()
{
    stringstream ss;
    vector< int > vf = Frames.Allocated( myID );
    int kp = 0;
    for( int f : vf )
    {
        ss << myID <<"\t"<< kp <<"\t"<< f << "\n";
        kp++;
    }
    ss << "\n";
    return ss.str();
}

string cTasks::PageTable()
{
    stringstream ss;
    ss << "\nPage Table\n";
    ss << "Process\tPage\tFrame\n\n";
    for( auto& t : vTasks )
    {
        ss << t.PageTable();
    }
    ss << "\n";
    return ss.str();
}

void ReadInputFile( const string fname )
{
    ifstream inf( fname.c_str() );
    if( ! inf.is_open() )
    {
        cout << "cannot open " << fname << "\n";
        return;
    }
    string line;
    while( getline( inf, line ))
    {
        //cout << line << "\n";
        if( line.find("RAM") == 0 )
        {
            RAM = atoi( line.substr(4).c_str() );
        }
        else if( line.find("PAGESIZE") == 0 )
        {
            PAGESIZE = atoi( line.substr(9).c_str() );

            Frames.Resize( RAM, PAGESIZE );
        }
        else if( line.find("NEW") == 0 )
        {
            // new process starting
            cout << line << "\n";
            int pn = atoi( line.substr(4).c_str() );
            int p = line.find(" ",5);
            int pm = atoi( line.substr( p ).c_str() );
            Tasks.Add( cProcess( pn, pm ));
        }
        else if( line.find("PM") == 0 )
        {
            cout << Frames.Text();
        }
        else if( line.find("PT") == 0 )
        {
            cout << Tasks.PageTable();
        }
    }
//    cout << "RAM: " << RAM << " PAGESIZE: " << PAGESIZE<< "\n";
//    cout << Tasks.Text();
}

int main(int argc, char** argv )
{
    ReadInputFile( argv[1] );

    return 0;
}
