//==============================================================================
//
//  Daemon.h
//
//  Copyright (C) 2017  Greg Utas
//
//  This file is part of the Robust Services Core (RSC).
//
//  RSC is free software: you can redistribute it and/or modify it under the
//  terms of the GNU General Public License as published by the Free Software
//  Foundation, either version 3 of the License, or (at your option) any later
//  version.
//
//  RSC is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
//  details.
//
//  You should have received a copy of the GNU General Public License along
//  with RSC.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef DAEMON_H_INCLUDED
#define DAEMON_H_INCLUDED

#include "Permanent.h"
#include <cstddef>
#include <set>
#include <string>
#include "RegCell.h"
#include "SysTypes.h"

namespace NodeBase
{
   class Thread;
}

//------------------------------------------------------------------------------

namespace NodeBase
{
//  A daemon is a thread that doesn't exit, but the purpose of this class is
//  to monitor such a thread and recreate it if it traps and is forced to exit.
//  During initialization and restarts, modules create threads, and each thread
//  registers with its daemon.
//    Although heartbeating between threads and daemons was considered, it was
//  not implemented for the following reasons:
//  o Many threads run when interrupted to handle work.  If this occurs often,
//    heartbeating will be a larger overhead.  If it occurs rarely, the thread
//    may have to wake up just to send a heartbeat, even if it has no work to
//    do.  This is also an overhead.
//  o The primary purpose of heartbeating is to create a new thread when the
//    existing one fails send a heartbeat.  But given that a thread cannot exit
//    without its daemon being notified, the primary risk is a thread that gets
//    into an infinite loop.  However, threads usually run locked, and a locked
//    thread is signalled if it runs too long, so agaain hearbeating has little
//    additional value.
//
class Daemon : public Permanent
{
   friend class Registry< Daemon >;
public:
   //  Virtual to allow subclassing.  An instance would be deleted if
   //  the threads no longer need to be monitored and recreated.
   //
   virtual ~Daemon();

   //  Creates threads when there are fewer than size_.  May be
   //  invoked by a Module during initializations and restarts.
   //
   void CreateThreads();

   //  Invoked by a thread when it is created.
   //
   void ThreadCreated(Thread* thread);

   //  Invoked by a thread when it is deleted.
   //
   void ThreadDeleted(Thread* thread);

   //  Returns a string that identifies the daemon.
   //
   const std::string& Name() const { return name_; }

   //  Returns the offset to did_.
   //
   static ptrdiff_t CellDiff();

   //  Overridden to display member variables.
   //
   void Display(std::ostream& stream,
      const std::string& prefix, const Flags& options) const override;

   //  Overridden for patching.
   //
   void Patch(sel_t selector, void* arguments) override;
protected:
   //  Protected because this class is virtual.  SIZE is the number of
   //  threads to be created and monitored.
   //
   Daemon(fixed_string name, size_t size);
private:
   //  Creates a thread that this daemon will manage.
   //
   virtual Thread* CreateThread() = 0;

   //  The type for iterating over our threads.
   //
   typedef std::set< Thread* >::iterator Iterator;

   //  Finds the entry for THREAD.
   //
   Iterator Find(Thread* thread);

   //  The daemon's identifier.
   //
   const std::string name_;

   //  The daemon's index in DaemonRegistry.
   //
   RegCell did_;

   //  The number of threads to be created.
   //
   size_t size_;

   //  The threads.
   //
   std::set< Thread* > threads_;
};
}
#endif
