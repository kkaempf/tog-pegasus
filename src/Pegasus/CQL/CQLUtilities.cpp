//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com)
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/CQL/CQLUtilities.h>

// Query includes
#include <Pegasus/Query/QueryCommon/QueryException.h>

// Pegasus Common includes
#include <Pegasus/Common/Tracer.h>

// standard includes
#include <errno.h>

// symbol defines
#define PEGASUS_SINT64_MIN (PEGASUS_SINT64_LITERAL(0x8000000000000000))
#define PEGASUS_UINT64_MAX PEGASUS_UINT64_LITERAL(0xFFFFFFFFFFFFFFFF)

// required for the windows compile
#ifndef _MSC_VER
#define _MSC_VER 0
#endif

PEGASUS_NAMESPACE_BEGIN

inline Uint8 _CQLUtilities_hexCharToNumeric(const Char16 c)
{
    Uint8 n;

    if (isdigit(c))
        n = (c - '0');
    else if (isupper(c))
        n = (c - 'A' + 10);
    else // if (islower(c))
        n = (c - 'a' + 10);

    return n;
}

Uint64 CQLUtilities::stringToUint64(const String &stringNum)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLUtilities::stringToUint64()");

  Uint64 x = 0;
  const Char16* p = stringNum.getChar16Data();
  const Char16* pStart = p;

  if (String::equal(stringNum, String::EMPTY))
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.EMPTY_STRING"),
                             String("String cannot be empty."));
    throw CQLRuntimeException(mload);    
  }
  
  if (!p)
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.NULL_INPUT"),
                             String("String cannot be NULL."));
    throw CQLRuntimeException(mload);    
  }

  // There cannot be a negative '-' sign
  if (*p == '-')
  {
     MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_NEG"),
                              String("String $0 cannot begin with '-'."),
                              stringNum);
    throw CQLRuntimeException(mload);    
  }
  if (*p == '+')
    p++;  // skip over the positive sign

  if (!isdigit(*p))
  {
     MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_NUM_FORMAT"),
                              String("String '$0' is badly formed.  It must be of the form; [+][0-9]*"),
                              stringNum);
    throw CQLRuntimeException(mload);    
  }

  // if binary
  Uint32 endString = stringNum.size() - 1;
  if ( (pStart[endString] == 'b') || (pStart[endString] == 'B') )
  {
    // Add on each digit, checking for overflow errors
    while ((*p == '0') || (*p == '1'))
    {
      // Make sure we won't overflow when we multiply by 2
      if (x > PEGASUS_UINT64_MAX/2)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x << 1;

      // We can't overflow when we add the next digit
      Uint64 newDigit = 0;
      if (*p++ == '1')
        newDigit = 1;
      if (PEGASUS_UINT64_MAX - x < newDigit)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x + newDigit;
    }

    // If we found a non-binary digit, report an error
    if (*p && (*p != 'b') && (*p != 'B'))
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_BIN_CHAR"),
                               String("Character '$0' in string '$1' is not a binary digit."),
                               String(p, 1), stringNum);
      throw CQLRuntimeException(mload);    
    }

    // return value from the binary string
    PEG_METHOD_EXIT();
    return x;      
  } // end if binary

  // if hexidecimal
  if ( (*p == '0') && ((p[1] == 'x') || (p[1] == 'X')) )
  {
    // Convert a hexadecimal string

    // Skip over the "0x"
    p+=2;

    // At least one hexadecimal digit is required
    if (!*p)
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_HEX_FORMAT"),
                               String("String '$0' needs a hexadecimal digit character following '0x'"),
                               stringNum);
      throw CQLRuntimeException(mload);    
    }

    // Add on each digit, checking for overflow errors
    while (isxdigit(*p))
    {
      // Make sure we won't overflow when we multiply by 16
      if (x > PEGASUS_UINT64_MAX/16)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x << 4;

      // We can't overflow when we add the next digit
      Uint64 newDigit = Uint64(_CQLUtilities_hexCharToNumeric(*p++));
      if (PEGASUS_UINT64_MAX - x < newDigit)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x + newDigit;
    }

    // If we found a non-hexadecimal digit, report an error
    if (*p)
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_HEX_CHAR"),
                             String("Character '$0' in string '$1' is not a hexidecimal digit."),
                             String(p, 1), stringNum);
      throw CQLRuntimeException(mload);    
    }

    // return value from the hex string
    PEG_METHOD_EXIT();
    return x;
  }  // end if hexidecimal     


  // Expect a positive decimal digit:

  // Add on each digit, checking for overflow errors
  while (isdigit(*p))
  {
    // Make sure we won't overflow when we multiply by 10
    if (x > PEGASUS_UINT64_MAX/10)
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                               String("String $0 caused an overflow."),
                               stringNum);
      throw CQLRuntimeException(mload);    
    }
    x = 10 * x;

    // Make sure we won't overflow when we add the next digit
    Uint64 newDigit = (*p++ - '0');
    if (PEGASUS_UINT64_MAX - x < newDigit)
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                               String("String $0 caused an overflow."),
                               stringNum);
      throw CQLRuntimeException(mload);    
    }

    x = x + newDigit;
  }

  // If we found a non-decimal digit, report an error
  if (*p)
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_DECIMAL_CHAR"),
                             String("Character '$0' in string '$1' is not a decimal digit."),
                             String(p, 1), stringNum);
    throw CQLRuntimeException(mload);    
  }

  // return the value for the decimal string
  PEG_METHOD_EXIT();
  return x;
}

