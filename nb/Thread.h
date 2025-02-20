//==============================================================================
//
//  Thread.h
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
#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "Pooled.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iosfwd>
#include <memory>
#include <string>
#include "Clock.h"
#include "NbTypes.h"
#include "Q1Way.h"
#include "RegCell.h"
#include "SysDecls.h"
#include "SysThread.h"
#include "SysTypes.h"
#include "ToolTypes.h"

namespace NodeBase
{
   class Daemon;
   class MsgBuffer;
   class SysMutex;
   class ThreadPriv;
   class ThreadStats;
}

//------------------------------------------------------------------------------

namespace NodeBase
{
//  Base class for threads.  All threads should subclass from this, as it
//  provides functions that support RSC's programming model.
//
class Thread : public Pooled
{
   friend class Debug;
   friend class Exception;
   friend class InitThread;
   friend class ModuleRegistry;
   friend class Registry< Thread >;
   friend class RootThread;
   friend class SchedCommand;
   friend class SysMutex;
   friend class ThreadRegistry;
public:
   //  Allows "Id" to refer to a thread identifier in this class hierarchy.
   //
   typedef ThreadId Id;

   //> Highest valid thread identifier.
   //
   static const Id MaxId;

   //  Returns the thread that is currently running.  Throws an exception
   //  if ASSERT is set and the running thread cannot be found.
   //
   static Thread* RunningThread(bool assert = true);

   //  Causes the current thread to run unpreemptably (run to completion).
   //  When a thread is entered, it is made unpreemptable before its Enter
   //  function is invoked.
   //
   static void MakeUnpreemptable();

   //  Causes the current thread to run preemptably.
   //
   static void MakePreemptable();

   //  Returns how long the running thread has run as a percentage (0 to 100)
   //  of the run-to-completion timeout.  Returns 0 for a preemptable thread.
   //
   static word RtcPercentUsed();

   //  Schedules the current thread out for MSECS.  If MSECS is TIMEOUT_IMMED,
   //  the thread is scheduled out but can run again immediately, although other
   //  threads may get to run first.  If MSECS is TIMEOUT_NEVER, the thread
   //  sleeps forever, although it can be interrupted.  Returns the reason why
   //  the pause ended.
   //
   static DelayRc Pause(msecs_t msecs = TIMEOUT_IMMED);

   //  Invokes Pause(TIMEOUT_IMMED) if RtcPercentUsed() returns LIMIT or more.
   //
   static void PauseOver(word limit);

   //  The number of msecs that the thread has run since it was scheduled in.
   //
   msecs_t MsecsSinceStart() const;

   //  Returns true if the thread has been scheduled to run.
   //
   bool IsScheduled() const;

   //  Awakens a thread if it has paused.  If it has not paused, it will be
   //  immediately reawakened if it pauses.  If it is blocked for some other
   //  reason, it remains blocked.  MASK can be used to set flags that the
   //  thread can access with Vector() when it resumes execution.  How these
   //  flags are interpreted is thread specific.
   //
   bool Interrupt(const Flags& mask = NoFlags);

   //  Returns the running thread's interrupt vector.
   //
   static std::atomic_uint32_t* Vector();

   //  Returns a flag in the running thread's interrupt vector.
   //
   static bool TestFlag(FlagId fid);

   //  Clears a flag in the running thread's interrupt vector.
   //
   static void ResetFlag(FlagId fid);

   //  Clears the flags in the running thread's interrupt vector.
   //
   static void ResetFlags();

   //  Must be called immediately before using a blocking operation.  WHY is
   //  the reason for blocking.  FUNC identifies the function that wants to
   //  call the blocking operation.  Returns false if the operation must not
   //  be performed.
   //
   static bool EnterBlockingOperation(BlockingReason why, fn_name_arg func);

   //  Must be called immediately after a blocking operation completes.
   //  FUNC identifies the function that has resumed execution.
   //
   static void ExitBlockingOperation(fn_name_arg func);

   //  Returns the reason, if any, that the thread is blocked.
   //
   BlockingReason GetBlockingReason() const;

