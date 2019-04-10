package com.ibm.bluematter.db2probspec.lexer;

import java.io.InputStream;
import com.ibm.bluematter.db2probspec.parser.Constant;

%%

%init{
    lineSeparator = System.getProperty("line.separator", "\n");
%init}

%{

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

%}

%eofval{
    return t(Constant.EOF, "EOF");
    //return Constant.EOF;
%eofval}

%yylexthrow{
LexicalError
%yylexthrow}

%class Lexer
%type Token
%function getToken
%public
%char
%line
%state COMMENT


DIGIT=[0-9]
INTEGER=(("-")?{DIGIT}+)

FPNUM=(({DIGIT}*"."{DIGIT}+)|({DIGIT}+"."{DIGIT}*))

DOUBLE=((("-")?{FPNUM}("e"{INTEGER})?)|({INTEGER}("e"{INTEGER})))

int_value = 0|[1-9][0-9]*
double_value = int_value"."int_value

letter = [A-Za-z]
white_space_char = [\ \t\n\r\f]
whitespace = {white_space_char}+
slashcomment = //.*
blockcomment=[^"*"]*"*"("*"|[^/"*"][^"*"]*"*")*/

%%

<YYINITIAL> "Constraints"                    { return t (Constant.CONSTRAINTS); }
<YYINITIAL> "on"                             { return t (Constant.ON); }
<YYINITIAL> "off"                            { return t (Constant.OFF); }
<YYINITIAL> "NumberOfCOMConstraints"         { return t (Constant.NUMBEROFCOMCONSTRAINTS); }

<YYINITIAL> "FEP"                            { return t (Constant.FEP); }
<YYINITIAL> "Lambda"                         { return t (Constant.LAMBDA); }

<YYINITIAL> "Verlet"                         { return t (Constant.VERLET); }
<YYINITIAL> "Respa"                          { return t (Constant.RESPA); }

<YYINITIAL> "FiniteRangeCutoffs"             { return t (Constant.FINITERANGECUTOFFS); }
<YYINITIAL> "Ewald"                          { return t (Constant.EWALD); }
<YYINITIAL> "P3ME"                           { return t (Constant.P3ME); }
<YYINITIAL> "StdCoulomb"                     { return t (Constant.STDCOULOMB); }

<YYINITIAL> "NVT"                            { return t (Constant.NVT); }
<YYINITIAL> "NPT"                            { return t (Constant.NPT); }
<YYINITIAL> "NVE"                            { return t (Constant.NVE); }

<YYINITIAL> "TIP3P"                          { return t (Constant.TIP3P); }
<YYINITIAL> "SPC"                            { return t (Constant.SPC); }
<YYINITIAL> "SPCE"                           { return t (Constant.SPCE); }
<YYINITIAL> "TIP3PFloppy"                     { return t (Constant.TIP3PFLOPPY); }
<YYINITIAL> "SPCFloppy"                      { return t (Constant.SPCFLOPPY); }
<YYINITIAL> "SPCEFloppy"                     { return t (Constant.SPCEFLOPPY); }

<YYINITIAL> "BoundingBox"                    { return t (Constant.BOUNDINGBOX); }
<YYINITIAL> "EwaldAlpha"                     { return t (Constant.EWALDALPHA); }
<YYINITIAL> "EwaldKmax"                      { return t (Constant.EWALDKMAX); }

<YYINITIAL> "InnerTimeStepInPicoSeconds"    { return t (Constant.INNERTIMESTEPINPICOSECONDS); }
<YYINITIAL> "NumberOfInnerTimeSteps"         { return t (Constant.NUMBEROFINNERTIMESTEPS); }
<YYINITIAL> "NumberOfOuterTimeSteps"         { return t (Constant.NUMBEROFOUTERTIMESTEPS); }
<YYINITIAL> "OuterTimeStepInPicoSeconds"     { return t (Constant.OUTERTIMESTEPINPICOSECONDS); }
<YYINITIAL> "DefineRespaLevel"               { return t (Constant.DEFINERESPALEVEL); }
<YYINITIAL> "DefineRespaLevelForForceCategory"     { return t (Constant.DEFINERESPALEVELFORFORCECATEGORY); }
<YYINITIAL> "Harmonic"                             { return t (Constant.HARMONIC); }
<YYINITIAL> "Torsion"                              { return t (Constant.TORSION); }
<YYINITIAL> "NonBonded"                              { return t (Constant.NONBONDED); }

