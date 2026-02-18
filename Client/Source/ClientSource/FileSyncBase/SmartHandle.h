// SmartHandle.h

#pragma once


// Class CSmartHandle works like Jeffrey Richter's EnsureCleanup class
// (Windows HANDLE wrapper which releases handle in destructor).
// The change: instead of release function pointer it uses template 
// class (using the same technique as described in 
// Modern C++ Design by Andrei Alexandrescu.
//
// Classes CCloseHandle, CCloseRegKey... are releasing policies.
// These classes are templates. Class CSmartHandle has second parameter
// ReleaseAlgorithm which is release policy template.
// This means, CSmartHandle template declaration contains
// nesting template. This code is compiled in Visual Stidio .NET 2003
// but not compiled in previous versions.
//
// Protected destructors prevent client code to release CSmartHandle
// using release pointer code (see Modern C++ Design, 
// 1.7  Destructors of Policy Classes).
//
// Additional PointedBy algorithm prevents misusing of the smart handle
// instance as smart pointer (using -> with handle is not compiled).



// Release algorithms (release policies)

template <typename T, class PointedBy>
struct CCloseHandle
{
    void Close(T handle)
    {
        CloseHandle(handle);
    }

protected:
    ~CCloseHandle()
    {
    }
};



template <typename T, class PointedBy>
struct CCloseRegKey
{
    void Close(T handle)
    {
        RegCloseKey(handle);
    }

protected:
    ~CCloseRegKey()
    {
    }
};


template <typename T, class PointedBy>
struct CCloseLibrary
{
    void Close(T handle)
    {
        FreeLibrary(handle);
    }

protected:
    ~CCloseLibrary()
    {
    }
};


template <typename T, class PointedBy>
struct CCloseViewOfFile
{
    void Close(T handle)
    {
        UnmapViewOfFile(handle);
    }

protected:
    ~CCloseViewOfFile()
    {
    }
};


// Empty class used as default CAutoHandle template parameter.
class CEmptyClass
{
};


// Class CSmartHandle which implements release policy.
// Second template parameter is ReleaseAlgorithm which is template itself.

template <typename HandleType, 
          template <class, class> class ReleaseAlgorithm, 
          class PointedBy = CEmptyClass,          // used for smart pointers
          HandleType NULL_VALUE = NULL>
class CSmartHandle : public ReleaseAlgorithm<HandleType, PointedBy>
{
public:
    CSmartHandle()
    {
        m_Handle = NULL_VALUE;
    }

    CSmartHandle(HandleType h)
    {
        m_Handle = h;
    }

    HandleType operator=(HandleType h) 
    { 
        CleanUp(); 
        m_Handle = h;
        return(*this);  
    }

    operator HandleType()
    {
        return m_Handle;
    }

    PointedBy* operator->()                 // for using as smart pointer
    {
        // NOTE: adding this operator allows to use CAutoHandle object as pointer.
        // However, if PointedBy is CHandlePlaceHolder (used for handles),
        // this is not compiled because CHandlePlaceHolder has no functions.
        // This is exactly what I need.
        return m_Handle;
    }

    bool IsValid()
    {
        return m_Handle != NULL_VALUE;
    }


    ~CSmartHandle()
    {
        CleanUp();
    }


    void CleanUp()
    {
        if ( m_Handle != NULL_VALUE )
        {
            Close(m_Handle);
            m_Handle = NULL_VALUE;
        }
    }

protected:
    HandleType m_Handle;
};


// Client code (definitions of standard Windows handles).
typedef CSmartHandle<HANDLE,  CCloseHandle, CEmptyClass, INVALID_HANDLE_VALUE>		                    CAutoGeneralHandle;
typedef CSmartHandle<HKEY,    CCloseRegKey>		                    CAutoRegKey;
typedef CSmartHandle<PVOID,   CCloseViewOfFile>	                    CAutoViewOfFile;
typedef CSmartHandle<HMODULE, CCloseLibrary>		                CAutoLibrary;
typedef CSmartHandle<HANDLE,  CCloseHandle, CEmptyClass, 
                    INVALID_HANDLE_VALUE>                           CAutoFile;


