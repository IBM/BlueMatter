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
 #ifndef TAILFILE_HPP
#define TAILFILE_HPP

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#define O_BINARY 0
#else
#include <unistd.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <assert.h>
#include <fcntl.h>
#define O_RDONLY _O_RDONLY
#define O_BINARY _O_BINARY
#define sleep Sleep
#define usleep(a) Sleep((a)/1000)
#endif

#ifndef PKFXLOG_TAILFILE
#define PKFXLOG_TAILFILE (1) 
#endif
//class Tail_With_Buffered_Unistd
//{
//	private:
//    int mFd;
//    int mLastEnd;
//    int mSleepInterval;
//    int mWait;
//    int mTimedOut;
//    float mMaxWait;  // Max wait in milliseconds
//    long mLastSeek;
//    int mSeekAhead;
//    int mCurrentSeekAhead;
//    int mSeekable;
//    char mFileNameTemplate[1024];
//    int mFileIndex;
//    int mMultiFile;
//    char mFileName[1024];
//    int mHaveDataFromThisFile;
//    int mFirstFile;
//    
//    enum {
//    	kBufferSize = 1<<22 
//    } ; 
//    char mBuffer[kBufferSize] ;
//    int mBufferCount ;
//    int mBufferIndex ;
//    
//	public: 
//	Tail_With_Buffered_Unistd() 
//	{ 
//		mBufferIndex = 0 ; 
//        mBufferCount = 0 ; 
//        mLastEnd = -1;
//        mMaxWait = 100000;
//        mWait = 1;
//        mFd = -1;
//	    // mFd = NULL;
//        mTimedOut = 0;
//	mLastSeek = 0;
//	mSeekAhead = 0;
//	mCurrentSeekAhead = 0;
//	mSeekable = 1;
//	mMultiFile = 0;
//	mFileIndex = 0;
//	mHaveDataFromThisFile = 0;
//		
//	}
//	int Open(
//        char *aFileName,                 // name of file to tail
//        float aMaxWait = 100,            // seconds to wait before failing
//        int   aWait = 1,                 // wait for the file to open if it doesn't
//	long  aSeekAhead = 0,            // don't read until there are this many bytes to be read
//        int   aStartOffset = 0,          // start offset - if negative, bytes from current end of file
//        int   aSleepInterval = 10000    // arg to call to usleep when file hasn't advanced.
//	) 
//	{
//    	mBufferIndex = 0 ; 
//    	mBufferCount = 0 ; 
//		
//        mSleepInterval = aSleepInterval;
//        mMaxWait       = aMaxWait * 1000;
//        if (mMaxWait < 1)
//            mMaxWait = 1;
//        mWait          = aWait;
//	mSeekAhead = aSeekAhead;
//	mCurrentSeekAhead = mSeekAhead;
//	mFirstFile = 1;
//
//        if( aFileName[0] == '-' )
//        {
//           mFd = STDIN_FILENO;
//	       // mFd = stdin;
//	   mSeekable = 0;
//	   // cerr << "Setting seekable to 0" << endl;
//        }
//        else
//        {
//	   if (strstr(aFileName, "%")) {
//	       strcpy(mFileNameTemplate, aFileName);
//	       mMultiFile = 1;
//	       sprintf(mFileName, mFileNameTemplate, mFileIndex);
//           } else {
//	       sprintf(mFileName, aFileName);
//	   } 
//           mFd = open( mFileName , O_RDONLY | O_BINARY );
//           // mFd = fopen( mFileName , "rb" );
//           float t = 0;
//	   // poll slowly until file appears
//           // if( mWait && mFd < 0 )
//           if( mWait && mFd == NULL )
//           {
//               while( mFd < 0  && t < mMaxWait)
//               // while( mFd == NULL  && t < mMaxWait)
//               {
//                   sleep(1);
//                   mFd = open( mFileName , O_RDONLY );
//                   // mFd = fopen( mFileName , "rb");
//                   t += 1;
//               }
//           }
//        }
//        return (mFd >= 0 ? 1 : 0);
//	    // return (mFd != NULL ? 1 : 0);
//	} 
//	void Close() 
//	{
//        if (OK())
//             close( mFd );
//	    // fclose(mFd);
//	}
//	int OK()
//	{
//        return (mFd != -1 && mTimedOut != 1);
//        // return (mFd != NULL && mTimedOut != 1);
//	}
//	int Read(void *aBuffer, int aBufLen, int retry = 0)
//	{
//		if (aBufLen <= 0) {
//		    cerr << "Attempt to TailRead bad number of bytes " << aBufLen;
//		    cerr << " from file " << mFileName << endl;
//		    cerr << "An application is trying to read a non-positive number of bytes from an rdg." << endl;
//		    cerr << "This usually means something is wrong with the file." << endl;
//		    cerr << "Is it corrupt?  Is it ascii when it should be binary?" << endl;
//		    return 0;
//		}
//		// Have we got enough in the internal buffer to satisfy the request ?
//		if ( (mBufferCount-mBufferIndex) >= aBufLen )
//		{
//			// Yes. Copy it from the buffer and return.
//			memcpy(aBuffer,mBuffer+mBufferIndex,aBufLen) ;
//			mBufferIndex += aBufLen ;
//			return 1 ;
//		}
//		// Have we got something in the internal buffer that we can clear out ?
//		if ( mBufferCount < mBufferIndex )
//		{
//			// Yes. This will put something into the caller's buffer
//			memcpy(aBuffer, mBuffer+mBufferIndex, mBufferCount-mBufferIndex) ;
//			aBuffer += (mBufferCount-mBufferIndex) ;
//			aBufLen -= (mBufferCount-mBufferIndex) ;
//		}
//		// The internal buffer is now empty
//	}
//} ;

