#include "CQLFactorRep.h"
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLFunction.h>
#include <Pegasus/CQL/CQLFactory.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLScope.h>
#include <Pegasus/CQL/QueryContext.h>
 
PEGASUS_NAMESPACE_BEGIN
/*
#define PEGASUS_ARRAY_T CQLFactorRep
#include <Pegasus/Common/ArrayImpl.h>
#undef PEGASUS_ARRAY_T
*/
CQLFactorRep::CQLFactorRep(const CQLFactorRep& inCQLFact)
{
   _CQLVal = inCQLFact._CQLVal;
   _CQLFunct = inCQLFact._CQLFunct;
   _CQLExp = inCQLFact._CQLExp;
   _invert = inCQLFact._invert;
   _simpleValue = inCQLFact._simpleValue;
}

CQLFactorRep::CQLFactorRep(CQLValue& inCQLVal)
{
   _CQLVal = inCQLVal;
   _simpleValue = true;
}

CQLFactorRep::CQLFactorRep(CQLExpression& inCQLExp)
{
   _CQLExp = inCQLExp;
   _simpleValue = false;
}

CQLFactorRep::CQLFactorRep(CQLFunction& inCQLFunc)
{
   _CQLFunct = inCQLFunc;
   _simpleValue = false;
}

CQLValue CQLFactorRep::getValue()
{
   return _CQLVal;
}

CQLValue CQLFactorRep::resolveValue(CIMInstance CI, QueryContext& QueryCtx)
{

   if(_CQLExp != CQLExpression())
   {
      return _CQLExp.resolveValue(CI,QueryCtx);
   }
   else if (_CQLFunct != CQLFunction())
   {
      return _CQLFunct.resolveValue(CI,QueryCtx);
   }
   else
   {
      _CQLVal.resolve(CI,QueryCtx);
      return _CQLVal;
   }
}

Boolean CQLFactorRep::isSimpleValue()
{
   return _simpleValue;
}

CQLFunction CQLFactorRep::getCQLFunction()
{
   return _CQLFunct;
}

CQLExpression CQLFactorRep::getCQLExpression()
{
   return _CQLExp;
}

String CQLFactorRep::toString()
{
   if(_CQLFunct != CQLFunction())
   {
      return _CQLFunct.toString();
   }
   else if(_CQLExp != CQLExpression())
   {
      return _CQLExp.toString();
   }
   else
   {
      return _CQLVal.toString();
   }
}

void CQLFactorRep::applyScopes(Array<CQLScope> inScopes)
{
   
   if(_CQLFunct != CQLFunction())
   {
      _CQLFunct.applyScopes(inScopes);
   }
   else if(_CQLExp != CQLExpression())
   {
      _CQLExp.applyScopes(inScopes);
   }
   else 
   {
      _CQLVal.applyScopes(inScopes);
   }
   return;
}

Boolean CQLFactorRep::operator==(const CQLFactorRep& rep){
	return true;
}
Boolean CQLFactorRep::operator!=(const CQLFactorRep& rep){
	return (!operator==(rep));                                                                                
}
PEGASUS_NAMESPACE_END
