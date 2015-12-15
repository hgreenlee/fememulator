from fememu_pycfg import apply_config
import os,sys
import ROOT as rt
rt.gSystem.Load("libFEMemulator.so")

cfg_file = sys.argv[1]
fin_list = sys.argv[2:len(sys.argv)-2]

#product_name = "opdigit_pmtreadout" # for opdetwaveform
product_name = "fifo_pmt_xmit"      # for fifo

tree = rt.TChain("%s_tree" % product_name)
for f in fin_list:
    tree.AddFile(f)

entry = 0
bytes = tree.GetEntry(entry)

config = rt.fememu.FEMBeamTriggerConfig()
apply_config(config,cfg_file)

fememu = rt.fememu.LLInterface(config)

product = None
while bytes>0:

    exec('product = tree.%s_branch' % product_name)

    out = fememu.Emulate(product)

    print '\033[93mEntry',entry,'\033[00m',
    if out.fire_time_v[0] >= 0:
        print 'Fire @,',out.fire_time_v[0],
    print

    # If this is fifo type, only use module address 4 (platform) or 5 (test-stand) ... (HACK)
    try:
        if br.size() and not br[0].module_address() == 5: continue
        
    except Exception:
        pass
    entry += 1
    tree.GetEntry( entry )

#print digit_array.size()

