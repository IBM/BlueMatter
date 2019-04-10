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
import java.io.InputStream;
import com.ibm.bluematter.db2probspec.parser.Constant;


public class Lexer {
  private final int YY_BUFFER_SIZE = 512;
  private final int YY_F = -1;
  private final int YY_NO_STATE = -1;
  private final int YY_NOT_ACCEPT = 0;
  private final int YY_START = 1;
  private final int YY_END = 2;
  private final int YY_NO_ANCHOR = 4;
  private final int YY_BOL = 128;
  private final int YY_EOF = 129;

private int lastId = -1;
private String filename = "";
private String lineSeparator;
public Lexer(InputStream in, String filename) {
this(in);
this.filename = filename;
}
private void error(String message) throws LexicalError {
throw new LexicalError(filename, yyline+1, message);
}
private Token t(int id, Object value) {
lastId = id;
return new Token(id, filename, yyline + 1, yychar, yychar + yylength(), value);
}
private Token t(int id) {
return t(id, yytext());
}
  private java.io.BufferedReader yy_reader;
  private int yy_buffer_index;
  private int yy_buffer_read;
  private int yy_buffer_start;
  private int yy_buffer_end;
  private char yy_buffer[];
  private int yychar;
  private int yyline;
  private boolean yy_at_bol;
  private int yy_lexical_state;

  public Lexer (java.io.Reader reader) {
    this ();
    if (null == reader) {
      throw (new Error("Error: Bad input stream initializer."));
    }
    yy_reader = new java.io.BufferedReader(reader);
  }

  public Lexer (java.io.InputStream instream) {
    this ();
    if (null == instream) {
      throw (new Error("Error: Bad input stream initializer."));
    }
    yy_reader = new java.io.BufferedReader(new java.io.InputStreamReader(instream));
  }

  private Lexer () {
    yy_buffer = new char[YY_BUFFER_SIZE];
    yy_buffer_read = 0;
    yy_buffer_index = 0;
    yy_buffer_start = 0;
    yy_buffer_end = 0;
    yychar = 0;
    yyline = 0;
    yy_at_bol = true;
    yy_lexical_state = YYINITIAL;

    lineSeparator = System.getProperty("line.separator", "\n");
  }

