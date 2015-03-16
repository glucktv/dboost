import os
import string

from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import tools

class AdapterHeader (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st, interface, suffix, templates, need_utils=False):
        self.st = st
        self.interface = interface
        self.suffix = suffix
        self.templates = templates
        self.class_name = interface + '_' + suffix
        self.module_name = ''
        self.need_utils = need_utils

    def visitAST(self, node):
        ig = (self.interface + '_' + self.suffix + '_HPP').upper()
        if self.need_utils:
            util = '#include "' + self.interface + '_util.hpp' + '"'
        else:
            util = ''
        self.st.out(self.templates[self.__class__.__name__]['head'], ig=ig, suffix=self.suffix, interface=self.interface, util=util)

        for n in node.declarations():
            if n.mainFile():
                n.accept(self)
        self.st.out("#endif // @ig@", ig=ig)

    def visitModule(self, node):
        self.module_name = node.identifier()
        self.st.out("""\
namespace @module_name@
{""", module_name=self.module_name)

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
        inherits = 'dboost::' + self.suffix

        self.st.out(self.templates[self.__class__.__name__]['interface'],
                    id=node.identifier(), inherits=inherits, class_name=self.class_name, ancestor=self.interface,
                    module_name=self.module_name, interface=self.interface)

        self.st.inc_indent()

        for n in node.contents():
            n.accept(self)

        self.st.dec_indent()
        self.st.out("""\
};
""")

    def visitOperation(self, node):
        self.st.out(self.templates[self.__class__.__name__]['operation'], operation=node.identifier(),
                    interface=self.interface, module_name=self.module_name)


class AdapterSource (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st, interface, suffix, templates):
        self.st = st
        self.interface = interface
        self.suffix = suffix
        self.templates = templates
        self.class_name = interface + '_' + suffix

        self.operations = []
        self.module_name = ''

    def visitAST(self, node):
        self.st.out(self.templates[self.__class__.__name__]['head'], class_name=self.class_name)

        for n in node.declarations():
            if n.mainFile():
                n.accept(self)

    def visitModule(self, node):
        self.module_name = node.identifier()
        self.st.out(self.templates[self.__class__.__name__]['module_top'],
                    class_name=self.class_name, suffix=self.suffix, interface=self.interface, module_name=self.module_name)

        for n in node.definitions():
            n.accept(self)

        operationsstr = ',\n'.join(map(lambda s: '{' + '"' + s + '"' + ', &' + self.class_name + '::call_' + s +'}', self.operations))
        self.st.out(self.templates[self.__class__.__name__]['module_bottom'], id=node.identifier(),
                    class_name=self.class_name, suffix=self.suffix, interface=self.interface,
                    operations=operationsstr)

    def visitInterface(self, node):
        if node.identifier() == self.interface:
            for n in node.contents():
                n.accept(self)

    def visitOperation(self, node):
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

        params_def = '\n'.join(map(lambda (ptype, name, is_out): ptype + ' ' + name + ';', paraml))
        params_serialize = 'dboost::iserializer is(m);\nis ' if len(paraml) > 0 else ''
        params_serialize += ' '.join(map(lambda (ptype, name, is_out): '& ' + name, paraml))
        if params_serialize != '':
            params_serialize += ';'
        params_out = filter(lambda (ptype, name, is_out): is_out, paraml)
        params_out_serialize = 'dboost::oserializer os(result.get());\nos ' if rtype != 'void' or len(params_out) > 0 else ''
        params_out_serialize += '& r ' if rtype != 'void' else ''
        params_out_serialize += ' '.join(map(lambda (ptype, name, is_out): '& ' + name, params_out))
        if params_out_serialize != '':
            params_out_serialize += ';'
        call = ''
        if rtype != 'void':
            call += rtype + ' r = '
        call += 't->' + node.identifier() + '('
        call += ', '.join(map(lambda (type, name, is_out): name, paraml))
        call += ')'

        self.st.out(self.templates[self.__class__.__name__]['operation'], operation=node.identifier(), interface=self.interface,
                    params_def=params_def, params_serialize=params_serialize, params_out_serialize=params_out_serialize,
                    call=call, module_name=self.module_name, class_name=self.class_name)

        self.st.out("""\
}
""")

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

    for interface in interfaces:
        with open(interface + '_' + suffix + '.hpp', 'w') as header:
            st = output.Stream(header, 2)

            cv = AdapterHeader(st, interface, suffix, templates, len(ag.getStructs()) != 0)
            tree.accept(cv)

    for interface in interfaces:
        with open(interface + '_' + suffix + '.cpp', 'w') as source:
            st = output.Stream(source, 2)

            cv = AdapterSource(st, interface, suffix, templates)
            tree.accept(cv)