# from a list of schemas, create an adjacency list representation of the
# tables involved with directed edges representing referential constraints

from schemadb2 import *

def schema2graph(dbName, schema):
    db = Database(dbName)
    graph = {}
    for schemaName in schema:
        sc = Schema(db, schemaName)
        tableList = sc.tables()
        for t in tableList:
            tableName = (schemaName.strip().upper(), t[0].strip().upper())
            adj = graph.setdefault(tableName, [])
            table = Table(sc, t[0])
            refs = table.references()
            for ref in refs:
                adj.append((ref.refSchema().strip().upper(), ref.refTable().strip().upper()))
        graph[tableName] = adj
    return graph

    

