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
    std::string Text()
    {
        std::stringstream ss;
        ss << myID <<" "<< myBytes;
        return ss.str();
    }

    /// String describing process page table
    std::string PageTable();

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
    std::string Text()
    {
        std::stringstream ss;
        for( auto& t : vTasks )
            ss << t.Text() << "\n";
        return ss.str();
    }

    /// String describing page table
    std::string PageTable();

private:
    std::vector< cProcess > vTasks;
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
    std::vector<int> Allocated( int id );

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
    std::string Ref( int p, int a );

    /// string displaying frames
    std::string Text();

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
    std::vector< int > myFrame;  ///< process ids frames allocated to, -1 for free
};

/** Memory Management Simulator */
class cMMSim
{
public:
    cMMSim()
        : myRAM( -1 )
        , myPAGESIZE( -1 )
    {
    }

    void ReadInputFile( const std::string fname );

    void RAM( int r )
    {
        myRAM = r;
        Resize();
    }
    void PAGESIZE( int p )
    {
        myPAGESIZE = p;
        Resize();
    }
    bool Add( const cProcess& p )
    {
        return myTasks.Add( p );
    }
    std::string Ref( int p, int a )
    {
        return myFrames.Ref( p, a );
    }
    void End( int p )
    {
        myTasks.End( p );
    }
    std::string FrameTable()
    {
        return myFrames.Text();
    }
    std::string PageTable()
    {
        return myTasks.Text();
    }
    cTasks& Tasks()
    {
        return myTasks;
    }
    cFrames& Frames()
    {
        return myFrames;
    }
private:
    int myRAM;
    int myPAGESIZE;
    cTasks myTasks;           // the runnig tasks
    cFrames myFrames;         // the memory

    void Resize()
    {
        if( myRAM > 0 && myPAGESIZE > 0 )
            myFrames.Resize( myRAM, myPAGESIZE );
    }
};

