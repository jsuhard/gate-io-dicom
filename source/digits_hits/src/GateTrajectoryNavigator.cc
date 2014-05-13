/*----------------------
  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/

#include "GateTrajectoryNavigator.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

#include "GateActions.hh"

GateTrajectoryNavigator::GateTrajectoryNavigator() : m_trajectoryContainer(NULL), m_positronTrackID(0), m_positronTrj(NULL), m_ionID(0), nVerboseLevel(0)
{
}

GateTrajectoryNavigator::~GateTrajectoryNavigator()
{
}

G4ThreeVector GateTrajectoryNavigator::FindSourcePosition()
{
  if (nVerboseLevel > 2)
    G4cout << "GateTrajectoryNavigator::FindSourcePosition" << G4endl;

  G4ThreeVector sourcePosition = G4ThreeVector();

  G4int sourceIndex = FindSourceIndex();

  if ((sourceIndex < 0) || (sourceIndex >= m_trajectoryContainer->entries())) {
    G4cout << "GateTrajectoryNavigator::FindSourcePosition : WARNING : sourceIndex out of range: " << sourceIndex << G4endl;
  } else {
    G4Trajectory* trjSource = (G4Trajectory*)((*m_trajectoryContainer)[sourceIndex]);
    sourcePosition = ((G4TrajectoryPoint*)(trjSource->GetPoint(0)))->GetPosition();
  }

  return sourcePosition;
}

G4int GateTrajectoryNavigator::FindSourceIndex()
{
  if (nVerboseLevel > 2)
    G4cout << "GateTrajectoryNavigator::FindSourceIndex" << G4endl;
  G4int sourceIndex = -1;
  G4bool found = false;
  G4int n_trajectories = m_trajectoryContainer->entries();
  for (G4int iTrj=0; (iTrj<n_trajectories) && (!found); iTrj++) {
    G4Trajectory* trj = (G4Trajectory*)((*m_trajectoryContainer)[iTrj]);
    if (trj->GetTrackID() == 1) {
      sourceIndex = iTrj;
      found = true;
    }
  }

  return sourceIndex;
}


void GateTrajectoryNavigator::SetIonID()
{
  G4int n_trajectories = m_trajectoryContainer->entries();

  m_ionID = 0;

  for (G4int iTrj=0; iTrj<n_trajectories; iTrj++) {
    G4Trajectory* trj = (G4Trajectory*)((*m_trajectoryContainer)[iTrj]);
    if (trj->GetCharge() > 2) m_ionID = 1;

  /*  G4cout << "GateTrajectoryNavigator::SetIonID:GetTrackID  " << trj->GetTrackID()
 	    << " Name <" << trj->GetParticleName() << ">" << G4endl;
    G4cout << "GateTrajectoryNavigator::SetIonID:GetCharge  " <<  trj->GetCharge() << G4endl;*/
    }
 }




G4int GateTrajectoryNavigator::FindPositronTrackID()
{
  if (nVerboseLevel > 2)
  G4cout << "GateTrajectoryNavigator::FindPositronTrackID" << G4endl;

  m_positronTrackID = 0;

  SetIonID();


  if (!m_trajectoryContainer) {
    G4cout << "GateTrajectoryNavigator::FindPositronTrackID : ERROR : NULL trajectoryContainer" << G4endl;
  } else {
    G4int n_trajectories = m_trajectoryContainer->entries();
    G4bool found = false;
    for (G4int iTrj=0; (iTrj<n_trajectories) && (!found); iTrj++) {
      G4Trajectory* trj = (G4Trajectory*)((*m_trajectoryContainer)[iTrj]);


      if ((trj->GetParentID() == m_ionID)&&(trj->GetPDGEncoding() == -11)) { // -11 == Positron


	m_positronTrackID = trj->GetTrackID();
	m_positronTrj = trj;
	found = true;
      }
    }
  }

  return m_positronTrackID;
}


