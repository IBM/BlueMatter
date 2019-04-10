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
 package com.ibm.bluematter.utils;

import java.util.*;
import java.io.*;

public class SimpleParser {

    String filename = null;
    BufferedReader fIn = null;
    int lineCount = 0;
    Hashtable ctp = null;


    public SimpleParser(String file) {
  filename = file;
        ctp = new Hashtable();

  try {
      fIn = new BufferedReader(new FileReader(file));
  } catch (FileNotFoundException e) {
            System.err.println(e.getMessage());
            System.exit(1);
        }
    }

    public Hashtable parse() throws Exception {
  String line = null;
        try {
            line         = fIn.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            System.out.println("SimpleParser::parser():: Error reading the rtp file");
            System.exit(1); 
        }

  lineCount = 0;
  while(line != null) {
//             if(ProbspecgenDB2.debug)
//                 System.out.println("SimpleParser::parse()::line: " + line);
            
      line.trim();	    
      if(line.startsWith("#") || line.equals("")) {
                line = fIn.readLine();
                lineCount++;
                continue; // skip comments and blank lines
            }
      StringTokenizer tokenLine = new StringTokenizer(line);

      String name = getNextToken(tokenLine);

      if (name.equals("WaterModel")) {
    String value = getNextToken(tokenLine);
    if ( value.equals("TIP3P") 
         || value.equals("TIP3PFLOPPY")
         || value.equals("SPC")
         || value.equals("SPCFLOPPY")
         || value.equals("SPCE")
         || value.equals("SPCEFLOPPY")
         || value.equals("CHARMM_TIP3P")
         || value.equals("CHARMM_TIP3PFLOPPY")
         ) {
//                     RunTimeParameters.setProbspecParam("WaterMethod", value);
//                     if(value.lastIndexOf("FLOPPY") == -1)
//                         RunTimeParameters.setEnumParam("DoShakeRattle", "1"); 
                }                
                else
                    throw new Exception ("Water model name: " + value + " not recognized");

                putIntoCtp(name,value);
      }
      else if ( name.equals("ShakeTolerance") ) {
    parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("ShakeMaxIterations") ) {
    parseLine(name, tokenLine, "I");
      }
      else if ( name.equals("RattleTolerance") ) {
    parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("RattleMaxIterations") ) {
    parseLine(name, tokenLine, "I");
      }
      else if ( name.equals("Ensemble") ) {
    String value = getNextToken(tokenLine);
    if (value.equals("NVT")) {
// 		    RunTimeParameters.setEnumParam("DoVelocityResample", "1"); 	
// 		    RunTimeParameters.setEnumParam("DoTemperatureControl", "1");	
// 		    RunTimeParameters.setEnumParam("DoNVT", "1");	
    }
    else if (value.equals("NPT")) {
// 		    RunTimeParameters.setEnumParam("DoVelocityResample", "1"); 	
// 		    RunTimeParameters.setEnumParam("DoTemperatureControl", "1");	
// 		    RunTimeParameters.setEnumParam("DoPressureControl", "1");
    }
                else if (value.equals("NPH")) {
//		    RunTimeParameters.setEnumParam("DoPressureControl", "1");
                }
    else if (value.equals("NVE")) {
        // do nothing for now
    }
    else {
        throw new Exception("Ensemble method: "+value+" is not recognized");
    }

                putIntoCtp(name,value);
      }
      else if ( name.equals("BoundingBox") ) {                

                Vector bbParams = new Vector();

                while(tokenLine.hasMoreElements()) {
                    bbParams.addElement(tokenLine.nextToken());
                }
                
                String boundingBox = null;
                
                switch( bbParams.size() ) {
                case 1: {
                    String value1 = (String) bbParams.elementAt(0);
                    checkDouble(value1);
                    value1 = "(" + value1 + "/2.0)";
                    boundingBox = new String ("{ -" + value1 + 
                                              " , -" +  value1 + 
                                              " , -" +  value1 +" , "
                                              +  value1 +" , "
                                              +  value1 +" , "
                                              +  value1 +" }");
                    break;
                }
                case 3: {
                    String value1 = (String) bbParams.elementAt(0);
                    checkDouble(value1);
                    value1 = "(" + value1 + "/2.0)";
                    String value2 = (String) bbParams.elementAt(1);
                    checkDouble(value2);
                    value2 = "(" + value2 + "/2.0)"; 
                    String value3 = (String) bbParams.elementAt(2);
                    checkDouble(value3);
                    value3 = "(" + value3 + "/2.0)";

                    boundingBox = new String ("{ -"+ value1 +" , -" 
                                              +  value2 +" , -"
                                              +  value3 +" , "
                                              +  value1 +" , "
                                              +  value2 +" , "
                                              +  value3 +" }");
                    break;
                }
                case 4: {
                    String xOrigin = (String) bbParams.elementAt(0);
                    checkDouble(xOrigin);
                    String yOrigin = (String) bbParams.elementAt(1);
                    checkDouble(yOrigin);
                    String zOrigin = (String) bbParams.elementAt(2);
                    checkDouble(zOrigin);

                    String length = (String) bbParams.elementAt(3);
                    checkDouble(length);
                    length = "(" + length + "/2.0)";

                    boundingBox = new String ("{ "+ xOrigin+ "-" +length +" , " 
                                              +  yOrigin+ "-" +length +" , "
                                              +  zOrigin+ "-" +length +" , "
                                              +  xOrigin+ "+" +length +" , "
                                              +  yOrigin+ "+" +length +" , "
                                              +  zOrigin+ "+" +length +" }");                    

                    break;
                }
                case 6: {
                    String value1 = (String) bbParams.elementAt(0);
                    checkDouble(value1);
                    String value2 = (String) bbParams.elementAt(1);
                    checkDouble(value2);
                    String value3 = (String) bbParams.elementAt(2);
                    checkDouble(value3);
                    String value4 = (String) bbParams.elementAt(3);
                    checkDouble(value4);
                    String value5 = (String) bbParams.elementAt(4);
                    checkDouble(value5);
                    String value6 = (String) bbParams.elementAt(5);
                    checkDouble(value6);


                    boundingBox = new String ("{ "+ value1 +" , " 
                                              +  value2 +" , "
                                              +  value3 +" , "
                                              +  value4 +" , "
                                              +  value5 +" , "
                                              +  value6 +" }");                    
                    break;
                }
                default:
                    throw new Exception("ERROR:: Invalid BoundingBox params: at line: " + lineCount);
                }

                
// 		String value = getNextToken(tokenLine);
// 		checkDouble(value);
// 		String value1 = getNextToken(tokenLine);
// 		checkDouble(value1);
// 		String value2 = getNextToken(tokenLine);
// 		checkDouble(value2);
// 		String value3 = getNextToken(tokenLine);
// 		checkDouble(value3);
// 		String value4 = getNextToken(tokenLine);
// 		checkDouble(value4);
// 		String value5 = getNextToken(tokenLine);
// 		checkDouble(value5);
// 		String boundingBox = new String("{" + value
// 						+ "," + value1
// 						+ "," + value2
// 						+ "," + value3
// 						+ "," + value4
// 						+ "," + value5 + "}");
    // RunTimeParameters.setParam( "BoundingBoxDef", boundingBox ); 		

                if(boundingBox != null)
                    putIntoCtp("BoundingBoxDef", boundingBox);		
                else 
                    throw new Exception("ERROR:: Problem reading bounding box parameters.");
      }
      else if ( name.equals("VelocityResamplePeriodInOTS") ) {
    parseLine(name, tokenLine, "I");
      }
      else if ( name.equals("VelocityResampleTargetTemperature") ) {
    parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("PressureControlPistonMass") ) {
    parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("PressureControlTarget") ) {
    parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("PressureControlPistonInitialVelocity") ) {
    parseLine(name, tokenLine, "D");	    
      }
      else if ( name.equals("ExternalPressure") ) {
    parseLine(name, tokenLine, "D");	    	    
      }
      else if ( name.equals("Integrator") ) {
    String value = getNextToken(tokenLine);
    if(value.equals("Respa"))
        {// RunTimeParameters.doRespaFlag();
                    }	
    else if(value.equals("Verlet")) {
        /// do nothing
    }
    else {
        throw new Exception("Integrator method: "+value+" is not recognized");
    }
                putIntoCtp(name,value);
            }
// 	    else if ( name.equals("InnerTimeStepInPicoSeconds") ) {
// 		parseLine(name, tokenLine, "D");	    	
//                 //                RunTimeParameters.setParam( "DoOuter", "0:int");
// 	    }
      else if ( name.equals("OuterTimeStepInPicoSeconds") ) {
//                 RunTimeParameters.setParam( "DoOuter", "1:int");
//                 RunTimeParameters.doOuter();	
    parseLine(name, tokenLine, "D");	    	    
      }
// 	    else if ( name.equals("NumberOfInnerTimeSteps") ) {
//                 //                RunTimeParameters.setParam( "DoOuter", "0:int");
// 		parseLine(name, tokenLine, "I");	    	    
// 	    }
      else if ( name.equals("NumberOfOuterTimeSteps") ) {
//                 RunTimeParameters.setParam( "DoOuter", "1:int");
//                 RunTimeParameters.doOuter();	
    parseLine(name, tokenLine, "I");	    	    	    
      }
      else if ( name.equals("DefineRespaLevel") ) {
                String respaLevel = getNextToken(tokenLine);
                String simTicks = getNextToken(tokenLine);
                
                putIntoCtp(name + " " + respaLevel, respaLevel+":"+simTicks);
//                 int numberOfRespaLevels = RunTimeParameters.getNumberOfRespaLevels();
//                 RunTimeParameters.incRespaLevels();
                
//                 String defineRespaLevelKey = name + numberOfRespaLevels;
//                 String respaLevelSimTicks = new String(respaLevel+":"+simTicks);
//                 RunTimeParameters.setRespaParam( defineRespaLevelKey, respaLevelSimTicks ); 	
            }
      else if ( name.equals("DefineRespaLevelForForceCategory") ) {
                String category = getNextToken(tokenLine);
                if( category.equals("Harmonic") || (category.equals("Torsion")) 
                   || (category.equals("NonBonded")) ) {
                    // do nothing.
                }
                else
                    throw new Exception ("Category name: " + category +" not recognized");

                String respaLevel = getNextToken(tokenLine);
                String key1 = name+" "+category+" "+respaLevel;
                //                System.out.println(key1);
                putIntoCtp(key1, respaLevel);
                
      }
      else if ( name.equals("LRFMethod") ) {
                String value = getNextToken(tokenLine);
                if(value.equals("StdCoulomb")) {
                    // RunTimeParameters.setEnumParam("LongRangeForceMethod", "STDCOULOMB"); 
                }
                else if(value.equals("FiniteRangeCutoffs")) {
                    // RunTimeParameters.setEnumParam("LongRangeForceMethod", "FRC"); 
                }
                else if(value.equals("Ewald")) {
                    // RunTimeParameters.setEnumParam("LongRangeForceMethod", "EWALD"); 
                }
                else if(value.equals("P3ME")) {
                    // RunTimeParameters.setEnumParam("LongRangeForceMethod", "P3ME"); 
                }
                else if(value.equals("Lekner")) {
                    // RunTimeParameters.setEnumParam("LongRangeForceMethod", "LEKNER"); 
                }
                else 
                    throw new Exception("Long Range Force Method: " + value + " is not recognized."); 
                putIntoCtp(name,value);
      }
      else if ( name.equals("LeknerEpsilon") ) {
    parseLine(name, tokenLine, "D");	    
      }
      else if ( name.equals("SwitchLowerCutoff") ) {
                // SwitchLowerCutoff <double cutoff> 
                if( tokenLine.countTokens() == 1 ) {
                    String value = getNextToken(tokenLine);
                    checkDouble(value);
                    putIntoCtp(name+" 0", value + " " + String.valueOf( 0 ));
                }
                // SwitchLowerCutoff <double cutoff> <respa level>
                else if( tokenLine.countTokens() == 2 ) {
                    String value = getNextToken(tokenLine);
                    checkDouble(value);

                    String value1 = getNextToken(tokenLine);
                    checkInt(value1);

                    putIntoCtp(name+" "+value1 ,value + " " + value1);
                }
                //parseLine(name, tokenLine, "D");	    
      }
      else if ( name.equals("SwitchDelta") ) {

                // SwitchDelta <double delta> 
                if( tokenLine.countTokens() == 1 ) {
                    String value = getNextToken(tokenLine);
                    checkDouble(value);
                    putIntoCtp(name+" 0",value + " " + String.valueOf( 0 ));
                }
                // SwitchDelta <double delta> <respa level>
                else if( tokenLine.countTokens() == 2 ) {
                    String value = getNextToken(tokenLine);
                    checkDouble(value);

                    String value1 = getNextToken(tokenLine);
                    checkInt(value1);

                    putIntoCtp(name+" "+value1,value + " " + value1);
                }

                // parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("EwaldAlpha") ) {
                parseLine(name, tokenLine, "D");
      }
      else if ( name.equals("EwaldKmax") ) {
                parseLine(name, tokenLine, "I");
      }
      else if ( name.equals("P3MEinitSpacing") ) {
                if( tokenLine.countTokens() == 1 ) {
                    String value = getNextToken(tokenLine);
                    checkDouble(value);
                
                    // RunTimeParameters.setParam(name, new String("{"+value+","+value+","+value+"}:XYZ"));
                    putIntoCtp(name, new String("{"+value+","+value+","+value+"}"));
                }
                else if( tokenLine.countTokens() == 3 ) {
                    String value = getNextToken(tokenLine);
                    checkDouble(value);

                    String value1 = getNextToken(tokenLine);
                    checkDouble(value1);

                    String value2 = getNextToken(tokenLine);
                    checkDouble(value2);

                    // RunTimeParameters.setParam(name, new String("{"+value+","+value1+","+value2+"}:XYZ"));
                    putIntoCtp(name, new String("{"+value+","+value+","+value+"}"));
                }
                else {
                    throw new Exception("Wrong number of parameters to P3MEinitSpacing");
                }
            }
      else if ( name.equals("P3MEchargeAssignment") ) {
                parseLine(name, tokenLine, "I");
      }
      else if ( name.equals("P3MEdiffOrder") ) {
                String value = getNextToken(tokenLine);
                if((value.equals("GRADIENT")) || (value.equals("ANALYTICAL")) || (value.equals("FINITE_2PT")) 
                   || (value.equals("FINITE_4PT")) || (value.equals("FINITE_6PT")) || (value.equals("FINITE_8PT"))
                   || (value.equals("FINITE_10PT")) || (value.equals("FINITE_12PT"))) {
                    // no nothing
                }
                else 
                    throw new Exception ("P3MEdiffOrder name: " + value + " not recognized");
                
                // RunTimeParameters.setParam(name, new String("PreMSD::"+value+":int"));
                putIntoCtp(name, new String("PreMSD::"+value));
      }
            else if( name.equals("NumberOfConstraints")) {
                parseLine(name, tokenLine, "I");
            }
            else if( name.equals("Constraints")) {
                String value = getNextToken(tokenLine);
                if(value.equals("on")  || value.equals("off"))
                    putIntoCtp(name, value);
                else 
                    throw new Exception ("ERROR:: Unexpected token: " + value + " possible candidates are: {on,off}");
            }
            else if( name.equals("WaterInit")) {
                String value = getNextToken(tokenLine);
                if(value.equals("on")  || value.equals("off"))
                    putIntoCtp(name, value);
                else 
                    throw new Exception ("ERROR:: Unexpected token: " + value + " possible candidates are: {on,off}");
            }            
            else if( name.equals("CheckSitesInBox")) {
                String value = getNextToken(tokenLine);
                if(value.equals("on")  || value.equals("off"))
                    putIntoCtp(name, value);
                else 
                    throw new Exception ("ERROR:: Unexpected token: " + value + " possible candidates are: {on,off}");

            }            
            else if( name.equals("FEP")) {
                String value = getNextToken(tokenLine);
                if(value.equals("on")  || value.equals("off"))
                    putIntoCtp(name, value);
                else 
                    throw new Exception ("ERROR:: Unexpected token: " + value + " possible candidates are: {on,off}");
            }
            else if( name.equals("RigidProtein")) {
                String value = getNextToken(tokenLine);
                if(value.equals("on")  || value.equals("off"))
                    putIntoCtp(name, value);
                else 
                    throw new Exception ("ERROR:: Unexpected token: " + value + " possible candidates are: {on,off}");
            }
            else if( name.equals("Lambda")) {
                parseLine(name, tokenLine, "D");
            }
            else if( name.equals("VelocityResampleRandomSeed")) {
                parseLine(name, tokenLine, "I");
            }
            else if( name.equals("SnapshotPeriodInOTS")) {
                parseLine(name, tokenLine, "I");
            }
            else if( name.equals("EmitEnergyTimeStepModulo")) {
                parseLine(name, tokenLine, "I");
            }
            else if( name.equals("Fragmentation")) {
                
                String value = getNextToken(tokenLine);
                 if( value.equals("asof")) {
                     // RunTimeParameters.setProbspecParam("Fragmentation", "all_sites_in_one_fragment");		
                 }
                 else if( value.equals("amof")) {
                     // RunTimeParameters.setProbspecParam("Fragmentation", "all_molecules_in_one_fragment");		
                 }
                 else if( value.equals("nsnf")) {
                     // RunTimeParameters.setProbspecParam("Fragmentation", "all_sites_in_their_own_fragment");		
                 }
                 else if (value.equals("nmnf")) {
                     // RunTimeParameters.setProbspecParam("Fragmentation", "all_molecules_in_their_own_fragment");		
                 }
                 else if (value.equals("residue")) {
                     // RunTimeParameters.setProbspecParam("Fragmentation", "all_molecules_in_their_own_fragment");		
                 }
                 else if (value.equals("rigid_group")) {
                     // RunTimeParameters.setProbspecParam("Fragmentation", "all_molecules_in_their_own_fragment");		
                 }
                 else {
                     throw new Exception("Fragmentation scheme: " + value + " is not recognized");
                 }
                putIntoCtp(name, value);
            }
      else {
    throw new Exception("Can not recognize name: " + name + " on line: "+lineCount);
      }
            line = fIn.readLine();
      lineCount++;
  }
    
        return ctp;
    }

