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
 #ifndef DATARECEIVERTRAJECTORYSENDER_HPP
#define DATARECEIVERTRAJECTORYSENDER_HPP

#include <ostream>
#include <fstream>
#include <BlueMatter/DataReceiver.hpp>
#include <BlueMatter/ExternalDatagram.hpp>
#include <sys/types.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>

#define SOCKET_ERROR        -1

using namespace std;

class DataReceiverTrajectorySender : public DataReceiver
{
public:
    ofstream mStream;
    FILE *mFile;
    int mNNonWaterAtoms;
    int mNWaters;
    int mDoAppend;
    int mStartIndex;
    int mEndIndex;
    int mNoWaters;
    int mPortNum;
    int mSocket;
    
    void setParams(int portnum)
    {      
	mPortNum = portnum;
    }
    
    virtual void init()
    {
	int hServerSocket;
	struct hostent* pHostInfo;
	struct sockaddr_in Address;
	socklen_t nAddressSize=sizeof(struct sockaddr_in);

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
 
	wVersionRequested = MAKEWORD( 2, 2 );
 
	err = WSAStartup( wVersionRequested, &wsaData );	
#endif

	hServerSocket=socket(AF_INET,SOCK_STREAM,0);
	
	assert(hServerSocket != SOCKET_ERROR);
	    
        Address.sin_addr.s_addr=INADDR_ANY;
	Address.sin_port=htons(mPortNum);
	Address.sin_family=AF_INET;

	printf("About to bind\n");
	
	if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
	    == SOCKET_ERROR)
	{
	    printf("\nCould not connect to host\n");
	    exit(-1);
	}
	printf("About to getsockname\n");
	
	getsockname( hServerSocket, (struct sockaddr *) &Address, &nAddressSize);

	printf("About to listen\n");
	
	if(listen(hServerSocket, 3) == SOCKET_ERROR)
	{
	    printf("\nCould not listen\n");
	    exit(-1);
	}
	printf("About to accept\n");
	
        mSocket=accept(hServerSocket,(struct sockaddr*)&Address, &nAddressSize);
	printf("Accepted\n");
    }    

    virtual void final()
    {
	close(mSocket);
    }

    inline void ShowError(int m, int n, char *s)
    {
	if (m == n)
	    return;
	printf(s);
	printf("\n");
	exit(-1);
    }

    float TranslationDistance(float L, float x)
    {
	float a = fabs(x);
	if (a < L/2)
	    return 0;
	int n = (a-L/2)/L + 1;
	return (x > 0) ? n*L : -n*L;
    }

    // returns nonzero if no translation
    int TranslationVector(float L, XYZ &p, XYZ &v)
    {
	v.mX = TranslationDistance(L, p.mX);
	v.mY = TranslationDistance(L, p.mY);
	v.mZ = TranslationDistance(L, p.mZ);
	return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }

    // returns nonzero if no translation
    int TranslationVector(XYZ &r, float L, XYZ &p, XYZ &v)
    {
	v.mX = TranslationDistance(L, p.mX-r.mX);
	v.mY = TranslationDistance(L, p.mY-r.mY);
	v.mZ = TranslationDistance(L, p.mZ-r.mZ);
	return (v.mX == 0 && v.mY == 0 && v.mZ == 0);
    }

    void TranslateIntoBox(float L, tSiteData *p, int k, int count, int tag)
    {
	XYZ v;
	if (TranslationVector(L, p[k+tag].mPosition, v))
	    return;
	for (int i=k; i<k+count; i++)
	    p[i].mPosition -= v;
    }

