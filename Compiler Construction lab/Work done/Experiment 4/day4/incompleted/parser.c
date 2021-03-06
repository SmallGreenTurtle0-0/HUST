/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "error.h"
#include "debug.h"


Token *currentToken;
Token *lookAhead;

extern Type* intType;
extern Type* charType;
extern SymTab* symtab;

void scan(void) {
  Token* tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType) {
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);
    scan();
  } else missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
}

void compileProgram(void) {
  Object* program;

  eat(KW_PROGRAM);
  eat(TK_IDENT);

  program = createProgramObject(currentToken->string);
  enterBlock(program->progAttrs->scope);

  eat(SB_SEMICOLON);

  compileBlock();
  eat(SB_PERIOD);

  exitBlock();
}

void compileBlock(void) {
  Object* constObj;
  ConstantValue* constValue;

  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);

    do {
      eat(TK_IDENT);
      // Check if a constant identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      // Create a constant object
      constObj = createConstantObject(currentToken->string);
      
      eat(SB_EQ);
      // Get the constant value
      constValue = compileConstant();
      constObj->constAttrs->value = constValue;
      // Declare the constant object 
      declareObject(constObj);
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  } 
  else compileBlock2();
}

void compileBlock2(void) {
  Object* typeObj;
  Type* actualType;

  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);

    do {
      eat(TK_IDENT);
      // Check if a type identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      // create a type object
      typeObj = createTypeObject(currentToken->string);
      
      eat(SB_EQ);
      // Get the actual type
      actualType = compileType();
      typeObj->typeAttrs->actualType = actualType;
      // Declare the type object
      declareObject(typeObj);
      
      eat(SB_SEMICOLON);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  } 
  else compileBlock3();
}

void compileBlock3(void) {
  Object* varObj;
  Type* varType;

  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);

    do {
      eat(TK_IDENT);
      // Check if a variable identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      // Create a variable object      
      varObj = createVariableObject(currentToken->string);

      eat(SB_COLON);
      // Get the variable type
      varType = compileType();
      varObj->varAttrs->type = varType;
      // Declare the variable object
      eat(SB_SEMICOLON);
      declareObject(varObj);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock4();
  } 
  else compileBlock4();
}

void compileBlock4(void) {
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void) {
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE)) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else compileProcDecl();
  }
}

void compileFuncDecl(void) {
  Object* funcObj;
  Type* returnType;

  eat(KW_FUNCTION);
  eat(TK_IDENT);
  // Check if a function identifier is fresh in the block
  checkFreshIdent(currentToken->string);
  // create the function object
  funcObj = createFunctionObject(currentToken->string);
  // declare the function object
  declareObject(funcObj);
  // enter the function's block
  enterBlock(funcObj->funcAttrs->scope);
  // parse the function's parameters
  compileParams();
  eat(SB_COLON);
  // get the funtion's return type
  returnType = compileBasicType();
  funcObj->funcAttrs->returnType = returnType;

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  // exit the function block
  exitBlock();
}

void compileProcDecl(void) {
  Object* procObj;

  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  // Check if a procedure identifier is fresh in the block
  checkFreshIdent(currentToken->string);
  // create a procedure object
  procObj = createProcedureObject(currentToken->string);
  // declare the procedure object
  declareObject(procObj);
  // enter the procedure's block
  enterBlock(procObj->procAttrs->scope);
  // parse the procedure's parameters
  compileParams();

  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  // exit the block
  exitBlock();
}

