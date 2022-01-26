#!/usr/bin/python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# GNU Radio version: 3.9.2.0 - ngsched

from gnuradio import blocks
import numpy
from gnuradio import gr
from gnuradio.filter import firdes
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import zynq


class loopback(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Simple Loopback")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.zynq_loopback_0 = zynq.zynq_loopback(0, 4096, 1)
        self.src = blocks.null_source(gr.sizeof_gr_complex)
        self.hd = blocks.head(gr.sizeof_gr_complex*1, 128*4096)

        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_gr_complex*1, 'output.bin', False)
        self.blocks_file_sink_0.set_unbuffered(False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.src, 0), (self.hd, 0))
        self.connect((self.hd, 0), (self.zynq_loopback_0, 0))
        self.connect((self.zynq_loopback_0, 0), (self.blocks_file_sink_0, 0))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

def main(top_block_cls=loopback, options=None):
    tb = top_block_cls()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()

    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