class Tail_With_Unistd
{
	private:
    int mFd;
    // FILE *mFd;
    int mLastEnd;
    int mSleepInterval;
    int mWait;
    int mTimedOut;
    float mMaxWait;  // Max wait in milliseconds
    long mLastSeek;
    int mSeekAhead;
    int mCurrentSeekAhead;
    int mSeekable;
    char mFileNameTemplate[1024];
    int mFileIndex;
    int mMultiFile;
    char mFileName[1024];
    int mHaveDataFromThisFile;
    int mFirstFile;
public:

    Tail_With_Unistd()
    {
        mLastEnd = -1;
        mMaxWait = 100000;
        mWait = 1;
        mFd = -1;
	    // mFd = NULL;
        mTimedOut = 0;
	mLastSeek = 0;
	mSeekAhead = 0;
	mCurrentSeekAhead = 0;
	mSeekable = 1;
	mMultiFile = 0;
	mFileIndex = 0;
	mHaveDataFromThisFile = 0;
	
    }

    int Open(
        char *aFileName,                 // name of file to tail
        float aMaxWait = 100,            // seconds to wait before failing
        int   aWait = 1,                 // wait for the file to open if it doesn't
	long  aSeekAhead = 0,            // don't read until there are this many bytes to be read
        int   aStartOffset = 0,          // start offset - if negative, bytes from current end of file
        int   aSleepInterval = 10000)    // arg to call to usleep when file hasn't advanced.
    {
        mSleepInterval = aSleepInterval;
        mMaxWait       = aMaxWait * 1000;
        if (mMaxWait < 1)
            mMaxWait = 1;
        mWait          = aWait;
	mSeekAhead = aSeekAhead;
	mCurrentSeekAhead = mSeekAhead;
	mFirstFile = 1;

        if( aFileName[0] == '-' )
        {
           mFd = STDIN_FILENO;
	       // mFd = stdin;
	   mSeekable = 0;
	   // cerr << "Setting seekable to 0" << endl;
        }
        else
        {
	   if (strstr(aFileName, "%")) {
	       strcpy(mFileNameTemplate, aFileName);
	       mMultiFile = 1;
	       sprintf(mFileName, mFileNameTemplate, mFileIndex);
           } else {
	       sprintf(mFileName, aFileName);
	   } 
           mFd = open( mFileName , O_RDONLY | O_BINARY );
           // mFd = fopen( mFileName , "rb" );
           float t = 0;
	   // poll slowly until file appears
           // if( mWait && mFd < 0 )
           if( mWait && mFd == NULL )
           {
               while( mFd < 0  && t < mMaxWait)
               // while( mFd == NULL  && t < mMaxWait)
               {
                   sleep(1);
                   mFd = open( mFileName , O_RDONLY );
                   // mFd = fopen( mFileName , "rb");
                   t += 1;
               }
           }
        }
        return (mFd >= 0 ? 1 : 0);
	    // return (mFd != NULL ? 1 : 0);
    }

