#include <TList.h>
#include <TMath.h>

#include "AliAnalysisTaskNanoSimple.h"
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliAODEvent.h"
#include "AliAODv0.h"

#include "AliNanoAODHeader.h"
#include "AliNanoAODTrack.h"

#include "AliAODConversionPhoton.h"

#include "AliLog.h"
#include "AliVParticle.h"
#include "AliVTrack.h"

ClassImp(AliAnalysisTaskNanoSimple)

//____________________________________________________________________
AliAnalysisTaskNanoSimple:: AliAnalysisTaskNanoSimple(const char* name):
AliAnalysisTaskSE(name),
// general configuration
fListOfHistos(0x0)
{
  // Default constructor

  DefineOutput(1, TList::Class());
  DefineOutput(2, TTree::Class());
}

AliAnalysisTaskNanoSimple::~AliAnalysisTaskNanoSimple() 
{ 
  // destructor
  
  if (fListOfHistos) 
    delete fListOfHistos;
}

//____________________________________________________________________
void  AliAnalysisTaskNanoSimple::UserCreateOutputObjects()
{
  // Create the output container
  
  // Initialize output list of containers
  if (fListOfHistos != NULL){
    delete fListOfHistos;
    fListOfHistos = NULL;
  }
  if (!fListOfHistos){
    fListOfHistos = new TList();
    fListOfHistos->SetOwner(kTRUE); 
  }

  // TODO add output histograms to this list. E.g.
  //fListOfHistos->Add(new TH2F("multVsMPI", ";n_{MPI};nch_alice;events", 20, -0.5, 19.5, 301, -0.5, 300.5));
  
  PostData(1, fListOfHistos);
}

//____________________________________________________________________
void  AliAnalysisTaskNanoSimple::UserExec(Option_t */*option*/)
{
  // exec (per event)

  if (!fInputEvent)
    return;
  
  const AliVVertex* vertex = fInputEvent->GetPrimaryVertex();
  if (vertex->GetNContributors() < 1)
    return;
  
  Printf("Vertex = %f", vertex->GetZ());
  
  // for custom variables, cast to nano AOD header
  AliNanoAODHeader* nanoHeader = dynamic_cast<AliNanoAODHeader*>(fInputEvent->GetHeader());
  static const Int_t kRefMult = nanoHeader->GetVarIndex("MultSelection.RefMult08");
  if (kRefMult != -1)
    Printf("Ref Mult = %f", nanoHeader->GetVar(kRefMult));

  static const Int_t kV0MCentrality = nanoHeader->GetCentrIndex();
  if (kV0MCentrality != -1)
    Printf("V0M centrality = %f", nanoHeader->GetVar(kV0MCentrality));
  
  unsigned int nTracks = fInputEvent->GetNumberOfTracks();
  for (unsigned int i = 0; i < nTracks; i++) {
    AliVTrack* track = (AliVTrack*) fInputEvent->GetTrack(i);
    Printf("pt = %f   ITS cluster = %d %d %d %d %d %d", track->Pt(), track->HasPointOnITSLayer(0), track->HasPointOnITSLayer(1), 
           track->HasPointOnITSLayer(2), track->HasPointOnITSLayer(3), track->HasPointOnITSLayer(4), track->HasPointOnITSLayer(5));
    
    //Printf("  TOF BC = %d", track->GetTOFBunchCrossing());
    //Printf("  ID = %d", track->GetID());
    
    // for custom variables, cast to nano AOD track
    AliNanoAODTrack* nanoTrack = dynamic_cast<AliNanoAODTrack*>(track);
    // NOTE Access to custom variables. Use static here for caching of index
    static Bool_t bPIDAvailable = AliNanoAODTrack::InitPIDIndex();
    static const Int_t kcstNSigmaTPCPr  = AliNanoAODTrack::GetPIDIndex(AliNanoAODTrack::kSigmaTPC, AliPID::kProton);
    static const Int_t kcstNSigmaTOFPr  = AliNanoAODTrack::GetPIDIndex(AliNanoAODTrack::kSigmaTOF, AliPID::kProton);
    if (nanoTrack && bPIDAvailable)
      Printf("  TPC_sigma_proton = %f  hasTOF = %d  TOF_sigma_proton = %f", nanoTrack->GetVar(kcstNSigmaTPCPr), nanoTrack->HasTOFPID(), nanoTrack->GetVar(kcstNSigmaTOFPr));
  }
  
  // V0 access - as usual
  AliAODEvent* aod = dynamic_cast<AliAODEvent*> (fInputEvent);
  if (aod->GetV0s()) {
    for (int i = 0; i < aod->GetNumberOfV0s(); i++) {
      Printf("V0 %d: dca = %f   daughter pT = %f", i, aod->GetV0(i)->DcaV0ToPrimVertex(), ((AliVTrack*) aod->GetV0(i)->GetDaughter(0))->Pt());
    }
  }

  // cascade access - as usual
  if (aod->GetCascades()) {
    for (int i = 0; i < aod->GetNumberOfCascades(); i++) {
      AliAODcascade* cascade = aod->GetCascade(i);
      Printf("Cascade %d: xi mass = %f", i, cascade->MassXi());
      for (int j=0; j<cascade->GetNDaughters(); j++)
        Printf("  Daughter %d pT = %f", j, ((AliVTrack*) cascade->GetDaughter(j))->Pt());
      for (int j=0; j<cascade->GetDecayVertexXi()->GetNDaughters(); j++)
        Printf("  Xi Daughter %d pT = %f", j, ((AliVTrack*) cascade->GetDecayVertexXi()->GetDaughter(j))->Pt());
    }
  }
  
  // Photon conversions: other way to get the list (no AliV0ReaderV1 needed), but identical objects
  static TClonesArray* conversionPhotons = dynamic_cast<TClonesArray*> (fInputEvent->FindListObject("conversionphotons"));
  if (conversionPhotons) {
    for (int i = 0; i < conversionPhotons->GetEntries(); i++) {
      auto photon = dynamic_cast<AliAODConversionPhoton*> (conversionPhotons->At(i));
      Printf("Conversion photon candidate %d: mass = %e \t pT = %f", i, photon->GetPhotonMass(), photon->GetPhotonPt());
    }
  }
}
