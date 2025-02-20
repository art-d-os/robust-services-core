//==============================================================================
//
//  StModule.cpp
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
#include "StModule.h"
#include "Debug.h"
#include "NbAppIds.h"
#include "NtModule.h"
#include "SbAppIds.h"
#include "SbModule.h"
#include "Singleton.h"
#include "StIncrement.h"
#include "SymbolRegistry.h"
#include "SysTypes.h"
#include "TestSessions.h"

using namespace NodeTools;
using namespace SessionBase;

//------------------------------------------------------------------------------

namespace SessionTools
{
bool StModule::Registered = Register();

//------------------------------------------------------------------------------

fn_name StModule_ctor = "StModule.ctor";

StModule::StModule() : Module(StModuleId)
{
   Debug::ft(StModule_ctor);
}

//------------------------------------------------------------------------------

fn_name StModule_dtor = "StModule.dtor";

StModule::~StModule()
{
   Debug::ft(StModule_dtor);
}

//------------------------------------------------------------------------------

fn_name StModule_Register = "StModule.Register";

bool StModule::Register()
{
   Debug::ft(StModule_Register);

   //  Create the modules required by SessionTools.
   //
   Singleton< SbModule >::Instance();
   Singleton< NtModule >::Instance();
   Singleton< StModule >::Instance();
   return true;
}

//------------------------------------------------------------------------------

fn_name StModule_Shutdown = "StModule.Shutdown";

void StModule::Shutdown(RestartLevel level)
{
   Debug::ft(StModule_Shutdown);

   Singleton< StIncrement >::Instance()->Shutdown(level);
   Singleton< TestFactory >::Instance()->Shutdown(level);
   Singleton< TestService >::Instance()->Shutdown(level);
   Singleton< TestProtocol >::Instance()->Shutdown(level);
}

//------------------------------------------------------------------------------

fn_name StModule_Startup = "StModule.Startup";

void StModule::Startup(RestartLevel level)
{
   Debug::ft(StModule_Startup);

   Singleton< TestProtocol >::Instance()->Startup(level);
   Singleton< TestService >::Instance()->Startup(level);
   Singleton< TestFactory >::Instance()->Startup(level);
   Singleton< StIncrement >::Instance()->Startup(level);

   //  Define symbols.
   //
   if(level < RestartCold) return;

   auto reg = Singleton< SymbolRegistry >::Instance();
   reg->BindSymbol("factory.test", TestFactoryId);
}
}