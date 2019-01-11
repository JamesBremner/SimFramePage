#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>

#include "cMMSim.h"

extern cMMSim SIM;

cProcess::cProcess( int id, int bytes )
    : myID( id )
    , myBytes( bytes )
{
    if( SIM.Tasks().Find( id ))
    {
        // already task running with same ID
        myBytes = 0;
        return;
    }
    if( ! SIM.Frames().Allocate( myID, myBytes ) )
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

std::string cFrames::Text()
{
    std::stringstream ss;
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
std::vector<int> cFrames::Allocated( int id )
{
    std::vector< int > vf;
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
    std::cout << "FrameFromPage " << process <<" "<<page<<"\n";
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
            SIM.Frames().Free( p );
        }
    }
}

std::string cFrames::Ref( int p, int a )
{
    int page = floor( (float)a / PageSizeBytes() );
    int offset = a - page * PageSizeBytes();
    std::vector<int> vf = Allocated( p );
    int frame = vf[ page ];

    std::stringstream ss;
    ss << "Process " << p << " referencing " << a
       << " at frame " << frame << " offset " << offset << "\n";
    return ss.str();
}

std::string cProcess::PageTable()
{
    std::stringstream ss;
    std::vector< int > vf = SIM.Frames().Allocated( myID );
    int kp = 0;
    for( int f : vf )
    {
        ss << myID <<"\t"<< kp <<"\t"<< f << "\n";
        kp++;
    }
    ss << "\n";
    return ss.str();
}

std::string cTasks::PageTable()
{
    std::stringstream ss;
    ss << "\nPage Table\n";
    ss << "Process\tPage\tFrame\n\n";
    for( auto& t : vTasks )
    {
        ss << t.PageTable();
    }
    ss << "\n";
    return ss.str();
}

void cMMSim::ReadInputFile( const std::string fname )
{
    std::ifstream inf( fname.c_str() );
    if( ! inf.is_open() )
    {
        std::cout << "cannot open " << fname << "\n";
        throw std::runtime_error("Cannot open input file");
    }
    std::string line;
    while( getline( inf, line ))
    {
        //cout << line << "\n";

        // split input line into space delimited parameters
        std::vector<std::string> vp;
        std::stringstream sst(line);
        std::string a;
        while( getline( sst, a, ' ' ) )
            vp.push_back(a);
        if( ! vp.size() )
            continue;

        if( vp[0] == "RAM" )
        {
            SIM.RAM( atoi( vp[1].c_str() ) );
        }
        else if(  vp[0] == "PAGESIZE" )
        {
            SIM.PAGESIZE( atoi( vp[1].c_str() ) );
        }
        else if(  vp[0] == "NEW" )
        {
            // new process starting
            std::cout << line << "\n";
            int pn = atoi( vp[1].c_str() );
            int pm = atoi( vp[2].c_str() );
            if( ! SIM.Add( cProcess( pn, pm )))
                std::cout << "Memory allocation failed\n";
        }
        else if(  vp[0] == "REF")
        {
            int pn = atoi( vp[1].c_str() );
            int pa = atoi( vp[2].c_str() );
            std::cout << SIM.Ref( pn, pa );
        }
        else if(  vp[0] == "END")
        {
            // process ending
            std::cout << line << "\n";
            SIM.End( atoi( vp[1].c_str() ) );
        }
        else if(  vp[0] == "PM")
        {
            std::cout << SIM.FrameTable();
        }
        else if(  vp[0] == "PT")
        {
            std::cout << SIM.PageTable();
        }
    }
//    cout << "RAM: " << RAM << " PAGESIZE: " << PAGESIZE<< "\n";
//    cout << Tasks.Text();
}