Sint64 CQLUtilities::stringToSint64(const String &stringNum)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLUtilities::stringToSint64()");
  
  Sint64 x = 0;
  Boolean invert = false;
  const Char16* p = stringNum.getChar16Data();
  const Char16* pStart = p;

  if (String::equal(stringNum, String::EMPTY))
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.EMPTY_STRING"),
                             String("String cannot be empty."));
    throw CQLRuntimeException(mload);    
  }
  
  if (!p)
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.NULL_INPUT"),
                             String("String cannot be NULL."));
    throw CQLRuntimeException(mload);    
  }

  // skip over the sign if there is one
  if (*p == '-')
  {
    invert = true;
    p++;
  }
  if (*p == '+')
    p++;

  if (!isdigit(*p))
  {
     MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_NUM_FORMAT"),
                              String("String '$0' is badly formed.  It must be of the form; [+-][0-9]*"),
                              stringNum);
    throw CQLRuntimeException(mload);    
  }

  // ********************
  // Build the Sint64 as a negative number, regardless of the
  // eventual sign (negative numbers can be bigger than positive ones)
  // ********************
  
  // if binary
  Uint32 endString = stringNum.size() - 1;
  if ( (pStart[endString] == 'b') || (pStart[endString] == 'B') )
  {
    // Add on each digit, checking for overflow errors
    while ((*p == '0') || (*p == '1'))
    {
      // Make sure we won't overflow when we multiply by 2
      if (x < PEGASUS_SINT64_MIN/2)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x << 1;

      // We can't overflow when we add the next digit
      Sint64 newDigit = 0;
      if (*p++ == '1')
        newDigit = 1;
      if (PEGASUS_SINT64_MIN - x > -newDigit)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x - newDigit;
    }

    // If we found a non-binary digit, report an error
    if (*p && (*p != 'b') && (*p != 'B'))
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_BIN_CHAR"),
                               String("Character '$0' in string '$1' is not a binary digit."),
                               String(p, 1), stringNum);
      throw CQLRuntimeException(mload);    
    }

    // Return the integer to positive, if necessary, checking for an
    // overflow error
    if (!invert)
    {
      if (x == PEGASUS_SINT64_MIN)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }
      x = -x;
    }
    
    // return value from the binary string
    PEG_METHOD_EXIT();
    return x;      
  }  // end if binary

  // if hexidecimal
  if ( (*p == '0') && ((p[1] == 'x') || (p[1] == 'X')) )
  {
    // Convert a hexadecimal string

    // Skip over the "0x"
    p+=2;

    // At least one hexidecimal digit is required
    if (!*p)
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_HEX_FORMAT"),
                               String("String '$0' needs a hexidecimal digit character following '0x'"),
                               stringNum);
      throw CQLRuntimeException(mload);    
    }

    // Add on each digit, checking for overflow errors
    while (isxdigit(*p))
    {
      // Make sure we won't overflow when we multiply by 16
      if (x < PEGASUS_SINT64_MIN/16)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x << 4;

      // We can't overflow when we add the next digit
      Sint64 newDigit = Sint64(_CQLUtilities_hexCharToNumeric(*p++));
      if (PEGASUS_SINT64_MIN - x > -newDigit)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }

      x = x - newDigit;
    }

    // If we found a non-hexidecimal digit, report an error
    if (*p)
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_HEX_CHAR"),
                             String("Character '$0' in string '$1' is not a hexidecimal digit."),
                             String(p, 1), stringNum);
      throw CQLRuntimeException(mload);    
    }

    // Return the integer to positive, if necessary, checking for an
    // overflow error
    if (!invert)
    {
      if (x == PEGASUS_SINT64_MIN)
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
      }
      x = -x;
    }
    
    // return value from the hex string
    PEG_METHOD_EXIT();
    return x;
  }  // end if hexidecimal     


  // Expect a positive decimal digit:

  // Add on each digit, checking for overflow errors
  while (isdigit(*p))
  {
    // Make sure we won't overflow when we multiply by 10
    if (x < PEGASUS_SINT64_MIN/10)
    {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
    }
    x = 10 * x;

    // Make sure we won't overflow when we add the next digit
    Sint64 newDigit = (*p++ - '0');
    if (PEGASUS_SINT64_MIN - x > -newDigit)
    {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
    }

    x = x - newDigit;
  }

  // If we found a non-decimal digit, report an error
  if (*p)
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_DECIMAL_CHAR"),
                             String("Character '$0' in string '$1' is not a decimal digit."),
                             String(p, 1), stringNum);
    throw CQLRuntimeException(mload);    
  }

  // Return the integer to positive, if necessary, checking for an
  // overflow error
  if (!invert)
  {
    if (x == PEGASUS_SINT64_MIN)
    {
        MessageLoaderParms mload(String("CQL.CQLUtilities.OVERFLOW"),
                                 String("String $0 caused an overflow."),
                                 stringNum);
        throw CQLRuntimeException(mload);    
    }
    x = -x;
  }

  // return the value for the decimal string
  PEG_METHOD_EXIT();
  return x;  
}

