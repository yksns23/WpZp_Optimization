#!/bin/bash

function create_xml_channel() {
# args: (1)run_number (2)mwp (3)input_file (4)cut_id (5)sxsec

cat << EndXML > "config/wpzp_mwp$2.xml"

<!-- Single channel configuration for run_$1 -->

<!DOCTYPE Channel  SYSTEM 'HistFactorySchema.dtd'>

  <Channel Name="run$1_mwp$2" InputFile="datafiles/$3" >

    <!-- Set the StatError type to Poisson. -->
    <StatErrorConfig RelErrorThreshold="0.05" ConstraintType="Poisson" />

    <!-- Signal -->
    <Sample Name="signal" HistoPath="/$4/signal/run_$1/" HistoName="snormed" NormalizeByTheory="True">
      <OverallSys Name="syst0" Low="0.998" High="1.002" />
      <NormFactor Name="sXsec" Val="$5" Low="0" High="1"  />
    </Sample>

    <!-- Background -->
    <Sample Name="bcombined" HistoPath="" HistoName="bcombined" NormalizeByTheory="True" >
      <OverallSys Name="syst1" Low='0.99' High='1.01'/>
    </Sample>

  </Channel>
EndXML
}

declare -a masses=(300 400 500 600 700 800 900 1000 1100 1200 1300 1400 1500 1600 1700 1800 1900)
declare -a cross_sections=(4.91e-02 5.11e-02 4.519e-02 3.827e-02 3.2e-02 2.662e-02 2.212e-02 1.844e-02 1.535e-02 1.279e-02 1.072e-02 8.986e-03 7.514e-03 6.294e-03 5.292e-03 4.441e-03 3.733e-03)
# Corresponding cross sections scaled by 1000 (pb -> fb)

for (( i=0; i<6; i++ )); do
  ((run = i + 1))
  create_xml_channel $run ${masses[i]} data_020457101143203100_binfit.root 020457101143203100 ${cross_sections[i]}
done
