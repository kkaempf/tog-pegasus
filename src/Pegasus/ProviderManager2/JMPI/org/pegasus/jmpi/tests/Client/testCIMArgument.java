//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
// Author:      Mark Hamzy,    hamzy@us.ibm.com
//
// Modified By: Mark Hamzy,    hamzy@us.ibm.com
//
//%/////////////////////////////////////////////////////////////////////////////
package Client;

import org.pegasus.jmpi.CIMArgument;
import org.pegasus.jmpi.CIMClient;
import org.pegasus.jmpi.CIMDataType;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMValue;

public class testCIMArgument
{
   private boolean DEBUG = false;

   /**
    * This returns the group name.
    *
    * @return String "class" testcase belongs in.
    */
   public String getGroup ()
   {
      return "CIMArgument";
   }

   public void setDebug (boolean fDebug)
   {
      DEBUG = fDebug;
   }

   public boolean main (String args[], CIMClient cimClient)
   {
      boolean fExecuted = false;
      boolean fRet      = true;

      for (int i = 0; i < args.length; i++)
      {
          if (args[i].equalsIgnoreCase ("debug"))
          {
              setDebug (true);
          }
      }

      if (!fExecuted)
      {
         fRet = testCIMArgument (cimClient);
      }

      return fRet;
   }

   private boolean testCIMArgument (CIMClient client)
   {
      if (client == null)
      {
         System.out.println ("FAILURE: testCIMArgument: client == null");

         return false;
      }

      // -----

      CIMArgument ca1      = null;
      CIMArgument ca2      = null;
      String      pszName  = "bob";
      String      pszValue = "lives";
      CIMValue    cv       = null;

      try
      {
         ca1 = new CIMArgument (pszName,
                                new CIMValue (new String (pszValue)));
      }
      catch (CIMException e)
      {
         System.out.println ("FAILURE: testCIMArgument: ca1: caught " + e);

         return false;
      }

      if (DEBUG)
      {
         System.out.println ("testCIMArgument: ca1 = " + ca1);
      }

      if (ca1 == null)
      {
         System.out.println ("FAILURE: testCIMArgument: ca1 == null");

         return false;
      }

      String pszRetValue = null;

      try
      {
         pszRetValue = (String)ca1.getValue ().getValue ();
      }
      catch (CIMException e)
      {
         System.out.println ("FAILURE: testCIMArgument: ca1.getValue ().getValue (): caught " + e);

         return false;
      }

      if (pszRetValue == null)
      {
         System.out.println ("FAILURE: testCIMArgument: ca1.getValue ().getValue () == null");

         return false;
      }

      if (DEBUG)
      {
         System.out.println ("testCIMArgument: ca1.getName ().equals (pszName) = " + (ca1.getName ().equals (pszName)));
         System.out.println ("testCIMArgument: (ca1.getType ().getType () == CIMDataType.STRING) = " + (ca1.getType ().getType () == CIMDataType.STRING));
         System.out.println ("testCIMArgument: (pszRetValue.equals (pszValue)) = " + (pszRetValue.equals (pszValue)));
      }

      if (  !(ca1.getName ().equals (pszName))
         || !(ca1.getType ().getType () == CIMDataType.STRING)
         || !(pszRetValue.equals (pszValue))
         )
      {
         System.out.println ("FAILURE: testCIMArgument: ca1.getXX");

         return false;
      }

      // -----

      try
      {
         ca2 = new CIMArgument (pszName,
                                new CIMValue (new String (pszValue)));
      }
      catch (CIMException e)
      {
         System.out.println ("FAILURE: testCIMArgument: ca1: caught " + e);

         return false;
      }

      if (DEBUG)
      {
         System.out.println ("testCIMArgument: ca2 = " + ca2);
      }

      if (ca2 == null)
      {
         System.out.println ("FAILURE: testCIMArgument: ca2 == null");

         return false;
      }
/* @TBD
      if (!ca1.equals (ca2))
      {
         System.out.println ("FAILURE: testCIMArgument: ca1.equals (ca2)");

         return false;
      }
*/

      // -----

      if (ca1.toString () == null)
      {
         System.out.println ("FAILURE: testCIMArgument: ca1.toString () == null");

         return false;
      }

      // -----

      CIMArgument ca3 = null;

      ca3 = new CIMArgument ("None");

      if (DEBUG)
      {
         System.out.println ("testCIMArgument: ca3 = " + ca3);
      }

      if (ca3 == null)
      {
         System.out.println ("FAILURE: testCIMArgument: ca3 == null");

         return false;
      }

      // -----

      String pszNewName = "NewNone";

      ca3.setName (pszNewName);

      if (DEBUG)
      {
         System.out.println ("testCIMArgument: ca3 = " + ca3);
      }

      if (!ca3.getName ().equals (pszNewName))
      {
         System.out.println ("FAILURE: testCIMArgument: ca3.setName");

         return false;
      }

      // -----

      try
      {
         cv = new CIMValue (new Integer (42));
      }
      catch (CIMException e)
      {
         System.out.println ("FAILURE: testCIMArgument: cv: caught " + e);

         return false;
      }

      ca3.setValue (cv);

      if (DEBUG)
      {
         System.out.println ("testCIMArgument: ca3.getType () = " + ca3.getType ());
         System.out.println ("testCIMArgument: (ca3.getType ().getType () == CIMDataType.SINT32) = " + (ca3.getType ().getType () == CIMDataType.SINT32));
      }

      if (!(ca3.getType ().getType () == CIMDataType.SINT32))
      {
         System.out.println ("FAILURE: testCIMArgument: ca3.setValue");

         return false;
      }

      // -----

      System.out.println ("SUCCESS: testCIMArgument");

      return true;
   }
}