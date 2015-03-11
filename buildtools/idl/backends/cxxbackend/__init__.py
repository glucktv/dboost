from omniidl import idlast, idltype, idlutil, idlvisitor, output
from cxxbackend import header, adapter, proxy, templates

def run(tree, args):
    header.run(tree, args)
    adapter.run(tree, args, templates.out)
    proxy.run(tree, args, templates.out)