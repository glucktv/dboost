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

class InterfacesAggregator (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

    def __init__(self, main):
        self.interfaces = set([])
        self.main = main
    def getInterfaces(self):
        return self.interfaces

    def visitAST(self, node):
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