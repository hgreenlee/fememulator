import matplotlib
import matplotlib.pyplot as plt
import matplotlib.colors as mpc
import matplotlib.dates as dts
import numpy as np
import pandas as pd
import itertools
import os
import ROOT
import datetime
from root_numpy import root2array, root2rec, tree2rec, array2root
from scipy.optimize import curve_fit
from scipy.misc import factorial
plt.rcParams.update({'font.size': 16})
#from larlite import larlite
#k=larlite.trigger()
from ROOT import larlite
larlite.trigger()
from ROOT import trigger
from fememu_pycfg import apply_config

# emulator configuration
cfg_file = 'debug_teststand.cfg'
config = trigger.fememu.FEMBeamTriggerConfig()
apply_config(config,cfg_file)
# emulator construction
emu = trigger.fememu.LLInterface(config)
f_trig = ROOT.TFile('debug_trig.root')
f_xmit = ROOT.TFile('debug_fifo.root')
t_trig = f_trig.Get('trigger_daq_tree')
t_xmit = f_xmit.Get('fifo_pmt_xmit_tree')
print 'trigger entries :',t_trig.GetEntries()
print 'xmit entries    :',t_xmit.GetEntries()
print emu._cfg.fVerbosity
print emu._cfg.fSetTriggerWindow
print emu._cfg.fDiscr0delay
print emu._cfg.fDiscr3delay
print emu._cfg.fDiscr0threshold
print emu._cfg.fDiscr3threshold
print emu._cfg.fDiscr0precount
print emu._cfg.fDiscr0deadtime
print emu._cfg.fDiscr3deadtime
print emu._cfg.fDiscr0width
print emu._cfg.fDiscr3width
print emu._cfg.fTriggerWinStartTick
print emu._cfg.fMinReadoutTicks
print emu._cfg.fFrontBuffer
print emu._cfg.fWindowSize
print emu._cfg.fTriggerThresMult
print emu._cfg.fTriggerThresPHMAX

for x in xrange(t_xmit.GetEntries()):
    t_xmit.GetEntry(x)
    t_trig.GetEntry(x)

    trig = t_trig.trigger_daq_branch
    ev_fifo = t_xmit.fifo_pmt_xmit_branch

    ch0,beamch=(None,None)
    for n in xrange(ev_fifo.size()):
        fifo = ev_fifo[n]
        if not fifo.module_address() == 5: continue
        channel = fifo.channel_number()
        if channel == 0:
            ch0 = fifo
        elif channel in [46,47]:
            beamch = fifo

    beamgate_sample = ch0.readout_sample_number_RAW()
    trig_tick =  (trig.TriggerTime()%1600)*64 - beamgate_sample
    beam_tick = beamch.readout_sample_number_RAW() - beamgate_sample

    emu_out = emu.Emulate(ev_fifo)
    print 'Trigger @ %d, Emulator fire @' % trig_tick,
    emu_fired = False
    for t in emu_out.fire_time_v:
        print t,
        if t >= 0: emu_fired = True
    print

    '''
    ev_fifo = t_xmit.fifo_pmt_xmit_branch
    frame_num = sample_num = 0
    
    ch0,ch1,beamch=(None,None,None)

    for n in xrange(ev_fifo.size()):
        fifo = ev_fifo[n]
        if not fifo.module_address() == 5: continue
        channel = fifo.channel_number()
        
        if channel == 0: 
            ch0 = fifo
        elif channel == 1:
            ch1 = fifo
        elif channel in [46,47]:
            beamch = fifo

    ch0_adc = np.array(ch0)
    ch1_adc = np.array(ch1)
    beamch_adc = np.array(beamch)

    # Get timings
    beamgate_sample = ch0.readout_sample_number_RAW()
    
    trig_num = ev_fifo.event_number()
    
    print trig_num, beamgate_sample, beamch.readout_sample_number_RAW()

    #trig_time,trig_frame,trig_sample = trig_times[trig_num]
    #trig_tick = trig_sample - beamgate_sample

    #beamch_tick = beamch.readout_sample_number_RAW() - beamgate_sample
    #beamch_fire = getFireTick(beamch_adc)
    '''