/* modifs pour cas du mode detector : PY Descourt 08/09/2009 */
std::vector<G4int> GateTrajectoryNavigator::FindAnnihilationGammasTrackID()
{

TrackingMode theMode =( (GateSteppingAction *)(G4RunManager::GetRunManager()->GetUserSteppingAction() ) )->GetMode();

  if (nVerboseLevel > 2)
    G4cout << "GateTrajectoryNavigator::FindAnnihilationGammasTrackID" << G4endl;

  std::vector<G4int> photonIndices;
  photonIndices.clear();

  SetIonID();

  if (!m_trajectoryContainer) {
    G4cout << "GateTrajectoryNavigator::FindAnnihilationGammasTrackID : ERROR : NULL trajectoryContainer" << G4endl;
  } else {

    // prepare the list of gammas for later analysis
    G4int n_trajectories = m_trajectoryContainer->entries();

    for (G4int iTrj=0; iTrj<n_trajectories; iTrj++) {

      G4Trajectory* trj = (G4Trajectory*)((*m_trajectoryContainer)[iTrj]);


      if (m_positronTrackID != 0) {

	// in case the positronTrackID has been found, we put in the list only
	// the gammas generated by the positron
	if ((trj->GetParentID() == m_positronTrackID)
	    && (trj->GetPDGEncoding() == 22) ) { // 22 == Gamma
	  photonIndices.push_back(iTrj);
	}
      } else {
	// in case the positron has not been found, we accept all the photons
	// either coming from the ion (assuming m_ionID==1) or shooted as primary
	// (both single and back-to-back pair)
	//if (((trj->GetParentID() != 0) ||(trj->GetParentID() == 0) || (trj->GetParentID() == m_ionID))
	  //  && (trj->GetPDGEncoding() == 22))
	    if ((trj->GetParentID() >= 0) && (trj->GetPDGEncoding() == 22))
	    { // 22 == Gamma
	  photonIndices.push_back(iTrj);

	}
      }
    }

    // we start the analysis of the gammas in the list

    // in both cases (in case a positron has been found or not) we look for
    // coincident vertices (annihilation gammas, or gammas from ion decay, or
    // user defined multiple gamma sources)
    G4int nPh = photonIndices.size();

    if (nPh == 1) {
      // if only 1 gamma, we take it
      G4int i1 = photonIndices[0];
      G4Trajectory* trj1 = (G4Trajectory*)((*m_trajectoryContainer)[i1]);
      m_photonIDVec.push_back(trj1->GetTrackID());
    } else if (nPh >= 2) {
      // if more than 1 gamma, we select those coming from a common vertex
      // this design should be open to more than 2 gammas, even if for the moment not
      // considered by GateAnalysis
      G4int i1;
      G4int i2;
      for (G4int j1=0; j1<nPh; j1++) {
	for (G4int j2=j1+1; j2<nPh; j2++) {
	  i1 = photonIndices[j1];
	  i2 = photonIndices[j2];
	  if ((i1 >= 0) && (i2 >= 0)) {
	    // both gammas were not already taken
	    G4Trajectory* trj1 = (G4Trajectory*)((*m_trajectoryContainer)[i1]);
	    G4Trajectory* trj2 = (G4Trajectory*)((*m_trajectoryContainer)[i2]);

	    G4ThreeVector vert1 = ((G4TrajectoryPoint*)(trj1->GetPoint(0)))->GetPosition();
	    G4ThreeVector vert2 = ((G4TrajectoryPoint*)(trj2->GetPoint(0)))->GetPosition();
	    // in detector mode the vertex position is stored at the last trajectory point
	    // not the first one
	    if (  theMode == kDetector ) // in tracker mode we store the infos about the number of compton and rayleigh
         {
          G4int n_points =trj1->GetPointEntries();
	      vert1 = ((G4TrajectoryPoint*)(trj1->GetPoint( n_points - 1 )))->GetPosition();
	            n_points =trj2->GetPointEntries();
	      vert2 = ((G4TrajectoryPoint*)(trj2->GetPoint( n_points - 1 )))->GetPosition();
	     }

	    G4double dist = (vert1-vert2).mag();
	    if (nVerboseLevel > 2)
	    G4cout << "[GateTrajectoryNavigator::FindAnnihilationGammasTrackID] : distance between gammas vertices : dist (mm) " << dist/mm << G4endl;
	    if (dist/mm < 1E-7) {
	     if (nVerboseLevel > 1) {
		G4cout << "[GateTrajectoryNavigator::FindAnnihilationGammasTrackID] : Found common vertex for the two annihilation gammas :"
		       << " tracks " << trj1->GetTrackID() << " and " << trj2->GetTrackID() << G4endl;
	     }
	      // we add both photons to the vertex
	      m_photonIDVec.push_back(trj1->GetTrackID());
	      m_photonIDVec.push_back(trj2->GetTrackID());
	      // we cancel the 2nd photon from the list to avoid double counting later
	      photonIndices[j2] = -1;
	    }
	  }
	}
      }
    }
  }

  /*if ((m_positronTrackID != 0) && ((m_photonIDVec.size() > 2) || (m_photonIDVec.size() == 1))) {
    // strange case: we would expect to see a true annihilation, thus only 2 gammas backto back from the positron
    // 0 can be normal (positron escaped and didn't annihilate), 1 is strange again.
    G4cout
      << "[GateTrajectoryNavigator::FindAnnihilationGammasTrackID]  WARNING : positron found and photonID vector size: " << m_photonIDVec.size() << G4endl;
  } */

  /*if ((m_positronTrackID != 0) && (m_photonIDVec.size() == 0)) {
    // still, it's not really what we want to see the positron going out of the scene without doing anything...
    //    if (nVerboseLevel > 0)
    if (nVerboseLevel > 0)
      G4cout
       << "[GateTrajectoryNavigator::FindAnnihilationGammasTrackID]  WARNING : positron found and photonID vector size: " << m_photonIDVec.size() << G4endl;
    G4Navigator *gNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
    G4ThreeVector null(0.,0.,0.);
    G4ThreeVector *ptr;
    ptr = &null;
    G4ThreeVector lastPoint = ((G4TrajectoryPoint*)(m_positronTrj->GetPoint(m_positronTrj->GetPointEntries()-1)))->GetPosition();
    G4String theLastVolumeName = gNavigator->LocateGlobalPointAndSetup(lastPoint,ptr,false)->GetName();
    if (nVerboseLevel > 0)
      G4cout << "GateTrajectoryNavigator::FindAnnihilationGammasTrackID: last position of the positron track has been found in volume: " << theLastVolumeName << G4endl;
  }*/


  return m_photonIDVec;
}


