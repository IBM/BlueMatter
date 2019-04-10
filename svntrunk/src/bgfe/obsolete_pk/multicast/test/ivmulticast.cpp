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
 /***************************************************************************
 * Project:         pK
 *
 * Module:          ivmulticast.C
 *
 * Purpose:         Unit test of multicast.
 *
 * Classification:  IBM Internal Use Only
 *
 * History:         2001/06/14 TJCW Created.
 *
 ***************************************************************************/

// Multicast for plimpton uses the machine as a 2D grid. Information about
// an atom (for real, a set of atoms) homed in each grid element is send to every
// node on the cross (horizontal and vertical line) of nodes centred on the grid
// element. When the multicast is complete, every pairing of atoms is somewhere on
// the grid ... actually 2 places, above and below the leading diagonal.

#include <pk/platform.hpp>
#include <pk/reactor.hpp>
#include <pk/mpi.hpp>

#include <pk/multicast.hpp>
#include <pk/plimptontarget.hpp>

// Static arrays to hold the slice of the grid that this node is working on.
int xId[PlimptonTarget::kExtentX] ;
int yId[PlimptonTarget::kExtentY] ;

// Static counter to figure when we are complete
int Arrivals ;

// This is what we send around about the atom in question
class AtomArg {
public:
   int mId ;
   int mPosition ;
   } ;

// This will be driven asynchronously (actually, by the reactor thread) when
// information about an atom arrives here. Its job is to file away the atom

void ReceiveAtom(AtomArg &A)
{

  int MyNode = Platform::Topology::GetAddressSpaceId() ;

  BegLogLine( 1 )
   << "ReceiveAtom on node "
   << MyNode
   << " received from node "
   << A.mId
   << EndLogLine ;

  // See whether it is from the row or from the column

  if (MyNode / PlimptonTarget::kExtentX == A.mId / PlimptonTarget::kExtentX)
  {
     // From the row
     xId[A.mId % PlimptonTarget::kExtentX] = A.mId ;
     Platform::Memory::ExportFence() ;
     Arrivals += 1 ;
     BegLogLine( 1 )
      << "ReceiveAtom slots into X["
      << A.mId % PlimptonTarget::kExtentX
      << "], "
      << Arrivals
      << " atoms now here."
      << EndLogLine ;
  }
  else
  {
     if (MyNode % PlimptonTarget::kExtentX == A.mId % PlimptonTarget::kExtentX)
     {
        // From the column
        yId[A.mId / PlimptonTarget::kExtentX] = A.mId ;
        Platform::Memory::ExportFence() ;
        Arrivals += 1 ;
        BegLogLine( 1 )
         << "ReceiveAtom slots into Y["
         << A.mId / PlimptonTarget::kExtentX
         << "], "
         << Arrivals
         << " atoms now here."
         << EndLogLine ;
     }
     else
     {
        // Stray, should not have got here
        BegLogLine( 1 )
         << "ReceiveAtom is a stray, should not have been sent here"
         << EndLogLine ;
     } /* endif */
  } /* endif */
}

// This function sequences the activity of the node
// -- multicast the data managed by this node
// -- wait to receive all the data being multicast to here
// (-- perform the arithmetic that is this node's job)
// (-- return the partial results as required)
// (-- wait for the completed partial results from the other nodes)
// (-- compute the total result)

// Rely on static initialisation for 'Arrivals', becuase this node may receive and act on Plimpton
// packets from other nodes before RunNodeManager is activated.
// Note that when running for real, 'Arrivals' can be cleared down in preparation for the next
// time step after all atoms have arrived for the current time step; there is sufficient synchrony
// in the application to make this safe.

void * RunNodeManager(void* dummy)
{
   int MyNode = Platform::Topology::GetAddressSpaceId() ;

    BegLogLine( 1 )
      << "multicast node manager starting on node "
      << MyNode
      << EndLogLine;

    PkMulticast<AtomArg, PlimptonTarget> MyDistributor ;

    AtomArg A ;
    A.mId = MyNode ;

    PlimptonTarget P ;
    P.mX = MyNode % P.kExtentX ;
    P.mY = MyNode / P.kExtentX ;

    MyDistributor.Multicast ( P, ReceiveAtom, A ) ;

    while (Arrivals < P.kExtentX + P.kExtentY - 2)
    {
       BegLogLine( 1 )
         << "multicast node manager waiting, "
         << Arrivals
         << " atoms are here so far."
         << EndLogLine ;
       sleep(1) ;
       Platform::Memory::ImportFence() ;
    } /* endwhile */

    BegLogLine( 1 )
      << "multicast node manager complete."
      << EndLogLine ;

    return NULL ;
}

int StartNodeManager(void* dummy)
{
   Platform::Thread::Create(RunNodeManager, NULL) ;
   return 0 ;
}

void PkMain(int argc, char **argv, char **envp)
  {

  // Bring up 'volume manager' threads on all nodes other than starter
  int addressSpaceCount = Platform::Topology::GetAddressSpaceCount() ;
  for (int k=1; k<addressSpaceCount; k+=1)
  {
     Platform::Reactor::Trigger(
       k,
       StartNodeManager,
       NULL,
       0
     ) ;
  } /* endfor */                 \

  // Run the node manager on thread 0
  RunNodeManager(NULL) ;

  // By this time, everyone will have had a chance to run the multicast.
  // Collapse the MPI cluster by returning.

  }





