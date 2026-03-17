#ifndef I3_MUTEX_H
#define I3_MUTEX_H

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

// Requisito indispensable para Windows
#if defined(I3_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
typedef CRITICAL_SECTION I3MUTEX;
#else
    // Placeholder para otras plataformas (p.ej. Linux/POSIX)
    // typedef pthread_mutex_t I3MUTEX; 
typedef int I3MUTEX;
#endif

// --- Clase Mutex Pesada (con Metadatos) ---
class I3_EXPORT_BASE i3Mutex : public i3ElementBase
{
    I3_CLASS_DEFINE(i3Mutex);

protected:
    I3MUTEX m_Mutex;

public:
    i3Mutex();
    virtual ~i3Mutex();

    void Enter();
    BOOL TryEnter();
    void Leave();
};

// --- Clase Mutex Ligera (Sin herencia pesada) ---
class I3_EXPORT_BASE i3Mutex2
{
protected:
    I3MUTEX m_Mutex;

public:
    i3Mutex2();
    ~i3Mutex2();

    void Enter();
    BOOL TryEnter();
    void Leave();
};

// --- Monitor RAII (Scoped Lock) ---
class I3_EXPORT_BASE i3Monitor2
{
protected:
    i3Mutex2* m_pMutex;

public:
    // Recibe el puntero y bloquea automáticamente
    explicit i3Monitor2(i3Mutex2* pMutex);
    // Libera automáticamente al salir del scope
    ~i3Monitor2();
};

#endif // I3_MUTEX_H