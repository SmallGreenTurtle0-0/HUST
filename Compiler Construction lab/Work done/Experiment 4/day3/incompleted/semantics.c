/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "semantics.h"
#include "semantics.h"
#include "error.h"
#include "symtab.h"

extern SymTab* symtab;
extern Token* currentToken;

Object* lookupObject(char *name) {
  // TODO
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
  // TODO
  Scope* scope = symtab->currentScope; 
  Object* object = findObject(scope->objList, name);
  if(object != NULL)
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredIdent(char* name) {
  // TODO
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredConstant(char* name) {
  // TODO
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_CONSTANT)
    error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredType(char* name) {
  // TODO
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_TYPE)
    error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredVariable(char* name) {
  // TODO
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_VARIABLE)
    error(ERR_INVALID_VARIABLE, currentToken->lineNo, currentToken->colNo);
  return object;

}

Object* checkDeclaredFunction(char* name) {
  // TODO
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_FUNCTION)
    error(ERR_INVALID_FUNCTION, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredProcedure(char* name) {
  // TODO
  Object* object = lookupObject(name);
  if(object == NULL)
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  if(object->kind != OBJ_PROCEDURE)
    error(ERR_INVALID_PROCEDURE, currentToken->lineNo, currentToken->colNo);
  return object;
}

Object* checkDeclaredLValueIdent(char* name) {
  // TODO
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


