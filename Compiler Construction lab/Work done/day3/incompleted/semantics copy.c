/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "semantics.h"
#include "error.h"
#include "symtab.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
   
  Scope* scope = symtab->currentScope; 
  Object* object = (Object*) malloc(sizeof(Object));
  while(scope != NULL) {
    object = findObject(scope->objList, name);
    if(object != NULL)
      return object;
      
    scope = scope->outer;
  }
  object = findObject(symtab->globalObjectList, name);
  return object;
}

void checkFreshIdent(char *name) {
   
  Scope* scope = symtab->currentScope; 
  Object* object = findObject(scope->objList, name);
  if(object != NULL)
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredIdent(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredConstant(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_CONSTANT)
    error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredType(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_TYPE)
    error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredVariable(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_VARIABLE)
    error(ERR_INVALID_VARIABLE, currentToken->lineNo, currentToken->colNo);
  return object;

}

Object* checkDeclaredFunction(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_FUNCTION)
    error(ERR_INVALID_FUNCTION, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredProcedure(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_PROCEDURE)
    error(ERR_INVALID_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredLValueIdent(char* name) {
   
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  switch (object->kind) {
  case OBJ_VARIABLE:
    break;
  case OBJ_PARAMETER:
    break;
  case OBJ_FUNCTION:
    if(object != symtab->currentScope->owner)
      error(ERR_INVALID_IDENT, currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_IDENT, currentToken->lineNo, currentToken->colNo);
    break;
  }
  return object;
}

void checkIntType(Type* type) {
  // TODO
  if(type->typeClass != TP_INT)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkCharType(Type* type) {
  // TODO
  if(type->typeClass != TP_CHAR)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkFloatType(Type* type) {
  // TODO
  if(type->typeClass != TP_FLOAT)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkResultType(Type* type) {
  // TODO
  if(type->typeClass != TP_FLOAT && type->typeClass != TP_INT)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkBasicType(Type* type) {
  // TODO
  switch (type->typeClass) {
  case TP_INT:
    break;
  case TP_CHAR:
    break;
  case TP_FLOAT:
    break;
  default:
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    break;
  }
  return;
}

void checkArrayType(Type* type) {
  // TODO
  if(type->typeClass != TP_ARRAY)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkTypeEquality(Type* type1, Type* type2) {
  // TODO
  if(type1->typeClass != type2->typeClass)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
  if(type1->typeClass == TP_ARRAY) {
    checkTypeEquality(type1->elementType, type2->elementType);
    if(type1->arraySize != type2->arraySize)
      error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
  }
}