    void Close()
    {
        if (OK())
             close( mFd );
	    // fclose(mFd);
    }

    int OK()
    {
        return (mFd != -1 && mTimedOut != 1);
        // return (mFd != NULL && mTimedOut != 1);
    }

    int Read( void *aBuffer, int aBufLen, int retry = 0 )
    {
	double t=0;
        double tick = mSleepInterval/1000.0;

	if (aBufLen <= 0) {
	    cerr << "Attempt to TailRead bad number of bytes " << aBufLen;
	    cerr << " from file " << mFileName << endl;
	    cerr << "An application is trying to read a non-positive number of bytes from an rdg." << endl;
	    cerr << "This usually means something is wrong with the file." << endl;
	    cerr << "Is it corrupt?  Is it ascii when it should be binary?" << endl;
	    return 0;
	}

        assert( mFd != -1 );
        // assert( mFd != NULL );

	if (mSeekable) {
	    if (retry)
		lseek(mFd, mLastSeek, SEEK_SET);
		// fseek(mFd, mLastSeek, SEEK_SET);
	    else
		mLastSeek = lseek(mFd, 0, SEEK_CUR);
		// mLastSeek = fseek(mFd, 0, SEEK_CUR);

	    // wait until buffer is very full
	    // this just makes sure it is primed for subsequent read
	    // if timeout, seekahead is wound down
	    if (mCurrentSeekAhead > 0) {
		long endseek = lseek(mFd, 0, SEEK_END);
		// int endseek = fseek(mFd, 0, SEEK_END);
		// cerr << "curseekahead, endseek " << mCurrentSeekAhead << " " << endseek << endl;
		while ((endseek-mLastSeek-aBufLen < mSeekAhead) && (t < mMaxWait)) {
		    // cerr << "seekahead pause0: endseek, last, buflen, currentseekahed " <<
			// endseek << " " << mLastSeek << " " << aBufLen << " " << mCurrentSeekAhead << endl;
		    usleep(mSleepInterval);
		    t += tick;
		    endseek = lseek(mFd, 0, SEEK_END);
		    // endseek = fseek(mFd, 0, SEEK_END);
		}
		lseek(mFd, mLastSeek, SEEK_SET);
		// fseek(mFd, mLastSeek, SEEK_SET);
		// if data appear to have stopped arriving, but there is stuff there...
		if (endseek-mLastSeek-aBufLen < mCurrentSeekAhead) {
		    // cerr << "seekahead pause1: endseek, last, buflen, currentseekahed " <<
			// endseek << " " << mLastSeek << " " << aBufLen << " " << mCurrentSeekAhead << endl;
		    mCurrentSeekAhead = 0;
		}
	    }
	}

        int TotalBytesRead = ::read( mFd, aBuffer, aBufLen );
        // int TotalBytesRead = fread( aBuffer, 1, aBufLen, mFd );
	if (TotalBytesRead > 0 && TotalBytesRead != EOF)
	    mHaveDataFromThisFile = 1;
	if (TotalBytesRead < 0 || TotalBytesRead == EOF) {
	    cerr << "NEGATIVE BYTES READ " << TotalBytesRead << " bytes to read " << aBufLen << " mFd " << mFd << endl;
	    cerr << "Error reading from file " << mFileName << endl;
	    perror("ERROR ");
	    return 0;
	}
        if (TotalBytesRead == aBufLen)
            return 1;

        char *p = (char *)aBuffer;
        p += TotalBytesRead;
        int BytesToRead = aBufLen - TotalBytesRead;
	int gotdata;

	int SubWait = 20000;  // for multifile, wait no longer than 20 seconds;

	// always wait at least 20 seconds
	if (mMaxWait < SubWait)
	    SubWait = mMaxWait;

	// If it is the first file and no data read yet, wait maxwait
	if (mFirstFile && !mHaveDataFromThisFile)
	    SubWait = mMaxWait;

	// If not multifile, always wait maxwait
	if (!mMultiFile)
	    SubWait = mMaxWait;

	int TryNewFile;

	int problem = 0;
	do {
	    t = 0;
	    TryNewFile = 0;

	    // THIS IS THE MAJOR WAIT LOOP OF THIS TAIL PROGRAM
	    while( (BytesToRead > 0) && (t < SubWait) && !problem)
	    {
	    	BegLogLine(PKFXLOG_TAILFILE)
	    	  << "Waiting in Tail, to read: " << BytesToRead << " " << t
	    	  << " " << SubWait << " " << mMaxWait 
	    	  << EndLogLine ;
		// cerr << "Waiting in Tail, to read: " << BytesToRead << " " << t;
		// cerr << " " << SubWait << " " << mMaxWait << endl;
		usleep( mSleepInterval );
		int BytesRead = ::read( mFd, p, BytesToRead );
		// int BytesRead = fread( p, 1, BytesToRead, mFd );
		if (BytesRead == EOF || BytesRead < 0) {
		    BytesRead = 0;
		}
		TotalBytesRead += BytesRead;
		BytesToRead -= BytesRead;
		p += BytesRead;
		t += tick;
		if (BytesRead > 0) {
		    mHaveDataFromThisFile = 1;
		}
	    }

	    // There has been a pause or a problem
	    // Make note if time has exceeded either the main timeout or
	    // the timeout between files - if multifile
	    if (t >= SubWait) {
		mTimedOut = 1;
		// cerr << "subwait timeout, timedout, multifile, havedata " << mTimedOut;
		// cerr << " " << mMultiFile << " " << mHaveDataFromThisFile << endl;
	    }

	    // Now, if it is a timeout and multifile and really did get data from
	    // previous file, then open the next file
	    if (mTimedOut && mMultiFile && mHaveDataFromThisFile) {
		// cerr << "in create new file" << endl;
		close(mFd);
		// fclose(mFd);
		mTimedOut = 0;
		mFileIndex++;
		sprintf(mFileName, mFileNameTemplate, mFileIndex);
		mFd = open( mFileName , O_RDONLY | O_BINARY );
		// mFd = fopen( mFileName , "rb" );
		// if the next file isn't there yet, it must be at last file
		if (mFd == 0)
		    problem = 1;
		mHaveDataFromThisFile = 0;
		TryNewFile = 1;
		mCurrentSeekAhead = mSeekAhead;
		// cerr << "Timedout, opening " << mFileName << endl;
	    }
	} while (!mTimedOut && mMultiFile && TryNewFile && !problem);

	// Many modes of rdg parsing have normal timeout at end
	// so this is not an error
        if (mTimedOut || (TotalBytesRead != aBufLen)) {
	    // cerr << "Tailfile error " << mFileName << endl;
	    // cerr << "Bytes To Read, tcount, maxtcount, timedout, totalread, buflen" << endl;
	    // cerr << BytesToRead << " " << t << " " << mMaxWait << " " << mTimedOut;
	    // cerr << " " << TotalBytesRead << " " << aBufLen << " " << endl;
	}
	mFirstFile = 0;
        return (TotalBytesRead == aBufLen ? 1 : 0);
    }
	
} ;

