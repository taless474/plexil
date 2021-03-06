/******************************************************************************
 *
 * PROJECT: IPC: Inter-Process Communication Package
 * 
 * (c) Copyright 2001 Reid Simmons.  All rights reserved.
 *
 * FILE: multiThread.h
 *
 * ABSTRACT: Enable IPC to deal with multi-threaded programs.
 *           Mutexes loosely based on David Apfelbaum's THREADS package.
 *
 * Copyright (c) 2008, Carnegie Mellon University
 *     This software is distributed under the terms of the 
 *     Simplified BSD License (see ipc/LICENSE.TXT)
 *
 * REVISION HISTORY
 *
 * $Log: multiThread.h,v $
 * Revision 2.2  2009/01/12 15:54:57  reids
 * Added BSD Open Source license info
 *
 * Revision 2.1  2002/01/03 20:52:14  reids
 * Version of IPC now supports multiple threads (Caveat: Currently only
 *   tested for Linux).
 * Also some minor changes to support Java version of IPC.
 *
 *
 * $Revision: 2.2 $
 * $Date: 2009/01/12 15:54:57 $
 * $Author: reids $
 *
 *****************************************************************************/

#ifndef INCmultiThread
#define INCmultiThread

#include <pthread.h>
#include <sys/time.h>

/* Deal with pthreads API differences here.
 *
 * In between Red Hat 5.2 and Red Hat 6.2, the invocation format
 * for setting the mutex type changed.  While the pthreads library
 * maintains a backward compatibility mode, there is a newer invocation format.
 *
 * For some reason, newer Linuxes don't support this POSIX-style variant
 * by default, so punt and go with the old API where available.
 */
#if defined ( REDHAT_6 )
#define PTHREAD_MUTEX_SETKIND pthread_mutexattr_settype
#define PTHREAD_MUTEX_RECURSIVE_KIND PTHREAD_MUTEX_RECURSIVE_NP
#elif defined ( REDHAT_52 )
#define PTHREAD_MUTEX_SETKIND pthread_mutexattr_setkind_np
#define PTHREAD_MUTEX_RECURSIVE_KIND PTHREAD_MUTEX_RECURSIVE_NP
#elif defined ( __APPLE__ )	/* Mac OS X */
#define PTHREAD_MUTEX_SETKIND pthread_mutexattr_settype
#define PTHREAD_MUTEX_RECURSIVE_KIND PTHREAD_MUTEX_RECURSIVE
#elif defined ( PTHREAD_CREATE_JOINABLE )	/* Try to guess it... */
#define PTHREAD_MUTEX_SETKIND pthread_mutexattr_settype
#define PTHREAD_MUTEX_RECURSIVE_KIND PTHREAD_MUTEX_RECURSIVE
/* This may produce a warning message, but the         *
 * pthreads library is, in fact, backwards compatible. */
#else
#define PTHREAD_MUTEX_SETKIND pthread_mutexattr_setkind_np
#define PTHREAD_MUTEX_RECURSIVE_KIND PTHREAD_MUTEX_RECURSIVE_NP
#endif

typedef struct {
  pthread_mutexattr_t  mutexAttributes;
  pthread_mutex_t      mutexData;
} MUTEX_TYPE, *MUTEX_PTR;

typedef struct {
  pthread_cond_t pingVar;
  MUTEX_TYPE     mutex;
} PING_THREAD_TYPE, *PING_THREAD_PTR;

typedef enum { Mutex_Success, Mutex_Failure, 
	       Mutex_Already_Locked } MUTEX_STATUS;

typedef enum { Ping_Success, Ping_Failure, Ping_Timeout } PING_STATUS;

MUTEX_STATUS initMutex(MUTEX_PTR mutex);
MUTEX_STATUS destroyMutex(MUTEX_PTR mutex);
MUTEX_STATUS lockMutex(MUTEX_PTR mutex);
MUTEX_STATUS unlockMutex(MUTEX_PTR mutex);
MUTEX_STATUS tryLockMutex(MUTEX_PTR mutex);

PING_STATUS initPing(PING_THREAD_PTR ping);
PING_STATUS pingThreads(PING_THREAD_PTR ping);
PING_STATUS waitForPing(PING_THREAD_PTR ping, struct timeval *timeout);

#endif /* INCmultiThread */
