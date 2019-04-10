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
 // **************************************************************************
// File: paramlisthandler.hpp
// Date: November 12, 2000
// Author: RSG (taken from probspec.cpp written by Mike Pitman)
// Description: Class definitions for singleton classes implemented
//		probspec.cpp that are used to load (push) parameters
//		into the internal data structures which are then used
//		to create the flat data structure which defines the
//		"problem" for a particular molecular system and force
//		field.
//
// **************************************************************************

#ifndef INCLUDE_PARAMLISTHANDLER_HPP
#define INCLUDE_PARAMLISTHANDLER_HPP

class SiteSpecHandler
{
private:
  // data
  static SiteSpecHandler* d_instance_p;
  // methods
  SiteSpecHandler();
public:
  static SiteSpecHandler* instance();
  void push(char *name,char *type, double mass, double charge,
      double eps, double sigma, double eps14, double sigma14)
  {
    PushSiteSpec(char *name,char *type, double mass, double charge,
     double eps, double sigma, double eps14, double sigma14);
  }

  void finalize();
};

class BondSpecHandler
{
private:
  // data
  static BondSpecHandler* d_instance_p;
  // methods
  BondSpecHandler();
public:
  static BondSpecHandler* instance();
  void push(char *s1, char *s2, double k0,double r0)
  {
    PushBondSpec(char *s1, char *s2, double k0,double r0);
  }

  void finalize();
};

class AngleSpecHandler
{
private:
  // data
  static AngleSpecHandler* d_instance_p;
  // methods
  AngleSpecHandler();
public:
  static AngleSpecHandler* instance();
  void push(char *s1, char *s2, char *s3, double k,double th0, double ku, double ru)
  {
    PushAngleSpec(char *s1, char *s2, char *s3, double k,double th0, double ku, double ru);
  }

  void finalize();
};

class TorsionSpecHandler
{
private:
  // data
  static TorsionSpecHandler* d_instance_p;
  // methods
  TorsionSpecHandler();
public:
  static TorsionSpecHandler* instance();
  void push(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
  {
    PushTorsionSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0);
  }

  void finalize();
};

class ImproperSpecHandler
{
private:
  // data
  static ImproperSpecHandler* d_instance_p;
  // methods
  ImproperSpecHandler();
public:
  static ImproperSpecHandler* instance();
  void push(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0)
  {
    PushImproperSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0);
  }

  void finalize();
};

void PushSiteSpec(char *name,char *type, double mass, double charge,
     double eps, double sigma, double eps14, double sigma14);
void PushBondSpec(char *s1, char *s2, double k0,double r0);
void PushAngleSpec(char *s1, char *s2, char *s3, double k,double th0, double ku, double ru);
void PushTorsionSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0);
void PushImproperSpec(char *s1, char *s2, char *s3, char *s4,double k0,int wells, double r0);

#endif