class Tail_With_Stdio
{
private:
    //int mFd;
    FILE *mFd;
    int mLastEnd;
    int mSleepInterval;
    int mWait;
    int mTimedOut;
    float mMaxWait;  // Max wait in milliseconds
    long mLastSeek;
    int mSeekAhead;
    int mCurrentSeekAhead;
    int mSeekable;
    char mFileNameTemplate[1024];
    int mFileIndex;
    int mMultiFile;
    char mFileName[1024];
    int mHaveDataFromThisFile;
    int mFirstFile;
public:

    Tail_With_Stdio()
    {
        mLastEnd = -1;
        mMaxWait = 100000;
        mWait = 1;
        // mFd = -1;
	mFd = NULL;
        mTimedOut = 0;
	mLastSeek = 0;
	mSeekAhead = 0;
	mCurrentSeekAhead = 0;
	mSeekable = 1;
	mMultiFile = 0;
	mFileIndex = 0;
	mHaveDataFromThisFile = 0;
    }

    int Open(
        char *aFileName,                 // name of file to tail
        float aMaxWait = 100,            // seconds to wait before failing
        int   aWait = 1,                 // wait for the file to open if it doesn't
	long  aSeekAhead = 0,            // don't read until there are this many bytes to be read
        int   aStartOffset = 0,          // start offset - if negative, bytes from current end of file
        int   aSleepInterval = 10000)    // arg to call to usleep when file hasn't advanced.
    {
        mSleepInterval = aSleepInterval;
        mMaxWait       = aMaxWait * 1000;
        if (mMaxWait < 1)
            mMaxWait = 1;
        mWait          = aWait;
	mSeekAhead = aSeekAhead;
	mCurrentSeekAhead = mSeekAhead;
	mFirstFile = 1;

        if( aFileName[0] == '-' )
        {
           // mFd = STDIN_FILENO;
	   mFd = stdin;
	   mSeekable = 0;
	   // cerr << "Setting seekable to 0" << endl;
        }
        else
        {
	   if (strstr(aFileName, "%")) {
	       strcpy(mFileNameTemplate, aFileName);
	       mMultiFile = 1;
	       sprintf(mFileName, mFileNameTemplate, mFileIndex);
           } else {
	       sprintf(mFileName, aFileName);
	   } 
           // mFd = open( mFileName , O_RDONLY | O_BINARY );
           mFd = fopen( mFileName , "rb" );
           float t = 0;
	   // poll slowly until file appears
           // if( mWait && mFd < 0 )
           if( mWait && mFd == NULL )
           {
               // while( mFd < 0  && t < mMaxWait)
               while( mFd == NULL  && t < mMaxWait)
               {
                   sleep(1);
                   // mFd = open( mFileName , O_RDONLY );
                   mFd = fopen( mFileName , "rb");
                   t += 1;
               }
           }
        }
        // return (mFd >= 0 ? 1 : 0);
	return (mFd != NULL ? 1 : 0);
    }

