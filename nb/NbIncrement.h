//==============================================================================
//
//  NbIncrement.h
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
#ifndef NBINCREMENT_H_INCLUDED
#define NBINCREMENT_H_INCLUDED

#include "CliCommand.h"
#include "CliIncrement.h"
#include "CliTextParm.h"
#include "NbTypes.h"
#include "SysTypes.h"

//------------------------------------------------------------------------------

namespace NodeBase
{
//  Displays system status information.  Defined here so that other increments
//  can subclass it.
//
class StatusCommand : public CliCommand
{
public:
   StatusCommand();
   virtual ~StatusCommand() = default;
protected:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Interfaces to the log subsystem.  Defined here so that other increments
//  can subclass it.
//
class LogsAction : public CliTextParm
{
public:
   LogsAction();
   virtual ~LogsAction() = default;
};

class LogsCommand : public CliCommand
{
public:
   static const id_t ListIndex = 1;
   static const id_t ExplainIndex = 2;
   static const id_t SuppressIndex = 3;
   static const id_t ThrottleIndex = 4;
   static const id_t CountIndex = 5;
   static const id_t BuffersIndex = 6;
   static const id_t WriteIndex = 7;
   static const id_t FreeIndex = 8;
   static const id_t LastNbIndex = 8;

   //  Set BIND to false if binding a subclass of LogsAction.
   //
   explicit LogsCommand(bool bind = true);
   virtual ~LogsCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Configures trace tools.  Defined here so that other increments can
//  subclass it.
//
class SetWhatParm : public CliTextParm
{
public:
   SetWhatParm();
   virtual ~SetWhatParm() = default;
};

class SetCommand : public CliCommand
{
public:
   static const id_t SetToolListIndex = 1;
   static const id_t SetBuffSizeIndex = 2;
   static const id_t LastNbIndex      = 2;

   //  Set BIND to false if binding a subclass of SetWhatParm.
   //
   explicit SetCommand(bool bind = true);
   virtual ~SetCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Includes an item in a trace.  Defined here so that other increments can
//  subclass it.
//
class IncludeWhatParm : public CliTextParm
{
public:
   IncludeWhatParm();
   virtual ~IncludeWhatParm() = default;
};

class IncludeCommand : public CliCommand
{
public:
   static const id_t IncludeAllIndex     = 1;
   static const id_t IncludeFactionIndex = 2;
   static const id_t IncludeThreadIndex  = 3;
   static const id_t LastNbIndex         = 3;

   //  Set BIND to false if binding a subclass of IncludeWhatParm.
   //
   explicit IncludeCommand(bool bind = true);
   virtual ~IncludeCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Excludes an item from a trace.  Defined here so that other increments can
//  subclass it.
//
class ExcludeWhatParm : public CliTextParm
{
public:
   ExcludeWhatParm();
   virtual ~ExcludeWhatParm() = default;
};

class ExcludeCommand : public CliCommand
{
public:
   static const id_t ExcludeFactionIndex = 1;
   static const id_t ExcludeThreadIndex  = 2;
   static const id_t LastNbIndex         = 2;

   //  Set BIND to false if binding a subclass of ExcludeWhatParm.
   //
   explicit ExcludeCommand(bool bind = true);
   virtual ~ExcludeCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Queries trace tool settings.  Defined here so that other increments can
//  subclass it.
//
class QueryWhatParm : public CliTextParm
{
public:
   QueryWhatParm();
   virtual ~QueryWhatParm() = default;
};

class QueryCommand : public CliCommand
{
public:
   static const id_t BufferIndex     = 1;
   static const id_t ToolsIndex      = 2;
   static const id_t SelectionsIndex = 3;
   static const id_t LastNbIndex     = 3;

   //  Set BIND to false if binding a subclass of QueryWhatParm.
   //
   explicit QueryCommand(bool bind = true);
   virtual ~QueryCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Saves data captured by trace tools in a file.  Defined here so that other
//  increments can subclass it.
//
class SaveWhatParm : public CliTextParm
{
public:
   SaveWhatParm();
   virtual ~SaveWhatParm() = default;
};

class SaveCommand : public CliCommand
{
public:
   static const id_t TraceIndex  = 1;
   static const id_t LastNbIndex = 1;

   //  Set BIND to false if binding a subclass of SaveWhatParm.
   //
   explicit SaveCommand(bool bind = true);
   virtual ~SaveCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  Disables a trace tool or clears the trace buffer or an item selected for
//  tracing.  Defined here so that other increments can subclass it.
//
class ClearWhatParm : public CliTextParm
{
public:
   ClearWhatParm();
   virtual ~ClearWhatParm() = default;
};

class ClearCommand : public CliCommand
{
public:
   static const id_t BufferIndex     = 1;
   static const id_t ToolsIndex      = 2;
   static const id_t SelectionsIndex = 3;
   static const id_t FactionIndex    = 4;
   static const id_t FactionsIndex   = 5;
   static const id_t ThreadIndex     = 6;
   static const id_t ThreadsIndex    = 7;
   static const id_t LastNbIndex     = 7;

   //  Set BIND to false if binding a subclass of ClearWhatParm.
   //
   explicit ClearCommand(bool bind = true);
   virtual ~ClearCommand() = default;
protected:
   word ProcessSubcommand(CliThread& cli, id_t index) const override;
private:
   word ProcessCommand(CliThread& cli) const override;
};

//------------------------------------------------------------------------------
//
//  The increment that provides basic CLI commands.
//
class NbIncrement : public CliIncrement
{
   friend class Singleton< NbIncrement >;
private:
   //  Private because this singleton is not subclassed.
   //
   NbIncrement();

   //  Private because this singleton is not subclassed.
   //
   ~NbIncrement();
};
}
#endif