// search the gamma from which this track comes --> photonID
G4int GateTrajectoryNavigator::FindPhotonID(G4int trackID)
{
  if (nVerboseLevel > 2)
    G4cout << "GateTrajectoryNavigator::FindPhotonID " << G4endl;
  G4int photonID = 0;

  if (!m_trajectoryContainer) {
    G4cout << "GateTrajectoryNavigator::FindPhotonID : ERROR : NULL trajectoryContainer" << G4endl;
  } else {

    if (m_photonIDVec.size() == 0) {
      G4cout << "GateTrajectoryNavigator::FindPhotonID : m_photonIDVec.size() == 0" << G4endl;
    } else {

      G4bool found;
      // search the gamma related to this hit --> photonID
      photonID = trackID;
      G4int photon1ID = m_photonIDVec[0];
      // in case there are 2 gammas OK, if not we put 0 (compatible with subsequent analysis)
      G4int photon2ID = (m_photonIDVec.size() >= 2) ? m_photonIDVec[1] : 0;
      G4int rootID = 0;

      // we go up and up, starting from the present trackID, to the parentID, the parentID, ecc until
      // we find that the ID of the track is equal to the ID of: one of the photons, or rootID(==0)
      while (!((photonID==photon1ID)||(photonID==photon2ID)||(photonID==rootID))) {
	found = false;
	G4int n_trajectories = m_trajectoryContainer->entries();
	for (G4int iTrj=0; (iTrj<n_trajectories) && (!found); iTrj++) {
	  G4Trajectory* trj = (G4Trajectory*)((*m_trajectoryContainer)[iTrj]);
	  if (photonID == trj->GetTrackID()) {
	    photonID = trj->GetParentID();
	    found = true;
	  }
	}
      }
      if (photonID == rootID) {
	if (nVerboseLevel > 2) G4cout
	  << "GateTrajectoryNavigator::FindPhotonID : trackID: " << trackID << " photonID = " << rootID << G4endl;
      }
      if (photonID == photon1ID) {
	photonID = 1;
      } else if (photonID == photon2ID) {
	photonID = 2;
      }

    }
  }

  return photonID;
}


G4int GateTrajectoryNavigator::FindPrimaryID(G4int trackID)
{
  G4int primaryID = 0;

  if (!m_trajectoryContainer) {
    G4cout << "GateTrajectoryNavigator::FindPrimaryID : ERROR : NULL trajectoryContainer" << G4endl;
  } else {
    G4int tempParentID = trackID;
    //G4int primaryIndex = -1;
    G4bool found;
    G4int n_trajectories = m_trajectoryContainer->entries();
    // we go up and up starting from the trackID, via the parentID's, until
    // the track we find is a primary (its parentID==0)
    do {
      primaryID = tempParentID;
      found = false;
      for (G4int iTrj=0; (iTrj<n_trajectories) && (!found); iTrj++) {
	G4Trajectory* trj = (G4Trajectory*)((*m_trajectoryContainer)[iTrj]);
	if (primaryID == trj->GetTrackID()) {
	  tempParentID = trj->GetParentID();
	  //primaryIndex = iTrj;
	  found = true;
	}
      }
    } while (tempParentID != 0);
  }

  return primaryID;
}


void GateTrajectoryNavigator::SetTrajectoryContainer(G4TrajectoryContainer* trajectoryContainer)
{
  if (nVerboseLevel > 2)
    G4cout << "GateTrajectoryNavigator::SetTrajectoryContainer" << G4endl;

  if (!trajectoryContainer) {
    G4cout << "GateTrajectoryNavigator::SetTrajectoryContainer : ERROR : NULL trajectoryContainer" << G4endl;
  } else {
    m_trajectoryContainer = trajectoryContainer;
  }

  Initialize();
}

void GateTrajectoryNavigator::Initialize()
{
  if (nVerboseLevel > 2)
    G4cout << "GateTrajectoryNavigator::Initialize" << G4endl;
  m_photonIDVec.clear();
  m_positronTrackID = -1;
}
