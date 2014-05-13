/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "GateEnergyFilter.hh"



//---------------------------------------------------------------------------
GateEnergyFilter::GateEnergyFilter(G4String name)
  :GateVFilter(name)
{
 
   fLowEnergy = 0.;
   fHighEnergy = 0.;  
   pEneMessenger = new GateEnergyFilterMessenger(this);

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
G4bool GateEnergyFilter::Accept(const G4Step* aStep) 
{
    G4double kinetic = aStep->GetPreStepPoint()->GetKineticEnergy();


    if ( fLowEnergy!=0. && kinetic < fLowEnergy  ) return false;
    if ( fHighEnergy!= 0. && kinetic >= fHighEnergy ) return false;
  
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
G4bool GateEnergyFilter::Accept(const G4Track* aTrack) 
{
    G4double kinetic = aTrack->GetKineticEnergy();


    if ( fLowEnergy!=0. && kinetic < fLowEnergy  ) return false;
    if ( fHighEnergy!= 0. && kinetic >= fHighEnergy ) return false;
  
    return true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void GateEnergyFilter::show(){
  G4cout << "------Filter: "<<GetObjectName()<<" particle list------"<<G4endl;



    G4cout << " LowE  " << G4BestUnit(fLowEnergy,"Energy") 
	   << " HighE " << G4BestUnit(fHighEnergy,"Energy")
	   << G4endl;
 

  G4cout << "-------------------------------------------"<<G4endl;

}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void GateEnergyFilter::SetEmin(G4double e){
  fLowEnergy  = e;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void GateEnergyFilter::SetEmax(G4double e){
  fHighEnergy = e;
}
//---------------------------------------------------------------------------


