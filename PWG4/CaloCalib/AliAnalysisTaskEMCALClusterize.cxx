/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id: $ */

//_________________________________________________________________________
// This analysis provides a new list of clusters to be used in other analysis
//
// Author: Gustavo Conesa Balbastre,
//         Adapted from analysis class from Deepa Thomas
//
//
//_________________________________________________________________________

// --- Root ---
#include "TString.h"
#include "TRefArray.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TGeoManager.h"

// --- AliRoot Analysis Steering
#include "AliAnalysisTask.h"
#include "AliAnalysisManager.h"
#include "AliESDEvent.h"
#include "AliGeomManager.h"
#include "AliVCaloCells.h"
#include "AliAODCaloCluster.h"
#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBEntry.h"
#include "AliLog.h"
#include "AliVEventHandler.h"

// --- EMCAL
#include "AliEMCALRecParam.h"
#include "AliEMCALAfterBurnerUF.h"
#include "AliEMCALGeometry.h"
#include "AliEMCALClusterizerNxN.h"
#include "AliEMCALClusterizerv1.h"
#include "AliEMCALRecPoint.h"
#include "AliEMCALDigit.h"
#include "AliCaloCalibPedestal.h"
#include "AliEMCALCalibData.h"
#include "AliEMCALRecoUtils.h"

#include "AliAnalysisTaskEMCALClusterize.h"

ClassImp(AliAnalysisTaskEMCALClusterize)

//________________________________________________________________________
AliAnalysisTaskEMCALClusterize::AliAnalysisTaskEMCALClusterize(const char *name) 
  : AliAnalysisTaskSE(name)
  , fGeom(0), fGeomName("EMCAL_FIRSTYEARV1"),  fGeomMatrixSet(kFALSE), fLoadGeomMatrices(kFALSE)
  , fCalibData(0),       fPedestalData(0),     fOCDBpath("raw://")
  , fDigitsArr(0),       fClusterArr(0),       fCaloClusterArr(0)
  , fRecParam(0),        fClusterizer(0),      fUnfolder(0),           fJustUnfold(kFALSE) 
  , fOutputAODBranch(0), fOutputAODBranchName("newEMCALClusters"),     fFillAODFile(kTRUE)
  , fRun(-1),            fRecoUtils(0)
  
  {
  //ctor
  for(Int_t i = 0; i < 10; i++) fGeomMatrix[i] = 0 ;
  fDigitsArr       = new TClonesArray("AliEMCALDigit",200);
  fClusterArr      = new TObjArray(100);
  fCaloClusterArr  = new TObjArray(100);
  fRecParam        = new AliEMCALRecParam;
  fBranchNames     = "ESD:AliESDHeader.,EMCALCells.";
  fRecoUtils       = new AliEMCALRecoUtils();
}

//________________________________________________________________________
AliAnalysisTaskEMCALClusterize::AliAnalysisTaskEMCALClusterize() 
  : AliAnalysisTaskSE("DefaultAnalysis_AliAnalysisTaskEMCALClusterize")
  , fGeom(0), fGeomName("EMCAL_FIRSTYEARV1"),  fGeomMatrixSet(kFALSE), fLoadGeomMatrices(kFALSE)
  , fCalibData(0),       fPedestalData(0),     fOCDBpath("raw://")
  , fDigitsArr(0),       fClusterArr(0),       fCaloClusterArr(0)
  , fRecParam(0),        fClusterizer(0),      fUnfolder(0),           fJustUnfold(kFALSE)
  , fOutputAODBranch(0), fOutputAODBranchName("newEMCALClusters"),     fFillAODFile(kFALSE)
  , fRun(-1),            fRecoUtils(0)
{
  // Constructor
  for(Int_t i = 0; i < 10; i++) fGeomMatrix[i] = 0 ;
  fDigitsArr       = new TClonesArray("AliEMCALDigit",200);
  fClusterArr      = new TObjArray(100);
  fCaloClusterArr  = new TObjArray(100);
  fRecParam        = new AliEMCALRecParam;
  fBranchNames     = "ESD:AliESDHeader.,EMCALCells.";
  fRecoUtils       = new AliEMCALRecoUtils();
}