#define STARTION 256
#define ENDION 258
#define NUMWATERS 1660
#define NUMNONWATERATOMS 259
#define BOXWIDTH 38.0
#define NUMHAIRPINATOMS 5239

    void MapHairpinIntoBox(tSiteData *sites)
    {
	int i;

	for (i=0; i<NUMWATERS; i++)
	    TranslateIntoBox(BOXWIDTH, sites, NUMNONWATERATOMS+3*i, 3, 1);
	for (i=STARTION; i<=ENDION; i++)
	    TranslateIntoBox(BOXWIDTH, sites, i, 1, 0);
    }


    void sendSites(int Index, int N, int CurrentStep, tSiteData *allsites)
    {
	int payload = 3*4*N+1+4+4+4;
	int nsent;
	char syncstr[] = "MDSend\0";
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(mSocket, &fdset);
	int nfds = mSocket+1;
	// printf("about to select\n");
	// select(nfds, NULL, &fdset, NULL, NULL);
	// printf("selected... about to send\n");
	nsent = send(mSocket, syncstr, 7, 0);
	ShowError(nsent, 7, "error in sync out");
	    
	// printf("Sent %d\n", nsent);
#ifdef WIN32
	int err = WSAGetLastError();
#endif
	// select(nfds, NULL, &fdset, NULL, NULL);
	nsent = send(mSocket, (char *)&payload, 4, 0);
	ShowError(nsent, 4, "error in payload");
	// select(nfds, NULL, &fdset, NULL, NULL);
	nsent = send(mSocket, "\0", 1, 0);
	ShowError(nsent, 1, "error in type");
	// select(nfds, NULL, &fdset, NULL, NULL);
	nsent = send(mSocket, (char *)&CurrentStep, 4, 0);
	ShowError(nsent, 4, "error in step" );
	// select(nfds, NULL, &fdset, NULL, NULL);
	nsent = send(mSocket, (char *)&N, 4, 0);
	ShowError(nsent, 4, "error in count");
	// select(nfds, NULL, &fdset, NULL, NULL);
	nsent = send(mSocket, (char *)&Index, 4, 0);
	ShowError(nsent, 4, "error in Index");
	// printf("about to send floats: index %d count %d Step %d\n", Index, N, CurrentStep);

	tSiteData *s = &allsites[Index];
	for (int i=0; i<N; i++, s++) {
	    float x = s->mPosition.mX;
	    float y = s->mPosition.mY;
	    float z = s->mPosition.mZ;
	    // select(nfds, NULL, &fdset, NULL, NULL);
	    nsent = send(mSocket, (char *)&x, 4, 0);
	    ShowError(nsent, 4, "error in floatx");
	    // select(nfds, NULL, &fdset, NULL, NULL);
	    nsent = send(mSocket, (char *)&y, 4, 0);
	    ShowError(nsent, 4, "error in floaty");
	    // select(nfds, NULL, &fdset, NULL, NULL);
	    nsent = send(mSocket, (char *)&z, 4, 0);
	    ShowError(nsent, 4, "error in floatz");
	}
	// sleep(0.1);
	// printf("just sent %d sites\n", N);
    }
    
#define MAXSITESPERPACKET 100

    virtual void sites(Frame *f)
    {
	// printf("sites called\n");
	
	unsigned CurrentStep = f->mOuterTimeStep;
	
	tSiteData *ps= f->mSiteData.getArray();
	int NSites = f->mSiteData.getSize();

        if (NSites == NUMHAIRPINATOMS)
	    MapHairpinIntoBox(ps);
	
	int CurrentIndex = 0;

	// printf("about to send sites for timestep %d, nsites=%d\n", CurrentStep, NSites);
	while (CurrentIndex < NSites) {
	    int NToSend = NSites-CurrentIndex;
	    if (NToSend > MAXSITESPERPACKET)
		NToSend = MAXSITESPERPACKET;
	    sendSites(CurrentIndex, NToSend, CurrentStep, ps);
	    CurrentIndex += NToSend;
	    // printf("about to sleep\n");
	    // sleep(0.1);
	    // printf("just slept\n");
	}
	printf("timestep %d sent\n", CurrentStep);
    }
};

#endif