<YYINITIAL> "P3MEinitSpacing"                      { return t (Constant.P3MEINITSPACING); }
<YYINITIAL> "P3MEchargeAssignment"                 { return t (Constant.P3MECHARGEASSIGNMENT); }
<YYINITIAL> "P3MEdiffOrder"                        { return t (Constant.P3MEDIFFORDER); }
<YYINITIAL> "GRADIENT"                             { return t (Constant.GRADIENT); }
<YYINITIAL> "ANALYTICAL"                           { return t (Constant.ANALYTICAL); }
<YYINITIAL> "FINITE_2PT"                           { return t (Constant.FINITE_2PT); }
<YYINITIAL> "FINITE_6PT"                           { return t (Constant.FINITE_4PT); }
<YYINITIAL> "FINITE_8PT"                           { return t (Constant.FINITE_8PT); }
<YYINITIAL> "FINITE_10PT"                          { return t (Constant.FINITE_10PT); }
<YYINITIAL> "FINITE_12PT"                          { return t (Constant.FINITE_12PT); }

<YYINITIAL> "ShakeTolerance"                       { return t (Constant.SHAKETOLERANCE); }
<YYINITIAL> "RattleTolerance"                      { return t (Constant.RATTLETOLERANCE); }
<YYINITIAL> "NumberOfShakeIterations"              { return t (Constant.NUMBEROFSHAKEITERATIONS); }
<YYINITIAL> "NumberOfRattleIterations"             { return t (Constant.NUMBEROFRATTLEITERATIONS); }

<YYINITIAL> "RandomNumberSeed"                     { return t (Constant.RANDOMNUMBERSEED); }
<YYINITIAL> "SnapshotPeriodInOTS"                  { return t (Constant.SNAPSHOTPERIODINOTS); }
<YYINITIAL> "EmitEnergyTimeStepModulo"             { return t (Constant.EMITENERGYTIMESTEPMODULO); }

<YYINITIAL> "ExternalPressure"                     { return t (Constant.EXTERNALPRESSURE); }
<YYINITIAL> "PressureControlPistonMass"            { return t (Constant.PRESSURECONTROLPISTONMASS); }
<YYINITIAL> "PressureControlTarget"                { return t (Constant.PRESSURECONTROLTARGET); }
<YYINITIAL> "PressureControlPistonInitialVelocity" { return t (Constant.PRESSURECONTROLPISTONINITIALVELOCITY); }
<YYINITIAL> "VelocityResamplePeriodInOTS"          { return t (Constant.VELOCITYRESAMPLEPERIODINOTS); }
<YYINITIAL> "VelocityResampleTargetTemperature"    { return t (Constant.VELOCITYRESAMPLETARGETTEMPERATURE); }
<YYINITIAL> "SwitchLowerCutoff"                    { return t (Constant.SWITCHLOWERCUTOFF); }
<YYINITIAL> "SwitchDelta"                          { return t (Constant.SWITCHDELTA); }

<YYINITIAL> "asof"                                 { return t (Constant.ALL_SITES_IN_ONE_FRAGMENT); }
<YYINITIAL> "amof"                                 { return t (Constant.ALL_MOLECULES_IN_ONE_FRAGMENT); }
<YYINITIAL> "nsnf"                                 { return t (Constant.ALL_SITES_IN_THEIR_OWN_FRAGMENT); }
<YYINITIAL> "nmnf"                                 { return t (Constant.ALL_MOLECULES_IN_THEIR_OWN_FRAGMENT); }

<YYINITIAL> {DOUBLE}                               { return t (Constant.DOUBLE_VAL, yytext()); }
<YYINITIAL> {INTEGER}                              { return t (Constant.INT_VAL, yytext()); }


<YYINITIAL> {slashcomment} {}
<YYINITIAL> {whitespace} {}
<YYINITIAL> "/*" { yybegin (COMMENT); }

<COMMENT> {blockcomment} {yybegin (YYINITIAL);}

<COMMENT> . {
error("Illegal comment");
}

<YYINITIAL> . { 
error("Invalid character : " + yytext());
}
