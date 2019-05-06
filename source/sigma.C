// A lightweight calculator for significance values (final)

#include <iostream>
#include <cstdlib>

#include <TFile.h>
#include <TNtuple.h>
#include <RooDataHist.h>
#include <RooRealVar.h>
#include <RooDataSet.h>

#include <RooWorkspace.h>
#include <RooStats/ModelConfig.h>
#include <RooStats/HypoTestCalculatorGeneric.h>
#include <RooStats/FrequentistCalculator.h>
#include <RooStats/ToyMCSampler.h>
#include <RooStats/HypoTestResult.h>
#include <RooStats/ProfileLikelihoodTestStat.h>

using namespace RooFit;
using namespace RooStats;

void sigma (    const char* infile,
                const char* mwp,
                const char* xsec,
                const char* workspace_name,
                const char* outfile,
                const char* sbmodel_name,
                const char* bmodel_name,
                const char* data_name     );



// Frequentist p value calculator
void sigma (    const char* infile,
                const char* mwp,
                const char* xsec,
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

  //----- Store Results -----//
  // Create ROOT file and store resulting data into tntuple 
  TFile *output = TFile::Open(outfile, "UPDATE");
  TNtuple *results = (TNtuple*) output->Get("significance");
  if (!results){
    TNtuple *tnt = new TNtuple("significance", "significance", "mwp:signal_xsec:significance:sig_error:p_value:p_error");
    results = tnt;
  }
  results->Fill((float_t) atof(mwp), (float_t) atof(xsec), (float_t) significance, (float_t) significance_error, (float_t) p_value, (float_t) p_error);
  results->Write("significance",TObject::kOverwrite);
  output->Close();
  file->Close();
}





int main(int argc, char* argv[]){
  if (argc < 4){
    std::cout << "Usage: " << argv[0] << " [infile] [mwp] [xsec] (optional: [workspace] [outfile] [sbmodel] [bmodel] [data])" << std::endl;
  return 1;
  }
  // Only first three arguments are required; hence the following
  else if (argc == 4) sigma(argv[1], argv[2], argv[3]);
  else if (argc == 5) sigma(argv[1], argv[2], argv[3], argv[4]);
  else if (argc == 6) sigma(argv[1], argv[2], argv[3], argv[4], argv[5]);
  else if (argc == 7) sigma(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
  else if (argc == 8) sigma(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
  else sigma(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]);
  return 0;
}
