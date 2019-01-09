Event selection for new physics with dark matter search.
Process information (in MG5 syntax):
p p > wp zp, (zp > n1 n1~), (wp > t b~, (t > w+ b, (w+ > j j) ) )

To view the Feynman diagram of the process, open PostScript file: "pp_wpzp.ps"

Search strategy:
1. Prerequirements:
   2 b-tagged jets, 1 or 2 non-b-tagged jet(s).
   MET > (variable) in association with dark matter candidate.
   No leptonic decay; only hadronic decay

2. Reconstructing Wp candidate mass:
   Linear cuts are applied.
   Find 1 or 2 non-b-tagged jet(s) with rest mass closest to that of W+.
   Find b-tagged jet s.t. when combined with W+, it has mass closest to that of top quark.
   Find the remaining b-tagged jet and combine with the top quark.
   Calculate the mass of this top-bottom combo ====> Wp mass.


----- How to use event_selection -----

Prerequisite: ROOT and Delphes libraries must be in PYTHONPATH

Input: Two files (generated from Delphes) -- tag_#_delphes_events.root and run_#_tag_#_banner.txt
Output: One file (.root) containing histograms and cut&event information
        (will open and edit)

In terminal, type:
python event_selection [channel] [run-start] [run-end] [tag] [output]

[channel]: enter whether signal, background, specify channel, etc.
[run-start] [run-end]: enter run # (e.g. 1 20  if run 01-20)
[tag]: enter tag # (e.g. 1 if tag_1)
[output]: enter output file name (e.g. "myresults.root")

Open this file to manually change cuts (lines 48-56)

----- File descriptions -----

event_selection: main python code for executing search command
reconstruction: module containing all the complicated functions and objects. One important aspect of this is the definition of a wpCut object. This object stores all the relevant information of the parameters for the selection described above.
