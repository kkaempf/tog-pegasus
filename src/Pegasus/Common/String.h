//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author:
//
// $Log: String.h,v $
// Revision 1.16  2001/04/14 07:35:05  mike
// Added config file loading to OptionManager
//
// Revision 1.15  2001/04/11 19:53:22  mike
// More porting
//
// Revision 1.14  2001/04/10 23:01:52  mike
// Added new TimeValue class and regression tests for it.
// Modified Stopwatch class to use TimeValue class.
//
// Revision 1.13  2001/04/10 22:42:55  karl
// Correct error in String find
//
// Revision 1.12  2001/04/09 20:18:47  karl
// add find substring function
//
// Revision 1.11  2001/04/04 20:02:27  karl
// documentation update
//
// Revision 1.10  2001/03/29 16:40:31  karl
// add doc
//
// Revision 1.9  2001/02/26 04:33:28  mike
// Fixed many places where cim names were be compared with operator==(String,String).
// Changed all of these to use CIMName::equal()
//
// Revision 1.8  2001/02/20 14:05:24  karl
// Comments for Document
//
// Revision 1.7  2001/02/11 17:19:30  mike
// added reverseFind() method
//
// Revision 1.6  2001/02/11 05:42:33  mike
// new
//
// Revision 1.5  2001/01/30 08:00:43  karl
// DOC++ Documentation update for header files
//
// Revision 1.4  2001/01/28 07:05:18  mike
// added instance name/reference converters
//
// Revision 1.3  2001/01/28 04:11:03  mike
// fixed qualifier resolution
//
// Revision 1.2  2001/01/24 16:16:38  karl
// Incorporate Doc++ Comments as documentation into .h files
//
// Revision 1.1.1.1  2001/01/14 19:53:15  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_String_h
#define Pegasus_String_h

#include <iostream>
#include <cstring>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Char16.h>
#include <Pegasus/Common/Array.h>

PEGASUS_NAMESPACE_BEGIN

/**
    The Pegasus String C++ Class implements the CIM string type.
    This class is based on the general handle/representation pattern
    defined for all the Pegasus objects.  However, it differes from
    most in that it implements "copy on assign" all of the others implement 
    "no copy on assign" semantics.	The string class uses the Array class and
    implements an array of characters.
 */
class PEGASUS_COMMON_LINKAGE String
{
public:

    /** Default constructor without parameters. This constructor creates a 
	null string
	<pre>
	    String test;
	</pre> 
    */
    String();

    /// Copy constructor.
    String(const String& x);

    /// Initialize with first n characters from x.
    String(const String& x, Uint32 n);

    /// Initialize with x.
    String(const Char16* x);

    /// Initialize with first n characters of x.
    String(const Char16* x, Uint32 n);

    /// Initialize from a plain old C-String:
    String(const char* x);

    /// Initialize from the first n characters of a plain old C-String:
    String(const char* x, Uint32 n);

    /// String destructor. Used by the representation of the String object
    ~String() 
    {
    }

    /** Assign this string with x.
	<pre>
	    String t1 = "abc";
	    String t2 = t1;
	</pre>
    */
    String& operator=(const String& x) { _rep = x._rep; return *this; }

    /// Assign this string with x.
    String& operator=(const Char16* x) { assign(x); return *this; }

    /// Assign this string with x.
    String& assign(const String& x) { _rep = x._rep; return *this; }

    /// Assign this string with x.
    String& assign(const Char16* x);

    /// Assign this string with first n characters of x.
    String& assign(const Char16* x, Uint32 n);

    /// Assign this string with the plain old C-String x.
    String& assign(const char* x);

    /// Assign this string with first n characters of the plain old C-String x.
    String& assign(const char* x, Uint32 n);

    /** Clear this string. After calling clear(), getLength() will return 0.
	<pre>
	    String test = "abc";
	    test.clear();	// String test is now NULL (length == 0)
	</pre>
    */
    void clear() { _rep.clear(); _rep.append('\0'); }

    /** Reserves memory for capacity characters. Notice that this does not
	change the size of the string (getSize() returns what it did before).
	If the capacity of the string is already greater or equal to the
	capacity argument, this method has no effect. After calling reserve(),
	getCapicty() returns a value which is greater or equal to the 
	capacity argument.
	@param capacity defines the capacity in characters to reserve.
    */
    void reserve(Uint32 capacity) { _rep.reserve(capacity + 1); }

    /** Returns the length of the String object.
	@return Length of the string in characters.
	<pre>
	    String s = "abcd";
	    assert(s.getLength() == 4);
	</pre>
    */
    Uint32 getLength() const { return _rep.getSize() - 1; }

    /** Returns a pointer to the first character in the null-terminated string 
	string.
	@param
	@return	Pointer to the first character of the String object
    	<pre>
	    String t1 = "abc";
	    const Char16* q = t1.getData();
	</pre>
    */
    const Char16* getData() const { return _rep.getData(); }

