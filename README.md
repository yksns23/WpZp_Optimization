Last edit: Jan 8, 2019

The MAIN PURPOSE of this repository is to DISPLAY MY CURRENT PROGRESS in my research project. Unfortunately, some of the functionalities I use with MG5+Pythia+Delphes cannot be shown here for practical reasons (i.e. requires GBs of memory).

NOTE: This repository is a WORKING PROGRESS. To see an example of a well-functioning code, open folder "event_selection" and examine the Python codes accordingly. (I authored them completely on my own.)
The codes under "statistical_analysis" are to be improved in many ways, but those who are familiar with RooFit/RooStats will see what I am trying to get at. The current issue is that my PDFs are not defined well in "stats_03.C" (named thus for it is a sort of test-code), and I am getting this error: "Error in <ROOT::Math::Fitter::SetFCN>: FCN function has zero parameters." That is why you see me also working on the .xml config for hist2workspace, because it seems to handle the complexity better than my C macro.

Feel free to add helpful comments/suggestions.
