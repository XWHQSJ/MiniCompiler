#pragma once
#ifndef MINICC_AST_PRINT_H
#define MINICC_AST_PRINT_H

#include "ast.h"
#include <string>

namespace minicc {

std::string ast_to_xml(const AstNode* node, int indent = 0);
std::string ast_to_json(const AstNode* node, int indent = 0);

} // namespace minicc

#endif
