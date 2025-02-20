//==============================================================================
//
//  Cxx.cpp
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
#include "Cxx.h"
#include <cctype>
#include <cstring>
#include <iomanip>
#include <ios>
#include <sstream>
#include "CodeFile.h"
#include "CxxArea.h"
#include "CxxCharLiteral.h"
#include "CxxDirective.h"
#include "CxxNamed.h"
#include "CxxRoot.h"
#include "CxxScope.h"
#include "CxxScoped.h"
#include "CxxStatement.h"
#include "CxxStrLiteral.h"
#include "CxxSymbols.h"
#include "CxxToken.h"
#include "Debug.h"
#include "Formatters.h"
#include "Library.h"
#include "Registry.h"
#include "Singleton.h"
#include "SysTypes.h"

using std::ostream;
using std::setw;
using std::string;
using namespace NodeBase;

//------------------------------------------------------------------------------

namespace CodeTools
{
fixed_string AccessStrings[Cxx::Access_N + 1] =
{
   PRIVATE_STR,
   PROTECTED_STR,
   PUBLIC_STR,
   ERROR_STR
};

ostream& Cxx::operator<<(ostream& stream, Access access)
{
   if((access >= 0) && (access < Access_N))
      stream << AccessStrings[access];
   else
      stream << AccessStrings[Access_N];
   return stream;
}

//------------------------------------------------------------------------------

fixed_string ClassTagStrings[Cxx::ClassTag_N + 1] =
{
   TYPENAME_STR,
   CLASS_STR,
   STRUCT_STR,
   UNION_STR,
   ERROR_STR
};

ostream& Cxx::operator<<(ostream& stream, ClassTag tag)
{
   if((tag >= 0) && (tag < Cxx::ClassTag_N))
      stream << ClassTagStrings[tag];
   else
      stream << ClassTagStrings[ClassTag_N];
   return stream;
}

//------------------------------------------------------------------------------

fixed_string EncodingStrings[Cxx::Encoding_N + 1] =
{
   "",
   "u8",
   "u",
   "U",
   "L",
   ERROR_STR
};

ostream& Cxx::operator<<(ostream& stream, Encoding code)
{
   if((code >= 0) && (code < Cxx::Encoding_N))
      stream << EncodingStrings[code];
   else
      stream << EncodingStrings[Encoding_N];
   return stream;
}

//------------------------------------------------------------------------------

string CharString(uint32_t c, bool s)
{
   switch(c)
   {
   case 0x00: return "\\0";
   case 0x07: return "\\a";
   case 0x08: return "\\b";
   case 0x0c: return "\\f";
   case 0x0a: return "\\n";
   case 0x0d: return "\\r";
   case 0x09: return "\\t";
   case 0x0b: return "\\v";
   case BACKSLASH: return "\\\\";
   case QUOTE:
      if(s) return "\\\"";
      break;
   case APOSTROPHE:
      if(!s) return "\\'";
      break;
   }

   if((c >= 32) && (c <= 126))
   {
      return string(1, c);  // displayable, not escaped
   }

   std::ostringstream stream;
   stream << std::hex << std::setfill('0');

   if(c <= UINT8_MAX)
      stream << BACKSLASH << 'x' << setw(2) << uint32_t(c);
   else if(c <= UINT16_MAX)
      stream << BACKSLASH << 'u' << setw(4) << uint32_t(c);
   else
      stream << BACKSLASH << 'U' << setw(8) << uint32_t(c);

   return stream.str();
}

//==============================================================================

const bool F = false;
const bool T = true;

const CxxWord CxxWord::Attrs[Cxx::NIL_KEYWORD + 1] =
{
   //      file   class   func  advance
   CxxWord("-",   "-",    "D",  F),  // AUTO
   CxxWord("-",   "-",    "b",  T),  // BREAK
   CxxWord("-",   "-",    "c",  T),  // CASE
   CxxWord("C",   "C",    "-",  T),  // CLASS
   CxxWord("DP",  "DP",   "D",  F),  // CONST
   CxxWord("DP",  "DP",   "D",  F),  // CONSTEXPR
   CxxWord("-",   "-",    "n",  T),  // CONTINUE
   CxxWord("-",   "-",    "o",  T),  // DEFAULT
   CxxWord("-",   "-",    "d",  T),  // DO
   CxxWord("E",   "E",    "E",  T),  // ENUM
   CxxWord("-",   "P",    "-",  F),  // EXPLICIT
   CxxWord("DP",  "-",    "-",  F),  // EXTERN
   CxxWord("-",   "-",    "-",  F),  // FINAL
   CxxWord("-",   "-",    "f",  T),  // FOR
   CxxWord("-",   "F",    "-",  T),  // FRIEND
   CxxWord("H",   "H",    "H",  F),  // HASH
   CxxWord("-",   "-",    "i",  T),  // IF
   CxxWord("P",   "P",    "-",  F),  // INLINE
   CxxWord("-",   "D",    "-",  F),  // MUTABLE
   CxxWord("N",   "-",    "-",  T),  // NAMESPACE
   CxxWord("-",   "P",    "-",  F),  // OPERATOR
   CxxWord("-",   "-",    "-",  F),  // OVERRIDE
   CxxWord("-",   "A",    "-",  T),  // PRIVATE
   CxxWord("-",   "A",    "-",  T),  // PROTECTED
   CxxWord("-",   "A",    "-",  T),  // PUBLIC
   CxxWord("-",   "-",    "r",  T),  // RETURN
   CxxWord("D",   "DP",   "D",  F),  // STATIC
   CxxWord("C",   "C",    "-",  T),  // STRUCT
   CxxWord("-",   "-",    "s",  T),  // SWITCH
   CxxWord("DCP", "DCFP", "-",  F),  // TEMPLATE
   CxxWord("-",   "-",    "t",  T),  // TRY
   CxxWord("T",   "T",    "T",  T),  // TYPEDEF
   CxxWord("C",   "C",    "-",  T),  // UNION
   CxxWord("U",   "U",    "U",  T),  // USING
   CxxWord("-",   "P",    "-",  F),  // VIRTUAL
   CxxWord("-",   "-",    "w",  T),  // WHILE
   CxxWord("-",   "P",    "-",  F),  // NVDTOR
   CxxWord("DP",  "DP",   "xD", F)   // NIL_KEYWORD
};

//------------------------------------------------------------------------------

fn_name CxxWord_ctor = "CxxWord.ctor";

CxxWord::CxxWord
   (const string& file, const string& cls, const string& func, bool adv) :
   fileTarget(file),
   classTarget(cls),
   funcTarget(func),
   advance(adv)
{
   Debug::ft(CxxWord_ctor);
}

//==============================================================================

const CxxOp CxxOp::Attrs[Cxx::NIL_OPERATOR + 1] =
{
   //                    str arg pri ovl rl sym
   CxxOp(           SCOPE_STR, 2, 18, F, F, F),  // SCOPE_RESOLUTION
   CxxOp(                 ".", 2, 17, F, F, F),  // REFERENCE_SELECT
   CxxOp(                "->", 2, 17, T, F, F),  // POINTER_SELECT
   CxxOp(                 "[", 2, 17, T, F, F),  // ARRAY_SUBSCRIPT
   CxxOp(                 "(", 0, 17, F, F, F),  // FUNCTION_CALL
   CxxOp(                "++", 1, 17, T, F, F),  // POSTFIX_INCREMENT
   CxxOp(                "--", 1, 17, T, F, F),  // POSTFIX_DECREMENT
   CxxOp(         DEFINED_STR, 1, 17, F, F, F),  // DEFINED
   CxxOp(          TYPEID_STR, 1, 17, F, F, F),  // TYPE_NAME
   CxxOp(      CONST_CAST_STR, 2, 17, F, F, F),  // CONST_CAST
   CxxOp(    DYNAMIC_CAST_STR, 2, 17, F, F, F),  // DYNAMIC_CAST
   CxxOp(REINTERPRET_CAST_STR, 2, 17, F, F, F),  // REINTERPRET_CAST
   CxxOp(     STATIC_CAST_STR, 2, 17, F, F, F),  // STATIC_CAST
   CxxOp(          SIZEOF_STR, 1, 16, F, T, F),  // SIZEOF_TYPE
   CxxOp(        NOEXCEPT_STR, 1, 16, F, T, F),  // NOEXCEPT
   CxxOp(                "++", 1, 16, T, T, F),  // PREFIX_INCREMENT
   CxxOp(                "--", 1, 16, T, T, F),  // PREFIX_DECREMENT
   CxxOp(                 "~", 1, 16, T, T, F),  // ONES_COMPLEMENT
   CxxOp(                 "!", 1, 16, T, T, F),  // LOGICAL_NOT
   CxxOp(                 "+", 1, 16, T, T, F),  // UNARY_PLUS
   CxxOp(                 "-", 1, 16, T, T, F),  // UNARY_MINUS
   CxxOp(                 "&", 1, 16, T, T, F),  // ADDRESS_OF
   CxxOp(                 "*", 1, 16, T, T, F),  // INDIRECTION
   CxxOp(             NEW_STR, 0, 16, T, T, F),  // OBJECT_CREATE
   CxxOp(       NEW_ARRAY_STR, 0, 16, T, T, F),  // OBJECT_CREATE_ARRAY
   CxxOp(          DELETE_STR, 1, 16, T, T, F),  // OBJECT_DELETE
   CxxOp(    DELETE_ARRAY_STR, 1, 16, T, T, F),  // OBJECT_DELETE_ARRAY
   CxxOp(                 "(", 2, 16, T, T, F),  // CAST
   CxxOp(                ".*", 2, 15, F, F, F),  // REFERENCE_SELECT_MEMBER
   CxxOp(               "->*", 2, 15, T, F, F),  // POINTER_SELECT_MEMBER
   CxxOp(                 "*", 2, 14, T, F, T),  // MULTIPLY
   CxxOp(                 "/", 2, 14, T, F, F),  // DIVIDE
   CxxOp(                 "%", 2, 14, T, F, F),  // MODULO
   CxxOp(                 "+", 2, 13, T, F, T),  // ADD
   CxxOp(                 "-", 2, 13, T, F, F),  // SUBTRACT
   CxxOp(                "<<", 2, 12, T, F, F),  // LEFT_SHIFT
   CxxOp(                ">>", 2, 12, T, F, F),  // RIGHT_SHIFT
   CxxOp(                 "<", 2, 11, T, F, T),  // LESS
   CxxOp(                "<=", 2, 11, T, F, T),  // LESS_OR_EQUAL
   CxxOp(                 ">", 2, 11, T, F, T),  // GREATER
   CxxOp(                ">=", 2, 11, T, F, T),  // GREATER_OR_EQUAL
   CxxOp(                "==", 2, 10, T, F, T),  // EQUALITY
   CxxOp(                "!=", 2, 10, T, F, T),  // INEQUALITY
   CxxOp(                 "&", 2,  9, T, F, T),  // BITWISE_AND
   CxxOp(                 "^", 2,  8, T, F, T),  // BITWISE_XOR
   CxxOp(                 "|", 2,  7, T, F, T),  // BITWISE_OR
   CxxOp(                "&&", 2,  6, T, F, T),  // LOGICAL_AND
   CxxOp(                "||", 2,  5, T, F, T),  // LOGICAL_OR
   CxxOp(                 "?", 3,  4, F, F, F),  // CONDITIONAL
   CxxOp(                 "=", 2,  3, T, T, F),  // ASSIGN
   CxxOp(                "*=", 2,  3, T, T, F),  // MULTIPLY_ASSIGN
   CxxOp(                "/=", 2,  3, T, T, F),  // DIVIDE_ASSIGN
   CxxOp(                "%=", 2,  3, T, T, F),  // MODULO_ASSIGN
   CxxOp(                "+=", 2,  3, T, T, F),  // ADD_ASSIGN
   CxxOp(                "-=", 2,  3, T, T, F),  // SUBTRACT_ASSIGN
   CxxOp(               "<<=", 2,  3, T, T, F),  // LEFT_SHIFT_ASSIGN
   CxxOp(               ">>=", 2,  3, T, T, F),  // RIGHT_SHIFT_ASSIGN
   CxxOp(                "&=", 2,  3, T, T, F),  // BITWISE_AND_ASSIGN
   CxxOp(                "^=", 2,  3, T, T, F),  // BITWISE_XOR_ASSIGN
   CxxOp(                "|=", 2,  3, T, T, F),  // BITWISE_OR_ASSIGN
   CxxOp(           THROW_STR, 0,  2, F, T, F),  // THROW
   CxxOp(                 ",", 2,  1, F, F, F),  // STATEMENT_SEPARATOR
   CxxOp(                 "$", 0,  0, F, F, F),  // START_OF_EXPRESSION
   CxxOp(           ERROR_STR, 0,  0, F, F, F),  // FALSE
   CxxOp(           ERROR_STR, 0,  0, F, F, F),  // TRUE
   CxxOp(           ERROR_STR, 0,  0, F, F, F),  // NULLPTR
   CxxOp(           ERROR_STR, 0,  0, F, F, F)   // NIL_OPERATOR
};

//------------------------------------------------------------------------------

fn_name CxxOp_ctor = "CxxOp.ctor";

CxxOp::CxxOp(const string& sym, size_t args,
   size_t prio, bool over, bool push, bool symm) :
   symbol(sym),
   arguments(args),
   priority(prio),
   overloadable(over),
   rightToLeft(push),
   symmetric(symm)
{
   Debug::ft(CxxOp_ctor);
}

//------------------------------------------------------------------------------

fn_name CxxOp_NameToOperator = "CxxOp.NameToOperator";

Cxx::Operator CxxOp::NameToOperator(const string& name)
{
   Debug::ft(CxxOp_NameToOperator);

   auto pos = name.rfind(OPERATOR_STR);
   if(pos == string::npos) return Cxx::NIL_OPERATOR;

   auto sym = name.substr(pos + strlen(OPERATOR_STR));
   pos = sym.find_first_not_of(SPACE);
   if(pos == string::npos) return Cxx::CAST;
   if(pos > 0) sym.erase(0, pos);

   auto last = sym.back();
   if((last == ')') || (last == ']')) sym.pop_back();

   for(size_t i = 0; i <= Cxx::STATEMENT_SEPARATOR; ++i)
   {
      if(Attrs[i].symbol.compare(sym) == 0) return Cxx::Operator(i);
   }

   return Cxx::NIL_OPERATOR;
}

//------------------------------------------------------------------------------

fn_name CxxOp_OperatorToName = "CxxOp.OperatorToName";

string CxxOp::OperatorToName(Cxx::Operator oper)
{
   Debug::ft(CxxOp_OperatorToName);

   auto& attrs = Attrs[oper];
   string name(OPERATOR_STR);
   if(isalpha(attrs.symbol.front())) name += SPACE;
   name += attrs.symbol;

   switch(oper)
   {
   case Cxx::ARRAY_SUBSCRIPT:
      name += ']';
      break;
   case Cxx::FUNCTION_CALL:
   case Cxx::CAST:
      name += ')';
   }

   return name;
}

//------------------------------------------------------------------------------

fn_name CxxOp_UpdateOperator = "CxxOp.UpdateOperator";

void CxxOp::UpdateOperator(Cxx::Operator& oper, size_t args)
{
   Debug::ft(CxxOp_UpdateOperator);

   auto& attrs = Attrs[oper];

   if((attrs.arguments == args) || (attrs.arguments == 0)) return;

   auto& token = attrs.symbol;

   for(size_t i = 0; i <= Cxx::STATEMENT_SEPARATOR; ++i)
   {
      auto& entry = Attrs[i];

      if((entry.arguments == args) && (entry.symbol.compare(token) == 0))
      {
         oper = Cxx::Operator(i);
         return;
      }
   }
}

//==============================================================================

CxxChar CxxChar::Attrs[UINT8_MAX + 1] = { };

//------------------------------------------------------------------------------

fn_name CxxChar_Initialize = "CxxChar.Initialize";

void CxxChar::Initialize()
{
   Debug::ft(CxxChar_Initialize);

   for(auto c = 0; c <= UINT8_MAX; ++c)
   {
      Attrs[c].validFirst = false;
      Attrs[c].validNext = false;
      Attrs[c].validOp = false;
      Attrs[c].validInt = false;
      Attrs[c].intValue = -1;
      Attrs[c].hexValue = -1;
      Attrs[c].octValue = -1;
   }

   for(size_t i = 0; i < ValidFirstChars.size(); ++i)
   {
      Attrs[ValidFirstChars[i]].validFirst = true;
   }

   for(size_t i = 0; i < ValidNextChars.size(); ++i)
   {
      Attrs[ValidNextChars[i]].validNext = true;
   }

   for(size_t i = 0; i < ValidOpChars.size(); ++i)
   {
      Attrs[ValidOpChars[i]].validOp = true;
   }

   for(size_t i = 0; i < ValidIntChars.size(); ++i)
   {
      Attrs[ValidIntChars[i]].validInt = true;
   }

   for(size_t i = 0; i < ValidIntDigits.size(); ++i)
   {
      Attrs[ValidIntDigits[i]].intValue = int8_t(i);
   }

   for(size_t i = 0; i < ValidHexDigits.size(); ++i)
   {
      auto h = (i <= 15 ? i : i - 6);
      Attrs[ValidHexDigits[i]].hexValue = int8_t(h);
   }

   for(size_t i = 0; i < ValidOctDigits.size(); ++i)
   {
      Attrs[ValidOctDigits[i]].octValue = int8_t(i);
   }
}

//==============================================================================

const Numeric Numeric::Nil(NIL, 0, F);
const Numeric Numeric::Bool(INT, 1, F);
const Numeric Numeric::Char(INT, sizeof(char) << 3, T);
const Numeric Numeric::Char16(INT, sizeof(char16_t) << 3, F);
const Numeric Numeric::Char32(INT, sizeof(char32_t) << 3, F);
const Numeric Numeric::Double(FLOAT, sizeof(double) << 3, T);
const Numeric Numeric::Enum(ENUM, sizeof(int) << 3, T);
const Numeric Numeric::Float(FLOAT, sizeof(float) << 3, T);
const Numeric Numeric::Int(INT, sizeof(int) << 3, T);
const Numeric Numeric::Long(INT, sizeof(long) << 3, T);
const Numeric Numeric::LongDouble(FLOAT, sizeof(long double) << 3, T);
const Numeric Numeric::LongLong(INT, sizeof(long long) << 3, T);
const Numeric Numeric::Pointer(PTR, sizeof(intptr_t), T);
const Numeric Numeric::Short(INT, sizeof(short) << 3, T);
const Numeric Numeric::uChar(INT, sizeof(char) << 3, F);
const Numeric Numeric::uInt(INT, sizeof(int) << 3, F);
const Numeric Numeric::uLong(INT, sizeof(long) << 3, F);
const Numeric Numeric::uLongLong(INT, sizeof(long long) << 3, F);
const Numeric Numeric::uShort(INT, sizeof(short) << 3, F);
const Numeric Numeric::wChar(INT, sizeof(wchar_t) << 3, F);

//------------------------------------------------------------------------------

fn_name Numeric_CalcMatchWith = "Numeric.CalcMatchWith";

TypeMatch Numeric::CalcMatchWith(const Numeric* that) const
{
   Debug::ft(Numeric_CalcMatchWith);

   //  Determine whether THAT can be implicitly converted to THIS.
   //
   switch(this->type_)
   {
   case INT:
      switch(that->type_)
      {
      case INT:
      case ENUM:
         if(this->bitWidth_ == that->bitWidth_)
         {
            if(this->signed_ != that->signed_) return Convertible;
            if(that->type_ == ENUM) return Promotable;
            return Compatible;
         }
         else if(this->bitWidth_ > that->bitWidth_)
         {
            if(that->signed_ && !this->signed_) return Convertible;
            return Promotable;
         }
         return Abridgeable;

      case PTR:
         if(this->bitWidth_ >= that->bitWidth_) return Convertible;
         return Abridgeable;

      case FLOAT:
         return Abridgeable;
      }

      return Incompatible;

   case FLOAT:
      if(that->type_ == FLOAT) return Convertible;
      if(that->type_ == INT) return Convertible;
      return Incompatible;

   case PTR:
      if(that->type_ == INT) return Convertible;
      return Incompatible;
   }

   return Incompatible;
}

//==============================================================================

CxxStats CxxStats::Info[CxxStats::Item_N] =
{
   CxxStats("MacroName", sizeof(MacroName)),
   CxxStats("Iff", sizeof(Iff)),
   CxxStats("Ifdef", sizeof(Ifdef)),
   CxxStats("Ifndef", sizeof(Ifndef)),
   CxxStats("Elif", sizeof(Elif)),
   CxxStats("Else", sizeof(Else)),
   CxxStats("Endif", sizeof(Endif)),
   CxxStats("Define", sizeof(Define)),
   CxxStats("Undef", sizeof(Undef)),
   CxxStats("Include", sizeof(Include)),
   CxxStats("Error", sizeof(Error)),
   CxxStats("Line", sizeof(Line)),
   CxxStats("Pragma", sizeof(Pragma)),
   CxxStats("IntLiteral", sizeof(IntLiteral)),
   CxxStats("FloatLiteral", sizeof(FloatLiteral)),
   CxxStats("BoolLiteral", sizeof(BoolLiteral)),
   CxxStats("CharLiteral", sizeof(CharLiteral)),
   CxxStats("StrLiteral", sizeof(StrLiteral)),
   CxxStats("NullPtr", sizeof(NullPtr)),
   CxxStats("Operation", sizeof(Operation)),
   CxxStats("Elision", sizeof(Elision)),
   CxxStats("Precedence", sizeof(Precedence)),
   CxxStats("BraceInit", sizeof(BraceInit)),
   CxxStats("Expression", sizeof(Expression)),
   CxxStats("ArraySpec", sizeof(ArraySpec)),
   CxxStats("TemplateParms", sizeof(TemplateParms)),
   CxxStats("MemberInit", sizeof(MemberInit)),
   CxxStats("QualName", sizeof(QualName)),
   CxxStats("TemplateParm", sizeof(TemplateParm)),
   CxxStats("TypeName", sizeof(TypeName)),
   CxxStats("DataSpec", sizeof(DataSpec)),
   CxxStats("FuncSpec", sizeof(FuncSpec)),
   CxxStats("Using", sizeof(Using)),
   CxxStats("Argument", sizeof(Argument)),
   CxxStats("BaseDecl", sizeof(BaseDecl)),
   CxxStats("Enum", sizeof(Enum)),
   CxxStats("Enumerator", sizeof(Enumerator)),
   CxxStats("Forward", sizeof(Forward)),
   CxxStats("Friend", sizeof(Friend)),
   CxxStats("Terminal", sizeof(Terminal)),
   CxxStats("Typedef", sizeof(Typedef)),
   CxxStats("Break", sizeof(Break)),
   CxxStats("Case", sizeof(Case)),
   CxxStats("Catch", sizeof(Catch)),
   CxxStats("Continue", sizeof(Continue)),
   CxxStats("Do", sizeof(Do)),
   CxxStats("Expr", sizeof(Expr)),
   CxxStats("For", sizeof(For)),
   CxxStats("If", sizeof(If)),
   CxxStats("Label", sizeof(Label)),
   CxxStats("NoOp", sizeof(NoOp)),
   CxxStats("Return", sizeof(Return)),
   CxxStats("Switch", sizeof(Switch)),
   CxxStats("Try", sizeof(Try)),
   CxxStats("While", sizeof(While)),
   CxxStats("Block", sizeof(Block)),
   CxxStats("ClassData", sizeof(ClassData)),
   CxxStats("FuncData", sizeof(FuncData)),
   CxxStats("SpaceData", sizeof(SpaceData)),
   CxxStats("Function", sizeof(Function)),
   CxxStats("Class", sizeof(Class)),
   CxxStats("ClassInst", sizeof(ClassInst)),
   CxxStats("Namespace", sizeof(Namespace)),
   CxxStats("CodeFile", sizeof(CodeFile)),
   CxxStats("CxxSymbols", sizeof(CxxSymbols))
};

//------------------------------------------------------------------------------

fn_name CxxStats_ctor = "CxxStats.ctor";

CxxStats::CxxStats(const string& item, size_t bytes) :
   name(item),
   size(bytes),
   in_use(0),
   strings(0),
   vectors(0)
{
   Debug::ft(CxxStats_ctor);
}

//------------------------------------------------------------------------------

fixed_string ItemHeader =
   "    ITEM TYPE   SIZE    IN USE    OBJECTS    STRINGS    VECTORS      TOTAL";
// 0         1         2         3         4         5         6         7
// 0123456789012345678901234567890123456789012345678901234567890123456789012345

void CxxStats::Display(ostream& stream)
{
   size_t totalNum = 0;
   size_t totalObj = 0;
   size_t totalStr = 0;
   size_t totalVec = 0;
   size_t totalMem = 0;
   size_t subtotal;
   size_t itemUse;

   //  Memory usage by strings and vectors is not determined until the Shrink
   //  function has been invoked.
   //
   Shrink();

   stream << ItemHeader << CRLF;

   for(size_t i = 0; i < Item_N; ++i)
   {
      auto& info = Info[i];
      stream << setw(13) << info.name << spaces(3);
      stream << setw(4) << info.size << spaces(2);
      stream << setw(8) << info.in_use << spaces(2);
      totalNum += info.in_use;
      subtotal = info.size * info.in_use;
      stream << setw(9) << subtotal << spaces(2);
      totalObj += subtotal;
      stream << setw(9) << info.strings << spaces(2);
      totalStr += info.strings;
      stream << setw(9) << info.vectors << spaces(2);
      totalVec += info.vectors;
      itemUse = subtotal + info.strings + info.vectors;
      stream << setw(9) << itemUse << CRLF;
      totalMem += itemUse;
   }

   stream << setw(30) << totalNum << setw(11) << totalObj;
   stream << setw(11) << totalStr << setw(11) << totalVec;
   stream << setw(11) << totalMem << CRLF;
}

//------------------------------------------------------------------------------

void CxxStats::Shrink()
{
   for(size_t i = 0; i < Item_N; ++i)
   {
      auto& info = Info[i];
      info.strings = 0;
      info.vectors = 0;
   }

   auto& files = Singleton< Library >::Instance()->Files();

   for(auto f = files.First(); f != nullptr; files.Next(f))
   {
      f->Shrink();
   }

   auto root = Singleton< CxxRoot >::Instance();
   root->Shrink();
   root->GlobalNamespace()->Shrink();
   Singleton< CxxSymbols >::Instance()->Shrink();
}
}
