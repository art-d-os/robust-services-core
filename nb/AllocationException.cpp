//==============================================================================
//
//  AllocationException.cpp
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
#include "AllocationException.h"
#include <ostream>
#include <string>
#include "Debug.h"

using std::ostream;
using std::string;

//------------------------------------------------------------------------------

namespace NodeBase
{
fn_name AllocationException_ctor = "AllocationException.ctor";

AllocationException::AllocationException(MemoryType type, size_t size) :
   Exception(true, 1),
   type_(type),
   size_(size)
{
   Debug::ft(AllocationException_ctor);
}

//------------------------------------------------------------------------------

fn_name AllocationException_dtor = "AllocationException.dtor";

AllocationException::~AllocationException() noexcept
{
   Debug::ft(AllocationException_dtor);
}

//------------------------------------------------------------------------------

void AllocationException::Display(ostream& stream, const string& prefix) const
{
   Exception::Display(stream, prefix);

   stream << prefix << "type : " << type_ << CRLF;
   stream << prefix << "size : " << size_ << CRLF;
}

//------------------------------------------------------------------------------

fixed_string AllocationExceptionExpl = "Allocation Failure";

const char* AllocationException::what() const noexcept
{
   return AllocationExceptionExpl;
}
}