  private boolean yy_eof_done = false;
  private final int YYINITIAL = 0;
  private final int COMMENT = 1;
  private final int yy_state_dtrans[] = {
    0,
    391
  };
  private void yybegin (int state) {
    yy_lexical_state = state;
  }
  private int yy_advance ()
    throws java.io.IOException {
    int next_read;
    int i;
    int j;

    if (yy_buffer_index < yy_buffer_read) {
      return yy_buffer[yy_buffer_index++];
    }

    if (0 != yy_buffer_start) {
      i = yy_buffer_start;
      j = 0;
      while (i < yy_buffer_read) {
        yy_buffer[j] = yy_buffer[i];
        ++i;
        ++j;
      }
      yy_buffer_end = yy_buffer_end - yy_buffer_start;
      yy_buffer_start = 0;
      yy_buffer_read = j;
      yy_buffer_index = j;
      next_read = yy_reader.read(yy_buffer,
          yy_buffer_read,
          yy_buffer.length - yy_buffer_read);
      if (-1 == next_read) {
        return YY_EOF;
      }
      yy_buffer_read = yy_buffer_read + next_read;
    }

    while (yy_buffer_index >= yy_buffer_read) {
      if (yy_buffer_index >= yy_buffer.length) {
        yy_buffer = yy_double(yy_buffer);
      }
      next_read = yy_reader.read(yy_buffer,
          yy_buffer_read,
          yy_buffer.length - yy_buffer_read);
      if (-1 == next_read) {
        return YY_EOF;
      }
      yy_buffer_read = yy_buffer_read + next_read;
    }
    return yy_buffer[yy_buffer_index++];
  }
  private void yy_move_end () {
    if (yy_buffer_end > yy_buffer_start &&
        '\n' == yy_buffer[yy_buffer_end-1])
      yy_buffer_end--;
    if (yy_buffer_end > yy_buffer_start &&
        '\r' == yy_buffer[yy_buffer_end-1])
      yy_buffer_end--;
  }
  private boolean yy_last_was_cr=false;
  private void yy_mark_start () {
    int i;
    for (i = yy_buffer_start; i < yy_buffer_index; ++i) {
      if ('\n' == yy_buffer[i] && !yy_last_was_cr) {
        ++yyline;
      }
      if ('\r' == yy_buffer[i]) {
        ++yyline;
        yy_last_was_cr=true;
      } else yy_last_was_cr=false;
    }
    yychar = yychar
      + yy_buffer_index - yy_buffer_start;
    yy_buffer_start = yy_buffer_index;
  }
  private void yy_mark_end () {
    yy_buffer_end = yy_buffer_index;
  }
  private void yy_to_mark () {
    yy_buffer_index = yy_buffer_end;
    yy_at_bol = (yy_buffer_end > yy_buffer_start) &&
                ('\r' == yy_buffer[yy_buffer_end-1] ||
                 '\n' == yy_buffer[yy_buffer_end-1] ||
                 2028/*LS*/ == yy_buffer[yy_buffer_end-1] ||
                 2029/*PS*/ == yy_buffer[yy_buffer_end-1]);
  }
  private java.lang.String yytext () {
    return (new java.lang.String(yy_buffer,
      yy_buffer_start,
      yy_buffer_end - yy_buffer_start));
  }
  private int yylength () {
    return yy_buffer_end - yy_buffer_start;
  }
  private char[] yy_double (char buf[]) {
    int i;
    char newbuf[];
    newbuf = new char[2*buf.length];
    for (i = 0; i < buf.length; ++i) {
      newbuf[i] = buf[i];
    }
    return newbuf;
  }
  private final int YY_E_INTERNAL = 0;
  private final int YY_E_MATCH = 1;
  private java.lang.String yy_error_string[] = {
    "Error: Internal error.\n",
    "Error: Unmatched input.\n"
  };
  private void yy_error (int code,boolean fatal) {
    java.lang.System.out.print(yy_error_string[code]);
    java.lang.System.out.flush();
    if (fatal) {
      throw new Error("Fatal Error.\n");
    }
  }
  private int[][] unpackFromString(int size1, int size2, String st) {
    int colonIndex = -1;
    String lengthString;
    int sequenceLength = 0;
    int sequenceInteger = 0;

    int commaIndex;
    String workString;

    int res[][] = new int[size1][size2];
    for (int i= 0; i < size1; i++) {
      for (int j= 0; j < size2; j++) {
        if (sequenceLength != 0) {
          res[i][j] = sequenceInteger;
          sequenceLength--;
          continue;
        }
        commaIndex = st.indexOf(',');
        workString = (commaIndex==-1) ? st :
          st.substring(0, commaIndex);
        st = st.substring(commaIndex+1);
        colonIndex = workString.indexOf(':');
        if (colonIndex == -1) {
          res[i][j]=Integer.parseInt(workString);
          continue;
        }
        lengthString =
          workString.substring(colonIndex+1);
        sequenceLength=Integer.parseInt(lengthString);
        workString=workString.substring(0,colonIndex);
        sequenceInteger=Integer.parseInt(workString);
        res[i][j] = sequenceInteger;
        sequenceLength--;
      }
    }
    return res;
  }
  private int yy_acpt[] = {
    /* 0 */ YY_NOT_ACCEPT,
    /* 1 */ YY_NO_ANCHOR,
    /* 2 */ YY_NO_ANCHOR,
    /* 3 */ YY_NO_ANCHOR,
    /* 4 */ YY_NO_ANCHOR,
    /* 5 */ YY_NO_ANCHOR,
    /* 6 */ YY_NO_ANCHOR,
    /* 7 */ YY_NO_ANCHOR,
    /* 8 */ YY_NO_ANCHOR,
    /* 9 */ YY_NO_ANCHOR,
    /* 10 */ YY_NO_ANCHOR,
    /* 11 */ YY_NO_ANCHOR,
    /* 12 */ YY_NO_ANCHOR,
    /* 13 */ YY_NO_ANCHOR,
    /* 14 */ YY_NO_ANCHOR,
    /* 15 */ YY_NO_ANCHOR,
    /* 16 */ YY_NO_ANCHOR,
    /* 17 */ YY_NO_ANCHOR,
    /* 18 */ YY_NO_ANCHOR,
    /* 19 */ YY_NO_ANCHOR,
    /* 20 */ YY_NO_ANCHOR,
    /* 21 */ YY_NO_ANCHOR,
    /* 22 */ YY_NO_ANCHOR,
    /* 23 */ YY_NO_ANCHOR,
    /* 24 */ YY_NO_ANCHOR,
    /* 25 */ YY_NO_ANCHOR,
    /* 26 */ YY_NO_ANCHOR,
    /* 27 */ YY_NO_ANCHOR,
    /* 28 */ YY_NO_ANCHOR,
    /* 29 */ YY_NO_ANCHOR,
    /* 30 */ YY_NO_ANCHOR,
    /* 31 */ YY_NO_ANCHOR,
    /* 32 */ YY_NO_ANCHOR,
    /* 33 */ YY_NO_ANCHOR,
    /* 34 */ YY_NO_ANCHOR,
    /* 35 */ YY_NO_ANCHOR,
    /* 36 */ YY_NO_ANCHOR,
    /* 37 */ YY_NO_ANCHOR,
    /* 38 */ YY_NO_ANCHOR,
    /* 39 */ YY_NO_ANCHOR,
    /* 40 */ YY_NO_ANCHOR,
    /* 41 */ YY_NO_ANCHOR,
    /* 42 */ YY_NO_ANCHOR,
    /* 43 */ YY_NO_ANCHOR,
    /* 44 */ YY_NO_ANCHOR,
    /* 45 */ YY_NO_ANCHOR,
    /* 46 */ YY_NO_ANCHOR,
    /* 47 */ YY_NO_ANCHOR,
    /* 48 */ YY_NO_ANCHOR,
    /* 49 */ YY_NO_ANCHOR,
    /* 50 */ YY_NO_ANCHOR,
    /* 51 */ YY_NO_ANCHOR,
    /* 52 */ YY_NO_ANCHOR,
    /* 53 */ YY_NO_ANCHOR,
    /* 54 */ YY_NO_ANCHOR,
    /* 55 */ YY_NO_ANCHOR,
    /* 56 */ YY_NO_ANCHOR,
    /* 57 */ YY_NO_ANCHOR,
    /* 58 */ YY_NO_ANCHOR,
    /* 59 */ YY_NO_ANCHOR,
    /* 60 */ YY_NO_ANCHOR,
    /* 61 */ YY_NO_ANCHOR,
    /* 62 */ YY_NO_ANCHOR,
    /* 63 */ YY_NO_ANCHOR,
    /* 64 */ YY_NO_ANCHOR,
    /* 65 */ YY_NO_ANCHOR,
    /* 66 */ YY_NO_ANCHOR,
    /* 67 */ YY_NO_ANCHOR,
    /* 68 */ YY_NO_ANCHOR,
    /* 69 */ YY_NO_ANCHOR,
    /* 70 */ YY_NO_ANCHOR,
    /* 71 */ YY_NO_ANCHOR,
    /* 72 */ YY_NOT_ACCEPT,
    /* 73 */ YY_NO_ANCHOR,
    /* 74 */ YY_NO_ANCHOR,
    /* 75 */ YY_NO_ANCHOR,
    /* 76 */ YY_NOT_ACCEPT,
    /* 77 */ YY_NO_ANCHOR,
    /* 78 */ YY_NOT_ACCEPT,
    /* 79 */ YY_NO_ANCHOR,
    /* 80 */ YY_NOT_ACCEPT,
    /* 81 */ YY_NO_ANCHOR,
    /* 82 */ YY_NOT_ACCEPT,
    /* 83 */ YY_NO_ANCHOR,
    /* 84 */ YY_NOT_ACCEPT,
    /* 85 */ YY_NO_ANCHOR,
    /* 86 */ YY_NOT_ACCEPT,
    /* 87 */ YY_NO_ANCHOR,
    /* 88 */ YY_NOT_ACCEPT,
    /* 89 */ YY_NO_ANCHOR,
    /* 90 */ YY_NOT_ACCEPT,
    /* 91 */ YY_NO_ANCHOR,
    /* 92 */ YY_NOT_ACCEPT,
    /* 93 */ YY_NO_ANCHOR,
    /* 94 */ YY_NOT_ACCEPT,
    /* 95 */ YY_NO_ANCHOR,
    /* 96 */ YY_NOT_ACCEPT,
    /* 97 */ YY_NO_ANCHOR,
    /* 98 */ YY_NOT_ACCEPT,
    /* 99 */ YY_NO_ANCHOR,
    /* 100 */ YY_NOT_ACCEPT,
    /* 101 */ YY_NO_ANCHOR,
    /* 102 */ YY_NOT_ACCEPT,
    /* 103 */ YY_NO_ANCHOR,
    /* 104 */ YY_NOT_ACCEPT,
    /* 105 */ YY_NO_ANCHOR,
    /* 106 */ YY_NOT_ACCEPT,
    /* 107 */ YY_NO_ANCHOR,
    /* 108 */ YY_NOT_ACCEPT,
    /* 109 */ YY_NO_ANCHOR,
    /* 110 */ YY_NOT_ACCEPT,
    /* 111 */ YY_NO_ANCHOR,
    /* 112 */ YY_NOT_ACCEPT,
    /* 113 */ YY_NO_ANCHOR,
    /* 114 */ YY_NOT_ACCEPT,
    /* 115 */ YY_NO_ANCHOR,
    /* 116 */ YY_NOT_ACCEPT,
    /* 117 */ YY_NOT_ACCEPT,
    /* 118 */ YY_NOT_ACCEPT,
    /* 119 */ YY_NOT_ACCEPT,
    /* 120 */ YY_NOT_ACCEPT,
    /* 121 */ YY_NOT_ACCEPT,
    /* 122 */ YY_NOT_ACCEPT,
    /* 123 */ YY_NOT_ACCEPT,
    /* 124 */ YY_NOT_ACCEPT,
    /* 125 */ YY_NOT_ACCEPT,
    /* 126 */ YY_NOT_ACCEPT,
    /* 127 */ YY_NOT_ACCEPT,
    /* 128 */ YY_NOT_ACCEPT,
    /* 129 */ YY_NOT_ACCEPT,
    /* 130 */ YY_NOT_ACCEPT,
    /* 131 */ YY_NOT_ACCEPT,
    /* 132 */ YY_NOT_ACCEPT,
    /* 133 */ YY_NOT_ACCEPT,
    /* 134 */ YY_NOT_ACCEPT,
    /* 135 */ YY_NOT_ACCEPT,
    /* 136 */ YY_NOT_ACCEPT,
    /* 137 */ YY_NOT_ACCEPT,
    /* 138 */ YY_NOT_ACCEPT,
    /* 139 */ YY_NOT_ACCEPT,
    /* 140 */ YY_NOT_ACCEPT,
    /* 141 */ YY_NOT_ACCEPT,
    /* 142 */ YY_NOT_ACCEPT,
    /* 143 */ YY_NOT_ACCEPT,
    /* 144 */ YY_NOT_ACCEPT,
    /* 145 */ YY_NOT_ACCEPT,
    /* 146 */ YY_NOT_ACCEPT,
    /* 147 */ YY_NOT_ACCEPT,
    /* 148 */ YY_NOT_ACCEPT,
    /* 149 */ YY_NOT_ACCEPT,
    /* 150 */ YY_NOT_ACCEPT,
    /* 151 */ YY_NOT_ACCEPT,
    /* 152 */ YY_NOT_ACCEPT,
    /* 153 */ YY_NOT_ACCEPT,
    /* 154 */ YY_NOT_ACCEPT,
    /* 155 */ YY_NOT_ACCEPT,
    /* 156 */ YY_NOT_ACCEPT,
    /* 157 */ YY_NOT_ACCEPT,
    /* 158 */ YY_NOT_ACCEPT,
    /* 159 */ YY_NOT_ACCEPT,
    /* 160 */ YY_NOT_ACCEPT,
    /* 161 */ YY_NOT_ACCEPT,
    /* 162 */ YY_NOT_ACCEPT,
    /* 163 */ YY_NOT_ACCEPT,
    /* 164 */ YY_NOT_ACCEPT,
    /* 165 */ YY_NOT_ACCEPT,
    /* 166 */ YY_NOT_ACCEPT,
    /* 167 */ YY_NOT_ACCEPT,
    /* 168 */ YY_NOT_ACCEPT,
    /* 169 */ YY_NOT_ACCEPT,
    /* 170 */ YY_NOT_ACCEPT,
    /* 171 */ YY_NOT_ACCEPT,
    /* 172 */ YY_NOT_ACCEPT,
    /* 173 */ YY_NOT_ACCEPT,
    /* 174 */ YY_NOT_ACCEPT,
    /* 175 */ YY_NOT_ACCEPT,
    /* 176 */ YY_NOT_ACCEPT,
    /* 177 */ YY_NOT_ACCEPT,
    /* 178 */ YY_NOT_ACCEPT,
    /* 179 */ YY_NOT_ACCEPT,
    /* 180 */ YY_NOT_ACCEPT,
    /* 181 */ YY_NOT_ACCEPT,
    /* 182 */ YY_NOT_ACCEPT,
    /* 183 */ YY_NOT_ACCEPT,
    /* 184 */ YY_NOT_ACCEPT,
    /* 185 */ YY_NOT_ACCEPT,
    /* 186 */ YY_NOT_ACCEPT,
    /* 187 */ YY_NOT_ACCEPT,
    /* 188 */ YY_NOT_ACCEPT,
    /* 189 */ YY_NOT_ACCEPT,
    /* 190 */ YY_NOT_ACCEPT,
    /* 191 */ YY_NOT_ACCEPT,
    /* 192 */ YY_NOT_ACCEPT,
    /* 193 */ YY_NOT_ACCEPT,
    /* 194 */ YY_NOT_ACCEPT,
    /* 195 */ YY_NOT_ACCEPT,
    /* 196 */ YY_NOT_ACCEPT,
    /* 197 */ YY_NOT_ACCEPT,
    /* 198 */ YY_NOT_ACCEPT,
    /* 199 */ YY_NOT_ACCEPT,
    /* 200 */ YY_NOT_ACCEPT,
    /* 201 */ YY_NOT_ACCEPT,
    /* 202 */ YY_NOT_ACCEPT,
    /* 203 */ YY_NOT_ACCEPT,
    /* 204 */ YY_NOT_ACCEPT,
    /* 205 */ YY_NOT_ACCEPT,
    /* 206 */ YY_NOT_ACCEPT,
    /* 207 */ YY_NOT_ACCEPT,
    /* 208 */ YY_NOT_ACCEPT,
    /* 209 */ YY_NOT_ACCEPT,
    /* 210 */ YY_NOT_ACCEPT,
    /* 211 */ YY_NOT_ACCEPT,
    /* 212 */ YY_NOT_ACCEPT,
    /* 213 */ YY_NOT_ACCEPT,
    /* 214 */ YY_NOT_ACCEPT,
    /* 215 */ YY_NOT_ACCEPT,
    /* 216 */ YY_NOT_ACCEPT,
    /* 217 */ YY_NOT_ACCEPT,
    /* 218 */ YY_NOT_ACCEPT,
    /* 219 */ YY_NOT_ACCEPT,
    /* 220 */ YY_NOT_ACCEPT,
    /* 221 */ YY_NOT_ACCEPT,
    /* 222 */ YY_NOT_ACCEPT,
    /* 223 */ YY_NOT_ACCEPT,
    /* 224 */ YY_NOT_ACCEPT,
    /* 225 */ YY_NOT_ACCEPT,
    /* 226 */ YY_NOT_ACCEPT,
    /* 227 */ YY_NOT_ACCEPT,
    /* 228 */ YY_NOT_ACCEPT,
    /* 229 */ YY_NOT_ACCEPT,
    /* 230 */ YY_NOT_ACCEPT,
    /* 231 */ YY_NOT_ACCEPT,
    /* 232 */ YY_NOT_ACCEPT,
    /* 233 */ YY_NOT_ACCEPT,
    /* 234 */ YY_NOT_ACCEPT,
    /* 235 */ YY_NOT_ACCEPT,
    /* 236 */ YY_NOT_ACCEPT,
    /* 237 */ YY_NOT_ACCEPT,
    /* 238 */ YY_NOT_ACCEPT,
    /* 239 */ YY_NOT_ACCEPT,
    /* 240 */ YY_NOT_ACCEPT,
    /* 241 */ YY_NOT_ACCEPT,
    /* 242 */ YY_NOT_ACCEPT,
    /* 243 */ YY_NOT_ACCEPT,
    /* 244 */ YY_NOT_ACCEPT,
    /* 245 */ YY_NOT_ACCEPT,
    /* 246 */ YY_NOT_ACCEPT,
    /* 247 */ YY_NOT_ACCEPT,
    /* 248 */ YY_NOT_ACCEPT,
    /* 249 */ YY_NOT_ACCEPT,
    /* 250 */ YY_NOT_ACCEPT,
    /* 251 */ YY_NOT_ACCEPT,
    /* 252 */ YY_NOT_ACCEPT,
    /* 253 */ YY_NOT_ACCEPT,
    /* 254 */ YY_NOT_ACCEPT,
    /* 255 */ YY_NOT_ACCEPT,
    /* 256 */ YY_NOT_ACCEPT,
    /* 257 */ YY_NOT_ACCEPT,
    /* 258 */ YY_NOT_ACCEPT,
    /* 259 */ YY_NOT_ACCEPT,
    /* 260 */ YY_NOT_ACCEPT,
    /* 261 */ YY_NOT_ACCEPT,
    /* 262 */ YY_NOT_ACCEPT,
    /* 263 */ YY_NOT_ACCEPT,
    /* 264 */ YY_NOT_ACCEPT,
    /* 265 */ YY_NOT_ACCEPT,
    /* 266 */ YY_NOT_ACCEPT,
    /* 267 */ YY_NOT_ACCEPT,
    /* 268 */ YY_NOT_ACCEPT,
    /* 269 */ YY_NOT_ACCEPT,
    /* 270 */ YY_NOT_ACCEPT,
    /* 271 */ YY_NOT_ACCEPT,
    /* 272 */ YY_NOT_ACCEPT,
    /* 273 */ YY_NOT_ACCEPT,
    /* 274 */ YY_NOT_ACCEPT,
    /* 275 */ YY_NOT_ACCEPT,
    /* 276 */ YY_NOT_ACCEPT,
    /* 277 */ YY_NOT_ACCEPT,
    /* 278 */ YY_NOT_ACCEPT,
    /* 279 */ YY_NOT_ACCEPT,
    /* 280 */ YY_NOT_ACCEPT,
    /* 281 */ YY_NOT_ACCEPT,
    /* 282 */ YY_NOT_ACCEPT,
    /* 283 */ YY_NOT_ACCEPT,
    /* 284 */ YY_NOT_ACCEPT,
    /* 285 */ YY_NOT_ACCEPT,
    /* 286 */ YY_NOT_ACCEPT,
    /* 287 */ YY_NOT_ACCEPT,
    /* 288 */ YY_NOT_ACCEPT,
    /* 289 */ YY_NOT_ACCEPT,
    /* 290 */ YY_NOT_ACCEPT,
    /* 291 */ YY_NOT_ACCEPT,
    /* 292 */ YY_NOT_ACCEPT,
    /* 293 */ YY_NOT_ACCEPT,
    /* 294 */ YY_NOT_ACCEPT,
    /* 295 */ YY_NOT_ACCEPT,
    /* 296 */ YY_NOT_ACCEPT,
    /* 297 */ YY_NOT_ACCEPT,
    /* 298 */ YY_NOT_ACCEPT,
    /* 299 */ YY_NOT_ACCEPT,
    /* 300 */ YY_NOT_ACCEPT,
    /* 301 */ YY_NOT_ACCEPT,
    /* 302 */ YY_NOT_ACCEPT,
    /* 303 */ YY_NOT_ACCEPT,
    /* 304 */ YY_NOT_ACCEPT,
    /* 305 */ YY_NOT_ACCEPT,
    /* 306 */ YY_NOT_ACCEPT,
    /* 307 */ YY_NOT_ACCEPT,
    /* 308 */ YY_NOT_ACCEPT,
    /* 309 */ YY_NOT_ACCEPT,
    /* 310 */ YY_NOT_ACCEPT,
    /* 311 */ YY_NOT_ACCEPT,
    /* 312 */ YY_NOT_ACCEPT,
    /* 313 */ YY_NOT_ACCEPT,
    /* 314 */ YY_NOT_ACCEPT,
    /* 315 */ YY_NOT_ACCEPT,
    /* 316 */ YY_NOT_ACCEPT,
    /* 317 */ YY_NOT_ACCEPT,
    /* 318 */ YY_NOT_ACCEPT,
    /* 319 */ YY_NOT_ACCEPT,
    /* 320 */ YY_NOT_ACCEPT,
    /* 321 */ YY_NOT_ACCEPT,
    /* 322 */ YY_NOT_ACCEPT,
    /* 323 */ YY_NOT_ACCEPT,
    /* 324 */ YY_NOT_ACCEPT,
    /* 325 */ YY_NOT_ACCEPT,
    /* 326 */ YY_NOT_ACCEPT,
    /* 327 */ YY_NOT_ACCEPT,
    /* 328 */ YY_NOT_ACCEPT,
    /* 329 */ YY_NOT_ACCEPT,
    /* 330 */ YY_NOT_ACCEPT,
    /* 331 */ YY_NOT_ACCEPT,
    /* 332 */ YY_NOT_ACCEPT,
    /* 333 */ YY_NOT_ACCEPT,
    /* 334 */ YY_NOT_ACCEPT,
    /* 335 */ YY_NOT_ACCEPT,
    /* 336 */ YY_NOT_ACCEPT,
    /* 337 */ YY_NOT_ACCEPT,
    /* 338 */ YY_NOT_ACCEPT,
    /* 339 */ YY_NOT_ACCEPT,
    /* 340 */ YY_NOT_ACCEPT,
    /* 341 */ YY_NOT_ACCEPT,
    /* 342 */ YY_NOT_ACCEPT,
    /* 343 */ YY_NOT_ACCEPT,
    /* 344 */ YY_NOT_ACCEPT,
    /* 345 */ YY_NOT_ACCEPT,
    /* 346 */ YY_NOT_ACCEPT,
    /* 347 */ YY_NOT_ACCEPT,
    /* 348 */ YY_NOT_ACCEPT,
    /* 349 */ YY_NOT_ACCEPT,
    /* 350 */ YY_NOT_ACCEPT,
    /* 351 */ YY_NOT_ACCEPT,
    /* 352 */ YY_NOT_ACCEPT,
    /* 353 */ YY_NOT_ACCEPT,
    /* 354 */ YY_NOT_ACCEPT,
    /* 355 */ YY_NOT_ACCEPT,
    /* 356 */ YY_NOT_ACCEPT,
    /* 357 */ YY_NOT_ACCEPT,
    /* 358 */ YY_NOT_ACCEPT,
    /* 359 */ YY_NOT_ACCEPT,
    /* 360 */ YY_NOT_ACCEPT,
    /* 361 */ YY_NOT_ACCEPT,
    /* 362 */ YY_NOT_ACCEPT,
    /* 363 */ YY_NOT_ACCEPT,
    /* 364 */ YY_NOT_ACCEPT,
    /* 365 */ YY_NOT_ACCEPT,
    /* 366 */ YY_NOT_ACCEPT,
    /* 367 */ YY_NOT_ACCEPT,
    /* 368 */ YY_NOT_ACCEPT,
    /* 369 */ YY_NOT_ACCEPT,
    /* 370 */ YY_NOT_ACCEPT,
    /* 371 */ YY_NOT_ACCEPT,
    /* 372 */ YY_NOT_ACCEPT,
    /* 373 */ YY_NOT_ACCEPT,
    /* 374 */ YY_NOT_ACCEPT,
    /* 375 */ YY_NOT_ACCEPT,
    /* 376 */ YY_NOT_ACCEPT,
    /* 377 */ YY_NOT_ACCEPT,
    /* 378 */ YY_NOT_ACCEPT,
    /* 379 */ YY_NOT_ACCEPT,
    /* 380 */ YY_NOT_ACCEPT,
    /* 381 */ YY_NOT_ACCEPT,
    /* 382 */ YY_NOT_ACCEPT,
    /* 383 */ YY_NOT_ACCEPT,
    /* 384 */ YY_NOT_ACCEPT,
    /* 385 */ YY_NOT_ACCEPT,
    /* 386 */ YY_NOT_ACCEPT,
    /* 387 */ YY_NOT_ACCEPT,
    /* 388 */ YY_NOT_ACCEPT,
    /* 389 */ YY_NOT_ACCEPT,
    /* 390 */ YY_NOT_ACCEPT,
    /* 391 */ YY_NOT_ACCEPT,
    /* 392 */ YY_NOT_ACCEPT,
    /* 393 */ YY_NOT_ACCEPT,
    /* 394 */ YY_NOT_ACCEPT,
    /* 395 */ YY_NO_ANCHOR,
    /* 396 */ YY_NOT_ACCEPT,
    /* 397 */ YY_NOT_ACCEPT,
    /* 398 */ YY_NOT_ACCEPT,
    /* 399 */ YY_NO_ANCHOR,
    /* 400 */ YY_NOT_ACCEPT,
    /* 401 */ YY_NOT_ACCEPT,
    /* 402 */ YY_NOT_ACCEPT,
    /* 403 */ YY_NOT_ACCEPT,
    /* 404 */ YY_NOT_ACCEPT,
    /* 405 */ YY_NOT_ACCEPT,
    /* 406 */ YY_NOT_ACCEPT,
    /* 407 */ YY_NOT_ACCEPT,
    /* 408 */ YY_NOT_ACCEPT,
    /* 409 */ YY_NOT_ACCEPT,
    /* 410 */ YY_NOT_ACCEPT,
    /* 411 */ YY_NOT_ACCEPT,
    /* 412 */ YY_NOT_ACCEPT,
    /* 413 */ YY_NOT_ACCEPT,
    /* 414 */ YY_NOT_ACCEPT,
    /* 415 */ YY_NOT_ACCEPT,
    /* 416 */ YY_NOT_ACCEPT,
    /* 417 */ YY_NOT_ACCEPT,
    /* 418 */ YY_NOT_ACCEPT,
    /* 419 */ YY_NOT_ACCEPT,
    /* 420 */ YY_NOT_ACCEPT,
    /* 421 */ YY_NOT_ACCEPT,
    /* 422 */ YY_NOT_ACCEPT,
    /* 423 */ YY_NOT_ACCEPT,
    /* 424 */ YY_NOT_ACCEPT,
    /* 425 */ YY_NOT_ACCEPT,
    /* 426 */ YY_NOT_ACCEPT,
    /* 427 */ YY_NOT_ACCEPT,
    /* 428 */ YY_NOT_ACCEPT,
    /* 429 */ YY_NOT_ACCEPT,
    /* 430 */ YY_NOT_ACCEPT,
    /* 431 */ YY_NOT_ACCEPT,
    /* 432 */ YY_NOT_ACCEPT,
    /* 433 */ YY_NOT_ACCEPT,
    /* 434 */ YY_NOT_ACCEPT,
    /* 435 */ YY_NOT_ACCEPT,
    /* 436 */ YY_NOT_ACCEPT,
    /* 437 */ YY_NOT_ACCEPT,
    /* 438 */ YY_NOT_ACCEPT,
    /* 439 */ YY_NOT_ACCEPT,
    /* 440 */ YY_NOT_ACCEPT,
    /* 441 */ YY_NOT_ACCEPT,
    /* 442 */ YY_NOT_ACCEPT,
    /* 443 */ YY_NOT_ACCEPT,
    /* 444 */ YY_NOT_ACCEPT,
    /* 445 */ YY_NOT_ACCEPT,
    /* 446 */ YY_NOT_ACCEPT,
    /* 447 */ YY_NOT_ACCEPT,
    /* 448 */ YY_NOT_ACCEPT,
    /* 449 */ YY_NOT_ACCEPT,
    /* 450 */ YY_NOT_ACCEPT,
    /* 451 */ YY_NOT_ACCEPT,
    /* 452 */ YY_NOT_ACCEPT,
    /* 453 */ YY_NOT_ACCEPT,
    /* 454 */ YY_NOT_ACCEPT,
    /* 455 */ YY_NOT_ACCEPT,
    /* 456 */ YY_NOT_ACCEPT,
    /* 457 */ YY_NOT_ACCEPT,
    /* 458 */ YY_NOT_ACCEPT,
    /* 459 */ YY_NOT_ACCEPT,
    /* 460 */ YY_NOT_ACCEPT,
    /* 461 */ YY_NOT_ACCEPT,
    /* 462 */ YY_NOT_ACCEPT,
    /* 463 */ YY_NOT_ACCEPT,
    /* 464 */ YY_NOT_ACCEPT,
    /* 465 */ YY_NOT_ACCEPT,
    /* 466 */ YY_NOT_ACCEPT,
    /* 467 */ YY_NOT_ACCEPT,
    /* 468 */ YY_NOT_ACCEPT,
    /* 469 */ YY_NOT_ACCEPT,
    /* 470 */ YY_NOT_ACCEPT,
    /* 471 */ YY_NOT_ACCEPT,
    /* 472 */ YY_NOT_ACCEPT,
    /* 473 */ YY_NOT_ACCEPT,
    /* 474 */ YY_NOT_ACCEPT,
    /* 475 */ YY_NOT_ACCEPT,
    /* 476 */ YY_NOT_ACCEPT,
    /* 477 */ YY_NOT_ACCEPT,
    /* 478 */ YY_NOT_ACCEPT,
    /* 479 */ YY_NOT_ACCEPT,
    /* 480 */ YY_NOT_ACCEPT,
    /* 481 */ YY_NOT_ACCEPT,
    /* 482 */ YY_NOT_ACCEPT,
    /* 483 */ YY_NOT_ACCEPT,
    /* 484 */ YY_NOT_ACCEPT,
    /* 485 */ YY_NOT_ACCEPT,
    /* 486 */ YY_NOT_ACCEPT,
    /* 487 */ YY_NOT_ACCEPT,
    /* 488 */ YY_NOT_ACCEPT,
    /* 489 */ YY_NOT_ACCEPT,
    /* 490 */ YY_NOT_ACCEPT,
    /* 491 */ YY_NOT_ACCEPT,
    /* 492 */ YY_NOT_ACCEPT,
    /* 493 */ YY_NOT_ACCEPT,
    /* 494 */ YY_NOT_ACCEPT,
    /* 495 */ YY_NOT_ACCEPT,
    /* 496 */ YY_NOT_ACCEPT,
    /* 497 */ YY_NOT_ACCEPT,
    /* 498 */ YY_NOT_ACCEPT,
    /* 499 */ YY_NOT_ACCEPT,
    /* 500 */ YY_NOT_ACCEPT,
    /* 501 */ YY_NOT_ACCEPT,
    /* 502 */ YY_NOT_ACCEPT,
    /* 503 */ YY_NOT_ACCEPT,
    /* 504 */ YY_NOT_ACCEPT,
    /* 505 */ YY_NOT_ACCEPT,
    /* 506 */ YY_NOT_ACCEPT,
    /* 507 */ YY_NOT_ACCEPT,
    /* 508 */ YY_NOT_ACCEPT,
    /* 509 */ YY_NOT_ACCEPT,
    /* 510 */ YY_NOT_ACCEPT,
    /* 511 */ YY_NOT_ACCEPT,
    /* 512 */ YY_NOT_ACCEPT,
    /* 513 */ YY_NOT_ACCEPT,
    /* 514 */ YY_NOT_ACCEPT,
    /* 515 */ YY_NOT_ACCEPT,
    /* 516 */ YY_NOT_ACCEPT,
    /* 517 */ YY_NOT_ACCEPT,
    /* 518 */ YY_NOT_ACCEPT,
    /* 519 */ YY_NOT_ACCEPT,
    /* 520 */ YY_NOT_ACCEPT,
    /* 521 */ YY_NOT_ACCEPT,
    /* 522 */ YY_NOT_ACCEPT,
    /* 523 */ YY_NOT_ACCEPT,
    /* 524 */ YY_NOT_ACCEPT,
    /* 525 */ YY_NOT_ACCEPT,
    /* 526 */ YY_NOT_ACCEPT,
    /* 527 */ YY_NOT_ACCEPT,
    /* 528 */ YY_NOT_ACCEPT,
    /* 529 */ YY_NOT_ACCEPT,
    /* 530 */ YY_NOT_ACCEPT,
    /* 531 */ YY_NOT_ACCEPT,
    /* 532 */ YY_NOT_ACCEPT,
    /* 533 */ YY_NOT_ACCEPT,
    /* 534 */ YY_NOT_ACCEPT,
    /* 535 */ YY_NOT_ACCEPT,
    /* 536 */ YY_NOT_ACCEPT,
    /* 537 */ YY_NOT_ACCEPT,
    /* 538 */ YY_NOT_ACCEPT,
    /* 539 */ YY_NOT_ACCEPT,
    /* 540 */ YY_NOT_ACCEPT,
    /* 541 */ YY_NOT_ACCEPT,
    /* 542 */ YY_NOT_ACCEPT,
    /* 543 */ YY_NOT_ACCEPT,
    /* 544 */ YY_NOT_ACCEPT,
    /* 545 */ YY_NOT_ACCEPT,
    /* 546 */ YY_NOT_ACCEPT,
    /* 547 */ YY_NOT_ACCEPT,
    /* 548 */ YY_NOT_ACCEPT,
    /* 549 */ YY_NOT_ACCEPT,
    /* 550 */ YY_NOT_ACCEPT,
    /* 551 */ YY_NOT_ACCEPT,
    /* 552 */ YY_NOT_ACCEPT,
    /* 553 */ YY_NOT_ACCEPT,
    /* 554 */ YY_NOT_ACCEPT,
    /* 555 */ YY_NOT_ACCEPT,
    /* 556 */ YY_NOT_ACCEPT,
    /* 557 */ YY_NOT_ACCEPT,
    /* 558 */ YY_NOT_ACCEPT,
    /* 559 */ YY_NOT_ACCEPT,
    /* 560 */ YY_NOT_ACCEPT,
    /* 561 */ YY_NOT_ACCEPT,
    /* 562 */ YY_NOT_ACCEPT,
    /* 563 */ YY_NOT_ACCEPT,
    /* 564 */ YY_NOT_ACCEPT,
    /* 565 */ YY_NOT_ACCEPT,
    /* 566 */ YY_NOT_ACCEPT,
    /* 567 */ YY_NOT_ACCEPT,
    /* 568 */ YY_NOT_ACCEPT,
    /* 569 */ YY_NOT_ACCEPT,
    /* 570 */ YY_NOT_ACCEPT,
    /* 571 */ YY_NOT_ACCEPT,
    /* 572 */ YY_NOT_ACCEPT,
    /* 573 */ YY_NOT_ACCEPT,
    /* 574 */ YY_NOT_ACCEPT,
    /* 575 */ YY_NOT_ACCEPT,
    /* 576 */ YY_NOT_ACCEPT,
    /* 577 */ YY_NOT_ACCEPT,
    /* 578 */ YY_NOT_ACCEPT,
    /* 579 */ YY_NOT_ACCEPT,
    /* 580 */ YY_NOT_ACCEPT,
    /* 581 */ YY_NOT_ACCEPT,
    /* 582 */ YY_NOT_ACCEPT,
    /* 583 */ YY_NOT_ACCEPT,
    /* 584 */ YY_NOT_ACCEPT,
    /* 585 */ YY_NOT_ACCEPT,
    /* 586 */ YY_NOT_ACCEPT,
    /* 587 */ YY_NOT_ACCEPT,
    /* 588 */ YY_NOT_ACCEPT,
    /* 589 */ YY_NOT_ACCEPT,
    /* 590 */ YY_NOT_ACCEPT,
    /* 591 */ YY_NOT_ACCEPT,
    /* 592 */ YY_NOT_ACCEPT,
    /* 593 */ YY_NOT_ACCEPT,
    /* 594 */ YY_NOT_ACCEPT,
    /* 595 */ YY_NOT_ACCEPT,
    /* 596 */ YY_NOT_ACCEPT,
    /* 597 */ YY_NOT_ACCEPT,
    /* 598 */ YY_NOT_ACCEPT,
    /* 599 */ YY_NOT_ACCEPT,
    /* 600 */ YY_NOT_ACCEPT,
    /* 601 */ YY_NOT_ACCEPT,
    /* 602 */ YY_NOT_ACCEPT,
    /* 603 */ YY_NOT_ACCEPT,
    /* 604 */ YY_NOT_ACCEPT,
    /* 605 */ YY_NOT_ACCEPT,
    /* 606 */ YY_NOT_ACCEPT,
    /* 607 */ YY_NOT_ACCEPT,
    /* 608 */ YY_NOT_ACCEPT,
    /* 609 */ YY_NOT_ACCEPT,
    /* 610 */ YY_NOT_ACCEPT,
    /* 611 */ YY_NOT_ACCEPT,
    /* 612 */ YY_NOT_ACCEPT,
    /* 613 */ YY_NOT_ACCEPT,
    /* 614 */ YY_NOT_ACCEPT,
    /* 615 */ YY_NOT_ACCEPT,
    /* 616 */ YY_NOT_ACCEPT,
    /* 617 */ YY_NOT_ACCEPT,
    /* 618 */ YY_NOT_ACCEPT,
    /* 619 */ YY_NOT_ACCEPT,
    /* 620 */ YY_NOT_ACCEPT,
    /* 621 */ YY_NOT_ACCEPT,
    /* 622 */ YY_NOT_ACCEPT,
    /* 623 */ YY_NOT_ACCEPT,
    /* 624 */ YY_NOT_ACCEPT,
    /* 625 */ YY_NOT_ACCEPT,
    /* 626 */ YY_NOT_ACCEPT
  };
  private int yy_cmap[] = unpackFromString(1,130,
"55:9,56,57,55,56,57,55:18,56,55:9,58,55:2,51,53,54,49,48,45,28,52:2,46,52,4" +
"7,52,55:7,35,33,1,39,18,17,42,41,31,55,37,20,16,10,15,19,55,24,29,30,55,22," +
"55:2,43,55:5,44,55,7,13,38,21,14,9,26,36,8,55,50,23,12,3,2,25,55,6,4,5,11,4" +
"0,27,34,32,55:6,0:2")[0];