   //  Write-enables memory that is normally write-protected.
   //
   static void MemUnprotect();

   //  Write-disables memory that is normally write-protected.
   //
   static void MemProtect();

   //  Returns the thread's identifier within ThreadRegistry.
   //
   Id Tid() const { return Id(tid_.GetId()); }

   //  Returns the native thread's identifier.
   //
   SysThreadId NativeThreadId() const;

   //  Returns the thread's scheduler faction.
   //
   Faction GetFaction() const { return faction_; }

   //  Changes the thread to FACTION.  Returns true on success.
   //
   bool ChangeFaction(Faction faction);

   //  Used to explicitly include or exclude the thread from a trace.
   //
   void SetStatus(TraceStatus status);

   //  Returns the thread's trace status.  To determine if the thread
   //  should actually be traced, use CalcStatus or FindStatus (below),
   //  which take additional criteria into account.
   //
   TraceStatus GetStatus() const;

   //  Calculates whether the thread should be traced.  DYNAMIC is true when
   //  the thread is running and deciding whether to trace work in progress;
   //  it is false when generating trace output, at which time trace records
   //  can be filtered to generate a subset report.
   //
   virtual TraceStatus CalcStatus(bool dynamic) const;

   //  Starts tracing unless it is already on.  The thread must be unpreemptable
   //  and must enable the desired trace tools and select the items to be traced
   //  before invoking this function.  If IMMEDIATE is set, the trace is output
   //  to a file as it is captured, which is useful when a crash is anticipated.
   //  If AUTOSTOP is set, tracing stops on the next context switch.
   //
   static TraceRc StartTracing(bool immediate, bool autostop);

   //  Stops a trace that was started by StartTracing.
   //
   static void StopTracing();

   //  Causes the thread to trap on SIGNAL.  If the thread is not critical,
   //  or if SIGNAL is final, the thread exits.  In other cases, the thread
   //  is reentered or recreated, resuming execution as if it was being
   //  entered for the first time.
   //
   void Raise(signal_t sig);

   //  Displays statistics.  May be overridden to include thread-specific
   //  statistics, but the base class version must be invoked.
   //
   virtual void DisplayStats(std::ostream& stream, const Flags& options) const;

   //  Returns the percentage of idle time during the most recent
   //  short interval.
   //
   static double PercentIdle();

   //  Invoked by SignalHandler (and, on Windows, SE_Handler) to handle SIG.
   //  CODE is for debugging.  Returns false if the running thread is unknown
   //  and SIG is not a break signal (that is, does not have PosixSignal::Break
   //  attribute set).
   //
   static bool HandleSignal(signal_t sig, uint32_t code);

   //  Returns a string containing the thread's class name and identifiers.
   //
   std::string to_str() const;

   //  Returns the offset to tid_.
   //
   static ptrdiff_t CellDiff();

   //  Overridden for restarts.  This is only invoked on threads that did
   //  not exit and survived the restart.
   //
   void Startup(RestartLevel level) override;

   //  Overridden for restarts.  This is only invoked on threads that did
   //  not exit when the restart began.
   //
   void Shutdown(RestartLevel level) override;

   //  Overridden to display member variables.
   //
   void Display(std::ostream& stream,
      const std::string& prefix, const Flags& options) const override;

   //  Overridden for patching.
   //
   void Patch(sel_t selector, void* arguments) override;

   //  Overridden to obtain a thread from its object pool.
   //
   static void* operator new(size_t size);
protected:
   //  Creates a thread that runs in the scheduler faction FACTION and
   //  is managed by DAEMON.  Protected because this class is virtual.
   //
   Thread(Faction faction, Daemon* daemon = nullptr);

   //  Protected to restrict deletion, which must always be done through
   //  Destroy, and exclusively within this class.
   //
   virtual ~Thread();

   //  Invoked as the last thing done in a leaf class constructor.  When
   //  a Thread is created, its native thread may actually start to run
   //  before the Thread has been fully constructed, so the thread will
   //  is held up until it has invoked this function.
   //
   void SetInitialized();

