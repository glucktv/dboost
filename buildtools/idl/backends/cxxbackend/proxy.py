import os
import string

from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import tools

class ProxyHeader (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st, interface, suffix, templates, need_utils=False):
        self.st = st
        self.interface = interface
        self.suffix = suffix
        self.templates = templates
        self.class_name = interface + '_' + suffix
        self.namespace = ''
        self.need_utils = need_utils

    def visitAST(self, node):
        ig = (self.interface + '_' + self.suffix + '_HPP').upper()
        if self.need_utils:
            util = '#include "' + self.interface + '_util.hpp' + '"'
        else:
            util = ''
        self.st.out(self.templates[self.__class__.__name__]['head'], ig=ig, interface=self.interface, util=util)

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

        self.st.out(self.templates[self.__class__.__name__]['interface'],
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
                if type in tools.ttsMap.values():
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

        self.st.out(self.templates[self.__class__.__name__]['operation'], rtype=rtype, id=node.identifier(), params=params, raises=raises)

    def visitBaseType(self, type):
        self.__result_type = tools.ttsMap[type.kind()]

    def visitDeclaredType(self, type):
        self.__result_type = idlutil.ccolonName(type.decl().scopedName())

    def visitSequenceType(self, type):
        self.__result_type = "std::vector<" + type.seqType().name() + ">"

    def visitStringType(self, type):
        self.__result_type = "std::string"

    def visitWStringType(self, type):
        self.__result_type = "std:wstring"


class ProxySource (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st, interface, suffix, module_prefix, templates):
        self.st = st
        self.interface = interface
        self.suffix = suffix
        self.templates = templates
        self.class_name = interface + '_' + suffix

        self.operations = []
        self.module_name = ''
        self.module_prefix = module_prefix

    def visitAST(self, node):
        self.st.out(self.templates[self.__class__.__name__]['head'], class_name=self.class_name)

        for n in node.declarations():
            if n.mainFile():
                n.accept(self)

    def visitModule(self, node):
        self.module_name = node.identifier()
        self.st.out(self.templates[self.__class__.__name__]['module'],
                    class_name=self.class_name, suffix=self.suffix, interface=self.interface,
                    module_name=self.module_name, module_prefix=self.module_prefix)

        for n in node.definitions():
            n.accept(self)

        self.st.out("""}""")

    def visitInterface(self, node):
        if node.identifier() == self.interface:
            for n in node.contents():
                n.accept(self)

    def __exceptionThrow(self, exception):
        r = 'if (strcmp("org.dboost.' + exception.replace('::', '.') + '", err.name) == 0) {\n'\
            '\tthrow ' + exception + '(err.message);\n}'
        return r

    def visitOperation(self, node):
        timeout = 'TIMEOUT_MS'
        for p in node.pragmas():
            key, value = p.text().split(' ')
            if key == 'operation_timeout':
                timeout = value
                break

        self.operations.append(node.identifier())
        node.returnType().accept(self)
        rtype = self.__result_type

        paraml = []
        i = 0
        for p in node.parameters():
            p.paramType().accept(self)
            ptype = self.__result_type
            paraml.append((ptype, 'param' + str(i), p.is_out()))
            i += 1

        params = ', '.join(map(lambda (ptype, name, is_out): ('const ' if not is_out else '') + ptype +
                                                             ('& ' if ptype not in tools.ttsMap.values() else '') + ' ' + name, paraml))
        params_serialize = 'dboost::oserializer os(msg.get());\nos ' if len(paraml) > 0 else ''
        params_serialize += ' '.join(map(lambda (ptype, name, is_out): '& ' + name, paraml))

        params_out = filter(lambda (type, name, is_out): is_out, paraml)
        params_out_serialize = 'dboost::iserializer is(reply.get());\n' + rtype + ' r;\nis ' if rtype != 'void' or len(params_out) > 0 else ''
        if rtype != 'void':
            params_out_serialize += ' & r'
        if len(params_out) > 0:
            params_out_serialize += ' '.join(map(lambda (type, name, is_out): '& ' + name, params_out))
        result = '' if rtype == 'void' else 'r'

        raisel = []
        if len(node.raises()) > 0:
            for r in node.raises():
                ename  = idlutil.ccolonName(r.scopedName())
                raisel.append(ename)

            raises = " throw (" + string.join(raisel, ", ") + ")"
        else:
            raises = ""
        exceptions_throw = '\n'.join(map(self.__exceptionThrow, raisel))

        self.st.out(self.templates[self.__class__.__name__]['operation'], class_name=self.class_name, rtype=rtype, operation=node.identifier(),
                    params=params, params_serialize=params_serialize, params_out_serialize=params_out_serialize,
                    result=result, raises=raises, exceptions_throw=exceptions_throw, timeout=timeout)

    def visitBaseType(self, type):
        self.__result_type = tools.ttsMap[type.kind()]

    def visitDeclaredType(self, type):
        self.__result_type = idlutil.ccolonName(type.decl().scopedName())

    def visitSequenceType(self, type):
        self.__result_type = "std::vector<" + type.seqType().name() + ">"

    def visitStringType(self, type):
        self.__result_type = "std::string"

    def visitWStringType(self, type):
        self.__result_type = "std::wstring"

def run(tree, args, templates, suffix):
    ag = tools.Aggregator()
    tree.accept(ag)
    interfaces = ag.getInterfaces()
    module_prefix = ag.getModulePrefix()

    for interface in interfaces:
        with open(interface + '_' + suffix + '.hpp', 'w') as header:
            st = output.Stream(header, 2)

            cv = ProxyHeader(st, interface, suffix, templates, len(ag.getStructs()) != 0)
            tree.accept(cv)

    for interface in interfaces:
        with open(interface + '_' + suffix + '.cpp', 'w') as source:
            st = output.Stream(source, 2)

            cv = ProxySource(st, interface, suffix, module_prefix, templates)
            tree.accept(cv)