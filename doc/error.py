import numpy as np
import pylab as pl

def e(d, w):
    r = 1.0 / d**2 + 1.0 / (d+w)**2
    return (r - 2.0 / (d + 0.5 * w)**2) / r

def w(d, a):
    return d * a


d = 1.0

a = np.linspace(0.01, 0.2, 100)

pl.plot(a, e(d, w(d, a)))
pl.show()



