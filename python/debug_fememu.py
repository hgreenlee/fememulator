from fememu_pycfg import apply_config
import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMBeamTrigger_LArLiteApp.so")

#
# Get configuration
#
cfg_file = sys.argv[1]
config = rt.trigger.fememu.FEMBeamTriggerConfig()
if not apply_config(config,cfg_file):
    print '\033[91m[ERROR]\033[00m exiting...'
    sys.exit(1)

#
# Prepare input TTree with input files
#
#product_name = "opdigit_pmtreadout" # for opdetwaveform
product_name = "fifo_pmt_xmit"      # for fifo
fin_list = sys.argv[2:len(sys.argv)]
tree = rt.TChain("%s_tree" % product_name)
for f in fin_list:
    tree.AddFile(f)

#
# Create FEMBeamTrigger algorithm w/ configuration
#
fememu = rt.trigger.fememu.LLInterface(config)

#
# Loop over TTree entries
#
product = None
for entry in xrange(tree.GetEntries()):

    tree.GetEntry(entry)
    
    exec('product = tree.%s_branch' % product_name)

    if config.fVerbosity <= rt.trigger.fememu.kINFO:
        print '\033[93mEntry',entry,'\033[00m'

    out = fememu.Emulate(product)

    # If this is fifo type, only use module address 4 (platform) or 5 (test-stand) ... (HACK)
    try:
        if br.size() and not br[0].module_address() == 5: continue
        
    except Exception:
        pass

    #if entry == 1: break
sys.exit(0)
