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
// Author:      Konrad Rzeszutek <konradr@us.ibm.com>
//
// Modified By: 
//
//%/////////////////////////////////////////////////////////////////////////////


#include "CMPI_Version.h"

#include <Pegasus/Common/Stack.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/CQL/CQLSelectStatementRep.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLValue.h>

#include "CMPI_Cql2Dnf.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN
#define PEGASUS_ARRAY_T CMPI_term_el
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
#define PEGASUS_ARRAY_T CMPI_TableauRow
# include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T


#define DDD(X)   if (_cmpi_trace) X;

extern int _cmpi_trace;


//
// Routine to map the CQL data to String
//
  String
CQL2String (const CQLExpression & o)
{
  CQLValue val;
  if (o.isSimpleValue ())
    {
      val = o.getTerms ()[0].getFactors ()[0].getValue ();
    }
  else
    return "NULL_VALUE";

  if (val.getValueType () == CQLValue::Null_type)
    return "NULL_VALUE";

  return o.toString ();
}

//
// Routine to map the CQL type to CMPIPredOp
//
CMPIPredOp
CQL2PredOp (ExpressionOpType op, Boolean isInverted)
{
  CMPIPredOp op_type = (CMPIPredOp) 0;

  switch (op)
    {
    case LT:
      if (isInverted)
        op_type = CMPI_PredOp_GreaterThan;
      else
        op_type = CMPI_PredOp_LessThan;
      break;
    case GT:
      if (isInverted)
        op_type = CMPI_PredOp_LessThan;
      else
        op_type = CMPI_PredOp_GreaterThan;
      break;
    case EQ:
      if (isInverted)
        op_type = CMPI_PredOp_NotEquals;
      else
        op_type = CMPI_PredOp_Equals;
      break;
    case LE:
      if (isInverted)
        op_type = CMPI_PredOp_GreaterThanOrEquals;
      else
        op_type = CMPI_PredOp_LessThanOrEquals;
      break;
    case GE:
      if (isInverted)
        op_type = CMPI_PredOp_LessThanOrEquals;
      else
        op_type = CMPI_PredOp_GreaterThanOrEquals;
      break;
    case NE:
      if (isInverted)
        op_type = CMPI_PredOp_Equals;
      else
        op_type = CMPI_PredOp_NotEquals;
      break;
      /* CMPI does not sport this operation. We convert the
         IS NULL to EQ operation. (or NE if "IS NULL" has 'NOT' in
         front (isInverted == true). */
    case IS_NULL:
      if (isInverted)
        op_type = CMPI_PredOp_NotEquals;
      else
        op_type = CMPI_PredOp_Equals;
      break;
    case IS_NOT_NULL:
      if (isInverted)
        op_type = CMPI_PredOp_Equals;
      else
        op_type = CMPI_PredOp_NotEquals;
      break;
    case ISA:
      if (isInverted)
        op_type = CMPI_PredOp_NotIsa;
      else
        op_type = CMPI_PredOp_Isa;
      break;
    case LIKE:
      if (isInverted)
        op_type = CMPI_PredOp_NotLike;
      else
        op_type = CMPI_PredOp_Like;
      break;
    default:
      break;
    }

  return op_type;
}

//
// Routine to map the CQL Value type to CMPI_QueryOperand type.
//
CMPI_QueryOperand::Type
CQL2Type (CQLValue::CQLValueType typ)
{
  switch (typ)
    {
    case CQLValue::Null_type:
      return CMPI_QueryOperand::NULL_TYPE;
    case CQLValue::Sint64_type:
      return CMPI_QueryOperand::SINT64_TYPE;
    case CQLValue::Uint64_type:
      return CMPI_QueryOperand::UINT64_TYPE;
    case CQLValue::Real_type:
      return CMPI_QueryOperand::REAL_TYPE;
    case CQLValue::String_type:
      return CMPI_QueryOperand::STRING_TYPE;
    case CQLValue::CIMDateTime_type:
      return CMPI_QueryOperand::DATETIME_TYPE;
    case CQLValue::CIMReference_type:
      return CMPI_QueryOperand::REFERENCE_TYPE;
    case CQLValue::CQLIdentifier_type:
      return CMPI_QueryOperand::PROPERTY_TYPE;
    case CQLValue::CIMObject_type:
      /* There is not such thing as object type. */
      break;
    case CQLValue::Boolean_type:
      return CMPI_QueryOperand::BOOLEAN_TYPE;
    default:
      break;
    }
  return CMPI_QueryOperand::NULL_TYPE;
}