    private String getNextToken(StringTokenizer st) throws Exception {
  if(!st.hasMoreElements())
      throw new Exception("Corrupted line: " + lineCount);

  return st.nextToken();
    }

    private boolean isDouble(String s) {
  try {
      double r = Double.parseDouble(s);
  }
  catch (NumberFormatException e) {
      return false;
  }
    
  return true;
    }

    private boolean isInt(String s) {
  try {
      int r = Integer.parseInt(s);
  }
  catch (NumberFormatException e) {
      return false;
  }
    
  return true;
    }

    private void checkDouble(String s) throws Exception {
  if(!isDouble(s))
      throw new Exception("Corrupt double encountered on line: " + lineCount);
    }

    private void checkInt(String s) throws Exception {
  if(!isInt(s))
      throw new Exception("Corrupt integer encountered on line: " + lineCount);
    }
    
    /**
     * Parse a line with a value (name value)
     */    
    private void parseLine(String name, StringTokenizer tokenLine, String type) throws Exception {
  String value = getNextToken(tokenLine);
  if(type.equals("D")) {
      checkDouble(value);
      // RunTimeParameters.setParam(name, value+":double");
            putIntoCtp(name, value);
            
  }
  else {
      checkInt(value);
      // RunTimeParameters.setParam(name, value+":int");
            putIntoCtp(name, value);
  }
    }

    private void putIntoCtp( String name, Object value) throws Exception {
        if(ctp.put(name, value) != null)
            throw new Exception("ERROR:: Ctp name: " + name + " is already defined with value " + value );
    }
}