//________________________________________________________________________
AliAnalysisTaskEMCALClusterize::~AliAnalysisTaskEMCALClusterize()
{
  //dtor 
  
  if (fDigitsArr){
    fDigitsArr->Clear("C");
    delete fDigitsArr; 
  }
  
  if (fClusterArr){
    fClusterArr->Delete();
    delete fClusterArr;
  }
  
  if (fCaloClusterArr){
    fCaloClusterArr->Delete();
    delete fCaloClusterArr; 
  }

  if(fClusterizer) delete fClusterizer;
  if(fUnfolder)    delete fUnfolder;   
  if(fRecoUtils)   delete fRecoUtils;

}

//-------------------------------------------------------------------
void AliAnalysisTaskEMCALClusterize::UserCreateOutputObjects()
{
  // Init geometry, create list of output clusters
  
  fGeom =  AliEMCALGeometry::GetInstance(fGeomName) ;	

  fOutputAODBranch = new TClonesArray("AliAODCaloCluster", 0);
  fOutputAODBranch->SetName(fOutputAODBranchName);
  AddAODBranch("TClonesArray", &fOutputAODBranch);
  Info("UserCreateOutputObjects","Create Branch: %s",fOutputAODBranchName.Data());
}

//________________________________________________________________________
void AliAnalysisTaskEMCALClusterize::UserExec(Option_t *) 
{
  // Main loop
  // Called for each event
  
  //Remove the contents of output list set in the previous event 
  fOutputAODBranch->Clear("C");
  
  AliVEvent   * event    = InputEvent();
  AliESDEvent * esdevent = dynamic_cast<AliESDEvent*>(InputEvent());

  if (!event) {
    Error("UserExec","Event not available");
    return;
  }
  
  //Magic line to write events to AOD file
  AliAnalysisManager::GetAnalysisManager()->GetOutputEventHandler()->SetFillAOD(fFillAODFile);
  LoadBranches();
  
  AccessOCDB();

  //-------------------------------------------------------------------------------------
  //Set the geometry matrix, for the first event, skip the rest
  //-------------------------------------------------------------------------------------
  if(!fGeomMatrixSet){
    if(fLoadGeomMatrices){
      for(Int_t mod=0; mod < (fGeom->GetEMCGeometry())->GetNumberOfSuperModules(); mod++){
        if(fGeomMatrix[mod]){
          if(DebugLevel() > 1) 
            fGeomMatrix[mod]->Print();
          fGeom->SetMisalMatrix(fGeomMatrix[mod],mod) ;  
        }
        fGeomMatrixSet=kTRUE;
      }//SM loop
    }//Load matrices
    else if(!gGeoManager){
      Info("UserExec","Get geo matrices from data");
      //Still not implemented in AOD, just a workaround to be able to work at least with ESDs	
      if(!strcmp(event->GetName(),"AliAODEvent")) {
        if(DebugLevel() > 1) 
          Warning("UserExec","Use ideal geometry, values geometry matrix not kept in AODs.");
      }//AOD
      else {	
        if(DebugLevel() > 1) 
          Info("UserExec","AliAnalysisTaskEMCALClusterize Load Misaligned matrices.");
        AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event) ;
        if(!esd) {
          Error("UserExec","This event does not contain ESDs?");
          return;
        }
        for(Int_t mod=0; mod < (fGeom->GetEMCGeometry())->GetNumberOfSuperModules(); mod++){
          if(DebugLevel() > 1) 
            esd->GetEMCALMatrix(mod)->Print();
          if(esd->GetEMCALMatrix(mod)) fGeom->SetMisalMatrix(esd->GetEMCALMatrix(mod),mod) ;
        } 
        fGeomMatrixSet=kTRUE;
      }//ESD
    }//Load matrices from Data 
  }//first event

  //Get the list of cells needed for unfolding and reclustering
  AliVCaloCells *cells= event->GetEMCALCells();
  Int_t nClustersOrg = 0;
  //-------------------------------------------
  //---------Unfolding clusters----------------
  //-------------------------------------------
  if (fJustUnfold) {
    
    //Fill the array with the EMCAL clusters, copy them
    for (Int_t i = 0; i < event->GetNumberOfCaloClusters(); i++)
    {
      AliVCluster *clus = event->GetCaloCluster(i);
      if(clus->IsEMCAL()){        
        //printf("Org Cluster %d, E %f\n",i, clus->E());
        AliESDCaloCluster * esdCluster = dynamic_cast<AliESDCaloCluster*> (clus);
        AliAODCaloCluster * aodCluster = dynamic_cast<AliAODCaloCluster*> (clus);
        if     (esdCluster){
          fCaloClusterArr->Add( new AliESDCaloCluster(*esdCluster) );   
        }//ESD
        else if(aodCluster){
          fCaloClusterArr->Add( new AliAODCaloCluster(*aodCluster) );   
        }//AOD
        else 
          Warning("UserExec()"," - Wrong CaloCluster type?");
        nClustersOrg++;
      }
    }
    
    //Do the unfolding
    fUnfolder->UnfoldClusters(fCaloClusterArr, cells);
    
    //CLEAN-UP
    fUnfolder->Clear();
    
    //printf("nClustersOrg %d\n",nClustersOrg);
  }
  //-------------------------------------------
  //---------- Recluster cells ----------------
  //-------------------------------------------
  
  else{
    //-------------------------------------------------------------------------------------
    //Transform CaloCells into Digits
    //-------------------------------------------------------------------------------------
    Int_t idigit =  0;
    Int_t id     = -1;
    Float_t amp  = -1; 
    Float_t time = -1; 
    
    Double_t cellAmplitude = 0;
    Double_t cellTime      = 0;
    Short_t  cellNumber    = 0;
        
    TTree *digitsTree = new TTree("digitstree","digitstree");
    digitsTree->Branch("EMCAL","TClonesArray", &fDigitsArr, 32000);
    
    for (Int_t icell = 0; icell < cells->GetNumberOfCells(); icell++)
    {
      if (cells->GetCell(icell, cellNumber, cellAmplitude, cellTime) != kTRUE)
        break;
      
      time = cellTime;
      amp  = cellAmplitude;
      id   = cellNumber;
      
      AliEMCALDigit *digit = (AliEMCALDigit*) fDigitsArr->New(idigit);
      digit->SetId(id);
      digit->SetAmplitude(amp);
      digit->SetTime(time);
      digit->SetTimeR(time);
      digit->SetIndexInList(idigit);
      digit->SetType(AliEMCALDigit::kHG);
      //if(Entry()==0) printf("Digit: Id %d, amp %f, time %e, index %d\n",id, amp,time,idigit);
      idigit++;
    }
    
    //Fill the tree with digits
    digitsTree->Fill();
    
    //-------------------------------------------------------------------------------------
    //Do the clusterization
    //-------------------------------------------------------------------------------------        
    TTree *clustersTree = new TTree("clustertree","clustertree");

    fClusterizer->SetInput(digitsTree);
    fClusterizer->SetOutput(clustersTree);
    fClusterizer->Digits2Clusters("");
    
    //-------------------------------------------------------------------------------------
    //Transform the recpoints into AliVClusters
    //-------------------------------------------------------------------------------------
    
    clustersTree->SetBranchStatus("*",0); //disable all branches
    clustersTree->SetBranchStatus("EMCALECARP",1); //Enable only the branch we need
    
    TBranch *branch = clustersTree->GetBranch("EMCALECARP");
    branch->SetAddress(&fClusterArr);
    branch->GetEntry(0);
    
    RecPoints2Clusters(fDigitsArr, fClusterArr, fCaloClusterArr);
    
    //---CLEAN UP-----
    fClusterizer->Clear();
    fDigitsArr  ->Clear("C");
    fClusterArr ->Delete(); // Do not Clear(), it leaks, why?

    clustersTree->Delete("all");
    digitsTree  ->Delete("all");
  }
  
  //Recalculate track-matching for the new clusters, only with ESDs
  if(esdevent)fRecoUtils->FindMatches(esdevent,fCaloClusterArr);

  
  //-------------------------------------------------------------------------------------
  //Put the new clusters in the AOD list
  //-------------------------------------------------------------------------------------
  
  Int_t kNumberOfCaloClusters   = fCaloClusterArr->GetEntries();
  //Info("UserExec","New clusters %d",kNumberOfCaloClusters);
  //if(nClustersOrg!=kNumberOfCaloClusters) Info("UserExec","Different number: Org %d, New %d\n",nClustersOrg,kNumberOfCaloClusters);
  for(Int_t i = 0; i < kNumberOfCaloClusters; i++){
    AliAODCaloCluster *newCluster = (AliAODCaloCluster *) fCaloClusterArr->At(i);
    //if(Entry()==0) Info("UserExec","newCluster E %f\n", newCluster->E());
    
    //Add matched track, if any, only with ESDs
    if(esdevent){
      Int_t trackIndex = fRecoUtils->GetMatchedTrackIndex(i);
      if(trackIndex >= 0){
        newCluster->AddTrackMatched(event->GetTrack(trackIndex));
        if(DebugLevel() > 1) 
          Info("UserExec","Matched Track index %d to new cluster %d \n",trackIndex,i);
      }
    }
    
    newCluster->SetID(i);
    new((*fOutputAODBranch)[i])  AliAODCaloCluster(*newCluster);
  }
  
  //---CLEAN UP-----
  fCaloClusterArr->Delete(); // Do not Clear(), it leaks, why?
}      

