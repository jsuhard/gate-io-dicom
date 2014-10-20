#include "GateModel.hh"

#include "G4VisManager.hh"
#include "G4VSolid.hh"
#include "G4Version.hh"

GateModel::GateModel(const G4VSolid & solid, const G4Transform3D & trans, const G4VisAttributes & att, const G4String category) : solid(solid), att(att), trans(trans) {
#if G4VERSION_NUMBER >= 960
    fType = "G4"+ category +"Model";
#endif
    fGlobalTag = solid.GetName();
    fGlobalDescription = fGlobalTag;
}

GateModel::~GateModel() {}

void GateModel::DescribeYourselfTo(G4VGraphicsScene & sceneHandler) {
    sceneHandler.PreAddSolid (trans, att);
    sceneHandler.AddSolid(solid);
    sceneHandler.PostAddSolid();
}