ConstantValue* compileUnsignedConstant(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value.intvalue);
    break;
   case TK_FLOAT:
    eat(TK_FLOAT);
    constValue = makeFloatConstant(currentToken->value.floatvalue);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // check if the constant identifier is declared and get its value
    obj = lookupObject(currentToken->string);
    if(obj == NULL)
      error(ERR_UNDECLARED_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    else if(obj->kind != OBJ_CONSTANT)
      error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    constValue = duplicateConstantValue(obj->constAttrs->value);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

ConstantValue* compileConstant(void) {
  ConstantValue* constValue;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    constValue->intValue = - constValue->intValue;
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue* compileConstant2(void) {
  ConstantValue* constValue;
  Object* obj;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value.intvalue);
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    constValue = makeFloatConstant(currentToken->value.floatvalue);
    break;
  case TK_IDENT:
    // check if the integer constant identifier is declared and get its value
    eat(TK_IDENT);
    obj = lookupObject(currentToken->string);
    if(obj == NULL)
      error(ERR_UNDECLARED_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    else if(obj->kind != OBJ_CONSTANT)
      error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    constValue = duplicateConstantValue(obj->constAttrs->value);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return constValue;
}

Type* compileType(void) {
  Type* type;
  Type* elementType;
  int arraySize;
  Object* obj;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER);
    type =  makeIntType();
    break;
  case KW_FLOAT:
    eat(KW_FLOAT);
    type = makeFloatType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  case KW_ARRAY:
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);

    arraySize = currentToken->value.intvalue;

    eat(SB_RSEL);
    eat(KW_OF);
    elementType = compileType();
    type = makeArrayType(arraySize, elementType);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // check if the type idntifier is declared and get its actual type
    obj = lookupObject(currentToken->string);
    if(obj == NULL)
      error(ERR_UNDECLARED_TYPE, lookAhead->lineNo, lookAhead->colNo);
    else if(obj->kind != OBJ_TYPE)
      error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    type = duplicateType(obj->typeAttrs->actualType);
    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

Type* compileBasicType(void) {
  Type* type;

  switch (lookAhead->tokenType) {
  case KW_INTEGER: 
    eat(KW_INTEGER); 
    type = makeIntType();
    break;
  case KW_FLOAT:
    eat(KW_FLOAT);
    type = makeFloatType();
    break;
  case KW_CHAR: 
    eat(KW_CHAR); 
    type = makeCharType();
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  return type;
}

void compileParams(void) {
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON) {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
}

void compileParam(void) {
  Object* param;
  Type* type;
  enum ParamKind paramKind;

  switch (lookAhead->tokenType) {
  case TK_IDENT:
    paramKind = PARAM_VALUE;
    break;
  case KW_VAR:
    eat(KW_VAR);
    paramKind = PARAM_REFERENCE;
    break;
  default:
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  eat(TK_IDENT);
  // check if the parameter identifier is fresh in the block
  param = createParameterObject(currentToken->string, paramKind, symtab->currentScope->owner);
  eat(SB_COLON);
  type = compileBasicType();
  param->paramAttrs->type = type;
  declareObject(param);
}

void compileStatements(void) {
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

Type* compileLValue(void) {
  // TODO: parse a lvalue (a variable, an array element, a parameter, the current function identifier)
  
  Object* var;
  Type* varType;

  eat(TK_IDENT);
  // check if the identifier is a function identifier, or a variable identifier, or a parameter  
  var = checkDeclaredLValueIdent(currentToken->string);

  switch (var->kind) {
  case OBJ_VARIABLE:
    varType = compileIndexes(var->varAttrs->type);
    break;
  case OBJ_FUNCTION:
    varType = var->funcAttrs->returnType;
    break;
  case OBJ_PARAMETER:
    varType = var->paramAttrs->type;
    break;
  default:
    break;
  }
  
  return varType;
}

void compileAssignSt(void) {
  // TODO: parse the assignment and check type consistency
  Type* type1;
  Type* type2;
  type1 = compileLValue();
  switch (lookAhead->tokenType) {
  case SB_ASSIGN:
    eat(SB_ASSIGN);
    break;
  case SB_ASSIGN_PLUS:
    eat(SB_ASSIGN_PLUS);
    break;
  case SB_ASSIGN_MINUS:
    eat(SB_ASSIGN_MINUS);
    break;
  case SB_ASSIGN_MOD:
    eat(SB_ASSIGN_MOD);
    break;
  case SB_ASSIGN_SLASH:
    eat(SB_ASSIGN_SLASH);
    break;
  case SB_ASSIGN_TIMES:
    eat(SB_ASSIGN_TIMES);
  default:
    break;
  }
  type2 = compileExpression();
  checkTypeEquality(type1, type2);
}

void compileCallSt(void) {
  Object* proc;

  eat(KW_CALL);
  eat(TK_IDENT);
  // check if the identifier is a declared procedure
  proc = checkDeclaredProcedure(currentToken->string);

  compileArguments(proc->procAttrs->paramList);
}

void compileGroupSt(void) {
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void) {
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();
}

void compileElseSt(void) {
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void) {
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void) {
  // TODO: Check type consistency of FOR's variable
  
  eat(KW_FOR);
  eat(TK_IDENT);

  // check if the identifier is a variable
  Object* obj;
  obj = checkDeclaredVariable(currentToken->string);
  if(obj->varAttrs->type->typeClass == TP_FLOAT)
    error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);
  // if(obj->varAttrs->type->typeClass == TP_CHAR)
  //   error(ERR_FOR_INDEX_CHAR, currentToken->lineNo, currentToken->colNo);

  eat(SB_ASSIGN);
  Type* type1;
  type1 = compileExpression();
  if(type1->typeClass == TP_FLOAT)
    error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);
  // if(type1->typeClass == TP_CHAR)
  //   error(ERR_FOR_INDEX_CHAR, currentToken->lineNo, currentToken->colNo);
  checkTypeEquality(type1, obj->varAttrs->type);

  eat(KW_TO);
  Type* type2;
  type2 = compileExpression();
  if(type2->typeClass == TP_FLOAT)
    error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);
  // if(type2->typeClass == TP_CHAR)
  //   error(ERR_FOR_INDEX_CHAR, currentToken->lineNo, currentToken->colNo);
  checkTypeEquality(type1, type2);

  eat(KW_DO);
  compileStatement();
}

void compileArgument(Object* param) {
  // TODO: parse an argument, and check type consistency
  //       If the corresponding parameter is a reference, the argument must be a lvalue

  if(param->paramAttrs->kind == PARAM_REFERENCE) {
    if(lookAhead->tokenType == TK_IDENT)
      checkDeclaredLValueIdent(lookAhead->string);
    else 
      error(ERR_TYPE_INCONSISTENCY, lookAhead->lineNo, lookAhead->colNo);
  }
  checkTypeEquality(compileExpression(), param->paramAttrs->type); 
}

void compileArguments(ObjectNode* paramList) {
  //TODO: parse a list of arguments, check the consistency of the arguments and the given parameters
  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);
    compileArgument(paramList->object);

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      paramList = paramList->next;
      if(paramList != NULL)
        compileArgument(paramList->object);
      else
        error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    }
    if(paramList->next != NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

    eat(SB_RPAR);
    break;
    // Check FOLLOW set 
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void) {
  // TODO: check the type consistency of LHS and RSH, check the basic type
  Type* type1;
  type1 = compileExpression();
  checkBasicType(type1);

  switch (lookAhead->tokenType) {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }
  Type* type2;
  type2 = compileExpression();
  
  checkTypeEquality(type1, type2);
}

