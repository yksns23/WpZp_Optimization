#!/usr/bin/env python

import ROOT

### Classes ###
class wpCut:
  """ Object to store all information for the cuts
  for event selection of the following process:
  p p > wp zp, (wp > t b, (t > w b, (w > j j))), zp > n1 n1 """

  def __init__ (self):
    # Mass of top quark and W+ Boson
    global MT, MW
    MT, MW = 1.730000e+02, 7.982436e+01
    self.cutID = "nocut" # Unique identifier for each cut
    self.btag = 0 # Number of b-tagged jet(s)
    self.nonbjet = 0  # Number of non-b-tagged jet(s)
    self.jet = 0 # MINIMUM number of total jet(s)
    self.mw_min = -1e06
    self.mw_max = +1e06
    self.mt_min = -1e06
    self.mt_max = 1e06
    self.MET = 0 # MET >= # GeV
    self.mw_lower = None  # mw_min = MW - mw_lower
    self.mw_upper = None  # mw_max = MW + mw_upper
    self.mt_lower = None  # mt_min = MT - mt_lower
    self.mt_upper = None  # mt_max = MT + mt_upper

  def updateID(self):
    self.cutID = "{:01d}{:01d}{:01d}{:01d}{:01d}{:01d}{:02d}"\
    .format(int(self.btag), int(self.nonbjet),\
    int(self.mw_lower), int(self.mw_upper),\
    int(self.mt_lower), int(self.mt_upper),\
    int(self.MET))

  def setBTag(self, btag):
    self.btag = btag
    self.jet = btag + self.nonbjet

  def setNonBJet(self, nonbjet):
    self.nonbjet = nonbjet
    self.jet = nonbjet + self.btag

  def setMWBosonLower(self, mw_lower):
    self.mw_lower = mw_lower
    self.mw_min = MW - mw_lower

  def setMWBosonUpper(self, mw_upper):
    self.mw_upper = mw_upper
    self.mw_max = MW + mw_upper

  def setMTopQuarkLower(self, mt_lower):
    self.mt_lower = mt_lower
    self.mt_min = MT - mt_lower

  def setMTopQuarkUpper(self, mt_upper):
    self.mt_upper = mt_upper
    self.mt_max = MT + mt_upper

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
    print "cutID = {}".format(information["cutID"])
    print "totalJet = {}".format(information["totalJet"])
    print "BTagJet = {}".format(information["BTagJet"])
    print "NonBTagJet = {}".format(information["NonBTagJet"])
    print "MWLower = {}".format(information["MWLower"])
    print "MWUpper = {}".format(information["MWUpper"])
    print "MTLower = {}".format(information["MTLower"])
    print "MTUpper = {}".format(information["MTUpper"])
    print "METLower = {}".format(information["METLower"])


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