  private int yy_rmap[] = unpackFromString(1,627,
"0,1,2,3,4,1,5,6,1:6,7,1:4,8,9,10,1,11,1:27,12,1:18,13,1,14,15,16,17,18,19,2" +
"0,1,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44" +
",45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,54,60,61,62,63,64,65,66,67,68" +
",69,70,71,16,47,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91" +
",92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,11" +
"2,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,1" +
"31,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149," +
"150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168" +
",169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,18" +
"7,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,2" +
"06,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224," +
"225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243" +
",244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,26" +
"2,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,2" +
"81,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299," +
"300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318" +
",319,320,321,322,323,324,325,326,327,328,329,13,17,330,331,332,333,334,335," +
"336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354" +
",355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,37" +
"3,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,3" +
"92,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,409,410," +
"411,412,413,414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429" +
",430,431,432,433,434,435,436,437,438,439,440,441,442,443,444,445,446,447,44" +
"8,449,450,451,452,453,454,455,456,457,458,459,460,461,462,463,464,465,466,4" +
"67,468,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485," +
"486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,501,502,503,504" +
",505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,520,521,522,52" +
"3,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,5" +
"42,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560," +
"561,562")[0];

  private int yy_nxt[][] = unpackFromString(563,59,
"1,2,73,77,79:3,81,79:2,83,79:4,85,79,87,89,91,93,79,95,79,97,79:3,3,99,101," +
"103,79,395,79,105,79:3,399,79,107,109,79:2,3:5,79,111,3,113,115,79,4:2,79,-" +
"1:61,72,-1:70,106,-1:13,3,-1:16,3:5,-1:2,3,6,-1:61,4:2,-1:15,133,-1:13,6,-1" +
":16,6:5,-1:2,6,-1:7,7:56,-1,7,-1:17,419,20,-1:48,544,-1:12,159,-1:16,160,-1" +
":37,165,-1:76,427,-1,425,-1:38,184,-1:58,319,-1:42,392:57,393,-1:3,401,-1:5" +
"8,5,-1:5,76,-1:77,74,-1:16,74:5,-1:2,74,-1:7,394:53,71,394:3,393,-1:9,9,-1:" +
"53,396,-1:7,490,-1:48,122,-1:68,125,-1:50,78,-1:7,397,-1:76,10,-1:30,525,-1" +
":8,80,-1:7,82,-1:2,84,-1:54,11,-1:11,12,-1:39,86,-1:52,400,-1:61,541,-1:9,8" +
"8,-1:12,90,-1:46,13,-1:51,92,-1:14,94,-1:6,580,-1:34,126,-1:54,96,-1:21,98," +
"-1:38,127,-1:57,579,-1:58,128,-1:65,100,-1:58,493,-1:51,102,-1:6,104,-1:60," +
"129,-1:45,398,-1,108,-1:13,110,-1:7,581,-1:8,491,-1:28,407,-1:16,130,-1:37," +
"112,-1:28,114,-1:30,402,-1,406,-1:56,551,-1:59,131,-1:64,117,-1:76,74,-1:16" +
",74:5,-1,132,74,-1:13,403,-1:72,134,-1:61,404,-1:35,14,-1:85,3,-1:16,3:5,-1" +
":2,3,119,-1:11,136,-1:80,6,-1:16,6:5,-1:2,6,-1:25,137,-1:93,7,-1:3,8,-1:11," +
"139,-1:82,140,-1:32,141,-1:58,15,-1:58,16,-1:58,17,-1:58,18,-1:82,408,-1:38" +
",582,-1:76,145,-1:32,146,-1:76,147,-1:53,19,-1:42,149,-1:81,152,-1:34,418,-" +
"1:107,496,-1:12,412,-1:82,153,-1:44,497,-1:47,416,-1:75,154,-1:46,155,-1:62" +
",498,-1:85,156,-1:25,414,-1:82,415,-1:46,499,-1:61,21,-1:58,161,-1:75,423,-" +
"1:22,163,-1:79,421,-1:42,22,-1:70,23,-1:82,504,-1:18,583,-1:86,168,-1:41,17" +
"2,-1:55,175,-1:55,176,-1:86,505,-1:29,24,-1:56,25,-1:65,178,-1:48,179,-1:79" +
",180,-1:65,182,-1:31,439,-1:73,436,-1:47,509,-1:72,432,-1:52,434,-1:67,188," +
"-1:78,189,-1:28,433,-1:50,191,-1:61,506,-1:54,193,-1:63,194,-1:73,507,-1:35" +
",531,-1:76,532,-1:18,443,-1:21,442,-1:59,26,-1:78,585,-1:43,440,-1:81,197,-" +
"1:39,533,-1:53,446,-1:96,203,447,512,204,-1:33,534,-1:49,445,-1:50,511,-1:8" +
"4,209,-1:37,210,-1:52,548,-1:79,215,-1:33,216,-1:95,27,-1:50,28,-1:31,218,-" +
"1:76,29,-1:38,449,-1:13,219,-1:8,220,-1:4,448,-1,221,-1:46,224,-1:84,227,-1" +
":3,455,-1:45,229,-1:56,30,-1:53,556,-1:44,456,-1:67,535,-1:46,232,-1:69,549" +
",-1:67,31,-1:53,557,-1:45,515,-1:77,235,-1:60,236,-1:27,561,-1:59,237,-1:64" +
",239,-1:54,457,-1:54,454,-1:84,516,-1:55,241,-1:62,32,-1:58,33,-1:58,34,-1:" +
"47,242,-1:71,244,-1:33,35,-1:57,459,-1:54,514,-1:69,518,-1:58,36,-1:77,37,-" +
"1:28,252,-1:76,38,-1:42,39,-1:70,253,-1:47,537,-1:60,255,-1:65,257,-1:74,40" +
",-1:58,41,-1:58,258,-1:49,260,-1:72,519,-1:27,261,-1:60,587,-1:59,42,-1:58," +
"536,-1:83,43,-1:60,44,-1:25,586,-1:62,264,-1:103,464,-1:22,563,-1:45,265,-1" +
":65,266,-1:88,268,-1:34,269,-1:51,270,-1:71,274,-1:44,275,-1:75,469,-1:46,2" +
"78,-1:59,521,-1:50,279,-1:62,558,-1:56,45,-1:64,565,-1:75,539,-1:31,281,-1:" +
"94,282,-1:34,283,-1:74,472,-1:59,471,-1:58,560,-1:32,285,-1:64,287,-1:55,29" +
"0,-1:71,591,-1:51,46,-1:84,474,-1:22,293,-1:56,530,-1:85,296,-1:35,297,-1:7" +
"5,298,-1:61,47,-1:58,562,-1:46,299,-1:58,48,-1:49,301,-1:65,302,-1:65,303,-" +
"1:44,588,-1:67,49,-1:63,478,-1:10,304,-1:49,50,-1:60,51,-1:41,309,-1:66,310" +
",-1:52,312,-1:57,480,-1:63,315,-1:65,316,-1:10,317,-1:43,318,-1:52,52,-1:56" +
",589,-1:80,605,-1:35,481,-1:90,482,-1:24,53,-1:58,322,-1:68,566,-1:58,324,-" +
"1:51,483,-1:81,325,-1:30,524,-1:61,326,-1:69,328,-1:47,595,-1:79,329,-1:38," +
"331,-1:81,54,-1:37,598,-1:79,485,-1:31,335,-1:70,336,-1:49,55,-1:61,337,-1:" +
"76,338,-1:49,339,-1:46,340,-1:74,344,-1:42,56,-1:55,604,-1:70,346,-1:46,347" +
",-1:60,57,-1:58,58,-1:58,59,-1:92,601,-1:31,349,-1:63,350,-1:14,575,-1:32,3" +
"51,-1:59,352,-1:56,60,-1:77,354,-1:42,355,-1:81,357,-1:66,358,-1:24,61,-1:5" +
"6,62,-1:60,360,-1:62,361,-1:64,590,-1:58,363,-1:65,364,-1:41,63,-1:59,365,-" +
"1:68,366,-1:44,368,-1:61,64,-1:62,488,-1:80,369,-1:32,65,-1:61,371,-1:80,66" +
",-1:43,373,-1:49,374,-1:76,375,-1:41,376,-1:66,377,-1:66,378,-1:43,379,-1:7" +
"7,380,-1:46,381,-1:49,382,-1:55,383,-1:79,384,-1:46,489,-1:53,385,-1:54,386" +
",-1:88,67,-1:64,388,-1:34,68,-1:52,389,-1:55,390,-1:85,69,-1:26,1,70:56,392" +
",75,-1,394:57,393,-1:2,116,-1:59,120,-1:57,123,-1:63,409,-1:65,118,-1:58,14" +
"4,-1:48,526,-1:75,150,-1:43,142,-1:87,143,-1:36,148,-1:50,151,-1:76,411,-1:" +
"37,420,-1:81,495,-1:37,158,-1:68,162,-1:52,426,-1:56,528,-1:82,529,-1:46,17" +
"3,-1:61,500,-1:75,501,-1:22,422,-1:79,164,-1:38,170,-1:69,584,-1:55,503,-1:" +
"55,177,-1:86,502,-1:34,441,-1:48,183,-1:79,437,-1:65,185,-1:31,196,-1:62,19" +
"0,-1:75,510,-1:48,201,-1:50,444,-1:61,202,-1:54,207,-1:63,199,-1:73,205,-1:" +
"35,211,-1:64,198,-1:62,452,-1:53,206,-1:74,214,-1:49,451,-1:70,228,-1:33,23" +
"1,-1:60,223,-1:74,225,-1:75,240,-1:37,238,-1:55,233,-1:69,453,-1:49,623,-1:" +
"49,249,-1:56,626,-1:74,243,-1:45,245,-1:57,254,-1:60,259,-1:65,460,-1:48,26" +
"7,-1:60,463,-1:57,465,-1:61,272,-1:64,276,-1:50,467,-1:56,280,-1:56,288,-1:" +
"94,292,-1:34,470,-1:75,475,-1:32,522,-1:61,294,-1:73,540,-1:49,300,-1:49,47" +
"7,-1:59,479,-1:66,311,-1:52,314,-1:57,320,-1:57,323,-1:59,613,-1:53,327,-1:" +
"62,332,-1:81,486,-1:43,343,-1:58,608,-1:65,367,-1:44,372,-1:57,387,-1:55,12" +
"1,-1:62,135,-1:65,413,-1:48,410,-1:59,417,-1:57,424,-1:68,166,-1:50,428,-1:" +
"54,167,-1:59,174,-1:63,429,-1:86,181,-1:24,195,-1:79,508,-1:65,186,-1:35,19" +
"2,-1:60,208,-1:74,212,-1:35,450,-1:64,200,-1:64,217,-1:70,513,-1:51,226,-1:" +
"53,246,-1:47,462,-1:61,250,-1:57,256,-1:60,262,-1:65,538,-1:48,466,-1:60,61" +
"5,-1:66,286,-1:58,476,-1:46,484,-1:62,333,-1:55,124,-1:60,527,-1:59,169,-1:" +
"55,430,-1:63,187,-1:59,552,-1:74,547,-1:35,213,-1:70,222,-1:63,230,-1:53,24" +
"7,-1:47,273,-1:69,263,-1:50,271,-1:66,291,-1:58,306,-1:47,594,-1:60,157,-1:" +
"59,171,-1:55,545,-1:63,435,-1:59,308,-1:74,234,-1:52,554,-1:53,248,-1:47,46" +
"8,-1:58,138,-1:64,313,-1:74,559,-1:47,461,-1:47,284,-1:80,458,-1:47,520,-1:" +
"47,289,-1:80,251,-1:36,295,-1:80,517,-1:36,305,-1:80,277,-1:36,307,-1:80,47" +
"3,-1:36,330,-1:80,321,-1:36,334,-1:80,341,-1:36,345,-1:80,342,-1:36,348,-1:" +
"80,370,-1:36,353,-1:58,356,-1:58,359,-1:58,362,-1:58,487,-1:67,405,-1:51,49" +
"2,-1:61,494,-1:64,543,-1:58,431,-1:74,438,-1:30,553,-1:58,555,-1:63,550,-1:" +
"65,567,-1:52,568,-1:62,573,-1:77,564,-1:29,546,-1:94,523,-1:28,542,-1:52,57" +
"0,-1:70,569,-1:75,577,-1:29,572,-1:70,571,-1:46,574,-1:58,576,-1:58,578,-1:" +
"61,592,-1:74,597,-1:42,596,-1:61,593,-1:58,600,-1:88,602,-1:25,599,-1:64,60" +
"3,-1:66,606,-1:68,609,-1:34,607,-1:56,611,-1:56,610,-1:88,614,-1:41,612,-1:" +
"69,616,-1:47,618,-1:56,617,-1:54,620,-1:55,619,-1:82,622,-1:59,621,-1:34,62" +
"4,-1:66,625,-1:44");

