#ifndef CODEGEN_H
#define CODEGEN_H

#include "../parser/AST.h"
#include <sstream>
#include <string>

namespace Compiler {

// Formatting options for pretty printing
struct FormatOptions {
    int indentSize = 4;
    bool useTabs = false;
    bool spaceBeforeParen = false;
    bool spaceInsideParen = false;
    bool newlineBeforeBrace = false;
    bool compactBlocks = false;
    int maxLineLength = 100;
};

// C++ Code Generator with Pretty Printing
class CodeGenerator : public ASTVisitor {
private:
    std::stringstream output;
    std::stringstream headerOutput;  // For header file generation
    int indentLevel;
    bool isGeneratingHeader;  // Flag for header vs source
    std::string currentClassName;  // Track current class for method definitions
    FormatOptions formatOpts;  // Formatting options for pretty printing
    
    void indent();
    void writeLine(const std::string& line);
    void write(const std::string& text);
    void writeFormatted(const std::string& text, const FormatOptions& options);  // Write with formatting
    std::string formatExpression(const std::string& expr, const FormatOptions& options);  // Pretty format expressions
    
    std::string convertType(const std::string& csType);
    std::string convertOperator(const std::string& op);
    
public:
    CodeGenerator();
    
    std::string generate(Program& program);
    std::string generateHeader(Program& program);
    std::string generateSource(Program& program);
    
    std::string getOutput() const { return output.str(); }
    std::string getHeaderOutput() const { return headerOutput.str(); }
    
    // Visitor methods
    void visit(BinaryExpression& node) override;
    void visit(UnaryExpression& node) override;
    void visit(LiteralExpression& node) override;
    void visit(IdentifierExpression& node) override;
    void visit(CallExpression& node) override;
    void visit(MemberExpression& node) override;
    
    void visit(ExpressionStatement& node) override;
    void visit(VariableDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ReturnStatement& node) override;
    
    void visit(FunctionDeclaration& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(NamespaceDeclaration& node) override;
    void visit(UsingDirective& node) override;
    void visit(InterfaceDeclaration& node) override;
    void visit(TryStatement& node) override;
    void visit(ThrowStatement& node) override;
    void visit(UsingStatement& node) override;
    
    void visit(Program& node) override;
};

} // namespace Compiler

#endif // CODEGEN_H
