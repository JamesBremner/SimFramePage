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

/** A process that requests frames */
class cProcess
{
public:

    /** CTOR
        @param[in] id of process
        @param[in] bytes of memory required

        Allocates memory

        The allocated memory will be set to zero
        if task creation fails for any reason
    */
    cProcess( int id, int bytes );

    /// String describing process
    string Text()
    {
        stringstream ss;
        ss << myID <<" "<< myBytes;
        return ss.str();
    }

    /// String describing process page table
    string PageTable();

    int ID() const
    {
        return myID;
    }
    int AllocatedBytes() const
    {
        return myBytes;
    }
private:
    int myID;
    int myBytes;
};

/** The current processes that have allocated frames */
class cTasks
{
public:

    /** Add a process, if it has been allocated memory
        @param[in] process to add
        @return true if successful
    */
    bool Add( const cProcess& p );

    /// Find process from id
    bool Find( int p );

    /** Stop process and free allocated memory
        @param[in] p id of process to be stopped
    */
    void End( int p );

    /// String describing process
    string Text()
    {
        stringstream ss;
        for( auto& t : vTasks )
            ss << t.Text() << "\n";
        return ss.str();
    }

    /// String describing page table
    string PageTable();

private:
    vector< cProcess > vTasks;
};

/** Track frames allocated to processes */
class cFrames
{
public:

    /** Set memory and frame size
        @param[in] RAM number of megabytes of memory
        @param[in] PAGESIZE page size in megabytes
    */
    void Resize( int RAM, int PAGESIZE );

    /** Allocate memory
        @param[in] id of process requesting memory
        @param[in] bytes requested
        @return true if allocation successful
    */
    bool Allocate( int id, int bytes );

    /** Free memory
        @param[in] id of process freeing memory allocated to it
    */
    void Free( int id );

    /** Memory allocated to process
        @param[in] id of process
        @return vector if fame indices allocated to process
    */
    vector<int> Allocated( int id );

    /** Frame from page
        @param[in] process id
        @param[in] page virtual page in process
        @return frame where page is allocated
    */
    int FrameFromPage( int process, int page );

    /** Reference an address
    @param[in] p process id
    @param[in] a virtual address
    @return string displaying reference details
    */
    string Ref( int p, int a );

    /// string displaying frames
    string Text();

    /// frame size in megabytes
    int PageSize()
    {
        return myPAGESIZE;
    }

    /// frame size in bytes
    int PageSizeBytes()
    {
        return 1000 * myPAGESIZE;
    }

private:
    int myRAM;
    int myPAGESIZE;
    vector< int > myFrame;  ///< process ids frames allocated to, -1 for free
};

int RAM;
int PAGESIZE;
cTasks Tasks;           // the runnig tasks
cFrames Frames;         // the memory

cProcess::cProcess( int id, int bytes )
    : myID( id )
    , myBytes( bytes )
{
    if( Tasks.Find( id )) {
        // already task running with same ID
        myBytes = 0;
        return;
    }
    if( ! Frames.Allocate( myID, myBytes ) )
    {
        // memory allocation failed
        myBytes = 0;
    }
}

void cFrames::Resize( int RAM, int PAGESIZE )
{
    myRAM = RAM;
    myPAGESIZE = PAGESIZE;
    myFrame.resize( RAM / PAGESIZE, -1 );
}

bool cFrames::Allocate( int id, int bytes )
{
    int pages = ceil( (float)(bytes+1) / PageSizeBytes() );

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

    // check not enough memory
    if( pages )
    {
        // free any memory that was allocated
        Free( id );
        return false;
    }
    return true;
}

void cFrames::Free( int id )
{
    for( int& a : myFrame )
    {
        if( a == id )
            a = -1;
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

int cFrames::FrameFromPage( int process, int page )
{
    cout << "FrameFromPage " << process <<" "<<page<<"\n";
    int kf = 0;
    for( int a : myFrame )
    {
        if( a == process )
        {
            page--;
            if( ! page )
            {
                return kf;
            }
        }
        kf++;
    }
    throw std::runtime_error("FrameFromPage failed");
}

bool cTasks::Add( const cProcess& p )
{
    if( ! p.AllocatedBytes() )
        return false;
    vTasks.push_back( p );
    return true;
}

bool cTasks::Find( int p )
{
    for( auto& t : vTasks )
    {
        if( t.ID() == p )
            return true;
    }
    return false;
}

void cTasks::End( int p )
{
    for( auto& t : vTasks )
    {
        if( t.ID() == p )
        {
            Frames.Free( p );
        }
    }
}

string cFrames::Ref( int p, int a )
{
    int page = floor( (float)a / PageSizeBytes() );
    int offset = a - page * PageSizeBytes();
    vector<int> vf = Allocated( p );
    int frame = vf[ page ];

    std::stringstream ss;
    ss << "Process " << p << " referencing " << a
       << " at frame " << frame << " offset " << offset << "\n";
    return ss.str();
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
        throw std::runtime_error("Cannot open input file");
    }
    string line;
    while( getline( inf, line ))
    {
        //cout << line << "\n";

        // split input line into space delimited parameters
        vector<string> vp;
        std::stringstream sst(line);
        std::string a;
        while( getline( sst, a, ' ' ) )
            vp.push_back(a);
        if( ! vp.size() )
            continue;

        if( vp[0] == "RAM" )
        {
            RAM = atoi( vp[1].c_str() );
        }
        else if(  vp[0] == "PAGESIZE" )
        {
            PAGESIZE = atoi( vp[1].c_str() );

            Frames.Resize( RAM, PAGESIZE );
        }
        else if(  vp[0] == "NEW" )
        {
            // new process starting
            cout << line << "\n";
            int pn = atoi( vp[1].c_str() );
            int pm = atoi( vp[2].c_str() );
            if( ! Tasks.Add( cProcess( pn, pm )))
                cout << "Memory allocation failed\n";
        }
        else if(  vp[0] == "REF")
        {
            int pn = atoi( vp[1].c_str() );
            int pa = atoi( vp[2].c_str() );
            cout << Frames.Ref( pn, pa );
        }
        else if(  vp[0] == "END")
        {
            // process ending
            cout << line << "\n";
            Tasks.End( atoi( vp[1].c_str() ) );
        }
        else if(  vp[0] == "PM")
        {
            cout << Frames.Text();
        }
        else if(  vp[0] == "PT")
        {
            cout << Tasks.PageTable();
        }
    }
//    cout << "RAM: " << RAM << " PAGESIZE: " << PAGESIZE<< "\n";
//    cout << Tasks.Text();
}

int main(int argc, char** argv )
{
    try
    {
        ReadInputFile( argv[1] );
    }
    catch( std::runtime_error& e )
    {
        cout << e.what() << "\n";
    }

    return 0;
}
