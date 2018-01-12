#ifndef _TRANSLATE_H
#define _TRANSLATE_H

#include "LinerIR.h"
#include "NodeTree.h"

void translate(Node* node);
Operand newOperand();
InterCode newInterCode();
InterCodes newInterCodes();

#endif
