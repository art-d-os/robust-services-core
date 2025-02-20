//==============================================================================
//
//  Editor.h
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
#ifndef EDITOR_H_INCLUDED
#define EDITOR_H_INCLUDED

#include <cstddef>
#include <list>
#include <string>
#include <vector>
#include "CodeTypes.h"
#include "CodeWarning.h"
#include "CxxFwd.h"
#include "SysTypes.h"

namespace NodeBase
{
   class CliThread;
}

using NodeBase::CliThread;
using NodeBase::word;
using std::string;

//------------------------------------------------------------------------------

namespace CodeTools
{
//  Source code editor.  It structures a file as follows:
//
//  <prolog>          comments containing file name and copyright notice
//  <#include-guard>  if a header file
//  <#include-bases>  #includes of headers that define base classes or
//                    declare functions that this file defines
//  <#include-exts>   #includes for external headers
//  <#include-ints>   #includes for internal headers
//  <forwards>        forward declarations
//  <usings>          using statements
//  <code>            declarations and/or definitions
//
class Editor
{
public:
   //  Creates an editor for the source code in FILE, whose code and
   //  warnings are loaded.  If the code couldn't be loaded, EXPL is
   //  updated with an explanation.
   //
   Editor(const CodeFile* file, std::string& expl);

   //  Not subclassed.
   //
   ~Editor() noexcept = default;

   //  Interactively fixes warnings in the code detected by Check().  If
   //  an error occurs, a non-zero value is returned and EXPL is updated
   //  with an explanation.
   //
   word Fix(CliThread& cli, const FixOptions& opts, string& expl);

   //  Formats the code.  Returns 0 if the file was unchanged, a positive
   //  number after successful changes, and a negative number on failure,
   //  in which case EXPL provides an explanation.
   //
   word Format(string& expl);

   //  Returns the number of commits made during >fix or >format commands.
   //
   static size_t CommitCount() { return Commits_; }

   //  Returns the log, if any, whose .warning matches LOG, whose .offset
   //  matches OFFSET, and whose .item matches ITEM.
   //
   CodeWarning* FindLog
      (const CodeWarning& log, const CxxNamed* item, word offset);
private:
   //  Writes out the editor's file.  Returns 0 if the file was successfully
   //  written; other values indicate failure.  Updates EXPL with a reason
   //  for any failure or a message that indicates which file was written.
   //
   word Write(string& expl);

   //  Returns the status of LOG.
   //  o NotFixed: will try to fix
   //  o Pending: previously fixed but not committed
   //  o Fixed: previously fixed and committed, or fixing not supported
   //
   WarningStatus FixStatus(const CodeWarning& log) const;

   //  Displays the code associated with LOG on the CLI.  FILE is set if the
   //  name of the file in which LOG occurs should be displayed.
   //
   void DisplayLog(const CliThread& cli, const CodeWarning& log, bool file);

   //  Fixes LOG.  Returns 0 on success.  A return value of -1 means that the
   //  file should be skipped; other values denote more serious errors.  EXPL
   //  is updated to provide any explanation, even when returning 0.  When the
   //  code has been edited, EXPL usually contains the revised line of code.
   //
   word FixWarning(const CodeWarning& log, string& expl);

   //  Invokes FixWarning if LOG's status is NotFixed and updates its status
   //  to Pending on success.
   //
   word FixLog(CodeWarning& log, string& expl);