    /** Allocates an 8 bit representation of this string. The user is 
	responsible for freeing the result. If any characters are truncated, 
	a TruncatedCharacter exception is thrown. This exception may
	be suppressed by passing true as the noThrow argument. Extra
	characters may be allocated at the end of the new string by
	passing a non-zero value to the extraBytes argument.
	@param extraBytes -  Defines the number of extra characters to be 
	allocated at the end of the new string. Default is zero.
	@param	noThrow - If true, no exception will be thrown if characters
	are truncated
	@return pointer to the new representation of the string
	@exception Throws TruncatedCharacter exception if any characters are
	truncated
	<pre>
	    String test = "abc";
	    char* p = test.allocateCString();
	</pre>
    */
    char* allocateCString(Uint32 extraBytes = 0, Boolean noThrow = false) const;

    /** Append the given string to a C-string. If the length is not Uint32(-1),
	then the lesser of the the length argument and the length of this 
	string is truncated. Otherwise, the entire string is trunctated. The 
	TruncatedCharacter exception is thrown if any characters are truncated.
	<pre>
	    const char STR0[] = "one two three four";
	    String s = STR0;
	    const char STR1[] = "zero ";
	    char* tmp = new char[strlen(STR1) + s.getLength() + 1];
	    strcpy(tmp, STR1);
	    s.appendToCString(tmp, 7);
	    assert(strcmp(tmp, "zero one two") == 0);
	</pre>
    */
    void appendToCString(
	char* str,
	Uint32 length = Uint32(-1),
	Boolean noThrow = false) const;

    /** Returns the Ith character of the String object.
	@exception - Throws exception "OutofBounds" if the index
	is outside the length of the string.
	<pre>
	    String t1 = "abc;
	    Char16 c = t1[1];	// character b
	</pre>
    */
    Char16& operator[](Uint32 i);

    /** Returns the Ith character of the String (const version).
	@exception - Throws exception "OutofBounds" if the index
	is outside the length of the string.
    
    */
    const Char16 operator[](Uint32 i) const;

    /** Append the given character to the string.
        <pre>
	     String s4 = "Hello";
	    s4.append(Char16(0x0000))
	</pre>
    */
    String& append(const Char16& c)
    {
	_rep.insert(_rep.getSize() - 1, c);
	return *this;
    }

    /// Append n characters from str to this String object.
    String& append(const Char16* str, Uint32 n);

    /// Append the characters of str to this String object.
    String& append(const String& str)
    {
	return append(str.getData(), str.getLength());
    }

    /** Overload operator += appends the parameter String to this String.
	@parm String to append.
	@return This String
	<pre>
	String test = "abc";
	test += "def";
	assert(test == "abcdef");
	</pre>
    */
    String& operator+=(const String& x)
    {
	return append(x);
    }

    /** Append the character given by c to this String object.
	@param c - Single character
    */
    String& operator+=(Char16 c)
    {
	return append(c);
    }

    /** Append the character given by c to this string.
	<pre>
	    String t1 = "abc";
	    t1 += 'd'
	    assert(t1 == "abcd");
	</pre>
    */
    String& operator+=(char c)
    {
	return append(Char16(c));
    }

    /** Remove size characters from the string starting at the given
	position. If size is -1, then all characters after pos are removed.
	@param pos - Position in string to start remove
	@param size - Number of characters to remove. Default is -1 which 
	causes all characters after pos to be removed
	<pre>
	    String s;
	    s = "abc";
	    s.remove(0, 1);
	    assert(String::equal(s, "bc"));
	    assert(s.getLength() == 2);
	    s.remove(0);
	    assert(String::equal(s, ""));
	    assert(s.getLength() == 0);
	</pre>
	@exception throws "OutOfBounds" exception if size is greater than
	length of String plus starting position for remove.
    */
    void remove(Uint32 pos, Uint32 size = Uint32(-1));

    /** Return a new String which is initialzed with <TT>length</TT> 
	characters from this string starting at <TT>pos</TT>. 
	@param <TT>pos</TT> is the positon in string to start getting the 
	substring. 
	@param <TT>length</TT> is the number of characters to get. If length
	is -1, then all characters after pos are added to the new string.
	@return String with the defined substring.
	<pre>
	    s = "abcdefg";
	    s.remove(3);
	    assert(String::equal(s, "abc"));
	</pre>
    */
    String subString(Uint32 pos, Uint32 length = Uint32(-1)) const;

    /** Find the position of the first occurence of the character c.
	If the character is not found, -1 is returned.
	@param c -  Char to be found in the String
	@return Position of the character in the string or -1 if not found.
    */
    Uint32 find(Char16 c) const;

