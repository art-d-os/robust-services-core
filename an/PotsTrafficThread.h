//==============================================================================
//
//  PotsTrafficThread.h
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
#ifndef POTSTRAFFICTHREAD_H_INCLUDED
#define POTSTRAFFICTHREAD_H_INCLUDED

#include "Thread.h"
#include <cstddef>
#include <iosfwd>
#include <string>
#include "BcAddress.h"
#include "Clock.h"
#include "NbTypes.h"
#include "SysTypes.h"

using namespace NodeBase;
using namespace CallBase;

namespace PotsBase
{
   class TrafficCall;
}

//------------------------------------------------------------------------------

namespace PotsBase
{
//  Thread for running POTS calls to test the system under load.
//
class PotsTrafficThread : public Thread
{
   friend class Singleton< PotsTrafficThread >;
public:
   //  The maximum call rate that can be supported.  It is based on the
   //  number of DNs that are available (Address::LastDN - StartDN) and
   //  HoldingTimeSecs, as well as wanting about 33% of DNs to be idle
   //  at any given time.
   //
   static const size_t MaxCallsPerMin;

   //  Criteria used when searching for a DN.
   //
   enum DnStatus
   {
      Unassigned,  // no circuit
      Assigned,    // idle or busy
      Idle,        // idle circuit
      Busy         // busy circuit
   };

   //  Sets the number of calls to be generated per minute.
   //
   void SetRate(word rate);

   //  Returns the number of calls to be generated per minute.
   //
   word GetRate() const { return callsPerMin_; }

   //  Displays status information.
   //
   void Query(std::ostream& stream) const;

   //  Returns a DN with the specified STATUS.
   //
   Address::DN FindDn(DnStatus status) const;

   //  Displays the number of traffic calls in each state.
   //
   static void DisplayStateCounts
      (std::ostream& stream, const std::string& prefix);

   //  Records the number of SECS that a POTS line was active on a call.
   //
   void RecordHoldingTime(secs_t secs);

   //  Records an aborted call.
   //
   void RecordAbort() { ++aborts_; }

   //  Overridden to display member variables.
   //
   void Display(std::ostream& stream,
      const std::string& prefix, const Flags& options) const override;
private:
   //  The frequency at which the thread wakes up to send messages when
   //  generating traffic.
   //
   static const msecs_t MsecsPerTick;

   //  The longest time horizon at which a future event can be scheduled.
   //
   static const secs_t MaxDelaySecs;

   //  The number of entries in the timewheel.  Successive entries are
   //  processed every MsecsPerTick.
   //
   static const size_t NumOfSlots;

   //  The first DN that will be allocated for running traffic.  It is
   //  assumed that all DNs between this one and Address::LastDN can be
   //  allocated.
   //
   static const Address::DN StartDN;

   //  The average call holding time, which can be found using the
   //  >traffic query command.
   //
   static const secs_t HoldingTimeSecs;

   //  The average number of POTS lines involved in 100 calls, which
   //  can be found using the >traffic query command.
   //
   static const size_t DNsPer100Calls;

   //  Private because this singleton is not subclassed.
   //
   PotsTrafficThread();

   //  Private because this singleton is not subclassed.
   //
   ~PotsTrafficThread();

   //  Creates new calls and progresses existing calls.
   //
   void SendMessages();

   //  Invoked when a call has progressed to its next state and wants to
   //  delay for MSECS.  If MSECS is 0, the call is deleted, else it is
   //  queued the timeslot that will be reached in MSECS.
   //
   void Enqueue(TrafficCall& call, msecs_t delay);

   //  Releases the resources that were allocated to run traffic.
   //
   void Takedown();

   //  Overridden to return a name for the thread.
   //
   c_string AbbrName() const override;

   //  Overridden to send messages to calls.
   //
   void Enter() override;

   //  Overridden to essentially run until we have no work remaining.
   //
   msecs_t InitialMsecs() const override;

   //  Overridden to survive warm restarts.
   //
   bool ExitOnRestart(RestartLevel level) const override;

   //  Overridden to delete the singleton.
   //
   void Destroy() override;

   //  The frequency at which the thread is waking up to perform work.
   //
   msecs_t timeout_;

   //  The number of calls to generate per minute.
   //
   word callsPerMin_;

   //  The maximum number of calls to generate during each tick.  It
   //  is set to twice the target rate.
   //
   size_t maxCallsPerTick_;

   //  The fractional number of calls (in thousandths) to generate
   //  during each tick.
   //
   size_t milCallsPerTick_;

   //  The first DN created for running traffic.
   //
   Address::DN firstDN_;

   //  The last DN created for running traffic.
   //
   Address::DN lastDN_;

   //  The timeslot in which work is currently being performed.
   //
   size_t currSlot_;

   //  The total number of calls created.
   //
   word totalCalls_;

   //  The number of active calls.
   //
   word activeCalls_;

   //  The total holding times for all POTS lines.
   //
   word totalTimes_;

   //  The number of holding times that were reported.
   //
   word totalReports_;

   //  The number of times an idle DN could not be found to originate
   //  a call.
   //
   word overflows_;

   //  The number of times a call was aborted because the traffic thread
   //  did not have enough time to do its work, resulting in a timeout
   //  in the POTS call.
   //
   word aborts_;

   //  Each active call is queued against the timeslot in which it will
   //  decide what to do next (typically, to send a messsage).
   //
   Q1Way< TrafficCall >* timewheel_;
};
}
#endif
