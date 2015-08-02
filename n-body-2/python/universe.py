
import struct
import glob
import numpy as np

class Body(object):
	def __init__(self, x, v, f, m, r):
		#print x
		self.x = np.array(x)
		self.v = v
		self.f = f
		self.m = m
		self.r = r

class Frame(object):
	def __init__(self, bodies):
		self.bodies = bodies
		
		#print bodies[0].x

	def stats(self):
		mx = np.zeros((3,))
		m = 0
		
		xmin = np.ones((3,)) * 10**10
		xmax = np.ones((3,)) * -10**10
		
		for body in self.bodies:
			mx += body.x * body.m
			m += body.m

			if body.x[0] < xmin[0]:
				xmin[0] = body.x[0]
			if body.x[1] < xmin[1]:
				xmin[1] = body.x[1]
			if body.x[2] < xmin[2]:
				xmin[2] = body.x[2]
			if body.x[0] > xmax[0]:
				xmax[0] = body.x[0]
			if body.x[1] > xmax[1]:
				xmax[1] = body.x[1]
			if body.x[2] > xmax[2]:
				xmax[2] = body.x[2]
		
		return mx/m, m, xmin, xmax

def read_body(binary, o):

	x = struct.unpack_from('fff', binary, o)
	o += 3 * 4
	v = struct.unpack_from('fff', binary, o)
	o += 3 * 4
	f = struct.unpack_from('fff', binary, o)
	o += 3 * 4

	m,r = struct.unpack_from('ff', binary, o)
	o += 2*4
	
	o += 4
	

	return Body(x, v, f, m, r), o

def read_frame(binary, o):
	bodies_size, = struct.unpack_from('I' ,binary, o)
	o += 4
	#print "bodies size = {}".format(bodies_size)
	
	bodies = []
	
	for i in range(bodies_size):
		body, o = read_body(binary, o)
		bodies.append(body)
	
	return Frame(bodies), o
	
def read_universe_file(filename):
	print filename

	with open(filename, 'rb') as f:
		binary = f.read()

	o = 0
	a,b,c,frames_size = struct.unpack_from('ii32sI', binary, o)
	o += 4+4+32+4
	
	print "  num steps:",a
	print " first step:",b
	print "       name:",repr(c)
	print "frames size:",frames_size
	
	frames = []

	for i in range(frames_size):
		frame, o = read_frame(binary, o)
		frames.append(frame)

	return frames

def read_universe_files(files_dat, rang):
	with open(files_dat, 'r') as f:
		l = f.read().split('\n')
		l = filter(lambda x: bool(x), l)
	
	universes = []
	
	if rang:
		l = l[rang[0]:rang[1]]

	for universe_filename in l:
		u = read_universe_file(universe_filename)
		universes.append(u)
		
	return universes

if __name__ == '__main__':
	files = glob.glob('files*')

	us = read_universe_files(files[0])
	
	print us[0][0].stats()



