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
 #include <asan_memory_server.h>

#ifndef PKFXLOG_MEMORY_SERVER
#define PKFXLOG_MEMORY_SERVER (0) 
#endif

void ASAN_MemoryServer::Connect( int sockD, int port, in_addr_t IONodeIP )
  {
    BegLogLine( PKFXLOG_MEMORY_SERVER ) 
      << "Connect:: Entering: sockD: " << sockD
      << " port: " << port 
      << " IPNodeIP: " <<  IONodeIP
      << EndLogLine;
    
  struct sockaddr_in RemoteAddr;
  RemoteAddr.sin_family = AF_INET;
  RemoteAddr.sin_port   = port ;
  RemoteAddr.sin_addr.s_addr = IONodeIP;
  
  while( 1 )
    {
      int rc = connect( sockD, 
      (struct sockaddr *) & RemoteAddr,
      sizeof( struct sockaddr_in ) );
      
      if( rc < 0 )
  {
    if( errno == ECONNREFUSED || errno == ECONNABORTED 
        || errno == ETIMEDOUT || errno == ECONNRESET || errno == EINVAL
        )
      {      
        sleep( 1 );
        continue;
      }
    else
      StrongAssertLogLine( 0 )
        << "ASAN_MemoryServer:: Error on connecting to gpfs"
        << EndLogLine;
  }
      else
  break;
    }    

  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "Connect:: Leaving: connected on"
    << " port: " << port 
    << " sockd: " << sockD 
    << EndLogLine;
  }

in_addr_t ASAN_MemoryServer::GetIONodeIP()
{
  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "GetIONodeIP:: Entering: "
    << EndLogLine;
  
  char* hostnameFile = "/var/asan/hostname";

  int fd = open( hostnameFile, O_RDONLY, S_IRUSR );

  StrongAssertLogLine( fd >= 0)
    << "GetIONodeIP:: Failed on file open"
    << " errno: " << errno
    << EndLogLine;

  char IP_String[ 64 ];

  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "GetIONodeIP:: About to read I/O node ip"
    << EndLogLine;

  int rlen = read( fd, IP_String, 64 );
  
  StrongAssertLogLine( rlen >= 0)
    << "GetIONodeIP:: Failed on file read"
    << " while reading file: " << hostnameFile
    << " errno: " << errno
    << EndLogLine;

  IP_String[ rlen + 1 ] = 0;

  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "GetIONodeIP:: IP_String: " << IP_String
    << EndLogLine;

  in_addr_t IO_node_IP =  inet_addr( IP_String ); 
  
  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "GetIONodeIP:: IO_node_IP: " << IO_node_IP
    << EndLogLine;
  
  return IO_node_IP;
}

void ASAN_MemoryServer::SendDisconnectionAck( int sockD )
{
  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "SendDisconnectionAck: About to send a disconnection ack on socket: " 
    <<  sockD 
    << EndLogLine;

  Gpfs2CN_DisconnectResponse DisconnectResponse;
  DisconnectResponse.Ack = GPFS2CN_ACK_OK;
  unsigned int wlen = writeAll( sockD, 
        (char *) &DisconnectResponse, 
        sizeof( Gpfs2CN_DisconnectResponse ));
  
  StrongAssertLogLine( wlen == sizeof( Gpfs2CN_DisconnectResponse ) )
    << "wlen: " << wlen
    << " sizeof( Gpfs2CN_DisconnectResponse ): " << sizeof( Gpfs2CN_DisconnectResponse )
    << EndLogLine;
}

void ASAN_MemoryServer::SendConnectionAck( int sockD )
{
  BegLogLine( PKFXLOG_MEMORY_SERVER )
    << "SendConnectionAck: About to send a connection ack on socket: " 
    << sockD
    << EndLogLine;

  Gpfs2CN_ConnectResponse ConnectResponse;
  ConnectResponse.Ack = GPFS2CN_ACK_OK;
  unsigned int wlen = writeAll( sockD, (char *) &ConnectResponse, sizeof( Gpfs2CN_ConnectResponse ) );

  StrongAssertLogLine( wlen == sizeof( Gpfs2CN_ConnectResponse ) )
    << "wlen: " << wlen
    << " sizeof( Gpfs2CN_ConnectResponse ): " << sizeof( Gpfs2CN_ConnectResponse )
    << EndLogLine;  
}

void ASAN_MemoryServer::ConnectAndAck( int sockD, int port, in_addr_t IONodeIP )
  {
    Connect( sockD, port, IONodeIP );

    BegLogLine( PKFXLOG_MEMORY_SERVER ) 
      << "ConnectAndAck:: Connected on port: " 
      << port 
      << EndLogLine;
    
    SendConnectionAck( sockD );
  }

