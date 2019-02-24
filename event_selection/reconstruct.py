#!/usr/bin/env python

import ROOT

### Classes ###
class wpCut:
  """ Object to store all information for the cuts
  for event selection of the following process:
  p p > wp zp, (wp > t b, (t > w b, (w > j j))), zp > n1 n1 """
  
  # Unique identifier for each cut
  # Invoke updateID method
  cutID = ""

  def __init__ (self):
    self.btag = 0 # Number of b-tagged jet(s)
    self.nonbjet = 0  # Number of non-b-tagged jet(s)
    self.jet = 0 # Number of total jet(s)
    self.mw_lower = -1e06
    self.mw_upper = 1e06
    self.mt_lower = -1e06
    self.mt_upper = 1e06
    self.MET = 0 # MET >= # GeV

  def updateID(self):
    self.cutID = "{:02d}{:02d}{:02d}{:02d}{:02d}{:02d}{:02d}"\
    .format(int(self.btag), int(self.jet),\
    int(self.mw_lower), int(self.mw_upper),\
    int(self.mt_lower), int(self.mt_upper),\
    int(self.MET))

  def setTotalJet(self, jet):
    self.jet = jet

  def setBTag(self, btag):
    self.btag = btag
    self.jet = btag + self.nonbjet

  def setNonBJet(self, nonbjet):
    self.nonbjet = nonbjet
    self.jet = nonbjet + self.btag

  def setMWBosonLower(self, mw_lower):
    self.mw_lower = mw_lower

  def setMWBosonUpper(self, mw_upper):
    self.mw_upper = mw_upper

  def setMTopQuarkLower(self, mt_lower):
    self.mt_lower = mt_lower

  def setMTopQuarkUpper(self, mt_upper):
    self.mt_upper = mt_upper

  def setMETLower(self, MET):
    self.MET = MET

  def getCutInfo(self):
    CutInfo = dict()
    CutInfo["cutID"] = int(self.cutID)
    CutInfo["totalJet"] = self.jet
    CutInfo["BTagJet"] = self.btag
    CutInfo["NonBTagJet"] = self.nonbjet
    CutInfo["MWLower"] = self.mw_lower
    CutInfo["MWUpper"] = self.mw_upper
    CutInfo["MTLower"] = self.mt_lower
    CutInfo["MTUpper"] = self.mt_upper
    CutInfo["METLower"] = self.MET
    return CutInfo

  def displayCutInfo(self):
    information = self.getCutInfo()
    for key, value in information.items():
      print "{} = {}".format(key, value)

### Functions ###
def mass_info (tag, *pid):
  " Displays current event mass information. "
  import os
  current_path = os.getcwd()
  run = current_path[-6:]
  file_name = run + '_' + tag + '_banner.txt'
  s = []
  with open(file_name, 'r') as f:
    count = 0
    for line in f:
      if any(str(n) in line for n in pid) and count < len(pid):
        l = line.split()
        s.append("{}={}".format(l[3], l[1]))
        count += 1
  info = "_".join(s)
  return info, s




def TLorentzVector_jet (jet):
  " Input: Delphes Jet object => Output: TLorentzVector object "
  v = ROOT.TLorentzVector()
  v.SetPtEtaPhiM(jet.PT, jet.Eta, jet.Phi, jet.Mass)
  return v




def single_jet_mass_fit (
    target_mass, list_of_jets,
    lower_bound=float('-inf'), upper_bound=float('inf'),
    *other):
  """ Find single jet from a list such that
      when combined with optional other jet(s),
      it yields the mass closest to the target mass.
      All input jet(s) must belong to ROOT.TLorentzVector class.
      *args--bounds: lower bound, upper bound in terms of difference.
      Output: reconstructed particle, remaining list of jets """

  n = len(list_of_jets)
  if n == 0:
    raise ValueError("Argument list_of_jets must be a list of nonzero length.")

  other_products = ROOT.TLorentzVector() # Sum of all other particles
  try:
    for j in other:
      other_products += j
  except:
    pass

  min_diff = upper_bound - lower_bound
  found = False
  for i in range(n):
    jet = list_of_jets[i]
    total = jet + other_products

    if not (lower_bound <= total.M() <= upper_bound):
      continue

    diff = abs(total.M() - target_mass)
    if diff < min_diff:
      min_diff, index, found = diff, i, True

  if not found:
    return None, list_of_jets

  candidate = list_of_jets.pop(index)
  if len(other) == 0:
    return candidate, list_of_jets
  else:
    return candidate+other_products, list_of_jets
  # End of function def single_jet_mass_fit



