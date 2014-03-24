from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string, os

class HeadersAggregator (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

    def __init__(self, main):
        self.includes = set([])
        self.main = main
    def getIncludes(self):
        return self.includes

    def visitAST(self, node):
        for d in node.declarations():
            if not d.mainFile():
                self.includes.add(d.file())
                d.accept(self)

class CppVisitor (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

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
        idltype.tk_octet:      "octet",
        idltype.tk_any:        "any",
        idltype.tk_TypeCode:   "CORBA::TypeCode",
        idltype.tk_Principal:  "CORBA::Principal",
        idltype.tk_longlong:   "long long",
        idltype.tk_ulonglong:  "unsigned long long",
        idltype.tk_longdouble: "long double",
        idltype.tk_wchar:      "wchar"
        }

    def __init__(self, st):
        self.st = st

    def visitAST(self, node):
        for n in node.declarations():
            if n.mainFile():
                n.accept(self)

    def visitModule(self, node):
        self.st.out("""\
namespace @id@ {""", id = node.identifier())

        self.st.inc_indent()

        for n in node.definitions():
            n.accept(self)

        self.st.dec_indent()

        self.st.out("""\
};
""")

    def visitInterface(self, node):
        if len(node.inherits()) != 0:
            inheritl = []
            for i in node.inherits():
                inheritl.append("public " + idlutil.ccolonName(i.scopedName()))
            inherits = ": " + string.join(inheritl, ", ") + " "
        else:
            inherits = ""
        
        self.st.out("""\
class @id@ @inherits@{""",
                    id = node.identifier(), inherits=inherits)

        self.st.inc_indent()

        for n in node.contents():
            n.accept(self)

        self.st.dec_indent()
        self.st.out("""\
};
""")

    def visitForward(self, node):
        self.st.out("""\
class @id@;""", id = node.identifier())


    def visitConst(self, node):
        node.constType().accept(self)
        type = self.__result_type

        if node.constKind() == idltype.tk_enum:
            value = "::" + idlutil.ccolonName(node.value().scopedName())

        elif node.constKind() == idltype.tk_string:
            value = '"' + idlutil.escapifyString(node.value()) + '"'

        elif node.constKind() == idltype.tk_wstring:
            value = 'L"' + idlutil.escapifyWString(node.value()) + '"'

        elif node.constKind() == idltype.tk_wchar:
            value = "L'" + idlutil.escapifyWString([node.value()]) + "'"

        elif node.constKind() in [idltype.tk_float, idltype.tk_double,
                                  idltype.tk_longdouble]:
            value = idlutil.reprFloat(node.value())

        elif node.constKind() == idltype.tk_fixed:
            value = node.value() + "d"
        else:
            value = str(node.value())
        
        self.st.out("""\
const @type@ @id@ = @value@;""",
                    type=type, id=node.identifier(), value=value)


    def visitTypedef(self, node):
        if node.constrType():
            node.aliasType().decl().accept(self)

        node.aliasType().accept(self)
        type  = self.__result_type
        decll = []
        for d in node.declarators():
            d.accept(self)
            decll.append(self.__result_declarator)

        decls = string.join(decll, ", ")

        self.st.out("""\
typedef @type@ @decls@;""",
               type=type, decls=decls)


    def visitStruct(self, node):
        self.st.out("""\
struct @id@ {""",
               id = node.identifier())

        for m in node.members():
            if m.constrType():
                self.st.inc_indent()
                m.memberType().decl().accept(self)
                self.st.dec_indent()

            m.memberType().accept(self)
            type = self.__result_type
            decll = []
            for d in m.declarators():
                d.accept(self)
                decll.append(self.__result_declarator)
            decls = string.join(decll, ", ")

            self.st.out("""\
  @type@ @decls@;""",

                   type=type, decls=decls)

        self.st.out("""\
};""")

    def visitStructForward(self, node):
        self.st.out("""\
struct @id@;""", id = node.identifier())


    def visitException(self, node):
        self.st.out("""\
class @id@ {""",
               id = node.identifier())

        for m in node.members():
            if m.constrType():
                self.st.inc_indent()
                m.memberType().decl().accept(self)
                self.st.dec_indent()

            m.memberType().accept(self)
            type = self.__result_type
            decll = []
            for d in m.declarators():
                d.accept(self)
                decll.append(self.__result_declarator)
            decls = string.join(decll, ", ")

            self.st.out("""\
  @type@ @decls@;""",

                        type=type, decls=decls)

        self.st.out("""\
};
""")

    def visitEnum(self, node):
        enuml = []
        for e in node.enumerators(): enuml.append(e.identifier())

        enums = string.join(enuml, ", ")

        self.st.out("""\
enum @id@ {@enums@};""",

               id=node.identifier(), enums=enums)

    def visitAttribute(self, node):
        if node.readonly():
            readonly = True
        else:
            readonly = False

        node.attrType().accept(self)
        type = self.__result_type

        ids  = string.join(node.identifiers(), ", ")

        if readonly:
            self.st.out("""\
virtual @type@ @ids@() const;""", type=type, ids=ids)
        else:
            self.st.out("""\
virtual @type@ @ids@() const;""", type=type, ids=ids)
            self.st.out("""\
virtual @type@ @ids@(const @type@);""", type=type, ids=ids)

    def visitOperation(self, node):
        comments = '\n'.join(map(lambda c: c.text(), node.comments()))
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
                    paraml.append(inout + ' ' + type + '& ' + p.identifier())
                else:
                    paraml.append(inout + ' ' + type + '* ' + p.identifier())
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

        self.st.out("""\
@comments@\
virtual @rtype@ @id@(@params@)@raises@;""",
               
               rtype=rtype, id=node.identifier(),
                    params=params, raises=raises, comments=comments)

    def visitBaseType(self, type):
        self.__result_type = self.ttsMap[type.kind()]

    def visitDeclaredType(self, type):
        self.__result_type = idlutil.ccolonName(type.decl().scopedName())


def run(tree, args):
    ha = HeadersAggregator(tree.file());
    tree.accept(ha)
    includes = map(lambda i: os.path.splitext(i)[0], ha.getIncludes())

    name, ext = os.path.splitext(tree.file())
    with open(name + '.hpp', 'w') as header:
        st = output.Stream(header, 2)

        st.out(
            '\n'.join(map(lambda i: '#include "' + i + '.hpp"', includes)));

        cv = CppVisitor(st)
        tree.accept(cv)