void ASAN_MemoryServer::AcceptConnection( int sockD )
{
  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "AcceptConnection:: on socket: " 
    << sockD 
    << EndLogLine;

  Gpfs2CN_Header hdr;
  
  unsigned int rlen = readAll( sockD , (char *) &hdr, sizeof( Gpfs2CN_Header ) );
    
  if( hdr.Op == GPFS2CN_OPERATION_CONNECT )
    {	    
      SendConnectionAck( sockD );
    }
  else
    StrongAssertLogLine( 0 )
      << "Error on trying to connect"
      << EndLogLine;

  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "AcceptConnection:: Connection accepted: on " 
    << sockD 
    << EndLogLine;
}

in_addr_t ASAN_MemoryServer::GenerateDiskPortFile()
  {
  in_addr_t IONodeIP =  GetIONodeIP();
  
  int CommSize = PkNodeGetCount();
  int Rank = PkNodeGetId();

  // Get the ordinal of the compute node
  // under an I/O node
  int SendSize = sizeof( in_addr_t ) * CommSize;
  in_addr_t* IONodeAddressSend = (in_addr_t *) PkHeapAllocate( SendSize );
  StrongAssertLogLine( IONodeAddressSend )
    << "IONodeAddressSend: " << (void *) IONodeAddressSend
    << EndLogLine;
    
  bzero( IONodeAddressSend, sizeof( in_addr_t ) * CommSize );
  IONodeAddressSend[ Rank ] = IONodeIP;

  in_addr_t* IONodeAddressRecv = (in_addr_t *) PkHeapAllocate( SendSize );
  StrongAssertLogLine( IONodeAddressRecv )
    << "IONodeAddressRecv: " << (void *) IONodeAddressRecv
    << EndLogLine;

  StrongAssertLogLine( sizeof( int ) == sizeof( in_addr_t ) )
    << "sizeof( int ): " << sizeof( int )
    << " sizeof( in_addr_t ): " << sizeof( in_addr_t )
    << EndLogLine;

  BegLogLine( PKFXLOG_MEMORY_SERVER )
    << "ASAN_MemoryServer::GenerateDiskPortFile():: Before GlobalTreeReduce()"
    << EndLogLine;

  GlobalTreeReduce( (unsigned long *) IONodeAddressSend,
        (unsigned long *) IONodeAddressRecv,
        SendSize,
         _BGL_TREE_OPCODE_ADD );

  BegLogLine( PKFXLOG_MEMORY_SERVER )
    << "ASAN_MemoryServer::GenerateDiskPortFile():: After GlobalTreeReduce()"
    << EndLogLine;

  int MyOrdinalInIONode = 0;
  for( int i = 0; i < Rank; i++ )
    {
      if( IONodeAddressRecv[ i ] == IONodeIP )
  {
    MyOrdinalInIONode++;
  }
    }

  PkHeapFree( IONodeAddressSend );
  PkHeapFree( IONodeAddressRecv );
  
  BegLogLine( PKFXLOG_MEMORY_SERVER )
    << "MemoryServer:: MyOrdinalInIONode: " <<  MyOrdinalInIONode
    << EndLogLine;
  
  char PortFilename[ 64 ]; 
  sprintf( PortFilename, "/var/asan/disk_port.%d", MyOrdinalInIONode );
  int port_fd = open( PortFilename, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR );

  StrongAssertLogLine( port_fd >= 0)
    << "Failed on file open"
    << " errno: " << errno
    << EndLogLine;

  AssignedPort = CONNECTION_SERVER_START_PORT + MyOrdinalInIONode;

  BegLogLine( PKFXLOG_MEMORY_SERVER )
    << "MemoryServer:: AssignedPort: " 
    << AssignedPort
    << EndLogLine;

  int wlen = write( port_fd, &AssignedPort, sizeof( int ) );

  StrongAssertLogLine( wlen >= 0 )
    << "Failed on file write"
    << " errno: " << errno
    << EndLogLine;
  
  int crc = close( port_fd  );

  StrongAssertLogLine( crc >= 0)
    << "Failed on file close"
    << " errno: " << errno
    << EndLogLine;

  return IONodeIP;
  }

char * ASAN_BufferManager::Buffer = NULL;
int ASAN_BufferManager::BufferSize = 0;