  public Token getToken ()
    throws java.io.IOException, 
LexicalError

    {
    int yy_lookahead;
    int yy_anchor = YY_NO_ANCHOR;
    int yy_state = yy_state_dtrans[yy_lexical_state];
    int yy_next_state = YY_NO_STATE;
    int yy_last_accept_state = YY_NO_STATE;
    boolean yy_initial = true;
    int yy_this_accept;

    yy_mark_start();
    yy_this_accept = yy_acpt[yy_state];
    if (YY_NOT_ACCEPT != yy_this_accept) {
      yy_last_accept_state = yy_state;
      yy_mark_end();
    }
    while (true) {
      if (yy_initial && yy_at_bol) yy_lookahead = YY_BOL;
      else yy_lookahead = yy_advance();
      yy_next_state = YY_F;
      yy_next_state = yy_nxt[yy_rmap[yy_state]][yy_cmap[yy_lookahead]];
      if (YY_EOF == yy_lookahead && true == yy_initial) {

    return t(Constant.EOF, "EOF");
    //return Constant.EOF;
      }
      if (YY_F != yy_next_state) {
        yy_state = yy_next_state;
        yy_initial = false;
        yy_this_accept = yy_acpt[yy_state];
        if (YY_NOT_ACCEPT != yy_this_accept) {
          yy_last_accept_state = yy_state;
          yy_mark_end();
        }
      }
      else {
        if (YY_NO_STATE == yy_last_accept_state) {
          throw (new Error("Lexical Error: Unmatched Input."));
        }
        else {
          yy_anchor = yy_acpt[yy_last_accept_state];
          if (0 != (YY_END & yy_anchor)) {
            yy_move_end();
          }
          yy_to_mark();
          switch (yy_last_accept_state) {
          case 1:
            
          case -2:
            break;
          case 2:
            { 
error("Invalid character : " + yytext());
}
          case -3:
            break;
          case 3:
            { return t (Constant.INT_VAL, yytext()); }
          case -4:
            break;
          case 4:
            {}
          case -5:
            break;
          case 5:
            { return t (Constant.ON); }
          case -6:
            break;
          case 6:
            { return t (Constant.DOUBLE_VAL, yytext()); }
          case -7:
            break;
          case 7:
            {}
          case -8:
            break;
          case 8:
            { yybegin (COMMENT); }
          case -9:
            break;
          case 9:
            { return t (Constant.OFF); }
          case -10:
            break;
          case 10:
            { return t (Constant.NPT); }
          case -11:
            break;
          case 11:
            { return t (Constant.NVE); }
          case -12:
            break;
          case 12:
            { return t (Constant.NVT); }
          case -13:
            break;
          case 13:
            { return t (Constant.FEP); }
          case -14:
            break;
          case 14:
            { return t (Constant.SPC); }
          case -15:
            break;
          case 15:
            { return t (Constant.ALL_SITES_IN_THEIR_OWN_FRAGMENT); }
          case -16:
            break;
          case 16:
            { return t (Constant.ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT); }
          case -17:
            break;
          case 17:
            { return t (Constant.ALL_SITES_IN_ONE_FRAGMENT); }
          case -18:
            break;
          case 18:
            { return t (Constant.ALL_MOLECULES_IN_ONE_FRAGMENT); }
          case -19:
            break;
          case 19:
            { return t (Constant.P3ME); }
          case -20:
            break;
          case 20:
            { return t (Constant.SPCE); }
          case -21:
            break;
          case 21:
            { return t (Constant.EWALD); }
          case -22:
            break;
          case 22:
            { return t (Constant.RESPA); }
          case -23:
            break;
          case 23:
            { return t (Constant.TIP3P); }
          case -24:
            break;
          case 24:
            { return t (Constant.LAMBDA); }
          case -25:
            break;
          case 25:
            { return t (Constant.VERLET); }
          case -26:
            break;
          case 26:
            { return t (Constant.TORSION); }
          case -27:
            break;
          case 27:
            { return t (Constant.HARMONIC); }
          case -28:
            break;
          case 28:
            { return t (Constant.GRADIENT); }
          case -29:
            break;
          case 29:
            { return t (Constant.NONBONDED); }
          case -30:
            break;
          case 30:
            { return t (Constant.EWALDKMAX); }
          case -31:
            break;
          case 31:
            { return t (Constant.SPCFLOPPY); }
          case -32:
            break;
          case 32:
            { return t (Constant.FINITE_2PT); }
          case -33:
            break;
          case 33:
            { return t (Constant.FINITE_4PT); }
          case -34:
            break;
          case 34:
            { return t (Constant.FINITE_8PT); }
          case -35:
            break;
          case 35:
            { return t (Constant.EWALDALPHA); }
          case -36:
            break;
          case 36:
            { return t (Constant.STDCOULOMB); }
          case -37:
            break;
          case 37:
            { return t (Constant.SPCEFLOPPY); }
          case -38:
            break;
          case 38:
            { return t (Constant.ANALYTICAL); }
          case -39:
            break;
          case 39:
            { return t (Constant.CONSTRAINTS); }
          case -40:
            break;
          case 40:
            { return t (Constant.FINITE_12PT); }
          case -41:
            break;
          case 41:
            { return t (Constant.FINITE_10PT); }
          case -42:
            break;
          case 42:
            { return t (Constant.SWITCHDELTA); }
          case -43:
            break;
          case 43:
            { return t (Constant.TIP3PFLOPPY); }
          case -44:
            break;
          case 44:
            { return t (Constant.BOUNDINGBOX); }
          case -45:
            break;
          case 45:
            { return t (Constant.P3MEDIFFORDER); }
          case -46:
            break;
          case 46:
            { return t (Constant.SHAKETOLERANCE); }
          case -47:
            break;
          case 47:
            { return t (Constant.P3MEINITSPACING); }
          case -48:
            break;
          case 48:
            { return t (Constant.RATTLETOLERANCE); }
          case -49:
            break;
          case 49:
            { return t (Constant.EXTERNALPRESSURE); }
          case -50:
            break;
          case 50:
            { return t (Constant.RANDOMNUMBERSEED); }
          case -51:
            break;
          case 51:
            { return t (Constant.DEFINERESPALEVEL); }
          case -52:
            break;
          case 52:
            { return t (Constant.SWITCHLOWERCUTOFF); }
          case -53:
            break;
          case 53:
            { return t (Constant.FINITERANGECUTOFFS); }
          case -54:
            break;
          case 54:
            { return t (Constant.SNAPSHOTPERIODINOTS); }
          case -55:
            break;
          case 55:
            { return t (Constant.P3MECHARGEASSIGNMENT); }
          case -56:
            break;
          case 56:
            { return t (Constant.PRESSURECONTROLTARGET); }
          case -57:
            break;
          case 57:
            { return t (Constant.NUMBEROFCOMCONSTRAINTS); }
          case -58:
            break;
          case 58:
            { return t (Constant.NUMBEROFOUTERTIMESTEPS); }
          case -59:
            break;
          case 59:
            { return t (Constant.NUMBEROFINNERTIMESTEPS); }
          case -60:
            break;
          case 60:
            { return t (Constant.NUMBEROFSHAKEITERATIONS); }
          case -61:
            break;
          case 61:
            { return t (Constant.NUMBEROFRATTLEITERATIONS); }
          case -62:
            break;
          case 62:
            { return t (Constant.EMITENERGYTIMESTEPMODULO); }
          case -63:
            break;
          case 63:
            { return t (Constant.PRESSURECONTROLPISTONMASS); }
          case -64:
            break;
          case 64:
            { return t (Constant.OUTERTIMESTEPINPICOSECONDS); }
          case -65:
            break;
          case 65:
            { return t (Constant.INNERTIMESTEPINPICOSECONDS); }
          case -66:
            break;
          case 66:
            { return t (Constant.VELOCITYRESAMPLEPERIODINOTS); }
          case -67:
            break;
          case 67:
            { return t (Constant.DEFINERESPALEVELFORFORCECATEGORY); }
          case -68:
            break;
          case 68:
            { return t (Constant.VELOCITYRESAMPLETARGETTEMPERATURE); }
          case -69:
            break;
          case 69:
            { return t (Constant.PRESSURECONTROLPISTONINITIALVELOCITY); }
          case -70:
            break;
          case 70:
            {
error("Illegal comment");
}
          case -71:
            break;
          case 71:
            {yybegin (YYINITIAL);}
          case -72:
            break;
          case 73:
            { 
error("Invalid character : " + yytext());
}
          case -73:
            break;
          case 74:
            { return t (Constant.DOUBLE_VAL, yytext()); }
          case -74:
            break;
          case 75:
            {
error("Illegal comment");
}
          case -75:
            break;
          case 77:
            { 
error("Invalid character : " + yytext());
}
          case -76:
            break;
          case 79:
            { 
error("Invalid character : " + yytext());
}
          case -77:
            break;
          case 81:
            { 
error("Invalid character : " + yytext());
}
          case -78:
            break;
          case 83:
            { 
error("Invalid character : " + yytext());
}
          case -79:
            break;
          case 85:
            { 
error("Invalid character : " + yytext());
}
          case -80:
            break;
          case 87:
            { 
error("Invalid character : " + yytext());
}
          case -81:
            break;
          case 89:
            { 
error("Invalid character : " + yytext());
}
          case -82:
            break;
          case 91:
            { 
error("Invalid character : " + yytext());
}
          case -83:
            break;
          case 93:
            { 
error("Invalid character : " + yytext());
}
          case -84:
            break;
          case 95:
            { 
error("Invalid character : " + yytext());
}
          case -85:
            break;
          case 97:
            { 
error("Invalid character : " + yytext());
}
          case -86:
            break;
          case 99:
            { 
error("Invalid character : " + yytext());
}
          case -87:
            break;
          case 101:
            { 
error("Invalid character : " + yytext());
}
          case -88:
            break;
          case 103:
            { 
error("Invalid character : " + yytext());
}
          case -89:
            break;
          case 105:
            { 
error("Invalid character : " + yytext());
}
          case -90:
            break;
          case 107:
            { 
error("Invalid character : " + yytext());
}
          case -91:
            break;
          case 109:
            { 
error("Invalid character : " + yytext());
}
          case -92:
            break;
          case 111:
            { 
error("Invalid character : " + yytext());
}
          case -93:
            break;
          case 113:
            { 
error("Invalid character : " + yytext());
}
          case -94:
            break;
          case 115:
            { 
error("Invalid character : " + yytext());
}
          case -95:
            break;
          case 395:
            { 
error("Invalid character : " + yytext());
}
          case -96:
            break;
          case 399:
            { 
error("Invalid character : " + yytext());
}
          case -97:
            break;
          default:
            yy_error(YY_E_INTERNAL,false);
          case -1:
          }
          yy_initial = true;
          yy_state = yy_state_dtrans[yy_lexical_state];
          yy_next_state = YY_NO_STATE;
          yy_last_accept_state = YY_NO_STATE;
          yy_mark_start();
          yy_this_accept = yy_acpt[yy_state];
          if (YY_NOT_ACCEPT != yy_this_accept) {
            yy_last_accept_state = yy_state;
            yy_mark_end();
          }
        }
      }
    }
  }
}