   //  Returns the number of milliseconds that the thread receives when it
   //  begins to run unpreemptably.  The default should only be overridden
   //  for compelling reasons.
   //
   virtual msecs_t InitialMsecs() const;

   //  Queues MSG for processing by the thread.  May be overridden, but the
   //  base class version must be invoked.  Protected so that a subclass can
   //  prevent direct access to its message queue.
   //
   virtual bool EnqMsg(MsgBuffer& msg);

   //  Dequeues the next message.  If no message is available, pauses for
   //  TIMEOUT to wait for a message.  May be overridden, but the base class
   //  version must be invoked.  Protected to restrict usage to subclasses.
   //
   virtual MsgBuffer* DeqMsg(msecs_t timeout);

   //  Dereferences a bad pointer during testing.
   //
   static void CauseTrap();

   //  Overridden to claim queued messages.  May be overridden, but this
   //  version must be invoked.
   //
   void Claim() override;

   //  Overridden to release resources during error recovery.  May be
   //  overridden, but this version must be invoked.
   //
   void Cleanup() override;

   //  Starts the next short interval for thread statistics.
   //
   static void StartShortInterval();
private:
   //  What to do after handling a signal or exception.
   //
   enum TrapAction
   {
      Continue,  // check if the thread should be recovered
      Release,   // return by invoking Exit
      Return     // return immediately
   };

   //  Returns an abbreviated version of the thread's name, which must be
   //  at most 7 characters long.
   //
   virtual c_string AbbrName() const = 0;

   //  The common entry function for all threads.  ARG is a pointer to
   //  the Thread object.
   //
   static main_t EnterThread(void* arg);

   //  Contains most of the entry code that is common to all threads.
   //  Implements the safety net that can recover from all exceptions.
   //
   main_t Start();

   //  The entry point to the thread.  If the thread needs an argument,
   //  it must be provided using a data member defined by the thread's
   //  subclass.  Before this function is invoked, every thread is made
   //  unpreemptable (see MakeUnpreemptable).  If this function returns,
   //  the thread is deleted and exits with a code of 0.
   //
   virtual void Enter() = 0;

   //  Invoked by EnterBlockingOperation to determine whether the thread is
   //  allowed to block.  The default version returns true.  This function
   //  exists to avoid making EnterBlockingOperation virtual.  That way, if
   //  a thread's vptr gets corrupted, EnterBlockingOperation will still be
   //  invoked.  WHY and FUNC are the same as for EnterBlockingOperation.
   //
   virtual bool BlockingAllowed
      (BlockingReason why, fn_name_arg func) { return true; }

   //  Invoked by ExitBlockingOperation to inform the thread that it is
   //  running again.  The default version does nothing.  This function's
   //  purpose is to effectively make ExitBlockingOperation virtual.  It is
   //  implemented as a separate function so that, if a thread's vptr gets
   //  corrupted, ExitBlockingOperation will still be invoked.  FUNC is the
   //  same as for ExitBlockingOperation.
   //
   virtual void ScheduledIn(fn_name_arg func) { }

   //  Invoked when a blocked thread will receive a signal (from Raise) that
   //  forces it to exit.  If possible, the thread should unblock itself so
   //  that it can exit before completing its blocking operation.  The default
   //  implementation does nothing and can be overridden as required.
   //
   virtual void Unblock();

   //  Invoked when a thread is reentered after a signal or exception.
   //  This allows the thread to clean up work in progress and
   //  o return false to simply exit the thread and delete it, after
   //    which its Daemon (if any) can recreate it, or
   //  o return true to reinvoke the thread's Enter function.
   //  The default version returns true and may be overridden as required.
   //  However, a thread is forced to exit if
   //  o it receives a signal that forces it to exit, or
   //  o it traps too often, or
   //  o it traps during trap recovery, except in this function.
   //
   virtual bool Recover();

   //  Invoked at the outset of a restart so that the thread can decide whether
   //  to exit or sleep until the restart is over.  The default version returns
   //  true.  Because it is desirable to delete and recreate all threads during
   //  a restart, this should only be overridden for compelling reasons.
   //
   virtual bool ExitOnRestart(RestartLevel level) const;

