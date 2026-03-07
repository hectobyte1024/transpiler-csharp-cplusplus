#include "CodeGenerator.h"
#include <algorithm>

namespace Compiler {

CodeGenerator::CodeGenerator() : indentLevel(0), isGeneratingHeader(false) {}

void CodeGenerator::indent() {
    for (int i = 0; i < indentLevel; i++) {
        if (isGeneratingHeader) {
            headerOutput << "    ";
        } else {
            output << "    ";
        }
    }
}

void CodeGenerator::writeLine(const std::string& line) {
    indent();
    if (isGeneratingHeader) {
        headerOutput << line << "\n";
    } else {
        output << line << "\n";
    }
}

void CodeGenerator::write(const std::string& text) {
    if (isGeneratingHeader) {
        headerOutput << text;
    } else {
        output << text;
    }
}

std::string CodeGenerator::convertType(const std::string& csType) {
    // Convert C# types to C++ types
    if (csType == "int") return "int";
    if (csType == "float") return "float";
    if (csType == "double") return "double";
    if (csType == "bool") return "bool";
    if (csType == "char") return "char";
    if (csType == "string") return "std::string";
    if (csType == "void") return "void";
    if (csType == "null") return "nullptr";
    
    // Custom types remain the same
    return csType;
}

std::string CodeGenerator::convertOperator(const std::string& op) {
    // Most operators are the same in C# and C++
    return op;
}

std::string CodeGenerator::generate(Program& program) {
    output.str("");
    output.clear();
    
    // Add standard includes
    writeLine("#include <iostream>");
    writeLine("#include <string>");
    writeLine("#include <memory>");
    writeLine("");
    
    program.accept(*this);
    
    return output.str();
}

std::string CodeGenerator::generateHeader(Program& program) {
    headerOutput.str("");
    headerOutput.clear();
    isGeneratingHeader = true;
    
    // Header guards
    writeLine("#ifndef GENERATED_H");
    writeLine("#define GENERATED_H");
    writeLine("");
    writeLine("#include <iostream>");
    writeLine("#include <string>");
    writeLine("#include <memory>");
    writeLine("#include <stdexcept>");
    writeLine("");
    
    program.accept(*this);
    
    writeLine("");
    writeLine("#endif // GENERATED_H");
    
    isGeneratingHeader = false;
    return headerOutput.str();
}

std::string CodeGenerator::generateSource(Program& program) {
    output.str("");
    output.clear();
    isGeneratingHeader = false;
    
    writeLine("#include \"generated.h\"");
    writeLine("");
    
    program.accept(*this);
    
    return output.str();
}

void CodeGenerator::visit(BinaryExpression& node) {
    write("(");
    node.left->accept(*this);
    write(" " + convertOperator(node.op) + " ");
    node.right->accept(*this);
    write(")");
}

void CodeGenerator::visit(UnaryExpression& node) {
    if (node.op == "++_post" || node.op == "--_post") {
        write("(");
        node.operand->accept(*this);
        write(node.op.substr(0, 2));
        write(")");
    } else {
        write("(" + convertOperator(node.op));
        node.operand->accept(*this);
        write(")");
    }
}

void CodeGenerator::visit(LiteralExpression& node) {
    if (node.type == "string") {
        write(node.value);
    } else if (node.type == "null") {
        write("nullptr");
    } else {
        write(node.value);
    }
}

void CodeGenerator::visit(IdentifierExpression& node) {
    write(node.name);
}

void CodeGenerator::visit(CallExpression& node) {
    node.callee->accept(*this);
    write("(");
    
    for (size_t i = 0; i < node.arguments.size(); i++) {
        if (i > 0) write(", ");
        node.arguments[i]->accept(*this);
    }
    
    write(")");
}

void CodeGenerator::visit(MemberExpression& node) {
    node.object->accept(*this);
    write(".");
    write(node.member);
}

void CodeGenerator::visit(ExpressionStatement& node) {
    indent();
    node.expression->accept(*this);
    write(";\n");
}

void CodeGenerator::visit(VariableDeclaration& node) {
    indent();
    write(convertType(node.type) + " " + node.name);
    
    if (node.initializer) {
        write(" = ");
        node.initializer->accept(*this);
    }
    
    write(";\n");
}

void CodeGenerator::visit(BlockStatement& node) {
    writeLine("{");
    indentLevel++;
    
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    
    indentLevel--;
    writeLine("}");
}

void CodeGenerator::visit(IfStatement& node) {
    indent();
    write("if (");
    node.condition->accept(*this);
    write(")\n");
    
    node.thenBranch->accept(*this);
    
    if (node.elseBranch) {
        writeLine("else");
        node.elseBranch->accept(*this);
    }
}

void CodeGenerator::visit(WhileStatement& node) {
    indent();
    write("while (");
    node.condition->accept(*this);
    write(")\n");
    
    node.body->accept(*this);
}

void CodeGenerator::visit(ForStatement& node) {
    indent();
    write("for (");
    
    if (node.initializer) {
        // For initializer, we need to handle it specially (no indent, no newline)
        auto varDecl = dynamic_cast<VariableDeclaration*>(node.initializer.get());
        if (varDecl) {
            write(convertType(varDecl->type) + " " + varDecl->name);
            if (varDecl->initializer) {
                write(" = ");
                varDecl->initializer->accept(*this);
            }
        } else {
            node.initializer->accept(*this);
        }
    }
    write("; ");
    
    if (node.condition) {
        node.condition->accept(*this);
    }
    write("; ");
    
    if (node.increment) {
        node.increment->accept(*this);
    }
    write(")\n");
    
    node.body->accept(*this);
}

void CodeGenerator::visit(ReturnStatement& node) {
    indent();
    write("return");
    
    if (node.value) {
        write(" ");
        node.value->accept(*this);
    }
    
    write(";\n");
}

void CodeGenerator::visit(FunctionDeclaration& node) {
    indent();
    
    // Write return type and name
    write(convertType(node.returnType) + " " + node.name + "(");
    
    // Write parameters
    for (size_t i = 0; i < node.parameters.size(); i++) {
        if (i > 0) write(", ");
        write(convertType(node.parameters[i].first) + " " + node.parameters[i].second);
    }
    
    write(")\n");
    
    // Write body
    if (node.body) {
        node.body->accept(*this);
    }
    
    write("\n");
}

void CodeGenerator::visit(ClassDeclaration& node) {
    currentClassName = node.name;
    
    // Generate template declaration if generics are used
    std::string templateDecl = "";
    if (!node.templateParams.empty()) {
        templateDecl = "template<";
        for (size_t i = 0; i < node.templateParams.size(); i++) {
            if (i > 0) templateDecl += ", ";
            templateDecl += "typename " + node.templateParams[i];
        }
        templateDecl += ">";
    }
    
    if (isGeneratingHeader) {
        // Header mode: generate class declaration
        if (!templateDecl.empty()) {
            writeLine(templateDecl);
        }
        
        std::string classLine = node.isAbstract ? "class " : "class ";
        classLine += node.name;
        
        // Add inheritance
        std::vector<std::string> bases;
        if (!node.baseClass.empty()) {
            bases.push_back("public " + node.baseClass);
        }
        for (const auto& iface : node.interfaces) {
            bases.push_back("public " + iface);
        }
        
        if (!bases.empty()) {
            classLine += " : ";
            for (size_t i = 0; i < bases.size(); i++) {
                if (i > 0) classLine += ", ";
                classLine += bases[i];
            }
        }
        
        writeLine(classLine);
        writeLine("{");
        
        indentLevel++;
        
        // Write fields
        if (!node.fields.empty()) {
            writeLine("private:");
            indentLevel++;
            for (auto& field : node.fields) {
                field->accept(*this);
            }
            indentLevel--;
            write("\n");
        }
        
        // Write method declarations (grouped by access modifier)
        if (!node.methods.empty()) {
            std::string currentAccess = "";
            
            for (auto& method : node.methods) {
                std::string access = "public";
                
                // Determine access modifier
                for (const auto& mod : method->modifiers) {
                    if (mod == "public" || mod == "private" || mod == "protected") {
                        access = mod;
                        break;
                    }
                }
                
                if (access != currentAccess) {
                    currentAccess = access;
                    indentLevel--;
                    writeLine(access + ":");
                    indentLevel++;
                }
                
                indent();
                
                // Write static if needed
                bool isStatic = false;
                bool isVirtual = false;
                bool isOverride = false;
                
                for (const auto& mod : method->modifiers) {
                    if (mod == "static") isStatic = true;
                    if (mod == "virtual" || mod == "abstract") isVirtual = true;
                    if (mod == "override") isOverride = true;
                }
                
                if (isStatic) write("static ");
                if (isVirtual && !isOverride) write("virtual ");
                
                write(convertType(method->returnType) + " " + method->name + "(");
                
                for (size_t i = 0; i < method->parameters.size(); i++) {
                    if (i > 0) write(", ");
                    write(convertType(method->parameters[i].first) + " " + method->parameters[i].second);
                }
                
                write(")");
                
                // Pure virtual or override
                if (method->isPureVirtual) {
                    write(" = 0");
                } else if (isOverride) {
                    write(" override");
                }
                
                write(";\n");
            }
        }
        
        indentLevel--;
        writeLine("};");
        write("\n");
        
    } else {
        // Source mode: generate method implementations
        for (auto& method : node.methods) {
            // Skip pure virtual methods
            if (method->isPureVirtual || !method->body) {
                continue;
            }
            
            // Skip methods in template classes (must be in header)
            if (!templateDecl.empty()) {
                continue;
            }
            
            indent();
            write(convertType(method->returnType) + " " + currentClassName + "::" + method->name + "(");
            
            for (size_t i = 0; i < method->parameters.size(); i++) {
                if (i > 0) write(", ");
                write(convertType(method->parameters[i].first) + " " + method->parameters[i].second);
            }
            
            write(")\n");
            
            method->body->accept(*this);
            write("\n");
        }
    }
    
    currentClassName = "";
}

void CodeGenerator::visit(NamespaceDeclaration& node) {
    writeLine("namespace " + node.name);
    writeLine("{");
    
    indentLevel++;
    
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
    
    indentLevel--;
    writeLine("}");
    write("\n");
}

void CodeGenerator::visit(UsingDirective& node) {
    // Convert C# using to C++ using
    writeLine("using namespace " + node.namespaceName + ";");
}

void CodeGenerator::visit(InterfaceDeclaration& node) {
    // Generate interface as pure virtual base class
    if (isGeneratingHeader) {
        writeLine("// Interface: " + node.name);
        writeLine("class " + node.name);
        writeLine("{");
        indentLevel++;
        writeLine("public:");
        indentLevel++;
        
        // All interface methods are pure virtual
        for (auto& method : node.methods) {
            indent();
            write("virtual " + convertType(method->returnType) + " " + method->name + "(");
            
            for (size_t i = 0; i < method->parameters.size(); i++) {
                if (i > 0) write(", ");
                write(convertType(method->parameters[i].first) + " " + method->parameters[i].second);
            }
            
            write(") = 0;\n");
        }
        
        // Virtual destructor
        indentLevel--;
        indentLevel--;
        writeLine("public:");
        indentLevel++;
        writeLine("virtual ~" + node.name + "() = default;");
        
        indentLevel--;
        writeLine("};");
        write("\n");
    }
    // No source mode implementation needed for interfaces
}

void CodeGenerator::visit(TryStatement& node) {
    writeLine("try");
    
    // Try block
    if (node.tryBlock) {
        node.tryBlock->accept(*this);
    }
    
    // Catch blocks (catchBlocks is std::vector<std::pair<string, unique_ptr<BlockStatement>>>)
    for (auto& catchPair : node.catchBlocks) {
        write(" ");
        indent();
        write("catch (");
        
        if (!catchPair.first.empty()) {
            write("const " + convertType(catchPair.first) + "& ex");
        } else {
            write("...");
        }
        
        write(")\n");
        
        if (catchPair.second) {
            catchPair.second->accept(*this);
        }
    }
    
    // Finally block (C++ doesn't have finally, use RAII wrapper)
    if (node.finallyBlock) {
        write("\n");
        indent();
        write("// Finally block - using RAII pattern\n");
        indent();
        write("{\n");
        indentLevel++;
        indent();
        write("struct FinallyGuard {\n");
        indentLevel++;
        indent();
        write("std::function<void()> finallyFunc;\n");
        indent();
        write("FinallyGuard(std::function<void()> f) : finallyFunc(f) {}\n");
        indent();
        write("~FinallyGuard() { if (finallyFunc) finallyFunc(); }\n");
        indentLevel--;
        indent();
        write("} guard([&]() ");
        node.finallyBlock->accept(*this);
        write(");\n");
        indentLevel--;
        indent();
        write("}\n");
    }
}

void CodeGenerator::visit(ThrowStatement& node) {
    indent();
    write("throw ");
    if (node.expression) {
        node.expression->accept(*this);
    }
    write(";\n");
}

void CodeGenerator::visit(UsingStatement& node) {
    // RAII wrapper for C# using statement
    // resource is a VariableDeclaration
    writeLine("// Using statement - RAII pattern");
    writeLine("{");
    indentLevel++;
    
    // Declare the resource from VariableDeclaration
    if (node.resource) {
        node.resource->accept(*this);
    }
    
    // Body
    if (node.body) {
        node.body->accept(*this);
    }
    
    indentLevel--;
    writeLine("}  // RAII cleanup on scope exit");
}

void CodeGenerator::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

void CodeGenerator::writeFormatted(const std::string& code, const FormatOptions& options) {
    std::string formatted = code;
    size_t pos = 0;
    
    // Apply indentation style
    std::string indentStr;
    if (options.useTabs) {
        indentStr = "\t";
    } else {
        indentStr = std::string(options.indentSize, ' ');
    }
    
    // Replace existing indentation
    size_t lineStart = 0;
    while ((pos = formatted.find('\n', lineStart)) != std::string::npos) {
        size_t nextLineStart = pos + 1;
        size_t firstNonSpace = formatted.find_first_not_of(" \t", nextLineStart);
        
        if (firstNonSpace != std::string::npos) {
            // Count current indent level
            int spaces = 0;
            for (size_t i = nextLineStart; i < firstNonSpace; i++) {
                spaces += (formatted[i] == '\t') ? 4 : 1;
            }
            
            int indentCount = spaces / 4;
            std::string newIndent;
            for (int i = 0; i < indentCount; i++) {
                newIndent += indentStr;
            }
            
            formatted.replace(nextLineStart, firstNonSpace - nextLineStart, newIndent);
        }
        
        lineStart = formatted.find('\n', nextLineStart);
        if (lineStart == std::string::npos) break;
    }
    
    // Apply spacing around parentheses
    if (options.spaceBeforeParen) {
        // Add space before opening paren for function calls
        for (const std::string& keyword : {"if", "while", "for", "switch", "catch"}) {
            std::string target = keyword + "(";
            std::string replacement = keyword + " (";
            pos = 0;
            while ((pos = formatted.find(target, pos)) != std::string::npos) {
                formatted.replace(pos, target.length(), replacement);
                pos += replacement.length();
            }
        }
    }
    
    // Apply brace placement
    if (!options.newlineBeforeBrace) {
        // Move opening braces to same line
        pos = 0;
        while ((pos = formatted.find("\n{", pos)) != std::string::npos) {
            // Check if there's only whitespace before the newline
            size_t prevChar = formatted.find_last_not_of(" \t", pos - 1);
            if (prevChar != std::string::npos) {
                formatted.replace(pos, 2, " {");
            }
            pos++;
        }
    }
    
    write(formatted);
}

std::string CodeGenerator::formatExpression(const std::string& expr, const FormatOptions& options) {
    std::string formatted = expr;
    
    // Add spaces around binary operators
    const std::vector<std::string> binaryOps = {"+", "-", "*", "/", "=", "==", "!=", "<", ">", "<=", ">="};
    for (const std::string& op : binaryOps) {
        size_t pos = 0;
        while ((pos = formatted.find(op, pos)) != std::string::npos) {
            // Check if not already surrounded by spaces
            bool needSpaceBefore = (pos == 0 || formatted[pos - 1] != ' ');
            bool needSpaceAfter = (pos + op.length() >= formatted.length() || 
                                   formatted[pos + op.length()] != ' ');
            
            if (needSpaceBefore && needSpaceAfter) {
                formatted.replace(pos, op.length(), " " + op + " ");
                pos += op.length() + 2;
            } else if (needSpaceBefore) {
                formatted.insert(pos, " ");
                pos += op.length() + 1;
            } else if (needSpaceAfter) {
                formatted.insert(pos + op.length(), " ");
                pos += op.length() + 1;
            } else {
                pos += op.length();
            }
        }
    }
    
    // Handle spacing inside parentheses
    if (options.spaceInsideParen) {
        size_t pos = 0;
        while ((pos = formatted.find('(', pos)) != std::string::npos) {
            if (pos + 1 < formatted.length() && formatted[pos + 1] != ' ' && formatted[pos + 1] != ')') {
                formatted.insert(pos + 1, " ");
            }
            pos++;
        }
        
        pos = 0;
        while ((pos = formatted.find(')', pos)) != std::string::npos) {
            if (pos > 0 && formatted[pos - 1] != ' ' && formatted[pos - 1] != '(') {
                formatted.insert(pos, " ");
                pos++;
            }
            pos++;
        }
    }
    
    return formatted;
}

} // namespace Compiler