//_____________________________________________________________________
Bool_t AliAnalysisTaskEMCALClusterize::AccessOCDB()
{
  //Access to OCDB stuff

  AliVEvent * event = InputEvent();
  if (!event)
  {
    Warning("AccessOCDB","Event not available!!!");
    return kFALSE;
  }

  if (event->GetRunNumber()==fRun)
    return kTRUE;
  fRun = event->GetRunNumber();

  if(DebugLevel() > 1 )
    Info("AccessODCD()"," Begin");

  fGeom = AliEMCALGeometry::GetInstance(fGeomName);
  
  
  AliCDBManager *cdb = AliCDBManager::Instance();
    

  if (fOCDBpath.Length()){
    cdb->SetDefaultStorage(fOCDBpath.Data());
    Info("AccessOCDB","Default storage %s",fOCDBpath.Data());
  }
  
  cdb->SetRun(event->GetRunNumber());

  //
  // EMCAL from RAW OCDB
  if (fOCDBpath.Contains("alien:"))
  {
    cdb->SetSpecificStorage("EMCAL/Calib/Data","alien://Folder=/alice/data/2010/OCDB");
    cdb->SetSpecificStorage("EMCAL/Calib/Pedestals","alien://Folder=/alice/data/2010/OCDB");
  }

  TString path = cdb->GetDefaultStorage()->GetBaseFolder();
  
  // init parameters:
  
  //Get calibration parameters	
  if(!fCalibData)
  {
    AliCDBEntry *entry = (AliCDBEntry*) 
      AliCDBManager::Instance()->Get("EMCAL/Calib/Data");
    
    if (entry) fCalibData =  (AliEMCALCalibData*) entry->GetObject();
  }
  
  if(!fCalibData)
    AliFatal("Calibration parameters not found in CDB!");
    
  //Get calibration parameters	
  if(!fPedestalData)
  {
    AliCDBEntry *entry = (AliCDBEntry*) 
      AliCDBManager::Instance()->Get("EMCAL/Calib/Pedestals");
    
    if (entry) fPedestalData =  (AliCaloCalibPedestal*) entry->GetObject();
  }
    
  if(!fPedestalData)
    AliFatal("Dead map not found in CDB!");

  InitClusterization();
  
  return kTRUE;
}

