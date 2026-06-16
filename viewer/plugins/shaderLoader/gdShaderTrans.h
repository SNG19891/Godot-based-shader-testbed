#pragma once

#include "plugin.h"
#include "glwidget.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObjectFormat>
#include <QDateTime>
#include <QElapsedTimer>
#include <QWidget>
#include <map>
#include <unordered_map>
#include "intermediate.h"
#include "localintermediate.h"
#include "InfoSink.h"
using namespace glslang;


#ifndef _GDSHADERTRANS_H
#define _GDSHADERTRANS_H

class GdShaderTrans
{
public:
    GdShaderTrans();
    void exportToGodot(QString& name);
};

#endif

/*
  * TIntermTraverser es una funcion construida en intermediate.h para
  * atravesar el AST construido y realizar las funciones que
  * el usuario quiera para cada caso.
  *
  * Las acciones adicionales de como debe procesarse en arbol se indican
  * en la declaracion de TIntermTraverser() y las intrucciones de como funcionan
  * estan en intermediate.h
  *
  * TIntermTraverser(bool preVisit = true, bool inVisit = false, bool postVisit = false, bool rightToLeft = false, bool includeDeclSymbol = false) :
            preVisit(preVisit),
            inVisit(inVisit),
            postVisit(postVisit),
            rightToLeft(rightToLeft),
            includeDeclSymbol(includeDeclSymbol),
            depth(0),
            maxDepth(0) { }

    virtual ~TIntermTraverser() { }

    virtual void visitSymbol(TIntermSymbol*)                     { }
    virtual void visitConstantUnion(TIntermConstantUnion*)       { }
    virtual bool visitBinary(TVisit, TIntermBinary*)             { return true; }
    virtual bool visitUnary(TVisit, TIntermUnary*)               { return true; }
    virtual bool visitSelection(TVisit, TIntermSelection*)       { return true; }
    virtual bool visitAggregate(TVisit, TIntermAggregate*)       { return true; }
    virtual bool visitLoop(TVisit, TIntermLoop*)                 { return true; }
    virtual bool visitBranch(TVisit, TIntermBranch*)             { return true; }
    virtual bool visitSwitch(TVisit, TIntermSwitch*)             { return true; }
    virtual bool visitVariableDecl(TVisit, TIntermVariableDecl*) { return true; }
  */

enum class ShaderStage { Vertex, Fragment };

//  Visitor principal: recorre el AST de glslang y emite código GDShader
class GLSLToGDShader : public TIntermTraverser {
public:
    GLSLToGDShader(TInfoSink& i, ShaderStage stage = ShaderStage::Vertex)
        : TIntermTraverser(true, true, true)
        , infoSink(i)
        , depth(0)
        , stage(stage)
    {
    }

    virtual bool visitBinary(TVisit, TIntermBinary* node);
    virtual bool visitUnary(TVisit, TIntermUnary* node);
    virtual bool visitAggregate(TVisit, TIntermAggregate* node);
    virtual bool visitSelection(TVisit, TIntermSelection* node);
    virtual void visitConstantUnion(TIntermConstantUnion* node);
    virtual void visitSymbol(TIntermSymbol* node);
    virtual bool visitLoop(TVisit, TIntermLoop* node);
    virtual bool visitBranch(TVisit, TIntermBranch* node);
    virtual bool visitSwitch(TVisit, TIntermSwitch* node);
    virtual bool visitVariableDecl(TVisit, TIntermVariableDecl* node);

    // Evalúa un sub-nodo y devuelve su representación como string
    std::string eval(TIntermNode* node);

    bool loadTranslationJSON(const QString& path);

    TInfoSink& infoSink;
    std::string globals;


protected:
    GLSLToGDShader(GLSLToGDShader&);
    GLSLToGDShader& operator=(GLSLToGDShader&);
    int depth;

private:
    ShaderStage stage;
    std::string currentExpr;

    std::string translateType(const TType& type)  const;
    std::string translateBuiltIn(const std::string& name)  const;
    std::string translateOperator(TOperator           op)    const;
    std::string translateBuiltInOp(TOperator          op) const;
    std::string constToString(const TConstUnionArray& cu,
        const TType& type)  const;
    std::string floatToString(double v) const;
    std::string indent()                          const;
    bool        isGLBuiltIn(const std::string& name)  const;
    void        emitDeclaration(TIntermSymbol* sym);

    std::unordered_map<std::string, std::string> builtinsVertex;
    std::unordered_map<std::string, std::string> builtinsFragment;
    std::unordered_map<std::string, std::string> functions;

    bool tryEmitLocalDecl(TIntermBinary* node);
    // Símbolos ya declarados (evita duplicados en uniforms/varyings)
    std::vector<std::string> declared;
    bool insideFunction = false;
    bool parameters = false;
    bool insideExpression = false;
};
