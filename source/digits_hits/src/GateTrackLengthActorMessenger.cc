/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/

#include "GateTrackLengthActorMessenger.hh"

#ifdef G4ANALYSIS_USE_ROOT

#include "GateTrackLengthActor.hh"


//-----------------------------------------------------------------------------
GateTrackLengthActorMessenger::GateTrackLengthActorMessenger(GateTrackLengthActor * v)
: GateActorMessenger(v),
  pActor(v)
{

  BuildCommands(baseName+pActor->GetObjectName());

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
GateTrackLengthActorMessenger::~GateTrackLengthActorMessenger()
{
  delete pLmaxCmd;
  delete pLminCmd;
  delete pNBinsCmd;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GateTrackLengthActorMessenger::BuildCommands(G4String base)
{
  G4String guidance;
  G4String bb;

  bb = base+"/setLmin";
  pLminCmd = new G4UIcmdWithADoubleAndUnit(bb, this);
  guidance = G4String("Set minimum length of the histogram");
  pLminCmd->SetGuidance(guidance);
  pLminCmd->SetParameterName("Lmin", false);
  pLminCmd->SetDefaultUnit("mm");

  bb = base+"/setLmax";
  pLmaxCmd = new G4UIcmdWithADoubleAndUnit(bb, this);
  guidance = G4String("Set maximum length of the histogram");
  pLmaxCmd->SetGuidance(guidance);
  pLmaxCmd->SetParameterName("Lmax", false);
  pLmaxCmd->SetDefaultUnit("mm");

  bb = base+"/setNumberOfBins";
  pNBinsCmd = new G4UIcmdWithAnInteger(bb, this);
  guidance = G4String("Set number of bins of the histogram");
  pNBinsCmd->SetGuidance(guidance);
  pNBinsCmd->SetParameterName("Nbins", false);

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GateTrackLengthActorMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue)
{

  if(cmd == pLminCmd) pActor->SetLmin(  pLminCmd->GetNewDoubleValue(newValue)  ) ;
  if(cmd == pLmaxCmd) pActor->SetLmax(  pLmaxCmd->GetNewDoubleValue(newValue)  ) ;
  if(cmd == pNBinsCmd) pActor->SetNBins(  pNBinsCmd->GetNewIntValue(newValue)  ) ;

  GateActorMessenger::SetNewValue(cmd,newValue);
}
//-----------------------------------------------------------------------------

#endif