Real64 CQLUtilities::stringToReal64(const String &stringNum)
{
  PEG_METHOD_ENTER(TRC_CQL,"CQLUtilities::stringToReal64()");
  
  Real64 x = 0;
  const Char16* p = stringNum.getChar16Data();
  Boolean neg = false;
  const Char16* pStart = p;

  if (String::equal(stringNum, String::EMPTY))
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.EMPTY_STRING"),
                             String("String cannot be empty."));
    throw CQLRuntimeException(mload);    
  }
  
  if (!p)
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.NULL_INPUT"),
                             String("String cannot be NULL."));
    throw CQLRuntimeException(mload);    
  }

  
  // Skip optional sign:

  if (*p == '+')
    p++;
  
  if (*p  == '-')
  {
    neg = true;
    p++;
  };
  
  // Check if it it is a binary or hex integer
  Uint32 endString = stringNum.size() - 1;
  if ((*p == '0' && (p[1] == 'x' || p[1] == 'X')) ||  // hex OR
      pStart[endString] == 'b' || pStart[endString] == 'B')  // binary
  {
    if (neg)
      x = stringToSint64(stringNum);
    else

// Check if the complier is MSVC 6, which does not support the conversion operator from Uint64 to Real64      
#if defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC) && (_MSC_VER < 1300)
    {
      Uint64 num = stringToUint64(stringNum);
      Sint64 half = num / 2;
      x = half;
      x += half;
      if (num % 2)  // if odd, then add the lost remainder
        x += 1;
    }
#else
      x = stringToUint64(stringNum);
#endif    
    PEG_METHOD_EXIT();
    return x;
  }  
  
  // Skip optional first set of digits:

  while (isdigit(*p))
    p++;

  // Test if optional dot is there
  if (*p++ == '.')
  {
    // One or more digits required:
    if (!isdigit(*p++))
    {
      MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_CHAR_POST_DOT"),
                               String("String '$0' must have a digit character following the decimal point."),
                               stringNum);
      throw CQLRuntimeException(mload);    
    }

    while (isdigit(*p))
      p++;

    // If there is an exponent now:
    if (*p)
    {
      // Test exponent:

      if (*p != 'e' && *p != 'E')
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_REAL_CHAR"),
                                 String("Character '$0' in string '$1` is invalid for a real number."),
                                 String(p-1, 1), stringNum);
        throw CQLRuntimeException(mload);    
      }
      p++;

      // Skip optional sign:

      if (*p == '+' || *p  == '-')
        p++;

      // One or more digits required:
      if (!isdigit(*p++))
      {
        MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_REAL_EXP"),
                                 String("String '$0' has an badly formed exponent.  It must be of the form [eE][+-][0-9]*..  Character '$1' is invalid."),
                                 stringNum, String(p, 1));
        throw CQLRuntimeException(mload);    
      }

      while (isdigit(*p))
        p++;
    }
  } // end-if optional decimal point
  if (*p && p - pStart <= stringNum.size())
  {
  //  printf("This is char # %d\n", p - pStart);
    MessageLoaderParms mload(String("CQL.CQLUtilities.INVALID_DECIMAL_CHAR"),
                             String("Character '$0' in string '$1' is not a decimal digit."),
                             String(p-1, 1), stringNum);
    throw CQLRuntimeException(mload);    
  }
  //
  // Do the conversion
  //
  char* end;
  errno = 0;
  CString temp = stringNum.getCString();
  x = strtod((const char *) temp, &end);
  if (*end || (errno == ERANGE))
  {
    MessageLoaderParms mload(String("CQL.CQLUtilities.CONVERSION_REAL_ERROR"),
                             String("String $0 was unable to be converted to a Real64.  It could be out of range."),
                             stringNum);
    throw CQLRuntimeException(mload);    
  }
  PEG_METHOD_EXIT();
//  printf("String %s = %.16e\n", (const char *)stringNum.getCString(), x);
  return x;
}

String CQLUtilities::formatRealStringExponent(const String &realString)
{
  String newString(realString);
  Uint32 expIndex = PEG_NOT_FOUND;
  Uint32 index = newString.size() - 1;
  
  expIndex = newString.find('E');
  if (expIndex == PEG_NOT_FOUND)
    expIndex = newString.find('e');

  if (expIndex == PEG_NOT_FOUND)
    return newString;  // no exponent symbol, so just return

  // format the exponent
  index = expIndex + 1;  // start index at next character
  if (newString[index] == '+')
    newString.remove(index, 1);  // remove the '+' symbol

  if (newString[index] == '-')
    index++;   // skip the '-' exponent sign

  while (newString[index] == '0' && index < newString.size())
  {
    newString.remove(index, 1);
  }

  // If only an 'e' is left (only 0's behind it) then strip the 'e'
  if (index >= newString.size())
    newString.remove(expIndex, 1);

  return newString;
}

PEGASUS_NAMESPACE_END
