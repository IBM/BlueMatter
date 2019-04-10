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
 #ifndef UDF_STATE_HPP
#define UDF_STATE_HPP

class UDF_State
{
public:

    struct Shake_StateParams
    {
	int MaxIterations;
	double Epsilon;
    };

    static Shake_StateParams Shake;

    static void Set_Shake(const int &MaxI, const double &Eps)
    {
	Shake.MaxIterations = MaxI;
	Shake.Epsilon = Eps;
    }

    struct Rattle_StateParams
    {
	int MaxIterations;
	double Epsilon;
    };

    static Rattle_StateParams Rattle;

    static void Set_Rattle(const int &MaxI, const double &Eps)
    {
	Rattle.MaxIterations = MaxI;
	Rattle.Epsilon = Eps;
    }

    struct SwitchStd5_StateParams
    {
        double Rl;
        double Ru;
        double A;
        double B;
        double C;
        double AA;
        double BB;
        double CC;
    };

    static SwitchStd5_StateParams SwitchStd5;

    static void Set_SwitchStd5(const double &Rl, const double &Ru)
    {
	SwitchStd5.Rl = Rl;
	SwitchStd5.Ru = Ru;

        double dR = Ru - Rl;

        SwitchStd5.A =  -6.0/(dR * dR * dR * dR * dR);
        SwitchStd5.B =  15.0/(dR * dR * dR * dR);
        SwitchStd5.C = -10.0/(dR * dR * dR);

        SwitchStd5.AA =  5.0 * SwitchStd5.A;
        SwitchStd5.BB =  4.0 * SwitchStd5.B;
        SwitchStd5.CC =  3.0 * SwitchStd5.C;
    }
};

#if !defined(MSD_COMPILE_CODE_ONLY)
UDF_State::SwitchStd5_StateParams UDF_State::SwitchStd5;
UDF_State::Shake_StateParams UDF_State::Shake;
UDF_State::Rattle_StateParams UDF_State::Rattle;
#endif

#endif

