from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import header, adapter, proxy, templates, serializer

def run(tree, args):
    header.run(tree, args)
    adapter.run(tree, args, templates.out, 'adaptor')
    proxy.run(tree, args, templates.out, 'proxy')
    serializer.run(tree, args, templates.out, 'util')