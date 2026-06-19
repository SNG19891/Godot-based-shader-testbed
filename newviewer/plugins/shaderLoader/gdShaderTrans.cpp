#include <QDir>
#include <QMenu>
#include <QProcess>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QOpenGLFramebufferObject>
#include "customdialog.h"
#include <fstream>
#include <sstream>
#include <cmath>
#ifndef __APPLE__
#include <GL/glu.h>
#else
#include <glu.h>
#endif

#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include "gdShaderTrans.h"

//glslang
#include <ShaderLang.h>
#include "intermediate.h"
#include "glslang/Public/ResourceLimits.h"
#include "Versions.h"
#include "localintermediate.h"
#include "InfoSink.h"

using namespace glslang;

GdShaderTrans::GdShaderTrans()
{

}

static std::string buildFinalShader(const std::string& vertGD,
    const std::string& fragGD, const std::string& globals)
{
    std::ostringstream out;
    out << "shader_type spatial;\n\n";
    out << "render_mode vertex_lighting; //este modo hace que sea el vertex shader el que se encargue de la iluminacion\n";
    out << "render_mode skip_vertex_transform;\n";
    out << "render_mode diffuse_lambert;\n";
    out << "render_mode unshaded; //elimina la iluminación\n";
    out << "//render_mode cull_disabled; //desactiva el backface culling (esta activado por defecto, descomentar esta linea para desactivarlo)\n\n";
    out << globals << "\n";
    out << "// ── Vertex ──────────────────────────────────────────────\n";
    out << vertGD << "\n";
    out << "// ── Fragment ────────────────────────────────────────────\n";
    out << "//Color solo es modificable en vertex()\n//Para modificar el color puro del fragmento se usa ALBEDO\n";
    out << fragGD << "\n";
    return out.str();
}

