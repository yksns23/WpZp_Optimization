// A lightweight calculator for local_p values

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

/*
  double poiValue = -1;
  bool noSystematics = false;
  // get the workspace out of the file
  RooWorkspace* w = (RooWorkspace*) file->Get(workspace_name);
  if(!w){
    cout <<"workspace not found" << endl;
    return;
  }
  w->Print();

  // get the modelConfig out of the file
  ModelConfig* sbModel = (ModelConfig*) w->obj(sbmodel_name);


  // get the modelConfig out of the file
  RooAbsData* data = w->data(data_name);

  // make sure ingredients are found
  if(!data || !sbModel){
    w->Print();
    cout << "data or ModelConfig was not found" <<endl;
    return;
  }
  // make b model
  ModelConfig* bModel = (ModelConfig*) w->obj(bmodel_name);


   // case of no systematics
   // remove nuisance parameters from model
   if (noSystematics) { 
      const RooArgSet * nuisPar = sbModel->GetNuisanceParameters();
      if (nuisPar && nuisPar->getSize() > 0) { 
         std::cout << "StandardHypoTestInvDemo" << "  -  Switch off all systematics by setting them constant to their initial values" << std::endl;
         RooStats::SetAllConstant(*nuisPar);
      }
      if (bModel) { 
         const RooArgSet * bnuisPar = bModel->GetNuisanceParameters();
         if (bnuisPar) 
            RooStats::SetAllConstant(*bnuisPar);
      }
   }


  if (!bModel ) {
      Info("StandardHypoTestInvDemo","The background model %s does not exist",bmodel_name);
      Info("StandardHypoTestInvDemo","Copy it from ModelConfig %s and set POI to zero", sbmodel_name);
      bModel = (ModelConfig*) sbModel->Clone();
      bModel->SetName(TString(sbmodel_name)+TString("B_only"));      
      RooRealVar * var = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
      if (!var) return;
      double oldval = var->getVal();
      var->setVal(0);
      //bModel->SetSnapshot( RooArgSet(*var, *w->var("lumi"))  );
      bModel->SetSnapshot( RooArgSet(*var)  );
      var->setVal(oldval);
  }
  
   if (!sbModel->GetSnapshot() || poiValue > 0) { 
      Info("StandardHypoTestDemo","Model %s has no snapshot  - make one using model poi",sbmodel_name);
      RooRealVar * var = dynamic_cast<RooRealVar*>(sbModel->GetParametersOfInterest()->first());
      if (!var) return;
      double oldval = var->getVal();
      if (poiValue > 0)  var->setVal(poiValue);
      //sbModel->SetSnapshot( RooArgSet(*var, *w->var("lumi") ) );
      sbModel->SetSnapshot( RooArgSet(*var) );
      if (poiValue > 0) var->setVal(oldval);
      //sbModel->SetSnapshot( *sbModel->GetParametersOfInterest() );
   }
*/
  ///// PART 2: Calculate local p for given mass /////

  //----- Configure Frequentist Method -----//
  // Set Test Statistic
  ProfileLikelihoodTestStat *profll = new ProfileLikelihoodTestStat(*sbModel->GetPdf());
  profll->SetOneSidedDiscovery(true);
  profll->SetVarName( "q_{0}/2" );

  // Set FrequentistCalculator
  HypoTestCalculatorGeneric *hc = new FrequentistCalculator(*data, *sbModel, *bModel);
  ((FrequentistCalculator*) hc)->SetToys(5000, 1250);

  // Set ToyMCSampler
  ToyMCSampler *sampler = (ToyMCSampler*)hc->GetTestStatSampler();
  int nEvents = data->numEntries();
  sampler->SetNEventsPerToy(nEvents);
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