    void Close()
    {
        if (OK())
            // close( mFd );
	    fclose(mFd);
    }

    int OK()
    {
        // return (mFd != -1 && mTimedOut != 1);
        return (mFd != NULL && mTimedOut != 1);
    }

    int Read( void *aBuffer, int aBufLen, int retry = 0 )
    {
	double t=0;
        double tick = mSleepInterval/1000.0;

	if (aBufLen <= 0) {
	    cerr << "Attempt to TailRead bad number of bytes " << aBufLen;
	    cerr << " from file " << mFileName << endl;
	    cerr << "An application is trying to read a non-positive number of bytes from an rdg." << endl;
	    cerr << "This usually means something is wrong with the file." << endl;
	    cerr << "Is it corrupt?  Is it ascii when it should be binary?" << endl;
	    return 0;
	}

        // assert( mFd != -1 );
        assert( mFd != NULL );

	if (mSeekable) {
	    if (retry)
		// lseek(mFd, mLastSeek, SEEK_SET);
		fseek(mFd, mLastSeek, SEEK_SET);
	    else
		// mLastSeek = lseek(mFd, 0, SEEK_CUR);
		mLastSeek = fseek(mFd, 0, SEEK_CUR);

	    // wait until buffer is very full
	    // this just makes sure it is primed for subsequent read
	    // if timeout, seekahead is wound down
	    if (mCurrentSeekAhead > 0) {
		// long endseek = lseek(mFd, 0, SEEK_END);
		int endseek = fseek(mFd, 0, SEEK_END);
		// cerr << "curseekahead, endseek " << mCurrentSeekAhead << " " << endseek << endl;
		while ((endseek-mLastSeek-aBufLen < mSeekAhead) && (t < mMaxWait)) {
		    // cerr << "seekahead pause0: endseek, last, buflen, currentseekahed " <<
			// endseek << " " << mLastSeek << " " << aBufLen << " " << mCurrentSeekAhead << endl;
		    usleep(mSleepInterval);
		    t += tick;
		    // endseek = lseek(mFd, 0, SEEK_END);
		    endseek = fseek(mFd, 0, SEEK_END);
		}
		// lseek(mFd, mLastSeek, SEEK_SET);
		fseek(mFd, mLastSeek, SEEK_SET);
		// if data appear to have stopped arriving, but there is stuff there...
		if (endseek-mLastSeek-aBufLen < mCurrentSeekAhead) {
		    // cerr << "seekahead pause1: endseek, last, buflen, currentseekahed " <<
			//endseek << " " << mLastSeek << " " << aBufLen << " " << mCurrentSeekAhead << endl;
		    mCurrentSeekAhead = 0;
		}
	    }
	}

        // int TotalBytesRead = ::read( mFd, aBuffer, aBufLen );
        int TotalBytesRead = fread( aBuffer, 1, aBufLen, mFd );
	if (TotalBytesRead > 0 && TotalBytesRead != EOF)
	    mHaveDataFromThisFile = 1;
	if (TotalBytesRead < 0 || TotalBytesRead == EOF) {
	    cerr << "NEGATIVE BYTES READ " << TotalBytesRead << " bytes to read " << aBufLen << " mFd " << mFd << endl;
	    cerr << "Error reading from file " << mFileName << endl;
	    perror("ERROR ");
	    return 0;
	}
        if (TotalBytesRead == aBufLen)
            return 1;

        char *p = (char *)aBuffer;
        p += TotalBytesRead;
        int BytesToRead = aBufLen - TotalBytesRead;
	int gotdata;

	int SubWait = 20000;  // for multifile, wait no longer than 20 seconds;

	// always wait at least 20 seconds
	if (mMaxWait < SubWait)
	    SubWait = mMaxWait;

	// If it is the first file and no data read yet, wait maxwait
	if (mFirstFile && !mHaveDataFromThisFile)
	    SubWait = mMaxWait;

	// If not multifile, always wait maxwait
	if (!mMultiFile)
	    SubWait = mMaxWait;

	int TryNewFile;

	int problem = 0;
	do {
	    t = 0;
	    TryNewFile = 0;

	    // THIS IS THE MAJOR WAIT LOOP OF THIS TAIL PROGRAM
	    while( (BytesToRead > 0) && (t < SubWait) && !problem)
	    {
		// cerr << "Waiting in Tail, to read: " << BytesToRead << " " << t;
		// cerr << " " << SubWait << " " << mMaxWait << endl;
		usleep( mSleepInterval );
		// int BytesRead = ::read( mFd, p, BytesToRead );
		int BytesRead = fread( p, 1, BytesToRead, mFd );
		if (BytesRead == EOF || BytesRead < 0) {
		    BytesRead = 0;
		}
		TotalBytesRead += BytesRead;
		BytesToRead -= BytesRead;
		p += BytesRead;
		t += tick;
		if (BytesRead > 0) {
		    mHaveDataFromThisFile = 1;
		}
	    }

	    // There has been a pause or a problem
	    // Make note if time has exceeded either the main timeout or
	    // the timeout between files - if multifile
	    if (t >= SubWait) {
		mTimedOut = 1;
		// cerr << "subwait timeout, timedout, multifile, havedata " << mTimedOut;
		// cerr << " " << mMultiFile << " " << mHaveDataFromThisFile << endl;
	    }

	    // Now, if it is a timeout and multifile and really did get data from
	    // previous file, then open the next file
	    if (mTimedOut && mMultiFile && mHaveDataFromThisFile) {
		// cerr << "in create new file" << endl;
		// close(mFd);
		fclose(mFd);
		mTimedOut = 0;
		mFileIndex++;
		sprintf(mFileName, mFileNameTemplate, mFileIndex);
		// mFd = open( mFileName , O_RDONLY | O_BINARY );
		mFd = fopen( mFileName , "rb" );
		// if the next file isn't there yet, it must be at last file
		if (mFd == NULL)
		    problem = 1;
		mHaveDataFromThisFile = 0;
		TryNewFile = 1;
		mCurrentSeekAhead = mSeekAhead;
		// cerr << "Timedout, opening " << mFileName << endl;
	    }
	} while (!mTimedOut && mMultiFile && TryNewFile && !problem);

	// Many modes of rdg parsing have normal timeout at end
	// so this is not an error
        if (mTimedOut || (TotalBytesRead != aBufLen)) {
	    // cerr << "Tailfile error " << mFileName << endl;
	    // cerr << "Bytes To Read, tcount, maxtcount, timedout, totalread, buflen" << endl;
	    // cerr << BytesToRead << " " << t << " " << mMaxWait << " " << mTimedOut;
	    // cerr << " " << TotalBytesRead << " " << aBufLen << " " << endl;
	}
	mFirstFile = 0;
        return (TotalBytesRead == aBufLen ? 1 : 0);
    }
};

