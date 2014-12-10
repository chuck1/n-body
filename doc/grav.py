import pylab as pl
import numpy as np

def f(r):
    return 1.0 / np.power(r,2)

def rep(r):
    return -1.0 / np.power(r,3)

x = np.logspace(-1,2,100)

y = f(x)

pl.plot(x,y)
pl.plot(x,rep(x))
pl.plot(x,f(x) + rep(x))

pl.ylim([-0.005,0.005])

pl.show()