def single_double_jet_mass_fit (
    target_mass, list_of_jets,
    lower_bound=float('-inf'), upper_bound=float('inf')):
  """ Find either single or double jet that
      best fits mass of W+. """
  min_diff = upper_bound - lower_bound
  found = False
  strategy = None

  n = len(list_of_jets)
  if n == 0:
    raise ValueError("Argument list_of_jets must be a list of nonzero length.")

  # Single jet strategy
  for i in range(n):
    jet = list_of_jets[i]
    if not (lower_bound <= jet.M() <= upper_bound):
      continue
    diff = abs(jet.M() - target_mass)
    if diff < min_diff:
      min_diff, index, found, strategy = diff, i, True, 'single'

  # Double jet strategy
  for i in range(n):
    for j in range(i+1,n):
      jet1 = list_of_jets[i]
      jet2 = list_of_jets[j]
      total = jet1 + jet2
      if not (lower_bound <= total.M() <= upper_bound):
        continue
      diff = abs(total.M() - target_mass)
      if diff < min_diff:
        min_diff, indices, found, strategy = diff, [j,i], True, 'double'

  # If neither strategy finds a W+ candidate
  if not found:
    return None, list_of_jets

  # Compare strategies and return
  if strategy == 'single':
    candidate = list_of_jets.pop(index)
    return candidate, list_of_jets
  elif strategy == 'double':
    jet1 = list_of_jets.pop(indices[0])
    jet2 = list_of_jets.pop(indices[1])
    return jet1+jet2, list_of_jets
  # End of function def single or double mass fit

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
  if cut.nonbjet == 1:
    mwHist = ROOT.TH1F("mw", "Distribution of reconstructed M_{j}", 20, 0.0, 200)
    mwHist.SetXTitle("M_{j} [GeV]")
    mwHist.SetYTitle("events")
  elif cut.nonbjet == 2:
    mwHist = ROOT.TH1F("mw", "Distribution of reconstructed M_{jj}", 20, 0.0, 200)
    mwHist.SetXTitle("M_{jj} [GeV]")
    mwHist.SetYTitle("events")
  elif cut.nonbjet == 12:
    mwHist = ROOT.TH1F("mw", "Distribution of reconstructed M_{j} or M_{jj}", 20, 0.0, 200)
    mwHist.SetXTitle("M_{j} or M_{jj} [GeV]")
    mwHist.SetYTitle("events")
  # Top Quark Reconstructed Mass
  mtHist = ROOT.TH1F("mt", "Distribution of reconstructed M_{bW}", 30, 0.0, 300)
  mtHist.SetXTitle("M_{bW} [GeV]")
  mtHist.SetYTitle("events")
  # Wp Reconstructed Mass
  mwpHist = ROOT.TH1F("mwp_"+event_type, "Distribution of reconstructed M_{tb}", 29, 175, 900)
  mwpHist.SetXTitle("M_{tb} [GeV]")
  mwpHist.SetYTitle("events")
  # MET
  METHist = ROOT.TH1F("Missing Transverse Energy", "MET", 40, 0.0, 1000)
  METHist.SetXTitle("MET [GeV]")
  METHist.SetYTitle("events")
  
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
    #  W_boson, non_btagged_jets = single_jet_mass_fit(
    #  MW, non_btagged_jets, cut.mw_min, cut.mw_max)
      # Find EITHER single or double jet
      W_boson, non_btagged_jets = single_double_jet_mass_fit(
        MW, non_btagged_jets, cut.mw_min, cut.mw_max)

    elif cut.nonbjet == 2:
      W_boson, non_btagged_jets = double_jet_mass_fit(
        MW, non_btagged_jets, cut.mw_min, cut.mw_max)
    if W_boson == None:
      continue
    mwHist.Fill(W_boson.M())  # Fill W+ histogram

    # Reconstruct top quark
    top_quark, btagged_jets = single_jet_mass_fit(
      MT, btagged_jets, cut.mt_min, cut.mt_max, W_boson)
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

######### Function: canvas_draw #########
### Put histograms on one TCanvas and return TCanvas
def canvas_draw (canvas_name, hist1, hist2, hist3, hist4):
  " Put four histograms on one TCanvas and return TCanvas. "
  c1 = ROOT.TCanvas(canvas_name, canvas_name, 50, 20, 1100, 610)
  pad1 = ROOT.TPad(hist1.GetName(), hist1.GetTitle(), 0, 0, 0.5, 0.5)
  pad2 = ROOT.TPad(hist2.GetName(), hist2.GetTitle(), 0, 0.5, 0.5, 1)
  pad3 = ROOT.TPad(hist3.GetName(), hist3.GetTitle(), 0.5, 0, 1, 0.5)
  pad4 = ROOT.TPad(hist4.GetName(), hist4.GetTitle(), 0.5, 0.5, 1, 1)
  #textpad = ROOT.TPad("Info", " (text) ", 0.425, 0.8, 0.60, 0.98)
  #textpad.SetFillStyle(4000)
  pad1.Draw()
  pad2.Draw()
  pad3.Draw()
  pad4.Draw()
  #textpad.Draw()

  pad1.cd()
  hist1.Draw()

  pad2.cd()
  hist2.Draw()

  pad3.cd()
  hist3.Draw()

  pad4.cd()
  hist4.Draw()
  """
  textpad.cd()
  text = ROOT.TPaveText(0.05, 0.1, 0.95, 0.9, "NB")
  text.AddText(" (text) ")
  text.SetFillColor(0)
  text.Draw()
  """
  c1.Update()
  return c1