void GdShaderTrans::exportToGodot(QString& name)
{
    glslang::InitializeProcess();

    //Get shaders code
    QFile v(name + ".vert");
    if (!v.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << "ShaderLoader: Couldn't read the vertex shader" << endl;
        glslang::FinalizeProcess();
        return;
    }


    QString vertex = QString::fromUtf8(v.readAll());

    QFile f(name + ".frag");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cerr << "ShaderLoader: Couldn't read the fragment shader" << endl;
        glslang::FinalizeProcess();
        return;
    }

    QString fragment = QString::fromUtf8(f.readAll());
    //Parser the shader
    glslang::TShader vShader(EShLangVertex);
    glslang::TShader fShader(EShLangFragment);

    QByteArray vertexBytes = vertex.toUtf8();
    QByteArray fragmentBytes = fragment.toUtf8();

    const char* vertexStrings[] = { vertexBytes.constData() };
    const char* fragmentStrings[] = { fragmentBytes.constData() };

    vShader.setStrings(vertexStrings, 1);
    fShader.setStrings(fragmentStrings, 1);

    const TBuiltInResource* resources = GetDefaultResources();//Funcion de ResourceLimits
    //bool parse(const TBuiltInResource* builtInResources, int defaultVersion, bool forwardCompatible, EShMessages messages)
    //forwarCompatible  give errors for use of deprecated features

    if (!vShader.parse(resources, 330, false, EShMsgDefault))
    {
        std::cerr << "ShaderLoader: Couldn't parse the fragment shader" << endl << vShader.getInfoLog() << endl;
        glslang::FinalizeProcess();
        return;
    }
    cout << "Vertex shader Parsed" << endl;

    if (!fShader.parse(resources, 330, false, EShMsgDefault))
    {
        std::cerr << "ShaderLoader: Couldn't parse the vertex shader" << endl << fShader.getInfoLog() << endl;
        glslang::FinalizeProcess();
        return;
    }
    cout << "Fragment shader Parsed" << endl;

    //Creating and linking the program with parsed Shaders
    glslang::TProgram program;
    program.addShader(&vShader);
    program.addShader(&fShader);

    //AST Vertex Shader
    if (!program.link(EShMsgDefault))
    {
        std::cerr << "ShaderLoader: Link error" << endl << program.getInfoLog() << endl;
        glslang::FinalizeProcess();
        return;
    }
    cout << "Shaders Linked" << endl;

    const glslang::TIntermediate* vertIntermediate =
        program.getIntermediate(EShLangVertex);
    const glslang::TIntermediate* fragIntermediate =
        program.getIntermediate(EShLangFragment);

    if (!vertIntermediate)
    {
        std::cerr << "No vertex intermediate" << endl;
        return;
    }
    else
    {
        std::cout << "Vertex AST Obteined" << endl;
    }
    TIntermNode* vRoot = vertIntermediate->getTreeRoot();
    if (!vRoot)
    {
        std::cerr << "Vertex root NULL" << endl;
        return;
    }


    //AST Fragment Shader
    if (!fragIntermediate)
    {
        std::cerr << "No fragment intermediate" << endl;
        return;
    }
    else
    {
        std::cout << "Fragment AST Obteined" << endl;
    }
    TIntermNode* fRoot = fragIntermediate->getTreeRoot();
    if (!fRoot)
    {
        std::cerr << "Fragment root NULL" << endl;
        return;
    }

    TInfoSink vInfoSink;
    TInfoSink fInfoSink;
    GLSLToGDShader vTranslator(vInfoSink, ShaderStage::Vertex);
    GLSLToGDShader fTranslator(fInfoSink, ShaderStage::Fragment);

    char* tmp = getenv("VTEXTURES");
    if (tmp == nullptr)
    {
        qDebug() << "ERROR EN ECONTRAR CARPETA DE ESCRITURA";
        return;
    }

    QString jsonDir = QDir::cleanPath(QString::fromUtf8(tmp) + "/../plugins/shaderLoader/translationsGLSLtoGDShader.json");

    //Carga los mapas de traduccion des de el JSON
    vTranslator.loadTranslationJSON(jsonDir);
    fTranslator.loadTranslationJSON(jsonDir);

    vRoot->traverse(&vTranslator);
    std::cout << "Vertex AST traversed" << std::endl;

    fRoot->traverse(&fTranslator);
    std::cout << "Fragment AST traversed" << std::endl;

    std::string finalShader = buildFinalShader(
        std::string(vInfoSink.debug.c_str()),
        std::string(fInfoSink.debug.c_str()),
        (vTranslator.globals + fTranslator.globals));

    std::cout << "\n===== GDShader output =====\n"
        << finalShader
        << "===========================\n";

    // Cambiar name por la direccion de la carpeta: .../newviewer/godot/GDshaders
    QString godotDir = QDir::cleanPath(QString::fromUtf8(tmp) + "/../godot/GDshaders");

    QFileInfo shaderInfo(name);

    QString shaderName = shaderInfo.baseName();
    //Archivo para debug
    //QString shaderName = "test";

    QString outPath =
        QDir(godotDir).filePath(shaderName + ".gdshader");

    QFile outFile(outPath);

    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        outFile.write(QByteArray::fromStdString(finalShader));
        std::cout << "Written to: " << outPath.toStdString() << std::endl;
    }
    else
    {
        std::cerr << "Could not write " << outPath.toStdString() << std::endl;
    }

    glslang::FinalizeProcess();

    //OPEN GODOT WITH GDSHADER
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helpers internos
// ─────────────────────────────────────────────────────────────────────────────

std::string GLSLToGDShader::indent() const
{
    return std::string(depth * 4, ' ');
}

bool GLSLToGDShader::isGLBuiltIn(const std::string& name) const
{
    return name.size() > 3 && name.substr(0, 3) == "gl_";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Mapeo de tipos  GLSL -> GDShader
// ─────────────────────────────────────────────────────────────────────────────

bool GLSLToGDShader::loadTranslationJSON(const QString& path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        std::cout << "FAILED OPEN JSON" << std::endl; //DEBUG
        return false;
    }
        

    QByteArray data = file.readAll();

    QJsonParseError err;

    QJsonDocument doc = QJsonDocument::fromJson(data);

    //DEBUG----------------
    if (err.error != QJsonParseError::NoError)
    {
        std::cout << "JSON ERROR: "
            << err.errorString().toStdString()
            << std::endl;
    }
    else
    {
        std::cout << "JSON PARSED OK" << std::endl;
    }
    //---------------------

    if (!doc.isObject())
        return false;

    QJsonObject root = doc.object();

    auto loadMap =
        [](const QJsonObject& obj,
            std::unordered_map<std::string, std::string>& map)
        {
            for (auto it = obj.begin(); it != obj.end(); ++it)
            {
                map[it.key().toStdString()] =
                    it.value().toString().toStdString();
            }
        };

    loadMap(root["vertex"].toObject(), builtinsVertex);
    loadMap(root["fragment"].toObject(), builtinsFragment);
    loadMap(root["functions"].toObject(), functions);

    //DEBUGG -------------------
    std::cout << "VERTEX MAP SIZE: "
    << builtinsVertex.size()
        << std::endl;

    std::cout << "FRAGMENT MAP SIZE: "
        << builtinsFragment.size()
        << std::endl;

    std::cout << "FUNCTION MAP SIZE: "
        << functions.size()
        << std::endl;
    //--------------------------
    return true;
}