   //  Invoked to destroy a thread.  The default version simply invokes
   //  delete but may be overridden to properly delete a Singleton.
   //
   virtual void Destroy();

   //  Returns a flag in the thread's interrupt vector.  See also TestFlag.
   //
   bool Test(FlagId fid) const;

   //  Clears a flag in the thread's interrupt vector.  See also ResetFlag.
   //
   void Reset(FlagId fid);

   //  Invoked by a thread when it is ready to run.
   //
   void Ready();

   //  Preempts a running thread.
   //
   void Preempt();

   //  Schedules a thread out when it yields or blocks.
   //
   void Suspend();

   //  Schedules the next thread when this one is suspending.
   //
   void Schedule() const;

   //  Schedules another thread after a thread yields or blocks, or
   //  after a preemptable thread has run for its allotted time.
   //  Returns false if no thread is running or ready.
   //
   static bool SwitchContext();

   //  Selects the next thread to run.
   //
   static Thread* Select();

   //  Invoked to signal the thread to run.
   //
   void Proceed();

   //  Invoked by a thread when it resumes execution.  FUNC is from
   //  ExitBlockingOperation.
   //
   void Resume(fn_name_arg func);

   //  Kills the thread.
   //
   void Kill();

   //  Returns true if the thread is unpreemptable.
   //
   bool IsLocked() const;

   //  Returns true if the thread can be traced.
   //
   bool IsTraceable() const;

   //  Returns the active thread.
   //
   static Thread* ActiveThread();

   //  Sets the active thread to nullptr if it matches THR.
   //
   static void ClearActiveThread(const Thread* thr);

   //  Returns the active thread if it is running unpreemptably.
   //
   static Thread* LockedThread();

   //  Returns true if the thread is not blocked.
   //
   bool IsReady() const;

   //  Returns the thread's daemon.
   //
   Daemon* GetDaemon() const { return daemon_; }

   //  Notes that the thread is trying to acquire MUTEX, which is nullptr
   //  if the mutex has been acquired.
   //
   void UpdateMutex(SysMutex* mutex);

   //  Returns the mutex that the thread is trying to acquire.
   //
   SysMutex* BlockingMutex() const;

   //  Notes that the thread has acquired (if true) or released a mutex.
   //
   void UpdateMutexCount(bool acquired);

   //  Returns the thread's mutex count.
   //
   uint8_t MutexCount() const;

   //  Returns the priority associated with FACTION.  If FACTION is out of
   //  range, it is set to BackgroundFaction after generating a log.
   //
   static SysThread::Priority FactionToPriority(Faction& faction);

   //  Returns true if the thread can generate a software log.  Returns false
   //  if a nested software log is already being generated, which is likely to
   //  eventually cause a stack overflow if further nesting is not prevented.
   //
   static bool EnterSwLog();

   //  Invoked when a software log has been generated (all = false) or a trap
   //  has occurred (all = true).
   //
   static void ExitSwLog(bool all);

   //  Invoked to exit a thread when it is safe to do so.  Does nothing if it
   //  currently unsafe to exit the thread.  OFFSET indicates where the exit
   //  was forced.
   //
   void ExitIfSafe(debug32_t offset);

   //  The signal handler that is registered for all threads.  It turns a
   //  signal into a C++ exception that is caught in Thread::Start.
   //
   static void SignalHandler(signal_t sig);

   //  Registers SignalHandler against all native signals.
   //
   static void RegisterForSignals();

   //  Handles a trap (signal or exception).  EX is nullptr if the exception
   //  is not a subclass of Exception.  E is nullptr if the exception is not
   //  derived from exception.  SIG is SIGNIL unless EX is a SignalException.
   //  STACK contains the thread's stack, if available.
   //
   TrapAction TrapHandler(const Exception* ex,
      const std::exception* e, signal_t sig, const std::ostringstream* stack);

   //  The same arguments as TrapHandler.  Generates the trap log.  Returns
   //  true if the thread has exceeded the trap limit.
   //
   bool LogTrap(const Exception* ex,
      const std::exception* e, signal_t sig, const std::ostringstream* stack);

