import scikit_build_example as bib


import numpy as np

kernel = np.array([ #przykłaowa macierz
    [0, -1, 0],
    [-1, 5, -1],
    [0, -1, 0],
], dtype=np.float32)

kernel_list = kernel.tolist()

bib.sin_wave(2, 4410, 3, 0.5) #amplituda, próbkowanie, czas trwania, częstotliwość
