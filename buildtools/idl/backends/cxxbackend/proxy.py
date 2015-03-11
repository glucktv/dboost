import os
import string

from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import tools

class ProxyHeader (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

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
        idltype.tk_TypeCode:   "CORBA::TypeCode",
        idltype.tk_Principal:  "CORBA::Principal",
        idltype.tk_longlong:   "long long",
        idltype.tk_ulonglong:  "unsigned long long",
        idltype.tk_longdouble: "long double",
        idltype.tk_wchar:      "wchar"
        }

    def __init__(self, st, interface, suffix, templates):
        self.st = st
        self.interface = interface
        self.suffix = suffix
        self.templates = templates
        self.class_name = interface + '_' + suffix
        self.namespace = ''

    def visitAST(self, node):
        ig = (self.interface + '_' + self.suffix + '_HPP').upper()
        self.st.out("""#ifndef @ig@
#define @ig@
#include <string>
#include <dbus_ptr.h>
#include <@interface@.hpp>
""", ig=ig, interface=self.interface)

        for n in node.declarations():
            if n.mainFile():
                n.accept(self)
        self.st.out("#endif // @ig@", ig=ig)

    def visitModule(self, node):
        self.st.out("""\
namespace @id@
{""", id=node.identifier())

        self.namespace = node.identifier()
#        self.st.inc_indent()

        self.st.out("""\
""")
        for n in node.definitions():
            n.accept(self)

#        self.st.dec_indent()

        self.st.out("""\
}
""")

    def visitInterface(self, node):
        if node.identifier() != self.interface:
            return
        inherits = self.namespace + '::' + self.interface

        self.st.out(self.templates[self.suffix]['interface'],
                    id=node.identifier(), inherits=inherits, class_name=self.class_name, ancestor=self.interface)

        self.st.inc_indent()

        for n in node.contents():
            n.accept(self)

        self.st.dec_indent()
        self.st.out("""\
};
""")

    def visitOperation(self, node):
        node.returnType().accept(self)
        rtype = self.__result_type

        paraml = []
        for p in node.parameters():
            if   p.is_in() and p.is_out(): inout = ""
            elif p.is_in():                inout = "const"
            else:                          inout = ""
            p.paramType().accept(self)
            type = self.__result_type
            if type != 'void':
                if type in self.ttsMap.values():
                    paraml.append(inout + ' ' + type + ' ' + p.identifier())
                else:
                    paraml.append(inout + ' ' + type + '& ' + p.identifier())
            else:
                paraml.append(inout + ' ' + type + ' ' + p.identifier())

        params = string.join(paraml, ", ")

        if len(node.raises()) > 0:
            raisel = []
            for r in node.raises():
                ename  = idlutil.ccolonName(r.scopedName())
                raisel.append(ename)

            raises = " throw (" + string.join(raisel, ", ") + ")"
        else:
            raises = ""

        self.st.out(self.templates['proxy']['operation'], rtype=rtype, id=node.identifier(), params=params, raises=raises)

    def visitBaseType(self, type):
        self.__result_type = self.ttsMap[type.kind()]

    def visitDeclaredType(self, type):
        self.__result_type = idlutil.ccolonName(type.decl().scopedName())

    def visitSequenceType(self, type):
        self.__result_type = "std::vector<" + type.seqType().name() + ">"

    def visitStringType(self, type):
        self.__result_type = "std::string"

    def visitWStringType(self, type):
        self.__result_type = "std:wstring"

class ProxySource (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    pass

def run(tree, args, templates):
    ia = tools.InterfacesAggregator(tree.file());
    tree.accept(ia)
    interfaces = ia.getInterfaces()

    suffix = 'proxy'
    for interface in interfaces:
        with open(interface + '_' + suffix + '.hpp', 'w') as header:
            st = output.Stream(header, 2)

            cv = ProxyHeader(st, interface, suffix, templates)
            tree.accept(cv)