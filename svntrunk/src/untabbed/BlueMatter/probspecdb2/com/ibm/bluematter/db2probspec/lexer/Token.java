/* Copyright 2001, 2019 IBM Corporation
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
 * following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 package com.ibm.bluematter.db2probspec.lexer;

import com.ibm.bluematter.db2probspec.parser.*;
import java_cup.runtime.Symbol;
import java.io.*;

public class Token implements LexerResult {

    private Symbol symbol;
    private String filename;
    private int lineno;
    private Object value;	// String, Boolean, Integer, Vector, null
    
  static int lastID;

  
    public Token (String filename, int lineno, Object value) {
    this (-1, filename, lineno, -1, -1, value);
    }

    public Token (int id, String filename, int lineno, int left, int right,
      Object value) {
    //super(id, left, right, value);
    
    symbol = new Symbol (id, left, right, this);
    lastID=id;
    this.filename = filename;
    this.lineno = lineno;
    this.value = value;
    }

    public int getCode () {
    return symbol.sym; 
  }
  
    public Symbol getSymbol () {
    return symbol;
  }
  
    public Object getValue () {
    return value;
  }

    public String getID () {
    switch (symbol.sym) {
// 			case Constant.EQEQ: return "EQEQ";
// 			case Constant.NEW: return "NEW";
// 			case Constant.LBRACE: return "LBRACE";
// 			case Constant.WHILE: return "WHILE";
// 			case Constant.PLUS: return "PLUS";
// 			case Constant.LBRACKET: return "LBRACKET";
// 			case Constant.RBRACKET: return "RBRACKET";
// 			case Constant.FALSE_CONST: return "FALSE_CONST";
// 			case Constant.COLON: return "COLON";
// 			case Constant.STRING_CONST: return "STRING_CONST";
// 			case Constant.STRING_TYPE: return "STRING_TYPE";
// 			case Constant.TRUE_CONST: return "TRUE_CONST";
// 			case Constant.MINUS: return "MINUS";
// 			case Constant.MULT: return "TIMES";
// 			case Constant.GT: return "GT";
// 			case Constant.RBRACE: return "RBRACE";
// 			case Constant.SEMI: return "SEMI";
// 			case Constant.MOD: return "MOD";
// 			case Constant.ELSE: return "ELSE";
// 			case Constant.COMMA: return "COMMA";
// 			case Constant.LPAREN: return "LPAREN";
// 			case Constant.EOF: return "EOF";
// 			case Constant.RETURN: return "RETURN";
// 			case Constant.STMT: return "STMT";
// 			case Constant.LENGTH: return "LENGTH";
// 			case Constant.error: return "error";
// 			case Constant.USES: return "USES";
// 			case Constant.BOOL_TYPE: return "BOOL_TYPE";
// 			case Constant.BANG: return "BANG";
// 			case Constant.ID: return "ID";
// 			case Constant.MODULE: return "MODULE";
// 			case Constant.RPAREN: return "RPAREN";
// 			case Constant.LT: return "LT";
// 			case Constant.EQ: return "EQ";
// 			case Constant.IF: return "IF";
// 			case Constant.INT_TYPE: return "INT_TYPE";
// 			case Constant.DOT: return "DOT";
// 			case Constant.AND: return "AND";
// 			case Constant.DIV: return "DIV";
// 			case Constant.OR: return "OR";
// 			case Constant.INT_CONST: return "INT_CONST";
// 		        case Constant.ARRAY: return "ARRAY";
      

// 			// Iota+ stuff
// 			case Constant.INTERFACE: return "INTERFACE";
// 			case Constant.OBJECT_TYPE: return "OBJECT_TYPE";
// 			case Constant.IMPLEMENTS: return "IMPLEMENTS";
// 			case Constant.NULL_CONST: return "NULL_CONST";
// 			case Constant.THIS: return "THIS";
// 			case Constant.CAST: return "CAST";
// 			case Constant.LEQ: return "LEQ";
// 			case Constant.GEQ: return "GEQ";
// 			case Constant.NEQ: return "NEQ";
// 			case Constant.INC: return "INC";
// 			case Constant.DEC: return "DEC";
// 			case Constant.BREAK: return "BREAK";
// 			case Constant.CLASS: return "CLASS";
    case Constant.DOUBLE_VAL: return "DOUBLE";
    default: { return "null";}
    }
    //		throw new IllegalStateException ("Unknown symbol code: " + symbol.sym);
  }

  public static String toString (int type) {
    switch(type){
// 			case Constant.AND: { return "&";}
// 			case Constant.ARRAY: { return "array";}
// 			case Constant.BANG: { return "!";}
// 			case Constant.BOOL_TYPE: { return "bool";}
// 			case Constant.COMMA: { return ",";}
// 			case Constant.COLON: { return ":";}
// 			case Constant.DIV: { return "/";}
// 			case Constant.DOT: { return ".";}
// 			case Constant.ELSE: { return "else";}
// 			case Constant.EOF: {return "eof"; }
// 			case Constant.EQ: { return "=";}
// 			case Constant.EQEQ: { return "==";}
// 			case Constant.FALSE_CONST: { return "false";}
// 			case Constant.GT: { return ">";}
// 			case Constant.ID: { return "id";}
// 			case Constant.IF: { return "if";}
// 			case Constant.INT_CONST: { return "integer";}
// 			case Constant.INT_TYPE: { return "int";}
// 			case Constant.LBRACKET: { return "[";}
// 			case Constant.LENGTH: { return "length";}
// 			case Constant.LPAREN: { return "(";}
// 			case Constant.LT: { return "<";}
// 			case Constant.MINUS: { return "-";}
// 			case Constant.MOD: { return "%";}
// 			case Constant.MULT: { return "*";}
// 			case Constant.NEW: { return "new";}
// 			case Constant.OR: { return "|";}
// 			case Constant.PLUS: { return "+";}
// 			case Constant.RBRACKET: { return "]";}
// 			case Constant.RETURN: { return "return";}
// 			case Constant.RPAREN: { return ")";}
// 			case Constant.SEMI: { return ";";}
// 			case Constant.STRING_TYPE: { return "string";}
// 			case Constant.TRUE_CONST: { return "true";}
// 			case Constant.USES: { return "uses";}
// 			case Constant.WHILE: { return "while";}

// 			// Iota+ stuff
// 			case Constant.INTERFACE: return "INTERFACE";
// 			case Constant.OBJECT_TYPE: return "OBJECT_TYPE";
// 			case Constant.IMPLEMENTS: return "IMPLEMENTS";
// 			case Constant.NULL_CONST: return "NULL_CONST";
// 			case Constant.THIS: return "THIS";
// 			case Constant.CAST: return "CAST";
// 			case Constant.LEQ: return "LEQ";
// 			case Constant.GEQ: return "GEQ";
// 			case Constant.NEQ: return "NEQ";
// 			case Constant.INC: return "INC";
// 			case Constant.DEC: return "DEC";
// 			case Constant.BREAK: return "BREAK";
// 			case Constant.CLASS: return "CLASS"; 

    default: {
        System.out.println("Invalid token conversion: "+type);
        System.exit(2);
      }
    }
    return null;
  }

  public String toString () {
    return (String) value;
    //return filename + ":" + lineno + ": \"" + value + "\"";
    }

    public void unparse (java.io.OutputStream o) {
    
    if (value != null) {
      try {
        o.write ((filename + ":" + lineno + ": " + this.getID()+ ": \"" +value + "\"").getBytes());
        //o.write (value.toString ().getBytes ());
      }
      catch (IOException e) {
      }
    }
    }

    public String getFilename () {
    return filename;
  }
  
    public int lineNumber () {
    return lineno;
  }
  
  public int getLineno() {
    return lineno;
  }
  
    public void setLineno (int i) {
    lineno = i;
  }
}
