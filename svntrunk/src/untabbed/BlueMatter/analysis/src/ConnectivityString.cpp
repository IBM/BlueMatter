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
 #include <BlueMatter/ConnectivityString.hpp> 

// Sample connectivity string: 1*256+3*1+1660*3

void ConnectivityString::parseMolEntry(char *s) {
  char *t = strstr(s,"*");
  assert(t!=NULL);
  char nmol[20];
  sscanf(s,"%[^*]*%*s",nmol);

  MoleculeRun m;
  m.m_Count = atoi(nmol);
  m.m_Size = atoi(++t);
  m_mr.push_back(m);
}

void ConnectivityString::parseRemaining(char *s) {
  if(strcmp(s,"")==0) return;

  char *t = strstr(s,"+");
  if(t==NULL) {
    parseMolEntry(s);
    return;
  }
  char mole[20];
  sscanf(s,"%[^+]+%*s",mole);
  parseMolEntry(mole);
  parseRemaining(++t);
}
  
// return the last 3-atom molecule's count
 int ConnectivityString::getNumberOfWaters() {
  assert(m_mr.size()>0);
        // printMoleculeRun();


  int answer=0;
  vector<MoleculeRun>::iterator it=m_mr.begin();
  for(;it!=m_mr.end();it++) {
    if(it->m_Size==3) {
      answer = it->m_Count;
    }
  }
  return answer;
}


 int ConnectivityString::getNumberOfMolecules() {
  assert(m_mr.size()>0);

        // printMoleculeRun();
  int answer=0;
  vector<MoleculeRun>::iterator it=m_mr.begin();
  for(;it!=m_mr.end();it++) {
    answer+=it->m_Count;
  }
  return answer;
}

int ConnectivityString::getNumberOfAtoms() {
  assert(m_mr.size()>0);

        // printMoleculeRun();
  int answer=0;
  vector<MoleculeRun>::iterator it=m_mr.begin();
  for(;it!=m_mr.end();it++) {
    answer+=it->m_Count*it->m_Size;
  }
  return answer;
}

void ConnectivityString::printMoleculeRun() {
  cout << "MoleculeRun: ... " << endl;
  vector<MoleculeRun>::iterator it=m_mr.begin();
  for(;it!=m_mr.end();it++) {
    cout <<  it->m_Count << " : " << it->m_Size << endl;
  }
  cout << endl;
}