def double_jet_mass_fit (
    target_mass, list_of_jets,
    lower_bound=float('-inf'), upper_bound=float('inf'),
    *other):
  """ Find double jet instead of single jet for
      single_jet_mass_fit function. """

  n = len(list_of_jets)
  if n < 2:
    raise ValueError("Argument list_of_jets must have length > 1.")
  other_products = ROOT.TLorentzVector() # Sum of all other particles
  try:
    for j in other:
      other_products += j
  except:
    pass

  min_diff = upper_bound - lower_bound
  found = False
  for i in range(n):
    for j in range(i+1,n):
      jet1 = list_of_jets[i]
      jet2 = list_of_jets[j]
      total = jet1 + jet2 + other_products

      if not (lower_bound <= total.M() <= upper_bound):
        continue

      diff = abs(total.M() - target_mass)
      if diff < min_diff:
        min_diff, indices, found = diff, [j,i], True

  if not found:
    return None, list_of_jets

  jet1 = list_of_jets.pop(indices[0])
  jet2 = list_of_jets.pop(indices[1])
  if len(other) == 0:
    return jet1+jet2, list_of_jets
  else:
    return jet1+jet2+other_products, list_of_jets
  # End of function def double mass fit

######### Function: event_information #########
### Get all relevant event information and return dict()
def event_information (banner, cut, *pid_list, **param):
  """ Event information for a single event.
  Includes: Nevents, mass of particles,
            cut information
  banner: string (e.g. "run_1_tag_1_banner.txt)
  cut: reconstruct.wpCut object
  *pid_list: *argv int -- PID of particles of interest
  **param: **kwarg parameter=value (e.g. POI="mass")
  """
  info = dict() # Return variable
  ### Info from _banner.txt
  # nevents, ebeam1, ebeam2, mass of particles
  with open(banner, 'r') as f:
    line = f.readline()
    while not line.startswith("# Running parameters"):
      line = f.readline()
    count = 0
    for line in f:
      if "= nevents" in line:
        info["nevents"] = int(line.split()[0])
      if count > len(pid_list):
        break
      if any(str(n) in line for n in pid_list):
        count += 1
        l = line.split()
        name, mass_value = l[3], float(l[1])
        info[name] = mass_value
          # e.g. mass_info["mt"] = 1.730000e+02
          # ---> mass_info = {"mt": 1.730000e+02}

  ### Info from reconstruct.wpCut object
  info.update(cut.getCutInfo())
  
  ### Optional info from **kwargs param
  info.update(param)

  return info

