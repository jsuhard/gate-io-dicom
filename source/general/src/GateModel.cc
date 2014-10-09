#include "GateModel.hh"

#include "G4VisManager.hh"
#include "G4VSolid.hh"

GateModel::GateModel(const G4VSolid & solid, const G4Transform3D & trans, const G4VisAttributes & att, const G4String category) : solid(solid), att(att), trans(trans) {
    fType = "G4"+ category +"Model";
    fGlobalTag = solid.GetName();
    fGlobalDescription = fGlobalTag;
}

GateModel::~GateModel() {}

void GateModel::DescribeYourselfTo(G4VGraphicsScene & sceneHandler) {
    sceneHandler.PreAddSolid (trans, att);
    sceneHandler.AddSolid(solid);
    sceneHandler.PostAddSolid();
}

