#! /bin/bash

# Automize everything!
# 1. Set cut info: pass arguments into command line
# 2. Event selection -> access data in MG5 -> output: .root file
# 3. .root file copied & moved to datafiles
# 4. .root file in datafiles opened and histograms normalized
# 5. Create .xml config files
# 6. hist2workspace .xml files
# 7. Resulting workspace file (.root) opened and statistical analysis performed
# 8. Visualize result

# Save this directory, since it'll return to it later
thisdir=$(pwd)

####### Step 1 #######
# User input
echo "Type run_start run_end for your signal"
read v1 v2
echo "\nType cut information: BTag nonBJet MWLow MWHigh MTLow MTHigh MET"
read v3 v4 v5 v6 v7 v8 v9

cutid=$v3$v4$v5$v6$v7$v8$v9

####### Step 2 #######
# Run event selection
data_directory="~/MG5_aMC_v2_6_3_2/RESEARCH"
cp -n event_selection/event_selection -t $data_directory
cd $data_directory
python event_selection/event_selection signal $v1 $v2 1 $v3 $v4 $v5 $v6 $v7 $v8 $v9
python event_selection/event_selection background_tbZ 1 1 1
python event_selection background_ttxZ_4j2b2vl 1 1 1
python event_selection background_ttxZ_lvl2j2b2vl 1 1 1

###### Step 3 ########
# Save the .root file both to MG5/selection_results and datafiles
datafile="data_$cutid.root"
find . -maxdepth 1 -type f -name $datafile -exec cp -t selection_results {} \; -exec mv -t $this_dir/create_workspace/datafiles {} \;

cd $this_dir

###### Step 4 ########
# Open the .root file in datafiles and normalize histograms
./normhist create_workspace/datafiles/$datafile $cutid $v1 $v2

###### Step 5 ########
# Create channel
./create_channel
for (( run=((v1)); run<=((v2)); run++ )); do
  create_xml_channel $run $datafile $cutid
done
# Create top-level config
./create_top_config
create_top_config $cutid



