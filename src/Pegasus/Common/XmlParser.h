//%/////////////////////////////////////////////////////////////////////////////
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
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// XmlParser.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_XmlParser_h
#define Pegasus_XmlParser_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/Stack.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMMON_LINKAGE XmlException : public Exception
{
public:

    enum Code
    {
	BAD_START_TAG = 1,
	BAD_END_TAG,
	BAD_ATTRIBUTE_NAME,
	EXPECTED_EQUAL_SIGN,
	BAD_ATTRIBUTE_VALUE,
	MINUS_MINUS_IN_COMMENT,
	UNTERMINATED_COMMENT,
	UNTERMINATED_CDATA,
	UNTERMINATED_DOCTYPE,
	TOO_MANY_ATTRIBUTES,
	MALFORMED_REFERENCE,
	EXPECTED_COMMENT_OR_CDATA,
	START_END_MISMATCH,
	UNCLOSED_TAGS,
	MULTIPLE_ROOTS,
	VALIDATION_ERROR,
	SEMANTIC_ERROR
    };

    XmlException(
	Code code, 
	Uint32 lineNumber,
	const String& message = String());

    XmlException::Code getCode() const { return _code; }

private:

    Code _code;
};

class PEGASUS_COMMON_LINKAGE XmlValidationError : public XmlException
{
public:

    XmlValidationError(Uint32 lineNumber, const String& message);
};

class PEGASUS_COMMON_LINKAGE XmlSemanticError : public XmlException
{
public:

    XmlSemanticError(Uint32 lineNumber, const String& message);
};

struct XmlAttribute
{
    const char* name;
    const char* value;
};

struct PEGASUS_COMMON_LINKAGE XmlEntry
{
    enum CIMType
    {
	XML_DECLARATION,
	START_TAG, 
	EMPTY_TAG, 
	END_TAG, 
	COMMENT,
	CDATA,
	DOCTYPE,
	CONTENT
    };

    enum { MAX_ATTRIBUTES = 10 };

    CIMType type;
    const char* text;
    XmlAttribute attributes[MAX_ATTRIBUTES];
    Uint32 attributeCount;

    void print() const;

    const XmlAttribute* findAttribute(const char* name) const;

    Boolean getAttributeValue(const char* name, Uint32& value) const;

    Boolean getAttributeValue(const char* name, Real32& value) const;

    Boolean getAttributeValue(const char* name, const char*& value) const;

    Boolean getAttributeValue(const char* name, String& value) const;
};

class PEGASUS_COMMON_LINKAGE XmlParser
{
public:

    // Warning: this constructor modifies the text.

    XmlParser(char* text);

    Boolean next(XmlEntry& entry);

    void putBack(XmlEntry& entry);

    ~XmlParser();

    Uint32 getStackSize() const { return _stack.size(); }

    Uint32 getLine() const { return _line; }

private:

    void _skipWhitespace(char*& p);

    Boolean _getElementName(char*& p);

    Boolean _getOpenElementName(char*& p, Boolean& openCloseElement);

    void _getAttributeNameAndEqual(char*& p);

    void _getAttributeValue(char*& p);

    void _getComment(char*& p);

    void _getCData(char*& p);

    void _getDocType(char*& p);

    void _getContent(char*& p);

    void _substituteReferences(char* text);

    void _getElement(char*& p, XmlEntry& entry);

    Uint32 _line;
    char* _text;
    char* _current;
    char _restoreChar;
    Stack<char*> _stack;
    Boolean _foundRoot;
    Stack<XmlEntry> _putBackStack;
};

inline void XmlAppendCString(Array<Sint8>& out, const char* str)
{
    out.append(str, strlen(str));
}

PEGASUS_COMMON_LINKAGE void XmlAppendString(
    Array<Sint8>& out, 
    const String& str);

PEGASUS_NAMESPACE_END

#endif /* Pegasus_XmlParser_h */
