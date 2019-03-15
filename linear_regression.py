#!/usr/bin/env python

# Linear regression for s_xsec vs. sigma plot
# Get values of s_xsec for which sigma = 3 or 5

USAGE="""
Usage: python linear_regression filename mwp1 mwp2 mwp3 mwp4 ...
"""

from ROOT import TFile, TTreeReader, TTreeReaderValue
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from sys import argv


def find_xsec_for_sigma (filename, mwp):
    """ Plot xsec for sigma = 3 and 5, for specified mwp. """
    # Open file
    f = TFile.Open(filename, "READ")

    # Set array
    xsec = []
    sig = []

    # Fill array
    for row in f.significance:
        if row.mwp == float(mwp):
            if -100 < row.significance < 100:   # Shouldn't be +/-inf
                xsec.append(row.signal_xsec)
                sig.append(row.significance)

    # Linear regression
    p = np.polyfit(sig, xsec, 1)

    # Result
    m, b = p[0], p[1]
    result_xsec_three_sigma = 3.0 * m + b
    result_xsec_five_sigma = 5.0 * m + b

    # Visualize
    plt.rc('text', usetex=True)
    plt.rc('font', family='serif')

    figure = plt.figure()
    plt.scatter(xsec, sig, c='R', marker='.')  # Data
    y = np.linspace(0, 5.5, 2)   # Regression line
    plt.plot(m*y+b, y, '-b')

    plt.xlabel(r'Cross section of signal [$10^3$ pb]')
    plt.ylabel(r'Significance [$\sigma$]')
    plt.title('mwp: {}'.format(mwp))

    return result_xsec_three_sigma, result_xsec_five_sigma, figure


# In the main function, find xsec for multiple values of mwp
def main():
    if len(argv) <= 2:
        print USAGE
        return

    mwp_list = []
    three_sigma = []
    five_sigma = []
    figures = []   # To put plots into one pdf file
    for mwp in argv[2:]:
        mwp_list.append(mwp)
        sig3, sig5, fig = find_xsec_for_sigma(argv[1], mwp)
        three_sigma.append(sig3)
        five_sigma.append(sig5)
        figures.append(fig)
    print three_sigma
    print five_sigma

    # Pdf save
    with PdfPages('xsec_sigma_plots.pdf') as pdf:
        for fig in figures:
            pdf.savefig(fig)

    # Plot three_sigma & five_sigma per mwp
    final_figure = plt.figure()
    plt.title(r'mwp vs. cross section at 3$\sigma$ and 5$\sigma$')
    plt.xlabel('mwp [GeV]')
    plt.ylabel('cross section [$10^3$ pb]')
    plt.ylim(0.0, 5.0)
    plt.plot(mwp_list, three_sigma, label='3$\sigma$')
    plt.plot(mwp_list, five_sigma, label='5$\sigma$')
    plt.legend()
    
    ax = final_figure.add_subplot(111)
    for i, j, k in zip(mwp_list, three_sigma, five_sigma):
        ax.annotate('{:.4f}'.format(j), xy=(i,j), xytext=(-12,7), textcoords='offset points')
        ax.annotate('{:.4f}'.format(k), xy=(i,k), xytext=(-12,7), textcoords='offset points')

    final_figure.show()

    # Close?
    user_input = 0
    while user_input != 'y':
        user_input = raw_input ('Close program? [y] ')

if __name__ == "__main__":
    main()