Type* compileExpression(void) {
  Type* type;
  
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    type = compileExpression2();
    checkResultType(type);
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type = compileExpression2();
    checkResultType(type);
    break;
  default:
    type = compileExpression2();
  }
  
  return type;
}

Type* compileExpression2(void) {
  Type* type1;
  Type* type2;

  type1 = compileTerm();
 
  type2 = compileExpression3();
  if (type2 == NULL) return type1;
  else {
    checkTypeEquality(type1,type2);
    return type1;
  }
  
}


Type* compileExpression3(void) {
  Type* type1;
  Type* type2;

  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    type1 = compileTerm();
    checkIntType(type1);
    type2 = compileExpression3();
    if (type2 != NULL)
      checkIntType(type2);
    return type1;
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type1 = compileTerm();
    checkIntType(type1);
    type2 = compileExpression3();
    if (type2 != NULL)
      checkIntType(type2);
    return type1;
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    return NULL;
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

Type* compileTerm(void) {
  // TODO: check type of Term2
  Type* type;

  type = compileFactor();
  //checkResultType(type);

  if(lookAhead->tokenType == SB_MOD && type->typeClass != TP_INT)
    error(ERR_MODULO_OPERATOR, currentToken->lineNo, currentToken->colNo);
  compileTerm2();

  return type;
}

void compileTerm2(void) {
  // TODO: check type of term2
  Type* type;

  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    type = compileFactor();
    checkResultType(type);
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
    type = compileFactor();
    checkResultType(type);
    compileTerm2();
    break;
  case SB_MOD:
    eat(SB_MOD);
    type = compileFactor();
    if(type->typeClass != TP_INT)
      error(ERR_MODULO_OPERATOR, currentToken->lineNo, currentToken->colNo);
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

Type* compileFactor(void) {
  // TODO: parse a factor and return the factor's type
  
  Object* obj;
  Type* type;

  switch (lookAhead->tokenType) {
  case TK_NUMBER:
    eat(TK_NUMBER);
    type = makeIntType();
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    type = makeFloatType();
    break;
  case TK_CHAR: 
    eat(TK_CHAR);
    type = makeCharType();
    break;
  case SB_LPAR:
    eat(SB_LPAR);
    type = compileExpression();
    eat(SB_RPAR);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // check if the identifier is declared
    if(lookAhead->tokenType == SB_LPAR)
      obj = checkDeclaredFunction(currentToken->string);
    else 
      obj = checkDeclaredIdent(currentToken->string);
    switch (obj->kind) {
    case OBJ_CONSTANT:
      if(obj->constAttrs->value->type == TP_INT)
        type = makeIntType();
      if(obj->constAttrs->value->type == TP_FLOAT)
        type = makeFloatType();
      if(obj->constAttrs->value->type == TP_CHAR)
        type = makeCharType();
      break;
    case OBJ_VARIABLE:
      if(obj->varAttrs->type->typeClass != TP_ARRAY)
        type = obj->varAttrs->type;
      else
        type = compileIndexes(obj->varAttrs->type);
      break;
    case OBJ_PARAMETER:
      type = obj->paramAttrs->type;
      break;
    case OBJ_FUNCTION:
      type = obj->funcAttrs->returnType;
      compileArguments(obj->funcAttrs->paramList);
      break;
    default: 
      error(ERR_INVALID_FACTOR,currentToken->lineNo, currentToken->colNo);
      break;
    }
    break;
  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }

  return type;
}

Type* compileIndexes(Type* arrayType) {
  // TODO: parse a sequence of indexes, check the consistency to the arrayType, and return the element type
  Type* index;
  Type* element;
  
  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);
    checkArrayType(arrayType);
    
    index = compileExpression();
    checkIntType(index);

    eat(SB_RSEL);

    arrayType = arrayType->elementType;
  }
  return arrayType;
}

int compile(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();

  compileProgram();

  printObject(symtab->program,0);

  cleanSymTab();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;

}