   //  Invoked at the outset of a restart.  It invokes ExitOnRestart (above)
   //  to see if the thread should exit now or sleep until the restart ends.
   //  Returns true if the thread plans to exit, and false if it will sleep.
   //
   bool Restarting(RestartLevel level);

   //  Coordinates activities associated with invoking FUNC, namely
   //  o capturing FUNC in a trace tool
   //  o throwing an exception when the running thread is to be trapped
   //  o checking stack usage by the running thread
   //
   static void FunctionInvoked(fn_name_arg func);

   //  Determines whether the running thread (THR) should be traced.  If THR
   //  is nullptr, it is updated to the running thread if this function must
   //  actually find the running thread to make its determination.
   //
   static bool TraceRunningThread(Thread*& thr);

   //  Returns the number of ticks still available to the locked thread.
   //
   ticks_t TicksLeft() const;

   //  Gives the running thread MSECS more time to run unpreemptably.
   //
   static void ExtendTime(msecs_t msecs);

   //  Invoked when a thread did not yield before the run-to-completion
   //  timer expired.
   //
   void RtcTimeout();

   //  Invoked to set or clear trap_.
   //
   void SetTrap(bool on);

   //  Invoked when a trap request is pending.
   //
   void TrapCheck();

   //  Invoked to check stack usage.
   //
   void StackCheck();

   //  Records the thread event associated with RID if the running thread (THR)
   //  is being traced.  If THR is nullptr, the running thread will be found.
   //  FUNC is the function in which the event occurred, and INFO (optional)
   //  provides debugging information.
   //
   static void Trace(Thread* thr, fn_name_arg func,
      TraceRecordId rid, word info = 0);

   //  Sets the signal to be raised or that is being handled.
   //
   void SetSignal(signal_t sig);

   //  Returns true if SIGNAL should be logged.
   //
   bool LogSignal(signal_t sig) const;

   //  Invoked when returning from Thread::Start.  SIG indicates why
   //  the thread is exiting.
   //
   main_t Exit(signal_t sig);

   //  Invoked by the destructor and Cleanup to free resources.  ORPHANED
   //  is set if the thread is not about to exit, in which case its native
   //  thread is registered as an orphan instead of being deleted.
   //
   void ReleaseResources(bool orphaned);

   //  Used during the shutdown phase of a restart to enable/disable the
   //  scheduling of specific factions.
   //
   static void RestrictFactions(bool enable);

   //  Displays a summary of the thread's statistics in STREAM.  TIME0 is
   //  the time that has elapsed during the current statistics measurement
   //  period.  TIME1 was the duration of the most recent short interval
   //  for thread statistics.
   //
   void DisplaySummary
      (std::ostream& stream, usecs_t time0, usecs_t time1) const;

   //  Displays a summary of all threads' statistics in STREAM.
   //
   static void DisplaySummaries(std::ostream& stream);

   //  Starts (stops) logging context switches if ON is true (false).
   //
   static TraceRc LogContextSwitches(bool on);

   //  Logs information about a context switch.
   //
   void LogContextSwitch() const;

   //  Displays context switches in STREAM.
   //
   static void DisplayContextSwitches(std::ostream& stream);

   //  The wrapper for the native thread.
   //
   std::unique_ptr< SysThread > systhrd_;

   //  The thread's manager, if any.
   //
   Daemon* daemon_;

   //  The thread's identifier in ThreadRegistry.
   //
   RegCell tid_;

   //  The thread's scheduler faction.
   //
   Faction faction_;

   //  Set when the thread has initialized.
   //
   bool initialized_;

   //  Set if the thread has been deleted.  This must be defined in
   //  the main Thread object, which resides in an ObjectPool block
   //  and can therefore be referenced for a while after the thread
   //  has been deleted.
   //
   bool deleted_;

   //  The thread's message queue.
   //
   Q1Way< MsgBuffer > msgq_;

   //  Per-thread data that is not required in the header.
   //
   std::unique_ptr< ThreadPriv > priv_;

   //  Per-thread statistics.
   //
   std::unique_ptr< ThreadStats > stats_;
};
}
#endif
