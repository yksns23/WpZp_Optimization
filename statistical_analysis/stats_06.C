// A lightweight calculator for local_p values (final)

#include "TFile.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooDataSet.h"

#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooStats/HypoTestCalculatorGeneric.h"
#include "RooStats/FrequentistCalculator.h"
#include "RooStats/ToyMCSampler.h"
#include "RooStats/HypoTestResult.h"

using namespace RooFit;
using namespace RooStats;

// Frequentist p value calculator
void local_p (  const char* file_name,
                const char* workspace_name = "combined",
                const char* sbmodel_name = "ModelConfig",
                const char* bmodel_name = "",
                const char* data_name = "asimovData"){

  ///// PART 1: Setup /////
  //----- Get File -----//
  TFile *file = TFile::Open(file_name);
  if(!file){
    cout << "File not found\nBye" << endl;
    return;
  }

  //----- Get Workspace -----//
  RooWorkspace *w = (RooWorkspace*) file->Get(workspace_name);
  if (!w){
    cout << "Workspace not found\nBye" << endl;
    return;
  }

  //----- Get ModelConfig -----//
  // signal + background model
  ModelConfig *sbModel = (ModelConfig*) w->obj(sbmodel_name);

  // Get observed data.
  RooAbsData *data = (RooAbsData*) w->data(data_name);
  if (!data){
    cout << "Data not found\nBye" << endl;
    return;
  }

  // background only model
  ModelConfig *bModel = (ModelConfig*) w->obj(bmodel_name);
  
  if (!bModel){
    bModel = (ModelConfig*) sbModel->Clone();
    bModel->SetName(TString(sbmodel_name)+TString("_bmodel"));
    RooRealVar *poi = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
    double oldval = poi->getVal();
    poi->setVal(0);
    bModel->SetSnapshot(RooArgSet(*poi));
    poi->setVal(oldval);
  }

  if (!sbModel->GetSnapshot()){
    RooRealVar *poi = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
    sbModel->SetSnapshot(RooArgSet(*poi));
  }    

  ///// PART 2: Calculate local p for given mass /////

  //----- Configure Frequentist Method -----//
  // Set Test Statistic
  ProfileLikelihoodTestStat *profll = new ProfileLikelihoodTestStat(*sbModel->GetPdf());
  profll->SetOneSidedDiscovery(true);
  profll->SetVarName( "q_{0}/2" );
  profll->SetPrintLevel(0);

  // Set FrequentistCalculator
  HypoTestCalculatorGeneric *hc = new FrequentistCalculator(*data, *sbModel, *bModel);
  ((FrequentistCalculator*) hc)->SetToys(5000, 1250);

  // Set ToyMCSampler
  ToyMCSampler *sampler = (ToyMCSampler*)hc->GetTestStatSampler();
  sampler->SetGenerateBinned(true);
  sampler->SetTestStatistic(profll);

  //----- Get Results -----//
  HypoTestResult *result = hc->GetHypoTest();
  result->SetPValueIsRightTail(true);
  result->SetBackgroundAsAlt(false);
  result->Print();

  // Plot
  HypoTestPlot *plot = new HypoTestPlot(*result, 100);
  plot->SetLogYaxis(true);
  plot->Draw();
}