//________________________________________________________________________________________
void AliAnalysisTaskEMCALClusterize::InitClusterization()
{
  //Select clusterization/unfolding algorithm and set all the needed parameters
  
  if (fJustUnfold){
    // init the unfolding afterburner 
    delete fUnfolder;
    fUnfolder =  new AliEMCALAfterBurnerUF(fRecParam->GetW0(),fRecParam->GetLocMaxCut());
    return;
 }

  //First init the clusterizer
  delete fClusterizer;
  if     (fRecParam->GetClusterizerFlag() == AliEMCALRecParam::kClusterizerv1)
    fClusterizer = new AliEMCALClusterizerv1 (fGeom, fCalibData, fPedestalData);
  else if(fRecParam->GetClusterizerFlag() == AliEMCALRecParam::kClusterizerNxN) 
    fClusterizer = new AliEMCALClusterizerNxN(fGeom, fCalibData, fPedestalData);
  else if(fRecParam->GetClusterizerFlag() > AliEMCALRecParam::kClusterizerNxN) {
   AliEMCALClusterizerNxN *clusterizer = new AliEMCALClusterizerNxN(fGeom, fCalibData, fPedestalData);
   clusterizer->SetNRowDiff(2);
   clusterizer->SetNColDiff(2);
   fClusterizer = clusterizer;
  } else{
    AliFatal(Form("Clusterizer < %d > not available", fRecParam->GetClusterizerFlag()));
  }
    
  //Now set the parameters
  fClusterizer->SetECAClusteringThreshold( fRecParam->GetClusteringThreshold() );
  fClusterizer->SetECALogWeight          ( fRecParam->GetW0()                  );
  fClusterizer->SetMinECut               ( fRecParam->GetMinECut()             );    
  fClusterizer->SetUnfolding             ( fRecParam->GetUnfold()              );
  fClusterizer->SetECALocalMaxCut        ( fRecParam->GetLocMaxCut()           );
  fClusterizer->SetTimeCut               ( fRecParam->GetTimeCut()             );
  fClusterizer->SetTimeMin               ( fRecParam->GetTimeMin()             );
  fClusterizer->SetTimeMax               ( fRecParam->GetTimeMax()             );
  fClusterizer->SetInputCalibrated       ( kTRUE                               );
    
  //In case of unfolding after clusterization is requested, set the corresponding parameters
  if(fRecParam->GetUnfold()){
    Int_t i=0;
    for (i = 0; i < 8; i++) {
      fClusterizer->SetSSPars(i, fRecParam->GetSSPars(i));
    }//end of loop over parameters
    for (i = 0; i < 3; i++) {
      fClusterizer->SetPar5  (i, fRecParam->GetPar5(i));
      fClusterizer->SetPar6  (i, fRecParam->GetPar6(i));
    }//end of loop over parameters
    
    fClusterizer->InitClusterUnfolding();
  }// to unfold
}

