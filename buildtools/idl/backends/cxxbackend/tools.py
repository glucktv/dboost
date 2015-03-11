from omniidl import idlast, idltype, idlutil, idlvisitor, output

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