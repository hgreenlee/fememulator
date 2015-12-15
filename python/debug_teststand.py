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
config.fSetTriggerWindow = True
config.fDiscr0delay = 3
config.fDiscr3delay = 3
config.fDiscr0threshold = 100
config.fDiscr3threshold = 200
config.fDiscr0precount = 3
config.fDiscr0deadtime = 6
config.fDiscr3deadtime = 6
config.fDiscr0width = 6
config.fDiscr3width = 6
config.fMinReadoutTicks = 500
config.fTriggerWinStartTick = 58
config.fWindowSize = 16
# Trigger conditions
config.fTriggerThresMult = 1
config.fTriggerThresPHMAX = 2000

fememu = rt.fememu.LLInterface(config)
#fememu._verbosity=rt.fememu.kDEBUG
#fememu._verbosity=rt.fememu.kINFO
fememu._verbosity=rt.fememu.kNORMAL
product = None
while bytes>0:

    exec('product = tree.%s_branch' % product_name)

    out = fememu.Emulate(product)

    print '\033[93mEntry',entry,'\033[00m',
    if out.fire_time_v[0] >= 0:
        print 'Fire @,',fire_time_v[0],
    print

    # If this is fifo type, only use module address 4 (platform) or 5 (test-stand) ... (HACK)
    try:
        if br.size() and not br[0].module_address() == 5: continue
        
    except Exception:
        pass
    entry += 1
    tree.GetEntry( entry )

#print digit_array.size()