   //  Most of the editing functions attempt to fix the warning reported in LOG,
   //  returning 0 on success.  Any other result indicates an error, in which
   //  case EXPL provides an explanation.  A return value of -1 means that the
   //  file should be skipped; other values denote more serious errors.
   //
   word AdjustLineIndentation(const CodeWarning& log, string& expl);
   word AdjustTags(const CodeWarning& log, string& expl);
   word AlignArgumentNames(const CodeWarning& log, string& expl);
   word ChangeClassToStruct(const CodeWarning& log, string& expl);
   word ChangeDebugFtName(const CodeWarning& log, string& expl);
   word ChangeStructToClass(const CodeWarning& log, string& expl);
   word EraseAdjacentSpaces(const CodeWarning& log, string& expl);
   word EraseAccessControl(const CodeWarning& log, string& expl);
   word EraseBlankLine(const CodeWarning& log, string& expl);
   word EraseConst(const CodeWarning& log, string& expl);
   word EraseEnumerator(const CodeWarning& log, string& expl);
   word EraseForward(const CodeWarning& log, string& expl);
   word EraseLineBreak(const CodeWarning& log, string& expl);
   word EraseMutableTag(const CodeWarning& log, string& expl);
   word EraseNoexceptTag(const CodeWarning& log, string& expl);
   word EraseOverrideTag(const CodeWarning& log, string& expl);
   word EraseSemicolon(const CodeWarning& log, string& expl);
   word EraseVirtualTag(const CodeWarning& log, string& expl);
   word EraseVoidArgument(const CodeWarning& log, string& expl);
   word InitByConstructor(const CodeWarning& log, string& expl);
   word InsertBlankLine(const CodeWarning& log, string& expl);
   word InsertDebugFtCall(const CodeWarning& log, string& expl);
   word InsertDefaultFunction(const CodeWarning& log, string& expl);
   word InsertForward(const CodeWarning& log, string& expl);
   word InsertInclude(const CodeWarning& log, string& expl);
   word InsertIncludeGuard(const CodeWarning& log, string& expl);
   word InsertLineBreak(const CodeWarning& log, string& expl);
   word InsertUsing(const CodeWarning& log, string& expl);
   word RenameIncludeGuard(const CodeWarning& log, string& expl);
   word ReplaceNull(const CodeWarning& log, string& expl);
   word ReplaceSlashAsterisk(const CodeWarning& log, string& expl);
   word ReplaceUsing(const CodeWarning& log, string& expl);
   word TagAsConstArgument(const CodeWarning& log, string& expl);
   word TagAsConstData(const CodeWarning& log, string& expl);
   word TagAsConstFunction(const CodeWarning& log, string& expl);
   word TagAsConstPointer(const CodeWarning& log, string& expl);
   word TagAsConstReference(const CodeWarning& log, string& expl);
   word TagAsDefaulted(const CodeWarning& log, string& expl);
   word TagAsExplicit(const CodeWarning& log, string& expl);
   word TagAsNoexcept(const CodeWarning& log, string& expl);
   word TagAsOverride(const CodeWarning& log, string& expl);
   word TagAsStaticFunction(const CodeWarning& log, string& expl);

   //  Erases the line of code referenced by LOG.
   //
   word EraseCode(const CodeWarning& log, string& expl);

   //  Erases the line of code referenced by LOG.  Comments on preceding
   //  lines, up to the next line of code, are also erased if a comment or
   //  right brace follows the erased code.  DELIMITERS are the characters
   //  where the code ends.
   //
   word EraseCode
      (const CodeWarning& log, const std::string& delimiters, string& expl);

   //  Sorts #include directives in standard order.
   //
   word SortIncludes(string& expl);

   //  Qualifies symbols so that using statements can be removed.
   //
   word ResolveUsings();

   //  Replaces multiple blank lines with a single blank line.  Always invoked
   //  on source that was changed.
   //
   word EraseBlankLinePairs();

   //  Removes trailing blanks.  Always invoked on source that was changed.
   //
   word EraseTrailingBlanks();

   //  Converts tabs to spaces.  Always invoked on source that was changed.
   //
   word ConvertTabsToBlanks();

   //  Stores a line of code.
   //
   struct SourceLine
   {
      SourceLine(const string& code, size_t line) noexcept :
         line(line), code(code) { }

      //  The code's line number (the first line is 0, the same as CodeWarning
      //  and Lexer).  A line added by the editor has a line number of SIZE_MAX.
      //
      const size_t line;

      //  The code.
      //
      string code;
   };

   //  The code is kept in a list.
   //
   typedef std::list< SourceLine > SourceList;

   //  Iterator for the source code.  This is an editor, so it doesn't bother
   //  with const iterators.
   //
   typedef std::list< SourceLine >::iterator Iter;

   //  Identifies a line of code and a position within that line.
   //
   struct CodeLocation
   {
      Iter iter;   // location in SourceList
      size_t pos;  // position in iter->code

      explicit CodeLocation(const Iter& i) noexcept :
         iter(i), pos(string::npos) { }

      CodeLocation(const Iter& i, size_t p) noexcept :
         iter(i), pos(p) { }
   };

