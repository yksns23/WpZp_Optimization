// A lightweight calculator for significance values (final)

#include "TFile.h"
#include "TNtuple.h"
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

void sigma (    const char* infile,
                float_t     mwp,
                const char* workspace_name,
                const char* outfile,
                const char* sbmodel_name,
                const char* bmodel_name,
                const char* data_name     );

void sigma_scan (          int  repeat,
                   const char*  infile[],
                       float_t  mwp[],
                   const char*  workspace_name[],
                   const char*  outfile,
                   const char*  sbmodel_name,
                   const char*  bmodel_name,
                   const char*  data_name          );



// Frequentist p value calculator
void sigma (    const char* infile,
                float_t     mwp,
                const char* workspace_name = "combined",
                const char* outfile = "sigma.root",
                const char* sbmodel_name = "ModelConfig",
                const char* bmodel_name = "",
                const char* data_name = "asimovData"){

  ///// PART 1: Setup /////
  //----- Get File -----//
  TFile *file = TFile::Open(infile, "READ");
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

  double_t significance = (double_t) result->Significance();
  double_t significance_error = (double_t) result->SignificanceError();
  double_t p_value = (double_t) result->NullPValue();
  double_t p_error = (double_t) result->NullPValueError();

  std::cout << "Expected significance is: " << significance <<
               "\nwith error : " << significance_error << std::endl;

  std::cout << "Expected p_value is: " << p_value <<
               "\nwith error : " << p_error << std::endl;

  delete profll;
  delete hc;

  /*
  result->Print();

  // Plot
  HypoTestPlot *plot = new HypoTestPlot(*result, 100);
  plot->SetLogYaxis(true);
  plot->Draw();
  */

  //----- Store Results -----//
  // Create ROOT file
  TFile *output = TFile::Open(outfile, "UPDATE");
  TNtuple *results = (TNtuple*) output->Get("significance");
  if (!results){
    TNtuple *tnt = new TNtuple("significance", "significance", "mwp:significance:sig_error:p_value:p_error");
    results = tnt;
  }
  results->Fill((float_t) mwp, (float_t) signif, (float_t) significance_error, (float_t) p_value, (float_t) p_error);
  results->Write("significance",TObject::kOverwrite);
  output->Close();
  file->Close();
}



// Do the same thing for multiple files
void sigma_scan (          int  repeat,  // how many files to process?
                   const char*  infile[],         // arrays
                       float_t  mwp[],            // ...
                   const char*  workspace_name[], // indices should match
                   const char*  outfile = "sigma.root",
                   const char*  sbmodel_name = "ModelConfig",
                   const char*  bmodel_name = "",
                   const char*  data_name = "asimovData"){

for (int i=0; i<repeat; i++){
  sigma(infile[i], mwp[i], workspace_name[i], outfile, sbmodel_name, bmodel_name, data_name);
}
}




// Call function for cut id 020457101143203100
const char* infile[6] = {"results/data_cut020457101143203100_binfit/wpzp_run1_mwp300_mwp_model.root",
"results/data_cut020457101143203100_binfit/wpzp_run2_mwp400_mwp_model.root",
"results/data_cut020457101143203100_binfit/wpzp_run3_mwp500_mwp_model.root",
"results/data_cut020457101143203100_binfit/wpzp_run4_mwp600_mwp_model.root",
"results/data_cut020457101143203100_binfit/wpzp_run5_mwp700_mwp_model.root",
"results/data_cut020457101143203100_binfit/wpzp_run6_mwp800_mwp_model.root"};
float_t mwp[6] = {300, 400, 500, 600, 700, 800};
const char* workspace_name[6] = {"run1_mwp300",
"run2_mwp400",
"run3_mwp500",6
"run4_mwp600",
"run5_mwp700",
"run6_mwp800"};


int main(){
  sigma_scan (6, infile, mwp, workspace_name, "sigma_020457101143203100.root");
return 0;}
