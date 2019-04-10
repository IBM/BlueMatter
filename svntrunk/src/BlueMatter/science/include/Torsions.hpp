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
 #ifndef TORSIONS_HPP
#define TORSIONS_HPP

#include <vector>
#include <BlueMatter/XYZ.hpp>
#include <BlueMatter/Geometry.hpp>

using namespace std;
#define DEGREES 57.295779513082323

template <class T>
class tTorsion
{
public:
    char m_Name[128];
    int m_a1;
    int m_a2;
    int m_a3;
    int m_a4;
    bool m_degrees;
    vector<int> m_states;
    tXYZ<T> *m_Positions;


    tTorsion(char *name, int a1, int a2, int a3, int a4)
        {
        strncpy(m_Name, name, 128);
        m_a1 = a1;
        m_a2 = a2;
        m_a3 = a3;
        m_a4 = a4;
        }

    T GetTorsion(tFragment<T> &f)
        {
        T tors;
        Torsion(f.m_Positions[f.m_Start + m_a1],
                f.m_Positions[f.m_Start + m_a2],
                f.m_Positions[f.m_Start + m_a3],
                f.m_Positions[f.m_Start + m_a4],
                tors);
        return tors;
        }

    string ClassifyGGT(tFragment<T> &f)
        // Classify the torsion as gauche+, gauche-, or trans
        // return                     P        M          T
        // accordingly
        {
        T tors = GetTorsion(f)*DEGREES;
        if ( (tors < -120.0) || (tors >= 120.0))
            {
            return string("T");  
            }
        else if ( (tors >= -120.0) && (tors < 0.0) )
            {
            return string("M");
            }
        else
            {
            return string("P");
            }
        }

    string ClassifyPN(tFragment<T> &f)
        // Classify as either Positive or Negative
        // good for polyunsaturated chains
        {
        T tors = GetTorsion(f)*DEGREES;
        if (tors < 0.0)
            {
            return string("N");
            }
        else
            {
            return string("P");
            }
        }

    int AddGGTState(tFragment<T> &f)
        {
        m_states.push_back(ClassifyGGT(f));
        }

    int AddPNState(tFragment<T> &f)
        {
        m_states.push_back(ClassifyPN(f));
        }

};

typedef tTorsion<float> fTorsion;
typedef tTorsion<double> dTorsion;

template <class T>
class tTorsionList
{
public:
    vector<tTorsion<T> > m_torsions;
    bool isGGT;

    tTorsionList(char *filename, bool setGGT=true)
        {
        char name[128];
        char buff[1024];

        isGGT = setGGT;

        FILE *f = fopen(filename, "r");
        assert(f);
        int a1, a2, a3, a4;
        while(fgets(buff,1024,f))
            {
            int retvals = sscanf(buff, "%s %d %d %d %d",
                                 name, &a1, &a2, &a3, &a4);
            if (retvals == 5)
                {
                m_torsions.push_back(tTorsion<T>(name, a1, a2, a3, a4));
                }
            else
                {
                cerr << "# retvals = " << retvals << endl;
                cerr << "# Error processing line: " << endl;
                cerr << "# " << buff << endl;
                }
            }
        }

    int NumTorsions()
        {
        return m_torsions.size();
        }

    string GetConformation(tFragment<T> &fragment)
        {
        string s = string("");
        if (isGGT)
            {
            for (vector<tTorsion<T> >::iterator t=m_torsions.begin();
                                            t!=m_torsions.end();
                                            t++)
                {
                string state = t->ClassifyGGT(fragment);
                s += string(state);
                }
            }
        else
            {
            for (vector<tTorsion<T> >::iterator t=m_torsions.begin();
                                            t!=m_torsions.end();
                                            t++)
                {
                string state = t->ClassifyPN(fragment);
                s += string(state);
                }
            }
        return s;
        }

    // This routine classifies fragments into states using pairs of 
    // torsions, instead of single torsions.  Four states are used to describe 
    // the two torsions:
    // T == tt
    // G == g+g- g-g+ g+g+ g-g-
    // F == tg+ tg-  (first is trans)
    // S == g+t g-t  (second is trans)
    string GetPairedConformation(tFragment<T> &fragment)
        {
        string s = string("");
        for (int i=0; i<m_torsions.size(); i+=2)
            {
            string s1 = m_torsions[i].ClassifyGGT(fragment);
            string s2 = m_torsions[i+1].ClassifyGGT(fragment);
            if (s1 == string("T"))
                {
                if (s2 == string("T"))
                    {
                    s += string("T");
                    }
                else
                    {
                    s += string("F");
                    }
                }
            else
                {
                if (s2 == string("T"))
                    {
                    s += string("S");
                    }
                else
                    {
                    s += string("G");
                    }
                }
            }
        return s;
        }