   //  Returns the position after CURR.
   //
   CodeLocation NextPos(const CodeLocation& curr);

   //  Returns the position before CURR.
   //
   CodeLocation PrevPos(const CodeLocation& curr);

   //  Returns the type of line referenced by ITER.
   //
   LineType GetLineType(const Iter& iter) const;

   //  Reads in the file's source code.  Returns 0 on success.  Any other
   //  result indicates that the code could not be read, in which case EXPL
   //  is updated with an explanation.
   //
   word GetCode(std::string& expl);

   //  Adds a line of source code from the file.  NEVER used to add new code.
   //
   void PushBack(const string& code);

   //  Returns the location of LINE.  Returns source_.end() if LINE is
   //  not found.
   //
   Iter FindLine(size_t line);

   //  Returns the location of LINE.  If LINE is not found, sets EXPL to
   //  an error message and returns source_.end().
   //
   Iter FindLine(size_t line, string& expl);

   //  Converts POS in the original source code to a line number and
   //  and offset.
   //
   CodeLocation FindPos(size_t pos);

   //  Looks for STR starting at iter->code[off].  If STR is found, returns
   //  its location, else returns {source_.end(), string::npos}.
   //
   CodeLocation Find(Iter iter, const string& str, size_t off = 0);

   //  The same as Find(iter, s, off), but searches backwards.
   //
   CodeLocation Rfind(Iter iter, const string& str, size_t off = string::npos);

   //  Returns the location of ID, starting at iter->code[pos].  Returns
   //  {source_.end(), string::npos} if ID was not found.  ID must be an
   //  identifier or keyword that is delimited by punctuation.  The search
   //  spans RANGE lines; if RANGE is nullptr, only one line is searched.
   //
   CodeLocation FindWord
      (Iter iter, size_t pos, const string& id, size_t* range = nullptr);

   //  Returns the location of the first non-blank character starting at
   //  iter->code[pos].  Returns {source_.end(), string::npos} if no such
   //  character was found.
   //
   CodeLocation FindNonBlank(Iter iter, size_t pos);

   //  Returns the location of the first non-blank character starting at
   //  iter->code[pos], reversing.  Returns {source_.end(), string::npos}
   //  if no such character was found.
   //
   CodeLocation RfindNonBlank(Iter iter, size_t pos);

   //  Returns the first occurrence of a character in CHARS, starting at
   //  iter->code[off].  Returns {source_.end(), string::npos} if none
   //  of those characters was found.
   //
   CodeLocation FindFirstOf(Iter iter, size_t off, const string& chars);

   //  If LOG.ITEM is a function, returns the location of the right
   //  parenthesis at the end of the function's argument list.  Returns
   //  {source_.end(), string::npos} on failure.
   //
   CodeLocation FindArgsEnd(const CodeWarning& log);

   //  If LOG.ITEM is a function, returns the location of the semicolon
   //  at the end of its declaration or the left brace at the end of its
   //  signature.  Returns {source_.end(), string::npos} on failure.
   //
   CodeLocation FindSigEnd(const CodeWarning& log);

   //  Returns the line that follows FUNC.
   //
   Iter FindFuncEnd(const Function* func);

   //  Inserts CODE at ITER and returns its location.
   //
   Iter Insert(Iter iter, const string& code);

   //  Inserts PREFIX on the line identified by ITER, starting at POS.  The
   //  prefix replaces blanks but leaves at least one space between it and
   //  the first non-blank character on the line.
   //
   void InsertPrefix(const Iter& iter, size_t pos, const string& prefix);

   //  Inserts a line break before POS, indents the new line accordingly,
   //  and returns the location of the first non-blank character on the new
   //  line.  Returns {iter, pos} if a line break was not inserted because
   //  the new line would have been empty.
   //
   CodeLocation InsertLineBreak(const Iter& iter, size_t pos);

   //  Deletes the line break at the end of the line referenced by CURR if
   //  the following line will also fit within LineLengthMax.  Returns true
   //  if the line break was deleted.
   //
   bool EraseLineBreak(const Iter& curr);

   //  Indicates where a blank line should be added when inserting new code.
   //
   enum BlankLineLocation
   {
      BlankNone,
      BlankBefore,
      BlankAfter
   };

