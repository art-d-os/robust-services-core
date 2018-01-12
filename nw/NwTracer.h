//==============================================================================
//
//  NwTracer.h
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
#ifndef NWTRACER_H_INCLUDED
#define NWTRACER_H_INCLUDED

#include "Permanent.h"
#include <cstddef>
#include <iosfwd>
#include "NbTypes.h"
#include "NwTypes.h"
#include "SysIpL3Addr.h"
#include "SysTypes.h"
#include "ToolTypes.h"

using namespace NodeBase;

//------------------------------------------------------------------------------

namespace NetworkBase
{
//  Interface for tracing messages to/from specific IP peers and ports.
//
class NwTracer : public Permanent
{
   friend class Singleton< NwTracer >;
public:
   //  Traces PEER according to STATUS.
   //
   TraceRc SelectPeer(const SysIpL3Addr& peer, TraceStatus status);

   //  Traces PORT according to STATUS.
   //
   TraceRc SelectPort(ipport_t port, TraceStatus status);

   //  Returns true if no peers are included or excluded.
   //
   bool PeersEmpty() const;

   //  Returns true if no ports are included or excluded.
   //
   bool PortsEmpty() const;

   //  Returns the trace status of PEER.
   //
   TraceStatus PeerStatus(const SysIpL3Addr& peer) const;

   //  Returns the trace status of PORT.
   //
   TraceStatus PortStatus(ipport_t port) const;

   //  Displays, in STREAM, everything that has been included or excluded.
   //
   void QuerySelections(std::ostream& stream) const;

   //  Removes everything of type FILTER that has been included or excluded.
   //
   TraceRc ClearSelections(FlagId filter);

   //  Determines wheter IPB, travelling in DIR, should be traced.
   //
   TraceStatus BuffStatus(const IpBuffer& ipb, MsgDirection dir) const;
private:
   //  Private because this singleton is not subclassed.
   //
   NwTracer();

   //  Private because this singleton is not subclassed.
   //
   ~NwTracer();

   //> The number of peers that can be specifically included or
   //  excluded from a trace.
   //
   static const size_t MaxPeerEntries = 8;

   //> The number of ports that can be specifically included or
   //  excluded from a trace.
   //
   static const size_t MaxPortEntries = 8;

   //  The trace status of a peer IP address.
   //
   struct PeerFilter
   {
      PeerFilter();
      PeerFilter(const SysIpL3Addr& a, TraceStatus s);

      SysIpL3Addr peer;    // peer
      TraceStatus status;  // whether included or excluded
   };

   //  The trace status of a host IP port.
   //
   struct PortFilter
   {
      PortFilter();
      PortFilter(ipport_t p, TraceStatus s);

      ipport_t port;        // host port
      TraceStatus status;   // whether included or excluded
   };

   //  If PEER is included or excluded, returns its index in peers_.
   //  Returns -1 if PEER is neither included nor excluded.
   //
   int FindPeer(const SysIpL3Addr& peer) const;

   //  If PORT is included or excluded, returns its index in ports_.
   //  Returns -1 if PORT is neither included nor excluded.
   //
   int FindPort(ipport_t port) const;

   //  A list of included or excluded peers.
   //
   PeerFilter peers_[MaxPeerEntries];

   //  A list of included or excluded ports.
   //
   PortFilter ports_[MaxPortEntries];
};
}
#endif
