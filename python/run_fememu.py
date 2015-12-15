import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMemulator.so")

fin = sys.argv[1]

#product_name = "opdigit_pmtreadout" # for opdetwaveform
product_name = "fifo_pmt_xmit"      # for fifo

tree = rt.TChain("%s_tree" % product_name)
tree.AddFile(fin)

entry = 0
bytes = tree.GetEntry(entry)

config = rt.fememu.FEMBeamTriggerConfig()
config.fSetTriggerWindow = False
config.fDiscr0delay = 3
config.fDiscr3delay = 3
config.fDiscr0threshold = 5
config.fDiscr3threshold = 10
config.fDiscr0precount = 3
config.fDiscr0deadtime = 6
config.fDiscr3deadtime = 6
config.fDiscr0width = 6
config.fDiscr3width = 6
config.fMinReadoutTicks = 500
config.fTriggerWinStartTick = 0
config.fWindowSize = 103
# Trigger conditions
config.fTriggerThresMult = 1
config.fTriggerThresPHMAX = 1000

fememu = rt.fememu.LLInterface(config)
fememu._verbosity=rt.fememu.kDEBUG
product = None
while bytes>0:

    exec('product = tree.%s_branch' % product_name)

    out = fememu.Emulate(product)
    entry += 1

    # If this is fifo type, only use module address 4 (platform) or 5 (test-stand) ... (HACK)
    try:
        if br.size() and not br[0].module_address() == 5: continue
        
    except Exception:
        pass

    tree.GetEntry( entry )
    if entry==10:
        break

#print digit_array.size()