   //  Returns the location where code to fix LOG should be inserted.  Sets
   //  BLANK to where a blank line should be added to set off the code, and
   //  sets COMMENT if the code should be commented.
   //
   Iter FindInsertionPoint
      (const CodeWarning& log, BlankLineLocation& blank, bool& comment);

   //  Returns the first line that follows comments and blanks.
   //
   Iter PrologEnd();

   //  Returns the location of the first #include.  Returns source_.end() if
   //  no #include was found.
   //
   Iter IncludesBegin();

   //  Returns the location of the statement that follows the last #include.
   //  Returns source_.end() if the last line was an #include.
   //
   Iter IncludesEnd();

   //  Find the first line of code (other than #include directives, forward
   //  declarations, and using statements).  Moves up past any comments that
   //  precede this point, and returns the position where these comments
   //  begin.
   //
   Iter CodeBegin();

   //  Returns true if a comment or brace follows the code referenced by ITER.
   //
   const bool CommentOrBraceFollows(const Iter& iter) const;

   //  If INCLUDE specifies a file in groups 1 to 4 (see CodeFile.CalcGroup),
   //  this simplifies sorting by replacing the characters that enclose the
   //  file name.
   //
   word MangleInclude(string& include, string& expl) const;

   //  If CODE is an #include directive, unmanagles and returns it, else
   //  simply returns it without any changes.
   //
   string DemangleInclude(string code) const;

   //  Inserts an INCLUDE directive.
   //
   word InsertInclude(string& include, string& expl);

   //  Inserts a FORWARD declaration at ITER.
   //
   word InsertForward(const Iter& iter, const string& forward, string& expl);

   //  Inserts a FORWARD declaration at ITER.  It is the first declaration in
   //  namespace NSPACE, so it must be enclosed in a new namespace scope.
   //
   word InsertNamespaceForward(const Iter& iter,
      const string& nspace, const string& forward, string& expl);

   //  Invoked after removing a forward declaration.  If the declaration was
   //  in a namespace that is now empty, erases the "namespace <name> { }".
   //
   word EraseEmptyNamespace(const Iter& iter);

   //  Returns the items within ITEM that were accessed via a using statement.
   //
   CxxNamedSet FindUsingReferents(const CxxNamed* item) const;

   //  Qualifies names used within ITEM in order to remove using statements.
   //
   void QualifyUsings(const CxxNamed* item);

   //  Within ITEM, qualifies occurrences of REF.
   //
   void QualifyReferent(const CxxNamed* item, const CxxNamed* ref);

   //  Indents the code referenced by ITER.  SPLIT is set a line break was
   //  just inserted to create a new line.  Returns the new position of the
   //  first non-blank character.
   //
   size_t Indent(const Iter& iter, bool split);

   //  Supplies the code for a Debug::Ft fn_name definition and invocation.
   //
   void DebugFtCode
      (const Function* func, std::string& defn, std::string& call) const;

   //  Adds the editor to Editors_ and returns 0.
   //
   word Changed();

   //  Sets EXPL to iter->code, adds the editor to Editors_, and returns 0.
   //
   word Changed(const Iter& iter, std::string& expl);

   //  Comparison functions for sorting #include directives.
   //
   static bool IsSorted1(const SourceLine& line1, const SourceLine& line2);
   static bool IsSorted2(const string& line1, const string& line2);

   //  The file from which the source code was obtained.
   //
   const CodeFile* const file_;

   //  The number of lines read so far.
   //
   size_t line_;

   //  The source code.
   //
   SourceList source_;

   //  The result of reading the source code.  A non-zero value
   //  indicates that an error occurred when reading the code.
   //
   word read_;

   //  Set if the #include directives have been sorted.
   //
   bool sorted_;

   //  Set if type aliases for symbols that were resolved by a using
   //  statement have been added to each class.
   //
   bool aliased_;

   //  The file's warnings.
   //
   std::vector< CodeWarning > warnings_;

   //  The editors that have modified their original code.  This allows an
   //  editor to modify other files (e.g. when a fix requires changes in
   //  both a function's declaration and definition).  After the changes
   //  have been made, all modified files can be committed.
   //
   static std::set< Editor* > Editors_;

   //  The number of files committed so far.
   //
   static size_t Commits_;
};
}
#endif
