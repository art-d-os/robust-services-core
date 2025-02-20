//==============================================================================
//
//  DipModule.cpp
//
//  Copyright (C) 2019  Greg Utas
//
//  Diplomacy AI Client - Part of the DAIDE project (www.daide.org.uk).
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
#include "DipModule.h"
#include "BotThread.h"
#include "BotTracer.h"
#include "Debug.h"
#include "DipProtocol.h"
#include "DipTypes.h"
#include "NwModule.h"
#include "Singleton.h"
#include "SysTypes.h"

using namespace NodeBase;
using namespace NetworkBase;

//------------------------------------------------------------------------------

namespace Diplomacy
{
bool DipModule::Registered = Register();

//------------------------------------------------------------------------------

fn_name DipModule_ctor = "DipModule.ctor";

DipModule::DipModule() : Module(DipModuleId)
{
   Debug::ft(DipModule_ctor);
}

//------------------------------------------------------------------------------

fn_name DipModule_Register = "DipModule.Register";

bool DipModule::Register()
{
   Debug::ft(DipModule_Register);

   //  Create the modules required by Diplomacy.
   //
   Singleton< NwModule >::Instance();
   Singleton< DipModule >::Instance();
   return true;
}

//------------------------------------------------------------------------------

fn_name DipModule_Shutdown = "DipModule.Shutdown";

void DipModule::Shutdown(RestartLevel level)
{
   Debug::ft(DipModule_Shutdown);

   Singleton< BotTcpService >::Instance()->Shutdown(level);
   Singleton< DipIpBufferPool >::Instance()->Shutdown(level);
}

//------------------------------------------------------------------------------

fn_name DipModule_Startup = "DipModule.Startup";

void DipModule::Startup(RestartLevel level)
{
   Debug::ft(DipModule_Startup);

   Singleton< DipIpBufferPool >::Instance()->Startup(level);
   Singleton< BotTcpService >::Instance()->Startup(level);
   Singleton< BotTracer >::Instance();
   Singleton< BotThread >::Instance()->Startup(level);
}
}
