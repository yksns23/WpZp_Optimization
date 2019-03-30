Last edit: Mar 29, 2019

Welcome!

I know that it is a bit of a mess! I will clean it up soon.

Author:

Kyungseop Yoon

University of California, Irvine

kyungsey@uci.edu

Repository of macros for Physics research project.
Goal: Optimize event selection for ATLAS detector for a hypothesized dark matter producing process (see => process_diagram.ps)

<Usage>
   
1) Open bash shell

2) ./automize

3) Enter cut information & run

4) Start ROOT session

5) .L visualize.C

6) Call function multigraph_signif("filename1.root", ... , "filename5.root");

   Files contained in sigma_results folder
   
   You type in the legend in ROOT-style LATEX format
   
   See sample_result.pdf
   

<What is a Cut ID?>

The unique identifier of each selection algorithm with different parameters (e.g. # of btagged jets, mass range of reconstructed W', etc.)

The cut ID simply concatenates the string of these values. See event_seleciton/reconstruct.py module.

--------------------------------------------
The files you see here are written in Python and C++. They are mostly macros intended to be executed in the command line, either directly or in a ROOT session.

The MAIN PURPOSE of this repository is to DISPLAY MY CURRENT PROGRESS in my research project. Unfortunately, some of the functionalities I use with MG5+Pythia+Delphes cannot be shown here for practical reasons (i.e. requires GBs of memory).

Feel free to add helpful comments/suggestions.
