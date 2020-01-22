from fememu_pycfg import apply_config
import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMBeamTrigger_LArLiteInterface")
#
# Get configuration
#
cfg_file = sys.argv[1]
config = rt.trigger.fememu.FEMBeamTriggerConfig()
if not apply_config(config,cfg_file):
    print('\033[91m[ERROR]\033[00m exiting...')
    sys.exit(1)

#
# Prepare input TTree with input files
#
product_name = "opdigit_pmtreadout" # for opdetwaveform
#product_name = "fifo_pmt_xmit"      # for fifo
fin_list = sys.argv[2:len(sys.argv)]
tree = rt.TChain("%s_tree" % product_name)
for f in fin_list:
    tree.AddFile(f)

#
# Create FEMBeamTrigger algorithm w/ configuration
#
fememu = rt.trigger.fememu.LLInterface(config)

from larlite import larlite as fmwk
# Create ana_processor instance
my_proc = fmwk.ana_processor()

# Set input root file
for x in range(len(sys.argv)-2):
    my_proc.add_input_file(sys.argv[x+2])

# Specify IO mode
my_proc.set_io_mode(fmwk.storage_manager.kREAD)

# Specify output root file name
my_proc.set_ana_output_file("ana.root");

# Attach an analysis unit ... here we use a base class which does nothing.
# Replace with your analysis unit if you wish.
my_unit = fmwk.SWTriggerAna()
my_unit.SetAlgo(fememu)
my_proc.add_process(my_unit)

print()
print("Finished configuring ana_processor. Start event loop!")
print()

# Let's run it.
my_proc.run();


