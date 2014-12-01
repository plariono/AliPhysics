/**************************************************************************
 * Contributors are not mentioned at all.                                 *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright noticxse appears in all *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
//-----------------------------------------------------------------
//                 AliAnalysisTaskNucleiv2SP class
//-----------------------------------------------------------------

class TTree;
class TParticle;
class TVector3;

#include "AliAnalysisManager.h"
#include <AliMCEventHandler.h>
#include <AliMCEvent.h>
#include <AliStack.h>

class AliESDVertex;
class AliAODVertex;
class AliESDv0;
class AliAODv0; 

#include <iostream>

#include "TList.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TNtuple.h"
#include "TGraph.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TChain.h"
#include "Riostream.h"
#include "AliLog.h"
#include "AliCascadeVertexer.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliExternalTrackParam.h"
#include "AliAODEvent.h"
#include "AliInputEventHandler.h"
#include "AliESDcascade.h"
#include "AliAODcascade.h"
#include "AliAnalysisTaskNucleiv2SP.h"
#include "AliESDtrackCuts.h"
#include "AliCentrality.h"


ClassImp(AliAnalysisTaskNucleiv2SP)

using std::cout;
using std::endl;
    
//________________________________________________________________________
AliAnalysisTaskNucleiv2SP::AliAnalysisTaskNucleiv2SP() 
: AliAnalysisTaskSE(), 
  fisPrimCut(kFALSE),
  fptc(1),     
  fListHist(0), 
  fHistEventMultiplicity(0), 
  fHistTrackMultiplicity(0),
  fHistTrackMultiplicityCentral(0),    
  fHistTrackMultiplicitySemiCentral(0),
  fHistTrackMultiplicityMB(0),
  fhBB(0),
  fhBBDeu(0),
  fhTOF(0),
  fhMassTOF(0),
  EPVzAvsCentrality(0), 
  EPVzCvsCentrality(0), 
  EPTPCvsCentrality(0), 
  EPVzvsCentrality(0), 
  EPTPCpvsCentrality(0), 
  EPTPCnvsCentrality(0), 
  hEvPlaneTPCvsEvPVz05(0),                      
  hEvPlaneTPCvsEvPVz075(0), 
  hEvPlaneTPCvsEvPVz1530(0),
  hEvPlaneTPCvsEvPVz3050(0),                      
  hEvPlaneTPCvsEvPVz2040(0),                      
  hEvPlaneTPCvsEvPVz4060(0),       
  hCos2DeltaTPCVzAvsCentrality(0),
  hCos2DeltaTPCVzCvsCentrality(0),
  hCos2DeltaVzAVzCvsCentrality(0),
  hCos2DeltaVzMVzAvsCentrality(0),
  hCos2DeltaVzMVzCvsCentrality(0),
  hCos2DeltaVzATPCvsCentrality(0),
  hCos2DeltaVzCTPCvsCentrality(0),
  hCos2DeltaVzCVzAvsCentrality(0),
  hCos2DeltaVzMTPCpvsCentrality(0),
  hCos2DeltaVzMTPCnvsCentrality(0),
  hCos2DeltaTPCpTPCnvsCentrality(0),
  hQVzAQVzCvsCentrality(0),
  hQxVzAvsCentrality(0),
  hQyVzAvsCentrality(0),
  hQxVzCvsCentrality(0),
  hQyVzCvsCentrality(0),
  hQxVzMvsCentrality(0),
  hQyVzMvsCentrality(0),
  ftree(0),           
  tCentrality(0),     
  tType(0),  
  tHasTOF(0),    
  tpT(0),  
  tMassTOF(0),
  tuqV0A(0),
  tuqV0C(0),
  tCharge(0),
  tCosdeltaphiTPC(0),
  tCosdeltaphiV0M(0),
  tCosdeltaphiV0A(0),
  tCosdeltaphiV0C(0),
  timpactXY(0),
  timpactZ(0),
  tpull(0),
  tphi(0),
  fESDtrackCuts(0),
  fESDtrackCutsEP(0),
  fPIDResponse(0)
{
  // Dummy Constructor 
  fESDtrackCuts   = new AliESDtrackCuts("AliESDtrackCuts","AliESDtrackCuts");
  fESDtrackCutsEP = new AliESDtrackCuts("AliESDtrackCutsEP","AliESDtrackCutsEP");
  Initialize();
  cout<<"Dummy constructor"<<endl;
}

//________________________________________________________________________
AliAnalysisTaskNucleiv2SP::AliAnalysisTaskNucleiv2SP(const char *name) 
: AliAnalysisTaskSE(name), 
  fisPrimCut(kFALSE),
  fptc(1),     
  fListHist(0), 
  fHistEventMultiplicity(0), 
  fHistTrackMultiplicity(0),
  fHistTrackMultiplicityCentral(0),    
  fHistTrackMultiplicitySemiCentral(0),
  fHistTrackMultiplicityMB(0),
  fhBB(0),
  fhBBDeu(0),
  fhTOF(0),
  fhMassTOF(0),
  EPVzAvsCentrality(0), 
  EPVzCvsCentrality(0), 
  EPTPCvsCentrality(0), 
  EPVzvsCentrality(0), 
  EPTPCpvsCentrality(0), 
  EPTPCnvsCentrality(0), 
  hEvPlaneTPCvsEvPVz05(0),                      
  hEvPlaneTPCvsEvPVz075(0), 
  hEvPlaneTPCvsEvPVz1530(0),
  hEvPlaneTPCvsEvPVz3050(0),                      
  hEvPlaneTPCvsEvPVz2040(0),                      
  hEvPlaneTPCvsEvPVz4060(0),       
  hCos2DeltaTPCVzAvsCentrality(0),
  hCos2DeltaTPCVzCvsCentrality(0),
  hCos2DeltaVzAVzCvsCentrality(0),
  hCos2DeltaVzMVzAvsCentrality(0),
  hCos2DeltaVzMVzCvsCentrality(0),
  hCos2DeltaVzATPCvsCentrality(0),
  hCos2DeltaVzCTPCvsCentrality(0),
  hCos2DeltaVzCVzAvsCentrality(0),
  hCos2DeltaVzMTPCpvsCentrality(0),
  hCos2DeltaVzMTPCnvsCentrality(0),
  hCos2DeltaTPCpTPCnvsCentrality(0),
  hQVzAQVzCvsCentrality(0),
  hQxVzAvsCentrality(0),
  hQyVzAvsCentrality(0),
  hQxVzCvsCentrality(0),
  hQyVzCvsCentrality(0),
  hQxVzMvsCentrality(0),
  hQyVzMvsCentrality(0),
  ftree(0),           
  tCentrality(0),     
  tType(0),  
  tHasTOF(0),    
  tpT(0),  
  tMassTOF(0),
  tuqV0A(0),
  tuqV0C(0),
  tCharge(0),
  tCosdeltaphiTPC(0),
  tCosdeltaphiV0M(0),
  tCosdeltaphiV0A(0),
  tCosdeltaphiV0C(0),
  timpactXY(0),
  timpactZ(0),
  tpull(0),
  tphi(0),
  fESDtrackCuts(0),
  fESDtrackCutsEP(0),
  fPIDResponse(0)
{
  // Define input and output slots here
  // Input slot #0 works with a TChain
  //DefineInput(0, TChain::Class());
  // Output slot #0 writes into a TList container ()

  //
  // create track cuts
  //
  fESDtrackCuts   = new AliESDtrackCuts("AliESDtrackCuts","AliESDtrackCuts");
  fESDtrackCutsEP = new AliESDtrackCuts("AliESDtrackCutsEP","AliESDtrackCutsEP");
  //
  cout<<"Real constructor"<<endl;
  Initialize();

  DefineInput(0, TChain::Class());
  DefineOutput(1, TList::Class());
  DefineOutput(2, TTree::Class());
  
}

void AliAnalysisTaskNucleiv2SP::Initialize()
{
  //
  // updating parameters in case of changes
  //
  fESDtrackCuts = AliESDtrackCuts::GetStandardITSTPCTrackCuts2011(fisPrimCut,kTRUE);
  fESDtrackCuts->SetMaxDCAToVertexXY(3);
  fESDtrackCuts->SetMaxDCAToVertexZ(2);
  fESDtrackCuts->SetEtaRange(-0.8,0.8);
  
  fESDtrackCutsEP = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts(); 

}

//________________________________________________________________________
Float_t AliAnalysisTaskNucleiv2SP::GetEventPlaneForCandidate(AliESDtrack* track0, const TVector2* q,AliEventplane *pl){
  
  // remove autocorrelations 
  
  TArrayF* qx = 0x0;
  TArrayF* qy = 0x0;
  TVector2 qcopy; 
  // if(!fEtaGap){
  qx = pl->GetQContributionXArray();
  qy = pl->GetQContributionYArray();
  qcopy = *q;
  
  TVector2 q0;
  if((track0->GetID()) < qx->fN){
    q0.Set(qx->At(track0->GetID()),qy->At(track0->GetID()));
  }
  
  qcopy = qcopy - q0;
  
  return qcopy.Phi()/2.;
  
}
//________________________________________________________________________
Float_t AliAnalysisTaskNucleiv2SP::GetPhi0Pi(Float_t phi){
  // Sets the phi angle in the range 0-pi
  Float_t result=phi;
  while(result<0){
    result=result+TMath::Pi();
  }
  while(result>TMath::Pi()){
    result=result-TMath::Pi();
  }
   return result;
}

//==================DEFINITION OF OUTPUT OBJECTS==============================

void AliAnalysisTaskNucleiv2SP::UserCreateOutputObjects()
{
  //-------------------------------------------------------
  fListHist = new TList();
  fListHist->SetOwner();  // IMPORTANT!
  
  if(! fHistEventMultiplicity ){

    fHistEventMultiplicity   = new TH1F( "fHistEventMultiplicity" , "Nb of Events" , 12 , -0.5,11.5);

    fHistEventMultiplicity->GetXaxis()->SetBinLabel(1,"All Events");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(2,"Events w/PV");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(3,"Events w/|Vz|<10cm");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(4,"Central Events");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(5,"Semi-Central Events");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(6,"MB Events");
    //from HF
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(7,"nEventsAnal");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(8,"nEvSelected");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(9,"nCandidatesSelected");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(10,"out of pt bounds");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(11,"mismatch lab");
    fHistEventMultiplicity->GetXaxis()->SetBinLabel(12,"non valid TPC EP");
    fListHist->Add(fHistEventMultiplicity);
  }

  if(! fHistTrackMultiplicity ){
    fHistTrackMultiplicity  = new TH2F( "fHistTrackMultiplicity", "Nb of Tracks MB Events |Vz| < 10", 250,0, 25000,105,-0.5,104.5);
    fHistTrackMultiplicity->GetXaxis()->SetTitle("Number of tracks");
    fHistTrackMultiplicity->GetYaxis()->SetTitle("Percentile");
    fListHist->Add(fHistTrackMultiplicity);
  } 

  if(! fHistTrackMultiplicityCentral ){
    fHistTrackMultiplicityCentral  = new TH2F( "fHistTrackMultiplicityCentral", "Nb of Tracks MB Events |Vz| < 10", 250,0, 25000,105,-0.5,104.5);
    fHistTrackMultiplicityCentral->GetXaxis()->SetTitle("Number of tracks");
    fHistTrackMultiplicityCentral->GetYaxis()->SetTitle("Percentile");
    fListHist->Add(fHistTrackMultiplicityCentral);
  } 
  if(! fHistTrackMultiplicitySemiCentral ){
    fHistTrackMultiplicitySemiCentral  = new TH2F( "fHistTrackMultiplicitySemiCentral", "Nb of Tracks MB Events |Vz| < 10", 250,0, 25000,105,-0.5,104.5);
    fHistTrackMultiplicitySemiCentral->GetXaxis()->SetTitle("Number of tracks");
    fHistTrackMultiplicitySemiCentral->GetYaxis()->SetTitle("Percentile");
    fListHist->Add(fHistTrackMultiplicitySemiCentral);
  } 
  if(! fHistTrackMultiplicityMB ){
    fHistTrackMultiplicityMB  = new TH2F( "fHistTrackMultiplicityMB", "Nb of Tracks MB Events |Vz| < 10", 250,0, 25000,105,-0.5,104.5);
    fHistTrackMultiplicityMB->GetXaxis()->SetTitle("Number of tracks");
    fHistTrackMultiplicityMB->GetYaxis()->SetTitle("Percentile");
    fListHist->Add(fHistTrackMultiplicityMB);
  } 
 
  if(! fhBB ){
    fhBB = new TH2F( "fhBB" , "BetheBlochTPC" , 240,-10,10,250,0,1000);
    fListHist->Add(fhBB);
  }
  
  if(! fhBBDeu ){
    fhBBDeu = new TH2F( "fhBBDeu" , "BetheBlochTPC - Deuteron" , 240,-10,10,250,0,1000);
    fListHist->Add(fhBBDeu);
  }
 
  if(! fhTOF ){
    fhTOF = new TH2F( "fhTOF" , "Scatter Plot TOF" , 240,-10,10,500,0,1.2);
    fListHist->Add(fhTOF);
  }
  if(! fhMassTOF){
    fhMassTOF=new TH1F ("fhMassTOF","Particle Mass - TOF", 100,0 ,10);
    fListHist->Add(fhMassTOF);
  }
  
  EPVzAvsCentrality  = new TH2D("EPVzAvsCentrality" , "EPVzAvsCentrality" , 80, -2, 2,105,-0.5,105.5);
  EPVzCvsCentrality  = new TH2D("EPVzCvsCentrality" , "EPVzCvsCentrality" , 80, -2, 2,105,-0.5,105.5);
  EPTPCvsCentrality  = new TH2D("EPTPCvsCentrality" , "EPTPCvsCentrality" , 80, -2, 2,105,-0.5,105.5);
  EPVzvsCentrality   = new TH2D("EPVzvsCentrality"  , "EPVzvsCentrality"  , 80, -2, 2,105,-0.5,105.5);
  EPTPCpvsCentrality = new TH2D("EPTPCpvsCentrality", "EPTPCpvsCentrality", 80, -2, 2,105,-0.5,105.5);
  EPTPCnvsCentrality = new TH2D("EPTPCnvsCentrality", "EPTPCnvsCentrality", 80, -2, 2,105,-0.5,105.5);

  fListHist->Add(EPVzAvsCentrality);
  fListHist->Add(EPVzCvsCentrality);
  fListHist->Add(EPTPCvsCentrality);
  fListHist->Add(EPVzvsCentrality);
  fListHist->Add(EPTPCpvsCentrality);
  fListHist->Add(EPTPCnvsCentrality);
  
  hEvPlaneTPCvsEvPVz05   = new TH2F("hEvPlaneTPCvsEvPVz05"  ,"hEvPlaneTPCvsEvPVz05"  ,100,-TMath::Pi(),TMath::Pi(),100,-TMath::Pi(),TMath::Pi());                      
  hEvPlaneTPCvsEvPVz075  = new TH2F("hEvPlaneTPCvsEvPVz075" ,"hEvPlaneTPCvsEvPVz075" ,100,-TMath::Pi(),TMath::Pi(),100,-TMath::Pi(),TMath::Pi()); 
  hEvPlaneTPCvsEvPVz1530 = new TH2F("hEvPlaneTPCvsEvPVz1530","hEvPlaneTPCvsEvPVz1530",100,-TMath::Pi(),TMath::Pi(),100,-TMath::Pi(),TMath::Pi());
  hEvPlaneTPCvsEvPVz3050 = new TH2F("hEvPlaneTPCvsEvPVz3050","hEvPlaneTPCvsEvPVz3050",100,-TMath::Pi(),TMath::Pi(),100,-TMath::Pi(),TMath::Pi());                      
  hEvPlaneTPCvsEvPVz2040 = new TH2F("hEvPlaneTPCvsEvPVz2040","hEvPlaneTPCvsEvPVz2040",100,-TMath::Pi(),TMath::Pi(),100,-TMath::Pi(),TMath::Pi());                      
  hEvPlaneTPCvsEvPVz4060 = new TH2F("hEvPlaneTPCvsEvPVz4060","hEvPlaneTPCvsEvPVz4060",100,-TMath::Pi(),TMath::Pi(),100,-TMath::Pi(),TMath::Pi());   
  
  fListHist->Add(hEvPlaneTPCvsEvPVz05);                      
  fListHist->Add(hEvPlaneTPCvsEvPVz075); 
  fListHist->Add(hEvPlaneTPCvsEvPVz1530);
  fListHist->Add(hEvPlaneTPCvsEvPVz3050);                      
  fListHist->Add(hEvPlaneTPCvsEvPVz2040);                      
  fListHist->Add(hEvPlaneTPCvsEvPVz4060);   

  hCos2DeltaTPCVzAvsCentrality   = new TH2F("hCos2DeltaTPCVzAvsCentrality"  ,"hCos2DeltaTPCVzAvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaTPCVzCvsCentrality   = new TH2F("hCos2DeltaTPCVzCvsCentrality"  ,"hCos2DeltaTPCVzCvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzAVzCvsCentrality   = new TH2F("hCos2DeltaVzAVzCvsCentrality"  ,"hCos2DeltaVzAVzCvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzMVzAvsCentrality   = new TH2F("hCos2DeltaVzMVzAvsCentrality"  ,"hCos2DeltaVzMVzAvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzMVzCvsCentrality   = new TH2F("hCos2DeltaVzMVzCvsCentrality"  ,"hCos2DeltaVzMVzCvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzATPCvsCentrality   = new TH2F("hCos2DeltaVzATPCvsCentrality"  ,"hCos2DeltaVzATPCvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzCTPCvsCentrality   = new TH2F("hCos2DeltaVzCTPCvsCentrality"  ,"hCos2DeltaVzCTPCvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzCVzAvsCentrality   = new TH2F("hCos2DeltaVzCVzAvsCentrality"  ,"hCos2DeltaVzCVzAvsCentrality"  ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzMTPCpvsCentrality  = new TH2F("hCos2DeltaVzMTPCpvsCentrality" ,"hCos2DeltaVzMTPCpvsCentrality" ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaVzMTPCnvsCentrality  = new TH2F("hCos2DeltaVzMTPCnvsCentrality" ,"hCos2DeltaVzMTPCnvsCentrality" ,100,-1.1,1.1,105,-0.5,105.5);
  hCos2DeltaTPCpTPCnvsCentrality = new TH2F("hCos2DeltaTPCpTPCnvsCentrality","hCos2DeltaTPCpTPCnvsCentrality",100,-1.1,1.1,105,-0.5,105.5);

  fListHist->Add(hCos2DeltaTPCVzAvsCentrality);
  fListHist->Add(hCos2DeltaTPCVzCvsCentrality);
  fListHist->Add(hCos2DeltaVzAVzCvsCentrality);
  fListHist->Add(hCos2DeltaVzMVzAvsCentrality);
  fListHist->Add(hCos2DeltaVzMVzCvsCentrality);
  fListHist->Add(hCos2DeltaVzATPCvsCentrality);
  fListHist->Add(hCos2DeltaVzCTPCvsCentrality);
  fListHist->Add(hCos2DeltaVzCVzAvsCentrality);
  fListHist->Add(hCos2DeltaVzMTPCpvsCentrality);  
  fListHist->Add(hCos2DeltaVzMTPCnvsCentrality); 
  fListHist->Add(hCos2DeltaTPCpTPCnvsCentrality);

 
  hQVzAQVzCvsCentrality = new TH2F("hQVzAQVzCvsCentrality","hQVzAQVzCvsCentrality",1000,-5,5,105,-0.5,105.5);
  fListHist->Add(hQVzAQVzCvsCentrality);
 
  hQxVzAvsCentrality = new TH2F("hQxVzAvsCentrality","hQxVzAvsCentrality",100,-5,5,105,-0.5,105.5);
  hQyVzAvsCentrality = new TH2F("hQyVzAvsCentrality","hQyVzAvsCentrality",100,-5,5,105,-0.5,105.5);
  hQxVzCvsCentrality = new TH2F("hQxVzCvsCentrality","hQxVzCvsCentrality",100,-5,5,105,-0.5,105.5);
  hQyVzCvsCentrality = new TH2F("hQyVzCvsCentrality","hQyVzCvsCentrality",100,-5,5,105,-0.5,105.5);
  hQxVzMvsCentrality = new TH2F("hQxVzMvsCentrality","hQxVzMvsCentrality",100,-5,5,105,-0.5,105.5);
  hQyVzMvsCentrality = new TH2F("hQyVzMvsCentrality","hQyVzMvsCentrality",100,-5,5,105,-0.5,105.5);

  fListHist->Add(hQxVzAvsCentrality);
  fListHist->Add(hQyVzAvsCentrality);
  fListHist->Add(hQxVzCvsCentrality);
  fListHist->Add(hQyVzCvsCentrality);
  fListHist->Add(hQxVzMvsCentrality);
  fListHist->Add(hQyVzMvsCentrality);

  if(!ftree){
   
    ftree = new TTree("ftree","ftree");
 
    ftree->Branch("tCentrality"      ,&tCentrality      ,"tCentrality/D"    );
    ftree->Branch("tType"            ,&tType            ,"tType/D"          );
    ftree->Branch("tHasTOF"          ,&tHasTOF          ,"tHasTOF/D"        );
    ftree->Branch("tpT"              ,&tpT              ,"tpT/D"            );
    ftree->Branch("tMassTOF"         ,&tMassTOF         ,"tMassTOF/D"       );
    ftree->Branch("tuqV0A"           ,&tuqV0A           ,"tuqV0A/D"         );
    ftree->Branch("tuqV0C"           ,&tuqV0C           ,"tuqV0C/D"         );
    ftree->Branch("tCharge"          ,&tCharge          ,"tCharge/D"        );
    ftree->Branch("tCosdeltaphiTPC"  ,&tCosdeltaphiTPC  ,"tCosdeltaphiTPC/D");
    ftree->Branch("tCosdeltaphiV0M"  ,&tCosdeltaphiV0M  ,"tCosdeltaphiV0M/D");
    ftree->Branch("tCosdeltaphiV0A"  ,&tCosdeltaphiV0A  ,"tCosdeltaphiV0A/D");
    ftree->Branch("tCosdeltaphiV0C"  ,&tCosdeltaphiV0C  ,"tCosdeltaphiV0C/D");
    ftree->Branch("timpactXY"        ,&timpactXY        ,"timpactXY/D"      );
    ftree->Branch("timpactZ"         ,&timpactZ         ,"timpactZ/D"       );
    ftree->Branch("tpull"            ,&tpull            ,"tpull/D"          );
    ftree->Branch("tphi"             ,&tphi             ,"tphi/D"          );

  }

  PostData(1,  fListHist);
  PostData(2,  ftree);
}// end UserCreateOutputObjects


//====================== USER EXEC ========================

void AliAnalysisTaskNucleiv2SP::UserExec(Option_t *) 
{
  // Main loop
  // Called for EACH event
  //  cout<<"AliAnalysisTaskNucleiv2SP Starting UserExec"<<endl;

  Info("AliAnalysisTaskNucleiv2SP","Starting UserExec");  
  
  AliVEvent *event = InputEvent();
  if (!event) { Printf("ERROR: Could not retrieve event"); return; }
  
 
  AliESDEvent* lESDevent = dynamic_cast<AliESDEvent*>(event);
  if (!lESDevent) {
    AliError("Cannot get the ESD event");
    return;
  }  
  
  fHistEventMultiplicity->Fill(1);
  fHistEventMultiplicity->Fill(7);
  
  //_____________________________________________________
  //   Centrality  
  
  AliCentrality *centrality = lESDevent->GetCentrality();
  Float_t percentile=centrality->GetCentralityPercentile("V0M");
  
  Int_t TrackNumber = lESDevent->GetNumberOfTracks();
  fHistTrackMultiplicity->Fill(TrackNumber,percentile); //tracce per evento
  
  //______________________________________________________
  // PID
  
  AliAnalysisManager *man=AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler* inputHandler = (AliInputEventHandler*) (man->GetInputEventHandler());
  fPIDResponse=inputHandler->GetPIDResponse(); 
  
  //=================================================================
  
  Float_t  impactXY=-999., impactZ=-999.;
  Double_t TPCSignal=0.;
  
  ULong_t  status=0;
 
  Double_t pmax  = 10;
  Double_t ptmax = 10;
  // Primary vertex cut
  
  const AliESDVertex *vtx = lESDevent->GetPrimaryVertexTracks();
    
  if(vtx->GetNContributors()<1) {
    
    // SPD vertex cut
    vtx = lESDevent->GetPrimaryVertexSPD();
    
    if(vtx->GetNContributors()<1) {
      Info("AliAnalysisTaskHelium3Pi","No good vertex, skip event");
      return; // NO GOOD VERTEX, SKIP EVENT 
    }
  }
  
  fHistEventMultiplicity->Fill(2); // analyzed events with PV
  
  if(TMath::Abs(vtx->GetZ())>10) return;
  fHistEventMultiplicity->Fill(3);

  Bool_t isSelectedCentral     = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & AliVEvent::kCentral);
  Bool_t isSelectedSemiCentral = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & AliVEvent::kSemiCentral);
  Bool_t isSelectedMB          = (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & AliVEvent::kMB);
    
  fHistTrackMultiplicity->Fill(TrackNumber,percentile); 
    
  Int_t eventtype = -999;
  
  //  cout<<"ET 1: "<<eventtype<<endl;
  
  if(isSelectedCentral){
    fHistEventMultiplicity->Fill(4);
    fHistTrackMultiplicityCentral->Fill(TrackNumber,percentile); 
    eventtype =1;
  }
  
  if(isSelectedSemiCentral){
    fHistEventMultiplicity->Fill(5);
    fHistTrackMultiplicitySemiCentral->Fill(TrackNumber,percentile); 
    eventtype =2;
  }
  
  if(isSelectedMB){
    if(percentile<0)return;
    if(percentile>=80)return;
    fHistEventMultiplicity->Fill(6);
    fHistTrackMultiplicityMB->Fill(TrackNumber,percentile); 
    eventtype =3;
  }
  
  //    cout<<"ET 2: "<<eventtype<<endl;
  
  if(eventtype!=1 && eventtype!=2 && eventtype!=3 )return;
  
  AliEventplane *pl=lESDevent->GetEventplane();
  
  
  if(!pl ){
    AliError("AliAnalysisTaskSENucleiv2SP::UserExec:no eventplane! v2 analysis without eventplane not possible!\n");
    fHistEventMultiplicity->Fill(12);
  }
    
  //Event plane from FLOW
  
  Double_t qxEPa = 0, qyEPa = 0;
  Double_t qxEPc = 0, qyEPc = 0;
  Double_t qxEP =  0 , qyEP = 0;
  
  Double_t evPlAngV0A = pl->CalculateVZEROEventPlane(lESDevent, 8, 2, qxEPa, qyEPa);
  Double_t evPlAngV0C = pl->CalculateVZEROEventPlane(lESDevent, 9, 2, qxEPc, qyEPc);
  Double_t evPlAngV0  = pl->CalculateVZEROEventPlane(lESDevent,10, 2, qxEP,  qyEP);
  
  Double_t Qx2  = 0, Qy2  = 0;
  Double_t Qx2p = 0, Qy2p = 0;
  Double_t Qx2n = 0, Qy2n = 0;
 
  for (Int_t iT = 0; iT < TrackNumber; iT++){
    
    AliESDtrack* track = lESDevent->GetTrack(iT);
    
    if (!track)
      continue;
    
    if ((TMath::Abs(track->Eta()) > 0.8) || (track->Pt() < 0.2) || (track->GetTPCNcls() < 70) || (track->Pt() >= 20.0))
      continue;
    if(!fESDtrackCutsEP->AcceptTrack(track))
      continue;
    if(track->Eta()>0 && track->Eta()<0.8){
      
      Qx2p += TMath::Cos(2*track->Phi());
      Qy2p += TMath::Sin(2*track->Phi());
    }
    if(track->Eta()<0 && track->Eta()> -0.8){
      
      Qx2n += TMath::Cos(2*track->Phi());
      Qy2n += TMath::Sin(2*track->Phi());
    }

    if(track->Eta()>0 && track->Eta()<0.8){ //half TPC
      Qx2 += TMath::Cos(2*track->Phi());
      Qy2 += TMath::Sin(2*track->Phi());
    }
  }
  
  Double_t evPlAngTPC  = TMath::ATan2(Qy2,  Qx2) /2.;
  Double_t evPlAngTPCn = TMath::ATan2(Qy2n, Qx2n)/2.;
  Double_t evPlAngTPCp = TMath::ATan2(Qy2p, Qx2p)/2.;

  EPVzAvsCentrality  ->Fill(evPlAngV0A  , percentile); 
  EPVzCvsCentrality  ->Fill(evPlAngV0C  , percentile); 
  EPVzvsCentrality   ->Fill(evPlAngV0   , percentile); 
  EPTPCvsCentrality  ->Fill(evPlAngTPC  , percentile); 
  EPTPCpvsCentrality ->Fill(evPlAngTPCp , percentile); 
  EPTPCnvsCentrality ->Fill(evPlAngTPCn , percentile); 

  if(percentile>=0 && percentile<=5)
    hEvPlaneTPCvsEvPVz05  ->Fill(evPlAngTPC,evPlAngV0); 
  if(percentile>=0 && percentile<=7.5)
    hEvPlaneTPCvsEvPVz075 ->Fill(evPlAngTPC,evPlAngV0); 
  if(percentile>=15 && percentile<=30)
    hEvPlaneTPCvsEvPVz1530->Fill(evPlAngTPC,evPlAngV0);
  if(percentile>=30 && percentile<50)
    hEvPlaneTPCvsEvPVz3050->Fill(evPlAngTPC,evPlAngV0);  
  if(percentile>=20 && percentile<=40)                    
    hEvPlaneTPCvsEvPVz2040->Fill(evPlAngTPC,evPlAngV0);   
  if(percentile>=40 && percentile<=60)                   
    hEvPlaneTPCvsEvPVz4060->Fill(evPlAngTPC,evPlAngV0);           

  // For TPC, V0M, V0c and V0A resolution 

  hCos2DeltaTPCVzAvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngTPC - evPlAngV0A)) , percentile);
  hCos2DeltaTPCVzCvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngTPC - evPlAngV0C)) , percentile);
  hCos2DeltaVzAVzCvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngV0A - evPlAngV0C)) , percentile);
  hCos2DeltaVzMVzAvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngV0  - evPlAngV0A)) , percentile);
  hCos2DeltaVzMVzCvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngV0  - evPlAngV0C)) , percentile);
  hCos2DeltaVzATPCvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngV0A - evPlAngTPC)) , percentile);
  hCos2DeltaVzCTPCvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngV0C - evPlAngTPC)) , percentile);
  hCos2DeltaVzCVzAvsCentrality  ->Fill(TMath::Cos(2.*(evPlAngV0C - evPlAngV0A)) , percentile);
  hCos2DeltaVzMTPCpvsCentrality ->Fill(TMath::Cos(2.*(evPlAngV0  - evPlAngTPCp)), percentile);
  hCos2DeltaVzMTPCnvsCentrality ->Fill(TMath::Cos(2.*(evPlAngV0  - evPlAngTPCn)), percentile);
  hCos2DeltaTPCpTPCnvsCentrality->Fill(TMath::Cos(2.*(evPlAngTPCp- evPlAngTPCn)), percentile);

  //Scalar Product
  
  Double_t  QV0AQV0C = qxEPa * qxEPc + qyEPa*qyEPc;
  hQVzAQVzCvsCentrality->Fill(QV0AQV0C,percentile);
  
  //NUA correction

  hQxVzAvsCentrality->Fill(qxEPa,percentile);
  hQyVzAvsCentrality->Fill(qyEPa,percentile);
  hQxVzCvsCentrality->Fill(qxEPc,percentile);
  hQyVzCvsCentrality->Fill(qyEPc,percentile);
  hQxVzMvsCentrality->Fill(qxEP ,percentile);
  hQyVzMvsCentrality->Fill(qyEP ,percentile);

  //====================================================================================================================
  
  // To remove auto-correlation
  TVector2 *q = 0x0;
  q = pl->GetQVector();

  Double_t ptcExp  = -999;
  Double_t pullTPC = -999;
  Double_t expbeta = -999;
  Double_t pullTOF = -999;
  Float_t deltaphiTPC = -3;
  Float_t deltaphiV0  = -3;
  Float_t deltaphiV0A = -3;
  Float_t deltaphiV0C = -3;

  Double_t massd   = 1.875612762;
  Double_t masst   = 2.808939;
  Double_t mass3he = 2.80892;

  Float_t  uqV0A = -999;
  Float_t  uqV0C = -999; 

  for (Int_t j=0; j<TrackNumber; j++) { //loop on tracks
      
    AliESDtrack *esdtrack=lESDevent->GetTrack(j);
    if (!fESDtrackCuts->AcceptTrack(esdtrack)) continue;
    
    status  = (ULong_t)esdtrack->GetStatus();
   
    Bool_t hasTOFout  = status&AliESDtrack::kTOFout; 
    Bool_t hasTOF     = kFALSE;
    if (hasTOFout) hasTOF = kTRUE;
    Float_t length = esdtrack->GetIntegratedLength(); 
    if (length < 350.) hasTOF = kFALSE;
    
    TPCSignal=esdtrack->GetTPCsignal(); 
      
    if(TPCSignal<10)continue;
    if(TPCSignal>1000)continue;
    if(!esdtrack->GetInnerParam()) continue;
    
    Double_t ptot = esdtrack->GetInnerParam()->GetP(); // momentum for dEdx determination
 
    //   if(ptot<0.2)continue;
    if(ptot<0.6)continue;
    fhBB->Fill(ptot*esdtrack->GetSign(),TPCSignal);
    esdtrack->GetImpactParameters(impactXY, impactZ);
              
    ptcExp = -999;
    if(fptc==1)
      ptcExp  = AliExternalTrackParam::BetheBlochAleph(ptot/(0.938*2),1.45802,27.4992,4.00313e-15,2.48485,8.31768);
    if(fptc==2)
      ptcExp  = AliExternalTrackParam::BetheBlochAleph(ptot/(0.938*3),1.45802,27.4992,4.00313e-15,2.48485,8.31768);
    if(fptc==3)
      ptcExp  = 4*AliExternalTrackParam::BetheBlochAleph(2*ptot/(0.938*3),1.74962,27.4992,4.00313e-15,2.42485,8.31768);
    
    pullTPC  = (TPCSignal - ptcExp)/(0.07*ptcExp);

    Double_t p    = esdtrack->P();
    Double_t tof  = esdtrack->GetTOFsignal()-fPIDResponse->GetTOFResponse().GetStartTime(p);
    Double_t tPhi = esdtrack->Phi();

    Float_t  beta = 0;
    Float_t  gamma = 0;
    Float_t  mass  = -99;
    Double_t pt  = esdtrack->Pt();
 
    if(fptc==1)
      expbeta = TMath::Sqrt(1-((massd*massd)/(ptot*ptot+massd*massd))); 
    if(fptc==2)
      expbeta = TMath::Sqrt(1-((masst*masst)/(ptot*ptot+masst*masst))); 
    if(fptc==3)
      expbeta = TMath::Sqrt(1-((mass3he*mass3he)/(ptot*ptot+mass3he*mass3he))); 
     

    if(fptc==3)
      pt = 2*pt;

    if(TMath::Abs(ptot) < pmax  && TMath::Abs(pt) < ptmax){
      // if(TMath::Abs(pullTPC) <= 3)
      
      //
      // Process TOF information
      //
      if(!hasTOF)continue;
      //      if (hasTOF) {
      beta = length / (2.99792457999999984e-02 * tof);
      gamma = 1/TMath::Sqrt(1 - beta*beta);
      mass = ptot/TMath::Sqrt(gamma*gamma - 1); // using inner TPC mom. as approx.
  
      //   cout<<expbeta<<" "<<beta<<" "<<(beta - expbeta)/(0.008*expbeta)<<endl;
      pullTOF  = (beta - expbeta)/(0.01*expbeta);

      if(TMath::Abs(ptot)< 2)
	if(TMath::Abs(pullTPC) > 3)continue;
      
      // if(TMath::Abs(ptot)< 4)
      // 	if( beta>1)continue;
      if(TMath::Abs(pullTOF) > 3)continue;

      if(fptc==1){
	if(TMath::Abs(mass) > 2.65)continue;
	if(TMath::Abs(mass) < 1.05)continue;
      }
      if(fptc==2){
	if(TMath::Abs(mass) > 5.0)continue;
	if(TMath::Abs(mass) < 1.8 )continue;
      }
      if(fptc==3){
	if(TMath::Abs(mass) > 5.0)continue;
	if(TMath::Abs(mass) < 1.8)continue;
      }
      fhMassTOF->Fill(mass);
      //}
      fhTOF->Fill(ptot*esdtrack->GetSign(),beta);
      fhBBDeu->Fill(ptot*esdtrack->GetSign(),TPCSignal);
      
   
      // Event Plane
      // Remove AutoCorrelation
      
      evPlAngTPC = GetEventPlaneForCandidate(esdtrack,q,pl);
      
      deltaphiTPC=TMath::Cos(2*GetPhi0Pi(tPhi-evPlAngTPC));
      deltaphiV0 =TMath::Cos(2*GetPhi0Pi(tPhi-evPlAngV0 ));
      deltaphiV0A=TMath::Cos(2*GetPhi0Pi(tPhi-evPlAngV0A));
      deltaphiV0C=TMath::Cos(2*GetPhi0Pi(tPhi-evPlAngV0C));
      
      // Scalar Product
      
      uqV0A = TMath::Cos(2*tPhi)*qxEPa+TMath::Sin(2*tPhi)*qyEPa;
      uqV0C = TMath::Cos(2*tPhi)*qxEPc+TMath::Sin(2*tPhi)*qyEPc;
       
      tCentrality      = percentile;
      tType            = eventtype;
      tHasTOF          = hasTOF;
      tpT              = pt;
      tMassTOF         = mass;
      tuqV0A           = uqV0A;
      tuqV0C           = uqV0C;
      tCharge          = esdtrack->GetSign();
      tCosdeltaphiTPC  = deltaphiTPC;
      tCosdeltaphiV0M  = deltaphiV0;
      tCosdeltaphiV0A  = deltaphiV0A;
      tCosdeltaphiV0C  = deltaphiV0C;
      timpactXY        = impactXY;
      timpactZ         = impactZ;
      tpull            = pullTPC;
      tphi             = tPhi;

      ftree->Fill();
    } 
  }  //track
  
  PostData(1, fListHist);
  PostData(2, ftree);
} //end userexec


//________________________________________________________________________

void AliAnalysisTaskNucleiv2SP::Terminate(Option_t *) 
{
  // Draw result to the screen
  // Called once at the end of the query
}

