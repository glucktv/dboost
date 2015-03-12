import os
import string

from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import tools

class AdapterHeader (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st, interface, suffix, templates):
        self.st = st
        self.interface = interface
        self.suffix = suffix
        self.templates = templates
        self.class_name = interface + '_' + suffix

    def visitAST(self, node):
        ig = (self.interface + '_' + self.suffix + '_HPP').upper()
        self.st.out(self.templates[self.__class__.__name__]['head'], ig=ig, suffix=self.suffix, interface=self.interface)

        for n in node.declarations():
            if n.mainFile():
                n.accept(self)
        self.st.out("#endif // @ig@", ig=ig)

    def visitModule(self, node):
        self.st.out("""\
namespace @id@
{""", id = node.identifier())

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
                    id=node.identifier(), inherits=inherits, class_name=self.class_name, ancestor=self.interface)

        self.st.inc_indent()

        for n in node.contents():
            n.accept(self)

        self.st.dec_indent()
        self.st.out("""\
};
""")

    def visitOperation(self, node):
        self.st.out(self.templates[self.__class__.__name__]['operation'], operation=node.identifier(), interface=self.interface)

class AdapterSource (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st):
        self.st = st

def run(tree, args, templates, suffix):
    ia = tools.InterfacesAggregator(tree.file())
    tree.accept(ia)
    interfaces = ia.getInterfaces()

    for interface in interfaces:
        with open(interface + '_' + suffix + '.hpp', 'w') as header:
            st = output.Stream(header, 2)

            cv = AdapterHeader(st, interface, suffix, templates)
            tree.accept(cv)
    #
    # with open(name + '_proxy.cpp', 'w') as source:
    #     st = output.Stream(source, 2)
    #
    #     cv = AdapterSource(st)
    #     tree.accept(cv)