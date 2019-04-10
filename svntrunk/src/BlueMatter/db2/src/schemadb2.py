# utility routines to construct objects encapsulating aspects of a db2
# database

import DB2


class Database:
    def __init__(self, dbName):
        self.__dbName=dbName
        self.__conn=DB2.Connection(dbName)
    def schemaList(self):
        cursor = self.__conn.cursor()
        cursor.execute("select schemaname, owner from syscat.schemata")
        schema = []
        foo = cursor.fetchall()
        cursor.close()
        # returns sequence of 2-sequences where first elt is schemaName
        # and second elt is owner
        return foo
    def connection(self):
        return self.__conn
    
class Schema:
    def __init__(self, database, schemaName): # takes Database object
        # and schema name as arguments
        self.__db=database
        self.__schemaName=schemaName
    def name(self):
        return self.__schemaName
    def database(self):
        return self.__db
    def tables(self):
        conn = self.__db.connection()
        cur = conn.cursor()
        cur.execute("select tabname, card, tableid from syscat.tables where tabschema = \'" + str(self.__schemaName).upper() + "\'")
        foo = cur.fetchall()
        cur.close()
        return foo

class Reference:
    def __init__(self, schema, table, constraint):
        self.__db = schema.database()
        self.__schema = schema
        self.__table = table
        self.__constraint = constraint
        cursor = self.__db.connection().cursor()
        cmd = "select reftabschema, reftabname, refkeyname from syscat.references where constname = \'"\
              + self.__constraint\
              + "\' and tabname = \'"\
              + self.__table.name()\
              + "\' and tabschema = \'"\
              + self.__schema.name()\
              + "\'"
        cursor.execute(cmd.upper())
        self.__reference = cursor.fetchone() # (rtabschema, rtabname, refkeyname)
        cmd = "select colname from syscat.keycoluse where constname =\'"\
              + self.__constraint\
              +"\' and tabschema = \'"\
              + self.__schema.name()\
              + "\' and tabname = \'"\
              + self.__table.name()\
              + "\'"
        cursor.execute(cmd.upper())
        self.__keyCols = []
        foo = cursor.fetchone()
        while (foo):
            self.__keyCols.append(foo[0])
            foo = cursor.fetchone()
        cmd = "select colname from syscat.keycoluse where constname =\'"\
              + self.__reference[2]\
              +"\' and tabschema = \'"\
              + self.__reference[0]\
              + "\' and tabname = \'"\
              + self.__reference[1]\
              + "\'"
        cursor.execute(cmd.upper())
        self.__refKeyCols = []
        foo = cursor.fetchone()
        while (foo):
            self.__refKeyCols.append(foo[0])
            foo = cursor.fetchone()
    def schema(self):
        return self.__schema
    def table(self):
        return self.__table
    def constraint(self):
        return self.__constraint
    def refSchema(self):
        return self.__reference[0]
    def refTable(self):
        return self.__reference[1]
    def refKey(self):
        return self.__reference[2]
    def keyCols(self):
        return self.__keyCols
    def refKeyCols(self):
        return self.__refKeyCols
    
class Table:
    def __init__(self, schema, tableName):
        self.__db = schema.database()
        self.__schema = schema
        self.__tableName = tableName
    def schema(self):
        return self.__schema
    def name(self):
        return self.__tableName
    def columns(self): # return colName, typeName, length, nulls
        cursor = self.__db.connection().cursor()
        cmd = "select colname, typename, length, nulls from syscat.columns where tabschema = \'"\
              + str(self.__schema.name()) + "\' and tabname = \'"\
              + str(self.__tableName) + "\'"
        cursor.execute(cmd.upper())
        foo = cursor.fetchall()
        cursor.close()
        return foo
    def references(self):
        cursor = self.__db.connection().cursor()
        cmd = "select constname from syscat.references where tabschema = \'"\
              + self.__schema.name()\
              + "\' and tabname = \'"\
              + self.__tableName\
              + "\'"
        cursor.execute(cmd.upper())
        foo = cursor.fetchone()
        refs = []
        while (foo):
            newRef = Reference(self.__schema, self, foo[0])
            refs.append(newRef)
            foo = cursor.fetchone()
        return refs
    def primary(self):
        idx = []
        cursor = self.__db.connection().cursor()
        cmd = "select indschema, indname from syscat.indexes where "\
              + "tabschema = \'"\
              + self.__schema.name()\
              + "\' and tabname = \'"\
              + self.__tableName\
              + "\'"\
              +" and uniquerule = \'P\'"
        cursor.execute(cmd.upper())
        foo = cursor.fetchone()
        if (foo):
            indschema = foo[0]
            indname = foo[1]
            cmd = "select colname from syscat.indexcoluse where "\
                  "indschema = \'" + indschema + "\' and "\
                  "indname = \'" + indname + "\' order by colseq"
            cursor.execute(cmd.upper())
            bar = cursor.fetchone()
            while(bar):
                idx.append(bar[0])
                bar = cursor.fetchone()
        return idx
    
              