void
CMPI_Cql2Dnf::_populateTableau ()
{

  cqs.validate ();
  cqs.applyContext ();
  cqs.normalizeToDOC ();

  CQLPredicate pred = cqs.getPredicate ();
  Array < CQLPredicate > pred_Array;
  Array < BooleanOpType > oper_Array = pred.getOperators ();

  if (pred.isSimple ())
    {
      pred_Array.append (pred);
    }
  else
    {
      pred_Array = pred.getPredicates ();
    }

  _tableau.reserveCapacity (pred_Array.size ());

  DDD(cout << "Expression:  " <<  cqs.toString() << endl);

  CMPI_TableauRow tr;
  CQLValue dummy ("");
  for (Uint32 i = 0; i < pred_Array.size (); i++)
    {
      CQLPredicate pred = pred_Array[i];

      if (pred.isSimple ())
        {

          CQLSimplePredicate simple = pred.getSimplePredicate ();

          CQLExpression lhs_cql = simple.getLeftExpression ();
          CQLExpression rhs_cql = simple.getRightExpression ();

          CMPIPredOp opr =
            CQL2PredOp (simple.getOperation (), pred.getInverted ());

          CQLValue lhs_val;
          CQLValue rhs_val;


          if (lhs_cql.isSimpleValue ())
            {
              //if (lhs_cql.getTerms ().size () != 0)
              lhs_val = lhs_cql.getTerms ()[0].getFactors ()[0].getValue ();
            }
          else
            {
              lhs_val = dummy;
            }
          if (rhs_cql.isSimpleValue ())
            {
              //if (rhs_cql.getTerms ().size () != 0)
              rhs_val = rhs_cql.getTerms ()[0].getFactors ()[0].getValue ();
            }
          else
            {
              rhs_val = dummy;
            }
          // Have both side of the operation, such as 'A < 2'
          CMPI_QueryOperand lhs (CQL2String (lhs_cql),
                                 CQL2Type (lhs_val.getValueType ()));
          CMPI_QueryOperand rhs (CQL2String (rhs_cql),
                                 CQL2Type (rhs_val.getValueType ()));

          // Add it as an row to the table row. 
          tr.append (CMPI_term_el (false, opr, lhs, rhs));
          if (i < oper_Array.size ())
            {
              // If the next operation is OR (disjunction), then treat the table row 
              // as as set of conjunctions and ..
              if (oper_Array[i] == OR)
                {
                  // .. put the table of conjunctives in the tableau.  Each element in the 
                  // tableau is a set of conjunctives. It is understood that the boolean
                  // logic seperating the conjunctives is the disjunctive operator (OR).
                  _tableau.append (tr);
                  // Clear the table of conjunctives for the next operands.
                  tr.clear ();
                }

              // If the operator is a conjunction, let the operands pile up on
              // the table row until there is disjunction (OR) or there are ..

            }
          else
            {
              // ... no more operations. This is the last operand. Add it
              // to the tableau as a disjunctions.

              _tableau.append (tr);
            }
        }
    }
}
CMPI_Cql2Dnf::CMPI_Cql2Dnf (const CQLSelectStatement qs):
cqs (qs)
{
  _tableau.clear ();
  _populateTableau ();
}


CMPI_Cql2Dnf::~CMPI_Cql2Dnf ()
{
}


PEGASUS_NAMESPACE_END
