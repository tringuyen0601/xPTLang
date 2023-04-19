import graphviz
from subprocess import check_call
import glob
graphPath = './graph/'


dot_files = glob.glob ( graphPath+'*.dot' )
print ( dot_files)

for dot_file in dot_files:
  filename = dot_file.split('dot')[0]
  print ( filename)
  check_call( ['dot', '-Tjpeg', dot_file, '-o',  filename + 'jpeg'])