######### Function: event_selection #########
### Select events according to requirements and cuts
def event_selection (
inputFile, cut, event_type="signal",
MT=1.730000e+02, MW=7.982436e+01):
  """ Event selection for a single input file.
  Input: delphes_events.root => Output: TH1F
  inputfile: string (e.g. "tag_1_delphes_events.root")
  cut: reconstruct.wpCut object
  event_type: string -- "signal", "background", etc. """

  chain = ROOT.TChain("Delphes")
  chain.Add(inputFile)

  # Create object of class ExRootTreeReader
  treeReader = ROOT.ExRootTreeReader(chain)
  numberOfEntries = treeReader.GetEntries()

  # Get pointers to branches used in this analysis
  branchJet = treeReader.UseBranch("Jet")
  branchElectron = treeReader.UseBranch("Electron")
  branchMuon = treeReader.UseBranch("Muon")
  branchMET = treeReader.UseBranch("MissingET")

  # Book histograms
  # It is important to have a uniform naming scheme
  # for the histogram objects, to access them in bulk
  # later in statistical analysis
  # W+ Reconstructed Mass
  mwHist = ROOT.TH1F("mw", "M_{W+}", 50, 0.0, 500)
  # Top Quark Reconstructed Mass
  mtHist = ROOT.TH1F("mt", "M_{bW+}", 50, 0.0, 500)
  # Wp Reconstructed Mass
  mwpHist = ROOT.TH1F("mwp_"+event_type, "M_{tb}", 29, 175, 900)
  # MET
  METHist = ROOT.TH1F("Missing Transverse Energy", "MET", 100, 0.0, 1000)
  
  # Loop over each event
  for entry in range(numberOfEntries):
    # Load selected branches with data from specified event
    treeReader.ReadEntry(entry)
  
    # Event requirement:
    # no lepton (hadronic channel only)
    # at least 1 MET (n1) with E > 25, at least 4 jets
    if branchElectron.GetEntries() != 0:
      if any(
             abs(branchElectron.At(i).Eta) < 2.5
             or branchElectron.At(i).PT > 10
             for i in range(branchElectron.GetEntries())):
        continue
    if branchMuon.GetEntries() != 0:
      if any(
             abs(branchMuon.At(i).Eta) < 2.5
             or branchMuon.At(i).PT > 10
             for i in range(branchMuon.GetEntries())):
        continue
    if branchMET.GetEntries() == 0 or branchMET.At(0).MET <= cut.MET:
      continue
    if branchJet.GetEntries() < cut.jet:
      continue

    # Select response jets
    # The idea is to select detected jets,
    # and to store them as TLorentzVector objects
    non_btagged_jets = [] # List of TLorentzVector object
    btagged_jets = []     # List of TLorentzVector object
    for i in range(branchJet.GetEntries()): # Iterate over jets
      jet = branchJet.At(i)
      # Jets must meet minimum requirement to be detected
      if jet.PT >= 20.0 and abs(jet.Eta) < 2.5:
        # Select between b and non-b jets
        if jet.BTag == 0:
          non_btagged_jets.append(TLorentzVector_jet(jet))
        elif jet.BTag == 1:
          btagged_jets.append(TLorentzVector_jet(jet))
    
    # After jet selection, if not (j j b b) then skip event
    if len(btagged_jets) < cut.btag or len(non_btagged_jets) < cut.nonbjet:
      continue

    # Now the main part
    # From [j1, j2, ...] and [b1, b2, ...],
    # select j's and b's such that they
    # best fit the signal process

    ### Cut 1: # of non b-tagged jets
    # Choose W+ boson candidate (W+ > j j)
    # Strategy: find 1 or 2 jet(s) with mass closest to mw+
    # reconstruct functions _jet_mass_fit returns a list:
    #                       candidate, jet list w/o candidate
    if cut.nonbjet == 1:
      W_boson, non_btagged_jets = single_jet_mass_fit(
        MW, non_btagged_jets, cut.mw_lower, cut.mw_upper)
    elif cut.nonbjet == 2:
      W_boson, non_btagged_jets = double_jet_mass_fit(
        MW, non_btagged_jets, cut.mw_lower, cut.mw_upper)
    if W_boson == None:
      continue
    mwHist.Fill(W_boson.M())  # Fill W+ histogram

    # Reconstruct top quark
    top_quark, btagged_jets = single_jet_mass_fit(
      MT, btagged_jets, cut.mt_lower, cut.mt_upper, W_boson)
    if top_quark == None:
      continue
    mtHist.Fill(top_quark.M())

    # Reconstruct wp
    bottom_quark = btagged_jets[0]
    Wp_boson = bottom_quark + top_quark
    mwpHist.Fill(Wp_boson.M())

    # Reconstruct MET
    METHist.Fill(branchMET.At(0).MET)

  return mwHist, mtHist, mwpHist, METHist #histograms
  # End of def event_selection
