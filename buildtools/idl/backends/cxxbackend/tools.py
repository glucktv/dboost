from omniidl import idlast, idltype, idlutil, idlvisitor, output

ttsMap = {
    idltype.tk_void:       "void",
    idltype.tk_short:      "short",
    idltype.tk_long:       "long",
    idltype.tk_ushort:     "unsigned short",
    idltype.tk_ulong:      "unsigned long",
    idltype.tk_float:      "float",
    idltype.tk_double:     "double",
    idltype.tk_boolean:    "boolean",
    idltype.tk_char:       "char",
    idltype.tk_octet:      "unsigned char",
    idltype.tk_any:        "any",
    idltype.tk_longlong:   "long long",
    idltype.tk_ulonglong:  "unsigned long long",
    idltype.tk_longdouble: "long double",
    idltype.tk_wchar:      "wchar"
}

class Aggregator (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self):
        self.structs = set([])
        self.interfaces = set([])
        self.includes = set([])
        self.stdincludes = set([])
        self.moduleprefix = "org.dboost"
        self.pragmas = {}

    def getIncludes(self):
        return (self.includes, self.stdincludes)
    def getStructs(self):
        return self.structs
    def getInterfaces(self):
        return self.interfaces
    def getModulePrefix(self):
        return self.moduleprefix
    def getPragmas(self):
        return self.pragmas

    def visitAST(self, node):
        for p in node.pragmas():
            key, value = p.text().split()
            if key == 'module_prefix':
                self.moduleprefix = value[1:-1]
            self.pragmas[key] = value
        for d in node.declarations():
            if not d.mainFile():
                self.includes.add(d.file())
                d.accept(self)
            else:
                d.accept(self)

    def visitModule(self, node):
        for n in node.definitions():
            n.accept(self)

    def visitInterface(self, node):
        for n in node.contents():
            n.accept(self)
        self.interfaces.add(node.identifier())

    def visitStruct(self, node):
        for n in node.members():
            n.accept(self)
        self.structs.add(node.identifier())

    def visitOperation(self, node):
        node.returnType().accept(self)
        for p in node.parameters():
            p.paramType().accept(self)

    def visitTypedef(self, node):
        node.aliasType().accept(self)

    def visitSequenceType(self, node):
        self.stdincludes.add("vector")

    def visitWStringType(self, node):
        self.stdincludes.add("wstring")

    def visitStringType(self, node):
        self.stdincludes.add("string")