std::string GLSLToGDShader::translateType(const TType& type) const
{
    if (type.isMatrix()) {
        int c = type.getMatrixCols(), r = type.getMatrixRows();
        if (c == 4 && r == 4) return "mat4";
        if (c == 3 && r == 3) return "mat3";
        if (c == 2 && r == 2) return "mat2";
        return "mat4";
    }
    if (type.isVector()) {
        int sz = type.getVectorSize();
        switch (type.getBasicType()) {
        case EbtFloat: return "vec" + std::to_string(sz);
        case EbtInt:   return "ivec" + std::to_string(sz);
        case EbtUint:  return "uvec" + std::to_string(sz);
        case EbtBool:  return "bvec" + std::to_string(sz);
        default: break;
        }
    }
    switch (type.getBasicType()) {
    case EbtFloat:   return "float";
    case EbtDouble:  return "float";   // GDShader no tiene double
    case EbtInt:     return "int";
    case EbtUint:    return "uint";
    case EbtBool:    return "bool";
    case EbtSampler: {
        const TSampler& s = type.getSampler();
        if (s.dim == Esd2D)   return "sampler2D";
        if (s.dim == Esd3D)   return "sampler3D";
        if (s.dim == EsdCube) return "samplerCube";
        return "sampler2D";
    }
    case EbtStruct:
        return type.getTypeName().c_str();
    default:
        return "float";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Mapeo de nombres  GLSL -> GDShader
// ─────────────────────────────────────────────────────────────────────────────
std::string GLSLToGDShader::translateBuiltIn(const std::string& name) const
{
    std::cout << "Searching builtin: [" << name << "]" << std::endl;

    const auto& builtins =
        (stage == ShaderStage::Fragment)
        ? builtinsFragment
        : builtinsVertex;



    std::cout << "Functions size: " << functions.size() << std::endl;

    auto it = builtins.find(name);

    if (it != builtins.end())
    {
        std::cout << "FOUND BUILTIN -> "
            << it->second << std::endl;

        return it->second;
    }

    auto fi = functions.find(name);

    if (fi != functions.end())
    {
        std::cout << "FOUND FUNCTION -> "
            << fi->second << std::endl;

        return fi->second;
    }

    std::cout << "NOT FOUND" << std::endl;

    return name;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Mapeo de operadores  GLSL -> GDShader
// ─────────────────────────────────────────────────────────────────────────────
std::string GLSLToGDShader::translateOperator(TOperator op) const
{
    switch (op) {
        // Asignaciones
    case EOpAssign:            return " = ";
    case EOpAddAssign:         return " += ";
    case EOpSubAssign:         return " -= ";
    case EOpMulAssign:         return " *= ";
    case EOpDivAssign:         return " /= ";
    case EOpModAssign:         return " %= ";
    case EOpAndAssign:         return " &= ";
    case EOpLeftShiftAssign:   return " <<= ";
    case EOpRightShiftAssign:  return " >>= ";
        // Aritméticos
    case EOpAdd:               return " + ";
    case EOpSub:               return " - ";
    case EOpMul:
    case EOpVectorTimesScalar:
    case EOpVectorTimesMatrix:
    case EOpMatrixTimesVector:
    case EOpMatrixTimesScalar:
    case EOpMatrixTimesMatrix:  return " * ";
    case EOpDiv:               return " / ";
    case EOpMod:               return " % ";
        // Comparación
    case EOpEqual:             return " == ";
    case EOpNotEqual:          return " != ";
    case EOpLessThan:          return " < ";
    case EOpGreaterThan:       return " > ";
    case EOpLessThanEqual:     return " <= ";
    case EOpGreaterThanEqual:  return " >= ";
        // Lógicos
    case EOpLogicalAnd:        return " && ";
    case EOpLogicalOr:         return " || ";
    case EOpLogicalXor:        return " ^^ ";
        // Bit a bit
    case EOpAnd:               return " & ";
    case EOpInclusiveOr:       return " | ";
    case EOpExclusiveOr:       return " ^ ";
    case EOpLeftShift:         return " << ";
    case EOpRightShift:        return " >> ";
        // Indexado / acceso a miembro
    case EOpIndexDirect:
    case EOpIndexIndirect:     return "[]";   // el caller pone los corchetes
    case EOpIndexDirectStruct:
    case EOpVectorSwizzle:     return ".";
    default:                   return " ? ";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constante -> string literal
// ─────────────────────────────────────────────────────────────────────────────
std::string GLSLToGDShader::constToString(const TConstUnionArray& cu,
    const TType& type) const
{
    int n = type.isVector() ? type.getVectorSize()
        : type.isMatrix() ? type.getMatrixCols() * type.getMatrixRows()
        : 1;

    std::ostringstream ss;
    if (n > 1) ss << translateType(type) << "(";
    for (int i = 0; i < n; ++i) {
        if (i) ss << ", ";
        switch (cu[i].getType()) {
        case EbtFloat:  ss << floatToString(cu[i].getDConst()); break;
        case EbtDouble: ss << floatToString(cu[i].getDConst()); break;
        case EbtInt:    ss << cu[i].getIConst(); break;
        case EbtUint:   ss << cu[i].getUConst() << "u"; break;
        case EbtBool:   ss << (cu[i].getBConst() ? "true" : "false"); break;
        default:        ss << "0"; break;
        }
    }
    if (n > 1) ss << ")";
    return ss.str();
}

std::string GLSLToGDShader::floatToString(double v) const
{
    std::ostringstream ss;
    ss.setf(std::ios::fixed);
    ss.precision(6); // ajustable

    ss << v;

    std::string s = ss.str();

    // Eliminar ceros innecesarios
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);

    // Si termina en '.', añadir 0
    if (s.back() == '.') {
        s += "0";
    }

    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Emite la declaración de uniform / varying al principio del shader
// ─────────────────────────────────────────────────────────────────────────────
void GLSLToGDShader::emitDeclaration(TIntermSymbol* sym)
{
    const std::string name = sym->getName().c_str();
    if (std::find(declared.begin(), declared.end(), name) != declared.end())
        return;
    declared.push_back(name);

    const TType& type = sym->getType();
    const TQualifier& q = type.getQualifier();
    std::string gdType = translateType(type);
    std::string arraySuffix = type.isArray()
        ? "[" + std::to_string(type.getOuterArraySize()) + "]"
        : "";

    // Si el nombre está en cualquiera de los mapas del JSON es un built-in
    // de GDShader y no necesita declararse en ningún caso.
    bool isBuiltIn = builtinsVertex.count(name) || builtinsFragment.count(name)
        || functions.count(name);
    if (isBuiltIn) return;

    if (q.storage == EvqUniform) {
        //La mayoria de operaciones que funcionan con float no funcionan con int
        if (gdType == "int") gdType = "float";
        globals += "uniform " + gdType + " " + name + arraySuffix + ";\n";
    }
    else if (q.storage == EvqVaryingOut || q.storage == EvqOut) {
        globals += "varying " + gdType + " " + name + arraySuffix + ";\n";
    }
    else if ((q.storage == EvqVaryingIn || q.storage == EvqIn)
        && stage == ShaderStage::Vertex) {
        globals += "varying " + gdType + " " + name + arraySuffix + ";\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  evalúa un sub-árbol y devuelve su texto GDShader
// ─────────────────────────────────────────────────────────────────────────────
std::string GLSLToGDShader::eval(TIntermNode* node)
{
    if (!node) return "";
    // Guardamos el estado del sink, usamos currentExpr como buffer temporal
    // La forma más sencilla: creamos un visitor temporal con un sink nuevo
    TInfoSink tmp;
    GLSLToGDShader sub(tmp, stage);
    sub.depth = depth;
    sub.declared = declared;
    sub.insideFunction = insideFunction;

    sub.builtinsVertex = builtinsVertex;
    sub.builtinsFragment = builtinsFragment;
    sub.functions = functions;
    node->traverse(&sub);
    return tmp.debug.c_str();
}

bool GLSLToGDShader::tryEmitLocalDecl(TIntermBinary* node)
{
    // Solo dentro de funciones
    if (!insideFunction) return false;

    TIntermSymbol* leftSym = node->getLeft()->getAsSymbolNode();
    if (!leftSym) return false;

    const std::string symName = leftSym->getName().c_str();

    // Descartar símbolos anónimos internos de glslang
    if (symName.find("anon@") != std::string::npos) return false;

    // Solo variables temporales / locales; no uniforms, no varyings
    TStorageQualifier stor = leftSym->getType().getQualifier().storage;
    if (stor != EvqTemporary && stor != EvqGlobal) return false;

    // Solo si aún no está declarada
    if (std::find(declared.begin(), declared.end(), symName) != declared.end())
        return false;

    // Emitir  "TYPE name = RHS;\n"
    declared.push_back(symName);
    std::string gdType = translateType(leftSym->getType());
    infoSink.debug << indent() << gdType << " " << symName << " = ";
    node->getRight()->traverse(this);
    infoSink.debug << ";\n";
    return true;
}



bool GLSLToGDShader::visitVariableDecl(TVisit visit, TIntermVariableDecl* node)
{
    if (visit != EvPreVisit) return true;

    TIntermSymbol* sym = node->getDeclSymbol();
    if (!sym) return true;

    const std::string name = sym->getName().c_str();
    const TType& type = sym->getType();
    std::string       gdType = translateType(type);
    std::string arraySuffix = type.isArray()
        ? "[" + std::to_string(type.getOuterArraySize()) + "]" : "";

    // Marcar como declarada para que visitBinary no la re-declare
    if (std::find(declared.begin(), declared.end(), name) == declared.end())
        declared.push_back(name);

    infoSink.debug << indent() << gdType << " " << name << arraySuffix;

    TIntermNode* init = node->getInitNode();
    if (init) {
        infoSink.debug << " = ";
        init->traverse(this);
    }
    infoSink.debug << ";\n";

    return false;   // hijos ya procesados a mano
}

// ═════════════════════════════════════════════════════════════════════════════
//  visitSymbol
// ═════════════════════════════════════════════════════════════════════════════
void GLSLToGDShader::visitSymbol(TIntermSymbol* node)
{
    const std::string raw = node->getName().c_str();

    // Filtrar símbolos internos de glslang
    if (raw.find("anon@") != std::string::npos)
        return;

    const TQualifier& q = node->getType().getQualifier();

    if (!insideFunction) {
        if (q.storage == EvqVaryingOut ||
            q.storage == EvqVaryingIn ||
            q.storage == EvqOut ||
            q.storage == EvqIn||
            q.storage == EvqUniform)
        {
            emitDeclaration(node);
        }
        return;
    }
    if (q.storage == EvqUniform) 
    {
        emitDeclaration(node);
    }
    if (parameters)
    {
        std::string type = translateType(node->getType());
        infoSink.debug << type << " ";
    }
    // Dentro de función: emitir el nombre traducido
    const std::string name = translateBuiltIn(raw);
    infoSink.debug << name;
}

// ═════════════════════════════════════════════════════════════════════════════
//  visitConstantUnion
// ═════════════════════════════════════════════════════════════════════════════
void GLSLToGDShader::visitConstantUnion(TIntermConstantUnion* node)
{
    infoSink.debug << constToString(node->getConstArray(), node->getType());
}


// ═════════════════════════════════════════════════════════════════════════════
//  visitBinary
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitBinary(TVisit visit, TIntermBinary* node)
{
    TOperator op = node->getOp();

    // ── Asignaciones ─────────────────────────────────────────────
    if ((op == EOpAssign || op == EOpAddAssign || op == EOpSubAssign || op == EOpMulAssign) && visit == EvPreVisit) {

        // Intentar declarar variable local automáticamente
        if (tryEmitLocalDecl(node)) {
            return false;
        }

        std::string lhs = eval(node->getLeft());
        lhs = translateBuiltIn(lhs);
        TIntermTyped* rhs = node->getRight();

        if (lhs.empty() ||
            lhs.find("anon@") != std::string::npos)
        {
            return false;
        }

        if (lhs.find("gl_Position") != std::string::npos ||
            lhs.find("POSITION") != std::string::npos || lhs == ".0u")
        {
            infoSink.debug << indent() << "POSITION = ";
            node->getRight()->traverse(this);
            infoSink.debug << ";\n";
            return false;
        }

        if ((lhs == "ALBEDO")
            && rhs->getType().isVector()
            && rhs->getType().getVectorSize() == 4)
        {
            infoSink.debug << indent() << lhs << translateOperator(op);
            infoSink.debug << "(";
            rhs->traverse(this);
            infoSink.debug << ").rgb;\n";
            return false;
        }

        infoSink.debug << indent() << lhs << translateOperator(op);
        rhs->traverse(this);
        infoSink.debug << ";\n";

        return false;
    }

    // ── Indexado de arrays ─────────────────────────────────────────
    if (op == EOpIndexDirect || op == EOpIndexIndirect) {

        if (visit == EvInVisit) {

            TIntermConstantUnion* idx =
                node->getRight()->getAsConstantUnion();

            if (idx && node->getLeft()->getType().isVector()) {
                int i = idx->getConstArray()[0].getIConst();
                static const char* swz[] = { "x", "y", "z", "w" };

                if (i >= 0 && i < 4) {
                    infoSink.debug << "." << swz[i];
                    return false;
                }
            }

            infoSink.debug << "[";
        }
        else if (visit == EvPostVisit) {
            infoSink.debug << "]";
        }

        return true;
    }

    // ── Struct ───────────────────────────────────────────
    if (op == EOpIndexDirectStruct) {
        if (visit == EvInVisit) infoSink.debug << ".";
        return true;
    }

    // ── Swizzle  v.xyz ───────────────────────────────────
    if (op == EOpVectorSwizzle) {
        if (visit == EvInVisit) {
            infoSink.debug << ".";
            TIntermAggregate* swizzleNode = node->getRight()->getAsAggregate();
            if (swizzleNode) {
                static const char* swz[] = { "x", "y", "z", "w" };
                for (TIntermNode* seq : swizzleNode->getSequence()) {
                    TIntermConstantUnion* idx = seq->getAsConstantUnion();
                    if (idx) {
                        int i = idx->getConstArray()[0].getIConst();
                        if (i >= 0 && i < 4)
                            infoSink.debug << swz[i];
                    }
                }
            }
            return false;  // no visitar el hijo derecho, ya lo procesamos
        }
        return true;
    }

    // ── Binario normal ─────────────────────────────────────────────
    if (visit == EvPreVisit)         infoSink.debug << "(";
    else if (visit == EvInVisit)     infoSink.debug << translateOperator(op);
    else if (visit == EvPostVisit)   infoSink.debug << ")";

    return true;
}
// ═════════════════════════════════════════════════════════════════════════════
//  visitUnary
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitUnary(TVisit visit, TIntermUnary* node)
{
    TOperator op = node->getOp();

    std::string fn = translateBuiltInOp(op);
    if (!fn.empty()) {
        if (visit == EvPreVisit) {
            infoSink.debug << fn << "(";
        }
        else if (visit == EvPostVisit) {
            infoSink.debug << ")";
        }
        return true;
    }

    if (visit == EvPreVisit) {
        switch (op) {
        case EOpNegative:        infoSink.debug << "(-";   break;
        case EOpLogicalNot:      infoSink.debug << "(!";   break;
        case EOpBitwiseNot:      infoSink.debug << "(~";   break;
        case EOpPreIncrement:    infoSink.debug << "(++";  break;
        case EOpPreDecrement:    infoSink.debug << "(--)"; break;
        case EOpConvNumeric: {
            const TType& dstType = node->getType();
            TBasicType dst = dstType.getBasicType();
            if (dst == EbtBool)  infoSink.debug << "bool(";
            else if (dst == EbtFloat) infoSink.debug << "float(";
            else if (dst == EbtInt)   infoSink.debug << "int(";
            else if (dst == EbtUint)  infoSink.debug << "uint(";
            else                      infoSink.debug << "(";
            break;
            }

        default:                 infoSink.debug << "(";    break;
        }
    }
    else if (visit == EvPostVisit) {
        switch (op) {
        case EOpPostIncrement:   infoSink.debug << "++);"; break;
        case EOpPostDecrement:   infoSink.debug << "--);"; break;
        default:                 infoSink.debug << ")";   break;
        }
    }
    return true;
}

// ═════════════════════════════════════════════════════════════════════════════
//  visitAggregate  (funciones, llamadas, constructores, secuencias)
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitAggregate(TVisit visit, TIntermAggregate* node)
{
    TOperator op = node->getOp();

    // ── Raíz del linker: contiene símbolos globales sueltos —
    if (op == EOpLinkerObjects) {
        if (visit == EvPreVisit) return true;  // visitar hijos
        return true;
    }

    // ── Definición de función ────────────────────────────────────────────────
    if (op == EOpFunction) {
        if (visit == EvPreVisit) {

            std::string returnType = translateType(node->getType());

            

            insideFunction = true;
            std::string rawName = node->getName().c_str();
            std::string fname = rawName.substr(0, rawName.find('('));

            // main() se convierte en vertex() o fragment() según la etapa
            if (fname == "main") {
                fname = (stage == ShaderStage::Vertex) ? "vertex" : "fragment";
                infoSink.debug
                    << "\n"
                    << "void"
                    << " ";
                infoSink.debug << fname << "(";
                ++depth;
                return true;
            }
            infoSink.debug
                << "\n"
                << returnType
                << " ";

            infoSink.debug << fname << "(";
            ++depth;
        }
        else if (visit == EvPostVisit) {
            --depth;
            infoSink.debug << "}\n";
            insideFunction = false;
        }
        return true;
    }

    // ── Lista de parámetros formales ────────────
    if (op == EOpParameters) {
        if (visit == EvPreVisit) {
            parameters = true;
            return true;
        } 
        if (visit == EvInVisit) {
            infoSink.debug << ", ";
            return true;
        }

        if (visit == EvPostVisit) {
            infoSink.debug << ") {\n";
            parameters = false;
        }
           

        return true;
    }

    // ── Llamada a función ────────────────────────────────────────────────────
    if (op == EOpFunctionCall) {
        if (visit == EvPreVisit) {
            std::string raw = node->getName().c_str();
            std::string fn = translateBuiltIn(raw.substr(0, raw.find('(')));
            infoSink.debug << fn << "(";
        }
        else if (visit == EvInVisit)  infoSink.debug << ", ";
        else                          infoSink.debug << ")";
        return true;
    }

    // ── Constructor de tipo  vec3(…) mat4(…) etc. ────────────────────────────
    if (op == EOpConstructStruct ||
        (op >= EOpConstructGuardStart && op <= EOpConstructGuardEnd))
    {
        if (visit == EvPreVisit)  infoSink.debug << translateType(node->getType()) << "(";
        else if (visit == EvInVisit)   infoSink.debug << ", ";
        else                           infoSink.debug << ")";
        return true;
    }

    // ── Secuencia de sentencias ──────────────────────────────────────────────
    /*if (op == EOpSequence) {
        if (visit == EvPreVisit)  infoSink.debug << indent();
        else if (visit == EvInVisit)   infoSink.debug << "\n";
        else                           infoSink.debug << "\n";
        return true;
    }*/
    if (op == EOpSequence)
        return true;

    // ── Funciones built-in por operador (sin, cos, etc.) ─────────────
    std::string fn = translateBuiltInOp(op);
    if (!fn.empty()) {
        if (visit == EvPreVisit)      infoSink.debug << fn << "(";
        else if (visit == EvInVisit)  infoSink.debug << ", ";
        else                          infoSink.debug << ")";
        return true;
    }


    // ── Funciones built-in guardadas como aggregate (abs, sin, mix, …) ───────
    if (visit == EvPreVisit) {
        std::string raw = node->getName().c_str();
        if (!raw.empty()) {
            infoSink.debug << raw.substr(0, raw.find('(')) << "(";
        }
        else {
            infoSink.debug << "?(";
        }
    }
    else if (visit == EvInVisit)  infoSink.debug << ", ";
    else                          infoSink.debug << ")";

    return true;
}

std::string GLSLToGDShader::translateBuiltInOp(TOperator op) const
{
    switch (op) {
    case EOpSin:        return "sin";
    case EOpCos:        return "cos";
    case EOpTan:        return "tan";
    case EOpAsin:       return "asin";
    case EOpAcos:       return "acos";
    case EOpAtan:       return "atan";

    case EOpPow:        return "pow";
    case EOpExp:        return "exp";
    case EOpLog:        return "log";
    case EOpExp2:       return "exp2";
    case EOpLog2:       return "log2";
    case EOpSqrt:       return "sqrt";
    case EOpInverseSqrt:return "inversesqrt";

    case EOpAbs:        return "abs";
    case EOpSign:       return "sign";
    case EOpFloor:      return "floor";
    case EOpCeil:       return "ceil";
    case EOpFract:      return "fract";
    case EOpMod:        return "mod";

    case EOpMin:        return "min";
    case EOpMax:        return "max";
    case EOpClamp:      return "clamp";
    case EOpMix:        return "mix";
    case EOpStep:       return "step";
    case EOpSmoothStep: return "smoothstep";

    case EOpLength:     return "length";
    case EOpDistance:   return "distance";
    case EOpDot:        return "dot";
    case EOpCross:      return "cross";
    case EOpNormalize:  return "normalize";

    case EOpReflect:    return "reflect";
    case EOpRefract:    return "refract";
    case EOpTexture:    return "texture";

    default:
        return "";
    }
}

// ═════════════════════════════════════════════════════════════════════════════
//  visitSelection   (if / else)
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitSelection(TVisit visit, TIntermSelection* node)
{
    // Manejamos manualmente para controlar el indentado
    if (visit != EvPreVisit) return false;

    infoSink.debug << indent() << "if (" << eval(node->getCondition()) << ") {\n";

    ++depth;
    if (node->getTrueBlock()) {
        infoSink.debug << indent();
        node->getTrueBlock()->traverse(this);
    }
    --depth;
    infoSink.debug << indent() << "}";

    if (node->getFalseBlock()) {
        infoSink.debug << " else {\n";
        ++depth;
        infoSink.debug << indent();
        node->getFalseBlock()->traverse(this);
        --depth;
        infoSink.debug << indent() << "}";
    }
    infoSink.debug << "\n";

    return false;
}

// ═════════════════════════════════════════════════════════════════════════════
//  visitSwitch
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitSwitch(TVisit visit, TIntermSwitch* node)
{
    if (visit != EvPreVisit) return false;

    infoSink.debug << indent() << "switch (" << eval(node->getCondition()) << ") {\n";
    ++depth;
    if (node->getBody()) node->getBody()->traverse(this);
    --depth;
    infoSink.debug << indent() << "}\n";

    return false;
}

// ═════════════════════════════════════════════════════════════════════════════
//  visitLoop   (for / while / do-while)
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitLoop(TVisit visit, TIntermLoop* node)
{
    if (visit != EvPreVisit) return false;

    if (node->testFirst()) {
        infoSink.debug << indent() << "while ("
            << eval(node->getTest()) << ") {\n";
    }
    else {
        infoSink.debug << indent() << "do {\n";
    }

    ++depth;
    infoSink.debug << indent();
    if (node->getBody()) node->getBody()->traverse(this);
    --depth;

    if (!node->testFirst()) {
        infoSink.debug << indent() << "} while ("
            << eval(node->getTest()) << ");\n";
    }
    else {
        infoSink.debug << indent() << "}\n";
    }

    return false;
}
// ═════════════════════════════════════════════════════════════════════════════
//  visitBranch   (return / break / continue / discard / case / default)
// ═════════════════════════════════════════════════════════════════════════════
bool GLSLToGDShader::visitBranch(TVisit visit, TIntermBranch* node)
{
    if (visit != EvPreVisit) return true;

    switch (node->getFlowOp()) {
    case EOpReturn:
        infoSink.debug << indent() << "return";
        if (node->getExpression()) {
            infoSink.debug << " ";
            node->getExpression()->traverse(this);
        }
        infoSink.debug << ";\n";
        break;
    case EOpBreak:
        infoSink.debug << indent() << "break;\n";
        break;
    case EOpContinue:
        infoSink.debug << indent() << "continue;\n";
        break;
    case EOpCase:
        // Retrocedemos un nivel para que el "case" quede alineado con el switch
        infoSink.debug << std::string((depth - 1) * 4, ' ') << "case ";
        if (node->getExpression()) node->getExpression()->traverse(this);
        infoSink.debug << ":\n";
        break;
    case EOpDefault:
        infoSink.debug << std::string((depth - 1) * 4, ' ') << "default:\n";
        break;
    case EOpKill:
        infoSink.debug << indent() << "discard;\n";
        break;
    default:
        break;
    }
    return false;
}