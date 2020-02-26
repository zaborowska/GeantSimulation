#ifndef TESTGEOMETRY_CALORIMETERHIT_H
#define TESTGEOMETRY_CALORIMETERHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/// Hadron Calorimeter hit
///
/// It records:
/// - the cell x ID and y ID and z ID
/// - the energy deposit 
/// - the cell position and rotation


/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the hit for the calorimeter.
 *  Based on B5HadCalorimeterHit from examples/basic/B5.
 *
 *  @author Anna Zaborowska
 */

namespace test {
class CalorimeterHit : public G4VHit {
public:
    CalorimeterHit();
    CalorimeterHit(G4int iRho,G4int iPhi,G4int iZ);
    CalorimeterHit(G4int iRho,G4int iPhi,G4int iZ, G4LogicalVolume* logVol);
    CalorimeterHit(const CalorimeterHit &right);
    virtual ~CalorimeterHit();

    const CalorimeterHit& operator=(const CalorimeterHit &right);
    int operator==(const CalorimeterHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();

    void SetRhoId(G4int z) { fRhoID = z; }
    G4int GetRhoId() const { return fRhoID; }

    void SetPhiId(G4int z) { fPhiID = z; }
    G4int GetPhiId() const { return fPhiID; }

    void SetZid(G4int z) { fzID = z; }
    G4int GetZid() const { return fzID; }

    void SetEdep(G4double de) { fEdep = de; }
    void AddEdep(G4double de) { fEdep += de; }
    G4double GetEdep() const { return fEdep; }

#ifdef RICHVIS
    void SetTime(G4double time) { fTime = time; }
    G4double GetTime() const { return fTime; }

    void SetPos(G4ThreeVector xyz) { fPos = xyz; }
    G4ThreeVector GetPos() const { return fPos; }

    void SetRot(G4RotationMatrix rmat) { fRot = rmat; }
    G4RotationMatrix GetRot() const { return fRot; }

    inline const G4LogicalVolume * GetLogV() { return fLogV; }
    inline void SetLogV(G4LogicalVolume* aLogV) { fLogV=aLogV; }

    void SetColour(G4int col) { fColour = col; }
    G4int GetColour() const { return fColour; }
#endif

public:
    G4int fRhoID;
    G4int fPhiID;
    G4int fzID;
    G4double fEdep;
#ifdef RICHVIS
    G4double fTime;
    G4ThreeVector fPos;
    G4RotationMatrix fRot;
    G4LogicalVolume* fLogV;
    // to draw hits: if debugKillProcess kills particle, change to 1 (different colour)
    G4int fColour;
#endif
};

typedef G4THitsCollection<CalorimeterHit> CalorimeterHitsCollection;

extern G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

inline void* CalorimeterHit::operator new(size_t)
{
    if (!CalorimeterHitAllocator)
        CalorimeterHitAllocator = new G4Allocator<CalorimeterHit>;
    return (void*)CalorimeterHitAllocator->MallocSingle();
}

inline void CalorimeterHit::operator delete(void* aHit)
{
    CalorimeterHitAllocator->FreeSingle((CalorimeterHit*) aHit);
}
}

#endif /* TESTGEOMETRY_CALORIMETERHIT_H */