void ASAN_BufferManager::AllocateBuffer( int MBCount )
  {
  int   MemoryBufferMB   = MBCount;
  BufferSize = MemoryBufferMB * 1024 * 1024;
  Buffer     = (char *) PkHeapAllocate( BufferSize );
  
  while( Buffer == NULL )
    {
      MemoryBufferMB--;
      BufferSize = MemoryBufferMB * 1024 * 1024;
      Buffer     = (char *) PkHeapAllocate( BufferSize );      
      
      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
  << "ASAN_BufferManager::AllocateBuffer : " << (void *) Buffer
  << " MemoryBufferMB: " << MemoryBufferMB
  << " BufferSize: " <<  BufferSize 
  << EndLogLine;
    }

  bzero( Buffer, BufferSize );

  BegLogLine( PKFXLOG_MEMORY_SERVER ) 
    << "ASAN_BufferManager::AllocateBuffer : " << (void *) Buffer
    << " MemoryBufferMB: " << MemoryBufferMB
    << " BufferSize: " <<  BufferSize 
    << EndLogLine;
  }

void ASAN_MemoryServer::Init()
{
  CurrentState = MEMORY_SERVER_STATE_START;
  BufferPtr = NULL;
  sockfd = -1;
  AssignedPort = -1;

}

void ASAN_MemoryServer::SetBufferPtr( char * aBufferPtr )
{
  BufferPtr = aBufferPtr;
}

char* 
ASAN_BufferManager::GetBufferPtr()
{
  return Buffer;
}

int 
ASAN_BufferManager::GetBufferSize()
{
  return BufferSize;
}

