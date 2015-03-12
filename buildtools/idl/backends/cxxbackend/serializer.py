import os

from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import tools

class Serializer (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):
    def __init__(self, st, templates, suffix):
        self.st = st
        self.templates = templates
        self.suffix = suffix

    def visitAST(self, node):
        self.interface = os.path.basename(node.file()).replace(".idl", "")
        ig = (self.interface + '_' + self.suffix + '.hpp').upper()
        self.st.out(self.templates[self.__class__.__name__]['head'], ig=ig, interface=self.interface)
        for n in node.declarations():
            if n.mainFile():
                n.accept(self)

    def visitModule(self, node):
        self.st.out("""\
namespace @id@
{""", id = node.identifier())
        self.st.out("""\
""")
        for n in node.definitions():
            n.accept(self)

        self.st.out("""\
}
""")

    def visitStruct(self, node):
        members = []
        for m in node.members():
            decll = []
            for d in m.declarators():
                decll.append(d.identifier())
            members += decll
        memberss = ' '.join(map(lambda s: '& s.' + s, members))
        self.st.out(self.templates[self.__class__.__name__]['struct'], struct=node.identifier(), members=memberss)

def run(tree, args, templates, suffix):
    sa = tools.Aggregator()
    tree.accept(sa)
    if len(sa.getStructs()) == 0:
        return
    name, ext = os.path.splitext(os.path.basename(tree.file()))
    with open(name + '_' + suffix + '.hpp', 'w') as header:
        st = output.Stream(header, 2)

        cv = Serializer(st, templates, suffix)
        tree.accept(cv)