class Tail_With_Stdio_2
{
	public:
    FILE * mFd ;
    char mFileName[1024];
    char mFileNameTemplate[1024];
    int mFileIndex;
    int mMaxWait;
    int mTimedOut ;     
    int Open(
        char *aFileName,                 // name of file to tail
        float aMaxWait = 100,            // seconds to wait before failing
        int   aWait = 1,                 // wait for the file to open if it doesn't
	long  aSeekAhead = 0,            // don't read until there are this many bytes to be read
        int   aStartOffset = 0,          // start offset - if negative, bytes from current end of file
        int   aSleepInterval = 10000)    // arg to call to usleep when file hasn't advanced.
    {
    	mMaxWait = aMaxWait ;
    	mTimedOut = 0 ;
    	strcpy(mFileNameTemplate,aFileName) ;
    	sprintf(mFileName,mFileNameTemplate,0) ;
    	mFileIndex = 1 ;
    	mFd = fopen(mFileName,"rb") ;
//    	cerr << "Max wait time " << mMaxWait ;
    	return ( mFd != NULL ) ? 1 : 0 ;
    }
        void Close()
    {
    	fclose(mFd) ;
    }
     int OK()
    {
    	        return (mFd != NULL && mTimedOut != 1);
    	
    }
     int Read( void *aBuffer, int aBufLen, int retry = 0 )
    {
    	if (aBufLen <= 0) {
	    cerr << "Attempt to TailRead bad number of bytes " << aBufLen
	         << " from file " << mFileName << endl
	         << "An application is trying to read a non-positive number of bytes from an rdg." << endl
	         << "This usually means something is wrong with the file." << endl
	         << "Is it corrupt?  Is it ascii when it should be binary?" << endl;
	    return 0;
	    }

        assert( mFd != NULL );
    	 int TotalBytesRead = fread( aBuffer, 1, aBufLen, mFd );
        if (TotalBytesRead == aBufLen)   return 1; // Plenty of data available

        // Partial read. Will have to do some more work
        char *p = (char *)aBuffer;
	    int BytesToRead = aBufLen - TotalBytesRead;
	    int WaitToGo = mMaxWait ;
        while ( BytesToRead > 0 && WaitToGo > 0)
        {
	        p += TotalBytesRead;
	        if ( TotalBytesRead > 0 ) WaitToGo = mMaxWait ; // Making progress; reset the spin count
	        // See if there is a new file
	        char nextFileName[1024] ;
	        sprintf(nextFileName,mFileNameTemplate,mFileIndex) ;
	        if ( 0 != strcmp(nextFileName,mFileName) ) 
	        {
	        	// we have continuation files. See if it exists yet
	        	FILE * nextFd = fopen( nextFileName , "rb" );
	        	if ( nextFd != NULL )
	        	{
	        		// It exists. move to it straight away.
	        		fclose(mFd) ;
	        		strcpy(mFileName, nextFileName) ;
	        		mFileIndex += 1 ;
	        		mFd = nextFd ;
	        		TotalBytesRead = fread( p, 1, BytesToRead, mFd );
	        	} else {
	        		// No new file yet. Sleep a while and try the old one again
	        		sleep(1) ;
	                WaitToGo -= 1 ;
	        		clearerr(mFd) ; // Hope this will cause the 'read' syscall to be retried
	        		TotalBytesRead = fread( p, 1, BytesToRead, mFd );
	        		
	        	}
	        	
	        } else {
	        	// No continuation files
	        	sleep(1) ;
   	            WaitToGo -= 1 ;
	        	clearerr(mFd) ; // Hope this will cause the 'read' syscall to be retried
	        	TotalBytesRead = fread( p, 1, BytesToRead, mFd );
	        }
	        BytesToRead -= TotalBytesRead ;
        }
        mTimedOut = ( BytesToRead == 0 ) ? 0 : 1 ;
        return ( BytesToRead == 0 ) ? 1 : 0 ;
    	
    }   
	
	
} ;

class Tail: public Tail_With_Stdio_2
{
} ;

#endif
