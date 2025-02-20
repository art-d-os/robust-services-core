//==============================================================================
//
//  NbAppIds.h
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
#ifndef NBAPPIDS_H_INCLUDED
#define NBAPPIDS_H_INCLUDED

#include "NbTypes.h"
#include "SysTypes.h"

//------------------------------------------------------------------------------

namespace NodeBase
{
//  Module identifiers.  A new module must define an identifier here.
//
//  Modules are currently initialized in ascending order of ModuleId.  The
//  order will eventually be based on that provided by Module::Dependencies
//  functions.  Until then, a new ModuleId must be inserted in the correct
//  location.  Renumbering existing identifiers or leaving gaps is OK.
//
                                           // namespace:      Module:
constexpr ModuleId NbModuleId = 1;         // NodeBase        NbModule
constexpr ModuleId NtModuleId = 2;         // NodeTools       NtModule
constexpr ModuleId CtModuleId = 3;         // CodeTools       CtModule
constexpr ModuleId NwModuleId = 4;         // NetworkBase     NwModule
constexpr ModuleId SbModuleId = 5;         // SessionBase     SbModule
constexpr ModuleId StModuleId = 6;         // SessionTools    StModule
constexpr ModuleId MbModuleId = 7;         // MediaBase       MbModule
constexpr ModuleId CbModuleId = 8;         // CallBase        CbModule
constexpr ModuleId PbModuleId = 9;         // PotsBase        PbModule
constexpr ModuleId OnModuleId = 10;        // OperationsNode  OnModule
constexpr ModuleId CnModuleId = 11;        // ControlNode     CnModule
constexpr ModuleId RnModuleId = 12;        // RoutingNode     RnModule
constexpr ModuleId SnModuleId = 13;        // ServiceNode     SnModule
constexpr ModuleId AnModuleId = 14;        // AccessNode      AnModule
constexpr ModuleId FirstAppModuleId = 15;  // start of applicaton modules

//------------------------------------------------------------------------------
//
//  Object pool identifiers.  A new object pool must define an identifier here.
//  Fixed identifiers are required so that it would be possible to serialize an
//  object in one software release and import it into another release.
//
enum ObjectPoolIds
{
   ThreadObjPoolId = 1,
   MsgBufferObjPoolId = 2,
   IpBufferObjPoolId = 3,
   SbIpBufferObjPoolId = 4,
   BtIpBufferObjPoolId = 5,
   ContextObjPoolId = 6,
   MessageObjPoolId = 7,
   MsgPortObjPoolId = 8,
   ProtocolSMObjPoolId = 9,
   TimerObjPoolId = 10,
   EventObjPoolId = 11,
   ServiceSMObjPoolId = 12,
   MediaEndptObjPoolId = 13,
   DipIpBufferObjPoolId = 14
};

//------------------------------------------------------------------------------
//
//  Reserved software debugging flags.  Ad hoc usage should begin with the
//  last FlagId defined here.
//
constexpr FlagId ThreadReenterFlag = 0;
constexpr FlagId ThreadCtorTrapFlag = 1;
constexpr FlagId ThreadRecoverTrapFlag = 2;
constexpr FlagId ShowToolProgress = 3;
constexpr FlagId CallTrapFlag = 4;
constexpr FlagId CipAlwaysOverIpFlag = 5;
constexpr FlagId CipIamTimeoutFlag = 6;
constexpr FlagId CipAlertingTimeoutFlag = 7;
constexpr FlagId DisableRootThreadFlag = 8;
constexpr FlagId ThreadRetrapFlag = 9;
constexpr FlagId FirstAppDebugFlag = 10;
}
#endif