//________________________________________________________________________________________
void AliAnalysisTaskEMCALClusterize::RecPoints2Clusters(TClonesArray *digitsArr, TObjArray *recPoints, TObjArray *clusArray)
{
  // Restore clusters from recPoints
  // Cluster energy, global position, cells and their amplitude fractions are restored
  
  for(Int_t i = 0; i < recPoints->GetEntriesFast(); i++)
  {
    AliEMCALRecPoint *recPoint = (AliEMCALRecPoint*) recPoints->At(i);
    
    const Int_t ncells = recPoint->GetMultiplicity();
    Int_t ncells_true = 0;
    
    // cells and their amplitude fractions
    UShort_t   absIds[ncells];  
    Double32_t ratios[ncells];
    
    for (Int_t c = 0; c < ncells; c++) {
      AliEMCALDigit *digit = (AliEMCALDigit*) digitsArr->At(recPoint->GetDigitsList()[c]);
      
      absIds[ncells_true] = digit->GetId();
      ratios[ncells_true] = recPoint->GetEnergiesList()[c]/digit->GetAmplitude();
      
      if (ratios[ncells_true] > 0.001) ncells_true++;
    }
    
    if (ncells_true < 1) {
      Warning("AliAnalysisTaskEMCALClusterize::RecPoints2Clusters", "skipping cluster with no cells");
      continue;
    }
    
    TVector3 gpos;
    Float_t g[3];
    
    // calculate new cluster position
    recPoint->EvalGlobalPosition(fRecParam->GetW0(), digitsArr);
    recPoint->GetGlobalPosition(gpos);
    gpos.GetXYZ(g);
    
    // create a new cluster
    AliAODCaloCluster *clus = new AliAODCaloCluster();
    clus->SetType(AliVCluster::kEMCALClusterv1);
    clus->SetE(recPoint->GetEnergy());
    clus->SetPosition(g);
    clus->SetNCells(ncells_true);
    clus->SetCellsAbsId(absIds);
    clus->SetCellsAmplitudeFraction(ratios);
    clus->SetDispersion(recPoint->GetDispersion());
    clus->SetChi2(-1); //not yet implemented
    clus->SetTOF(recPoint->GetTime()) ; //time-of-flight
    clus->SetNExMax(recPoint->GetNExMax()); //number of local maxima
    Float_t elipAxis[2];
    recPoint->GetElipsAxis(elipAxis);
    clus->SetM02(elipAxis[0]*elipAxis[0]) ;
    clus->SetM20(elipAxis[1]*elipAxis[1]) ;
    clus->SetDistToBadChannel(recPoint->GetDistanceToBadTower()); 
    clusArray->Add(clus);
  } // recPoints loop
}
