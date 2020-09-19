import sys
import pathlib

sys.path.append(str(pathlib.Path(__file__).parent.absolute()) + '/.libs')

import math
from scipy.io.wavfile import read, write
import numpy as np
import ssr

source_filename = '../data/samples/example1.wav'

options = {
    'sample_rate': '44100',
    'block_size': '128',
    'hrir_file': '../data/impulse_responses/hrirs/hrirs_fabian_min_phase_eq.wav'
}

renderer = ssr.BinauralRenderer(options)
renderer.add_source({})
renderer.reference_position(1, 0)

sources = []
signal_in = read(source_filename)[1]
sources.append(signal_in)
sources = np.asarray(sources).transpose()

signal_out = np.asarray(renderer.process(sources, True))
write('out.wav', 44100, signal_out)


renderer.partial_process_begin(sources, True)
block_n = 0
block_coeff = int(options['block_size']) / 20000
while renderer.partial_process():
    block_n += 1
    renderer.reference_position(
        math.cos(block_n * block_coeff), math.sin(block_n * block_coeff))

signal_out = np.asarray(renderer.partial_process_result())
write('out2.wav', 44100, signal_out)
