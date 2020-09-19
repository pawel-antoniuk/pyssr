# pyssr - a Python library for spatial audio reproduction
**pyssr** is a light Python wrapper around **SoundScape Renderer**. This is just an experimental fork of SoundScape Renderer with some Python integration code. In the future, code of the original SoundScape Renderer should be removed from this repository in favor of the git submodule.

Code responsible for Python integration is in the py/ directory.

## Example
```python
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
```

## Original README
This is the source distribution of SoundScape Renderer (SSR) licensed under the
GPLv3+. Please consult the file COPYING for more information about this license.

The user manual in the doc/ directory contains relevant informations about the
SSR, including installation instructions. Additional (very detailed)
installation instructions can be found in the file INSTALL.

For questions, bug reports and feature requests:

Contact:       ssr@spatialaudio.net
Website:       http://spatialaudio.net/ssr/
Documentation: http://ssr.readthedocs.io/en/latest/

Copyright (c) 2016-2018 Division of Applied Acoustics
                        Chalmers University of Technology

Copyright (c) 2012-2014 Institut für Nachrichtentechnik, Universität Rostock

Copyright (c) 2006-2012 Quality & Usability Lab
                        Deutsche Telekom Laboratories, TU Berlin