    /** Find the position of the first occurence of the string object. 
	This function finds one string inside another
	If the matching substring is not found, -1 is returned.
	@param s -  String object to be found in the String
	@return Position of the substring in the String or -1 if not 
	found. 
    */
    Uint32 find(const String& s) const;

    /** Find substring
	@ param - 16 bit character pointer
	@seealso find
    */
    Uint32 find(const Char16* s) const;

    /** find substring
	@param char* to substring
    */
    Uint32 find(const char* s) const;
 
    /** Same as find() but start looking in reverse (last character first).
	@Seealso find
	@return Position of the character in the string or -1 if not found.
	
	NOTE: This function is defined only for char* input, not for 
	String. 
    */
    Uint32 reverseFind(Char16 c) const;

    /** Compare the first n characters of the two strings. 
    	@param s1 - First null-terminated string for the comparison
	@param s2 - Second null-terminated string for the comparison
	@param n - Number of characters to compare
	@return Return -1 if s1 is lexographically less than s2. If they
	are equavalent return 0. Otherwise return 1.
    */
    static int compare(const Char16* s1, const Char16* s2, Uint32 n);

    /** Compare two null-terminated strings. 
    	@param s1 - First null-terminated string for the comparison
	@param s2 - Second null-terminated string for the comparison
	@return If s1 is less than s2, return -1; if equal return 0;
	otherwise, return 1.
	
	NOTE: Use the comparison operators <,<= > >= to compare
	String objects.
    */
    static int compare(const Char16* s1, const Char16* s2);

    /** Compare two String objects for equality.
	@param s1 - First <TT>String</TT> for comparison.
	@param s2 - Second <TT>String</TT> for comparison.
	
	@return Boolean true if the two strings are equal.
	<pre>
	    String s1 = "Hello World";
	    String s2 = s1;
	    String s3(s2);
	    assert(String::equal(s1, s3));
	</pre>
    */
    static Boolean equal(const String& x, const String& y);

    /// Return true if the two strings are equal.
    static Boolean equal(const String& x, const Char16* y);

    /// Return true if the two strings are equal.
    static Boolean equal(const Char16* x, const String& y);

    /// Return true if the two strings are equal.
    static Boolean equal(const String& x, const char* y);

    /// Return true if the two strings are equal.
    static Boolean equal(const char* x, const String& y);

    /// Convert the plain old C-string to lower case:
    static void toLower(char* str);

    /**	EMPTY - Represent an empty string.
	This member is used to represent empty strings. Using this member
	avoids an expensive construction of an empty string (e.g., String()).
    */
    static const String EMPTY;

private:

    static Uint32 _min(Uint32 x, Uint32 y) { return x < y ? x : y; }

    Array<Char16> _rep;
};

/** String operator ==. Test for equality between two strings of any of the 
    types String or char*.
    @return Boolean - True if the strings are equal
    
*/
inline Boolean operator==(const String& x, const String& y)
{
    return String::equal(x, y);
}

/** String operator ==. Test for equality between two strings
    
*/ 
inline Boolean operator==(const String& x, const char* y)
{
    return String::equal(x, y);
}

/** String operator ==. Test for equality between two strings
    
*/
inline Boolean operator==(const char* x, const String& y)
{
    return String::equal(x, y);
}

/** String operator ==. Test for equality between two strings
    
*/  
inline Boolean operator!=(const String& x, const String& y)
{
    return !String::equal(x, y);
}

PEGASUS_COMMON_LINKAGE std::ostream& operator<<(
    std::ostream& os, 
    const String& x);

/** overload operator +	 - Concatenates String objects.
    
    <pre>
	String t1 = "abc";
	String t2;
	t2 = t1 + "def"
	assert(t2 == "abcdef");
    </pre>
*/
inline String operator+(const String& x, const String& y)
{
    return String(x).append(y);
}

/** overload operator < - Compares String obects.
    <pre>
	String t1 = "def";
	String t2 = "a";
	assert (t2 < t1);
    </pre>
*/
inline Boolean operator<(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) < 0;
}

/** overload operator <= compares String objects.

*/
inline Boolean operator<=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) <= 0;
}

/** Overload operator > compares String objects
*/
inline Boolean operator>(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) > 0;
}

/** overload operator >= - Compares String objects
*/
inline Boolean operator>=(const String& x, const String& y)
{
    return String::compare(x.getData(), y.getData()) >= 0;
}

/** Return a version of this string whose characters have been shifted
    to lower case.
*/
PEGASUS_COMMON_LINKAGE String ToLower(const String& str);

/** Compare two strings but ignore any case differences.
*/
PEGASUS_COMMON_LINKAGE int CompareIgnoreCase(const char* s1, const char* s2);

/** Get the next line from the input file.
*/
PEGASUS_COMMON_LINKAGE Boolean GetLine(std::istream& is, String& line);


PEGASUS_NAMESPACE_END

#endif /* Pegasus_String_h */

