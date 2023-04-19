import numpy as np
import glob
import struct


chosen_order = "little"
byte_order = {
  "big":">",
  "little":"<"
  }
datatype= {
  "int":"l",
  "float":"f",
  "char":"c"
}
filename = "./rle_encode/rle_input/xenon1_csr_ind_RLDin.txt"
location = "."+ filename.split('.')[1] + "_B"

threadsize =  5
print(location)
filecontent = open ( filename,"rb")
bytefile = open ( location ,"wb")
for item in filecontent:
  newitem = int(item) 
  bytefile.write( struct.pack( byte_order[chosen_order] + datatype["int"], int(newitem)))
bytefile.close()
filecontent.close()
