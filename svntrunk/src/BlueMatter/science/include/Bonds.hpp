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
 #ifndef BONDS_HPP
#define BONDS_HPP

#include <vector>
#include <BlueMatter/XYZ.hpp>

using namespace std;

class OrderBond
{
public:
    char m_Name[128];
    int m_carbon;
    int m_hydrogens[3];
    int m_numh;
    int m_last_returned;
    double m_op;
    double m_op2;
    int m_num_counts;

    OrderBond(char *name, int carbon, int hydrogen)
        {
        strcpy(m_Name, name);
        m_carbon = carbon;
        m_hydrogens[0] = hydrogen;
        m_hydrogens[1] = -1;
        m_hydrogens[2] = -1;
        m_numh = 1;
        m_last_returned = 0;
        m_op = 0.0;
        m_op2 = 0.0;
        m_num_counts = 0;
        }

    OrderBond(char *name, int carbon, int hydrogen1, int hydrogen2)
        {
        strcpy(m_Name, name);
        m_carbon = carbon;
        m_hydrogens[0] = hydrogen1;
        m_hydrogens[1] = hydrogen2;
        m_hydrogens[2] = -1;
        m_numh = 2;
        m_last_returned = 0;
        m_op = 0.0;
        m_op2 = 0.0;
        m_num_counts = 0;
        }

    OrderBond(char *name, int carbon, int hydrogen1, int hydrogen2,
              int hydrogen3)
        {
        strcpy(m_Name, name);
        m_carbon = carbon;
        m_hydrogens[0] = hydrogen1;
        m_hydrogens[1] = hydrogen2;
        m_hydrogens[2] = hydrogen3;
        m_numh = 3;
        m_last_returned = 0;
        m_op = 0.0;
        m_op2 = 0.0;
        m_num_counts = 0;
        }

    void AddValue(double val)
        // Given an instantaneous val, accumulate the 
        // second Legendre polynomial
        {
        double op = 0.5 - 1.5*val*val;
        //double op = acos(val)*180.0/3.14159;
        m_op += op;
        m_op2 += op*op;

        m_num_counts++;
        }

    double GetOP()
        {
        double op = m_op / (double) m_num_counts;
        return (op);
        }

    double GetOPFluc()
        {
        double op = m_op / (double) m_num_counts;
        double op2 = m_op2 / (double) m_num_counts;
        return sqrt(op2 - op*op);
        }
    
    int SetIndices(int *c_index, int *h_index)
        {
        if (m_last_returned == m_numh)
            {
            m_last_returned = 0;
            return 0;
            }
        else
            {
            *c_index = m_carbon;
            *h_index = m_hydrogens[m_last_returned];
            m_last_returned++;
            return 1;
            }
        }
};

class OrderBondList
{
public:
    vector<OrderBond> m_bonds;
    FragmentList *m_Fragments;
    int m_nbonds;

    OrderBondList(char *filename, FragmentList *fragments)
        {
        char name[128];
        char buff[1024];
        int carbon, h1, h2, h3;
        m_Fragments = fragments;
        //int frag_size = fragments->m_fragments[0].m_Count;
        FILE *f = fopen(filename, "r");
        assert(f);
        while(fgets(buff, 1024, f))
            {
            /*
            // Sanity check -- are the indices for these atoms within the 
            // size of an individual fragment
            if ( (carbon < frag_size) &&
                 (h1 < frag_size) &&
                 (h2 < frag_size) )
                {
                m_bonds.push_back(OrderBond(name,carbon,h1,h2));
                }
            else
                {
                cerr << "Warning: bond specification " << carbon 
                        << " " << h1 << " " << h2
                        << " is outside fragment size range "
                        << frag_size << endl;
                }
            */
            int retvals = sscanf(buff,"%s %d %d %d %d", 
                                      name, &carbon, &h1, &h2, &h3);
            if (retvals == 3)
                {
                m_bonds.push_back(OrderBond(name,carbon,h1));
                }
            else if (retvals == 4)
                {
                m_bonds.push_back(OrderBond(name,carbon,h1,h2));
                }
            else if (retvals == 5) 
                {
                m_bonds.push_back(OrderBond(name,carbon,h1,h2,h3));
                }
            else
                {
                cerr << "# Error processing line, skipping: " << buff << endl;
                }
            }
        m_nbonds = m_bonds.size();
        }
};

#endif