//int main(int argc, char* argv[])
void ASAN_MemoryServer::Run()
{
  Init();
  in_addr_t IONodeIP = GenerateDiskPortFile();
  ASAN_BufferManager::AllocateBuffer( 500 );
  SetBufferPtr( ASAN_BufferManager::GetBufferPtr() );
  
  // Signal the I/O node to start gpfs
  char SignalFile[] = "/var/asan/ComputeNodeReady";
  int fd = open( SignalFile, 
     O_CREAT | O_TRUNC | O_RDWR,
     S_IRUSR | S_IWUSR );

  StrongAssertLogLine( fd >= 0 )
    << "ASAN_MemoryServer:: Error creating the file: " << fd
    << "errno: " << errno
    << EndLogLine;

  close( fd );
     
  BegLogLine( PKFXLOG_MEMORY_SERVER )
    << "ASAN_MemoryServer:: Created a file: " <<  SignalFile 
    << EndLogLine;

  while( 1 )
    {      
      BegLogLine( PKFXLOG_MEMORY_SERVER )
  << "ASAN_MemoryServer:: Entering current state: " <<  CurrentState
  << EndLogLine;
      
      switch( CurrentState )
  {
  case MEMORY_SERVER_STATE_START:
    {
      BegLogLine( PKFXLOG_MEMORY_SERVER )
        << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_START"
        << EndLogLine;

      // Should be in this state only ONCE at the start of the program.
      sockfd = socket(AF_INET, SOCK_STREAM, 0 );
      
      BegLogLine( PKFXLOG_MEMORY_SERVER )
        << "ASAN_MemoryServer:: MemoryDevice:: About to connect " 
        << " AssignedPort: " << AssignedPort
        << " sockfd: " << sockfd
        << EndLogLine;

      // Initial connection
      Connect( sockfd, AssignedPort, IONodeIP );
      
      AcceptConnection( sockfd );
      
      BegLogLine( PKFXLOG_MEMORY_SERVER )
        << "ASAN_MemoryServer:: MemoryDevice:: Connected " 
        << " AssignedPort: " << AssignedPort
        << " sockfd: " << sockfd
        << EndLogLine;
     
      CurrentState = MEMORY_SERVER_STATE_CONNECTED;    

      break;
    }
  case MEMORY_SERVER_STATE_CONNECTED:
    {
      BegLogLine( PKFXLOG_MEMORY_SERVER )
        << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_CONNECTED"
        << EndLogLine;

      // Read and process incoming events
      Gpfs2CN_Header hdr;
      unsigned int rlen = readAll( sockfd, (char *) &hdr, sizeof( Gpfs2CN_Header ) );
      
      StrongAssertLogLine( rlen == sizeof( Gpfs2CN_Header ) )
        << "rlen: " << rlen
        << " sizeof( Gpfs2CN_Header ): " << sizeof( Gpfs2CN_Header )
        << EndLogLine;
      
      switch( hdr.Op )
        {
        case GPFS2CN_OPERATION_CONNECT:
    {
      // This is the result of multiple threads opening
      // disks. We could get a connect request in a connected state
      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_CONNECT "
        << "Warning:: Got connection request when memory server " 
        << " is in a connected state, as if memory client Disconnect() "
        << " was never called\n" 
        << EndLogLine;

      SendConnectionAck( sockfd );
      break;
    }
        case GPFS2CN_OPERATION_DISCONNECT:
    {    
      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_DISCONNECT "
        << EndLogLine;

      SendDisconnectionAck( sockfd );

      // CurrentState = MEMORY_SERVER_STATE_DISCONNECTED;
      break;
    }
        case GPFS2CN_OPERATION_READ:
    {
      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_READ"
        << EndLogLine;

      Gpfs2CN_ReadRequest ReadRequest;
      unsigned int rlen = readAll( sockfd, 
                 (char *) &ReadRequest, 
                 sizeof( Gpfs2CN_ReadRequest ) );
      
      StrongAssertLogLine( rlen == sizeof( Gpfs2CN_ReadRequest ) )
        << "rlen: " << rlen
        << " sizeof( Gpfs2CN_ReadRequest ): " << sizeof( Gpfs2CN_ReadRequest )
        << EndLogLine;
      
      unsigned int len = ReadRequest.Len;
      unsigned long long offset = ReadRequest.Offset;

      unsigned int inode = ReadRequest.Inode;
      unsigned long long offsetInFile = ReadRequest.OffsetInFile;

      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: About to write " 
        << " offset: " << offset 
        << " len: " << len 
        << " inode: " << inode
        << " offsetInFile: " << offsetInFile
        << EndLogLine;
      
      unsigned int wlen = writeAll( sockfd, (char *) &BufferPtr[offset], len );
      StrongAssertLogLine( wlen == len )
        << "wlen: " << wlen
        << " len: " << len
        << EndLogLine;

      break;
    }
        case GPFS2CN_OPERATION_WRITE:
    {
      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: Entering : GPFS2CN_OPERATION_WRITE"
        << EndLogLine;

      Gpfs2CN_WriteRequest WriteRequest;
      unsigned int rlen = readAll( sockfd, (char *) &WriteRequest, sizeof( Gpfs2CN_WriteRequest ) );
      StrongAssertLogLine( rlen == sizeof( Gpfs2CN_WriteRequest ) )
        << "rlen: " << rlen
        << " sizeof( Gpfs2CN_WriteRequest ): " << sizeof( Gpfs2CN_WriteRequest )
        << EndLogLine;
      
      unsigned int len = WriteRequest.Len;
      unsigned long long offset = WriteRequest.Offset;
      
      unsigned int inode = WriteRequest.Inode;
      unsigned long long offsetInFile = WriteRequest.OffsetInFile;

      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: About to read " 
        << " offset: " << offset 
        << " len: " << len 
        << " inode: " << inode
        << " offsetInFile: " << offsetInFile
        << EndLogLine;

      rlen = readAll( sockfd, (char *) &BufferPtr[offset], len );

      StrongAssertLogLine( rlen == len )
        << "rlen: " << rlen
        << " len: " << len
        << EndLogLine;

      break;
    }
        default:
    {
      StrongAssertLogLine( 0 )
        << "ASAN_MemoryServer:: ERROR:: unrecognized operation: "
        << hdr.Op
        << EndLogLine;
    }
        }

      break;
    }
  case MEMORY_SERVER_STATE_DISCONNECTED:
    {
      BegLogLine( PKFXLOG_MEMORY_SERVER )
        << "ASAN_MemoryServer:: Entering : MEMORY_SERVER_STATE_DISCONNECTED"
        << EndLogLine;
      
      Gpfs2CN_Header hdr;
      
      unsigned int rlen = readAll( sockfd , (char *) &hdr, sizeof( Gpfs2CN_Header ) );
      StrongAssertLogLine( rlen == sizeof( Gpfs2CN_Header ) )
        << "rlen: " << rlen
        << " sizeof( Gpfs2CN_Header ): " << sizeof( Gpfs2CN_Header )
        << EndLogLine;

      BegLogLine( PKFXLOG_MEMORY_SERVER ) 
        << "ASAN_MemoryServer:: state::disconnected:: op: " <<  hdr.Op
        << EndLogLine;
      
      switch( hdr.Op )
        {
        case GPFS2CN_OPERATION_CONNECT:
    {	      
      SendConnectionAck( sockfd );
      CurrentState = MEMORY_SERVER_STATE_CONNECTED;
      break;
    }
        case GPFS2CN_OPERATION_DISCONNECT:
    {		
      // Result of multiple gpfs threads opening/closing disks
      SendDisconnectionAck( sockfd );
      CurrentState = MEMORY_SERVER_STATE_DISCONNECTED;
      break;
    }
        default:
    {		  
      StrongAssertLogLine( 0 )
        << "ERROR:: Unrecognized operation: " << hdr.Op
        << " in disconnected state"
        << EndLogLine;
      
      break;
    }	    
        }
      
      break;
    }
  default:
    {
      StrongAssertLogLine( 0 )
        << "ERROR: Unrecognized state encountered: " << CurrentState
        << EndLogLine;
    }
  }
    }

  PkCo_Barrier();  
}