    vector<T> GetAngles(tFragment<T> &fragment, vector<T> &v)
        {
        v.clear();
        for (vector<tTorsion<T> >::iterator t=m_torsions.begin();
                                        t!=m_torsions.end();
                                        t++)
            {
            T val = t->GetTorsion(fragment);
            v.push_back(val);
            }
        return v;
        }
};


typedef tTorsionList<float> fTorsionList;
typedef tTorsionList<double> TorsionList;

class ConformationList
{
public:
    map<string,int> m_hist;
    int m_num_tors;
    int m_num_confs;
    int m_count;

    ConformationList()
        {
        m_num_tors = 0;
        m_num_confs = 0;
        m_count = 0;
        }

    ConformationList(int num_tors)
        {
        m_num_tors = num_tors;
        m_num_confs = 0;
        m_count = 0;
        }

    void SetNumTors(int num_tors)
        {
        m_num_tors = num_tors;
        }

    int operator[](string conf)
        {
        assert(IsConf(conf));
        return GetConf(conf);
        }

    int GetConf(string &conf)
        {
        if (HasConf(conf))
            {
            return m_hist[conf];
            }
        else
            {
            return 0;
            }
        }

    bool IsGGTConf(string &conf)
        {
        // is the length right?
        if (conf.length() != m_num_tors) 
            {
            return false;
            }

        // does it consist only of valid characters
        // P (gauche+) M (gauche-) T (trans)
        for (string::iterator s = conf.begin();
                              s!= conf.end();
                              s++)
            {
            //if ( (*s != "T") && (*s != "P") && (*s != "M") )
            if ( (*s != 'T') && (*s != 'P') && (*s != 'M')  
              && (*s != 'F') && (*s != 'S') && (*s != 'G') )
                {
                return false;
                }
            }
        return true;
        }


    bool IsPNConf(string &conf)
        {
        // is the length right?
        if (conf.length() != m_num_tors) 
            {
            return false;
            }

        // does it consist only of valid characters
        // P (gauche+) M (gauche-) T (trans)
        for (string::iterator s = conf.begin();
                              s!= conf.end();
                              s++)
            {
            //if ( (*s != "T") && (*s != "P") && (*s != "M") )
            if ( (*s != 'P') && (*s != 'N') )
                {
                return false;
                }
            }
        return true;
        }

    bool IsConf(string &conf)
        {
        return (IsGGTConf(conf) || IsPNConf(conf));
        }

    bool HasConf(string &conf)
        {
        assert(IsConf(conf)); // good for safety but costly
        map<string,int>::iterator i = m_hist.find(conf);
        if (i == m_hist.end())
            {
            return false;
            }
        else
            {
            return true;
            }
        }

    void CountConformation(string &conf)
        {
        if (HasConf(conf))
            {
            m_hist[conf]++;
            }
        else
            {
            m_hist[conf]=1;
            m_num_confs++;
            }
        m_count++;
        }

    int GetNumConfs()
        {
        return m_num_confs;
        }

    int GetNumCounts()
        {
        return m_count;
        }

    void PrintHist()
        {
        cout << "# Num Confs = " << GetNumCounts() << endl;
        cout << "# Num Distinct Confs = " << GetNumConfs() << endl;
        for (map<string,int>::iterator conf = m_hist.begin();
                                       conf!= m_hist.end();
                                       conf++)
            {
            float prob = (float)(conf->second) / (float) GetNumCounts();
            cout << conf->first << " " << conf->second << " " << prob << endl;
            }
        }

    string MirrorConf(string conf)
        {
        for (int i = 0; i<conf.length();i++)
            {
            if (conf[i] == 'M')
                {
                conf[i] = 'P';
                }
            else if (conf[i] == 'P')
                {
                conf[i] = 'M';
                }
            } 
        return conf;
        }

    bool IsMirrorConf(const string &conf1, string conf2)
        // in conf2, change all M's to P's, and vice versa
        // then return true if conf1 == conf2
        {
        conf2 = MirrorConf(conf2);
        return (conf1 == conf2);
        }

     ConformationList * MergeMirrorConfs()
        {
        ConformationList *new_list = new ConformationList;
        new_list->m_num_tors = m_num_tors;
        for (map<string,int>::iterator conf = m_hist.begin();
                                       conf!= m_hist.end();
                                       conf++)
            {
            string mirror = MirrorConf(conf->first);
            // if the mirror of the present conformation is present,
            // add its value 
            // otherwise, create a new entry
            if (new_list->m_hist.find(mirror) != new_list->m_hist.end())
                {
                // the mirror is present
                new_list->m_hist[mirror] += conf->second;
                new_list->m_count += conf->second;

                }
            else
                {
                // mirror not present, so we add the actual conf
                new_list->m_hist[conf->first] = conf->second;
                new_list->m_count += conf->second;
                new_list->m_num_confs++;
                }
            }
        return new_list;
        }
};

#endif
