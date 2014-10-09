/**
 * \class       GateModel
 * \brief       Displays an object in a scene without interfering with physical volumes.
 * \author      Jérôme Suhard <jerome@suhard.fr>
 * \date        10/2014
 * \copyright   GNU Lesser General Public Licence (LGPL).
 */

#ifndef GATEMODEL_HH
#define GATEMODEL_HH

#include "G4VModel.hh"

class G4VSolid;
class G4VisAttributes;

class GateModel: public G4VModel {

public:
    GateModel(const G4VSolid & solid, const G4Transform3D & trans, const G4VisAttributes & att, const G4String category = "Gate");
    ~GateModel();
    void DescribeYourselfTo(G4VGraphicsScene & sceneHandler);

private:
    const G4VSolid & solid;
    const G4VisAttributes & att;
    const G4Transform3D & trans;
};

#endif /* GATEMODEL_HH */

