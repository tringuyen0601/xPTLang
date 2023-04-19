import sys
sys.path.append('./variable')
import variable as va 
def has_keyword (content):
  keyword = ['while', 'if', 'cond']
  for key in keyword:
    if key in content:
      return True
  return False

def print_action ( content):
  if  has_keyword(content):
    print('special state')
    return str(content.split(',')[1])
  opt = content.split(',')[1]
  src1 = content.split(',')[2]
  src2 = content.split(',')[3]
  dst = content.split(',')[4]

  
  if (opt == 'ADD'):
    return str('S_'+dst +'=S_' + src1 + '+S_'+src2)
  elif (opt == 'ADDI'):
    return str('S_'+dst +'=S_' + src1 + '+'+src2)
  elif (opt == 'SUB'):
    return str('S_'+dst +'=S_' + src1 + '-S_'+src2)
  elif (opt == 'SUBI'):
    return str('S_'+dst +'=S_' + src1 + '-'+src2)
  elif (opt == 'MUL'):
    return str('S_'+dst +'=S_' + src1 + '*S_'+src2)
  elif (opt == 'MULI'):
    return str('S_'+dst +'=S_' + src1 + '*'+src2)
  elif (opt == 'DIV'):
    return str('S_'+dst +'=S_' + src1 + '/S_'+src2)
  elif (opt == 'DIVI'):
    return str('S_'+dst +'=S_' + src1 + '/'+src2)
  elif (opt == 'LSHIFT'):
    return str('S_'+dst +'=S_' + src1 + '<<S_'+src2)
  elif (opt == 'LSHIFTI'):
    return str('S_'+dst +'=S_' + src1 + '<<'+src2)
  elif (opt == 'RSHIFT'):
    return str('S_'+dst +'=S_' + src1 + '>>S_'+src2)
  elif (opt == 'RSHIFTI'):
    return str('S_'+dst +'=S_' + src1 + '>>'+src2)
  elif (opt == 'AND'):
    return str('S_'+dst +'=S_' + src1 + '&S_'+src2)
  elif ( opt == 'ANDI'): 
    return str('S_'+dst +'=S_' + src1 + '&'+src2)
  elif (opt == 'OR'):
    return str('S_'+dst +'=S_' + src1 + '|S_'+src2)
  elif (opt == 'ORI'):
    return str('S_'+dst +'=S_' + src1 + '|'+src2)

    return ''

def tx_convert (content):
  x = int(content)
  if x < va.alphabet:
    return str(x)
  elif va.alphabet <= x < va.epsilon:
    return '!' +str(x) 
  elif va.var_start <= x < (va.var_start + va.max_var):
    return 'S_' + str(int(x- va.var_start)) 
  elif va.input_start <= x < (va.input_start+ va.max_var):
    return 'I_' + str( x- va.input_start)
  elif va.output_start <= x < (va.output_start + va.max_var):
    return 'O_' + str( int(x - va.output_start))
  elif (va.var_start + va.alphabet) <= x < (va.var_start + va.alphabet + va.max_var):
    return '!S_'+ str( x- va.var_start - va.alphabet)
  elif (va.input_start + va.alphabet) <= x < (va.input_start + va.alphabet + va.max_var):
    return '!I_'+ str( x- va.input_start - va.alphabnet)
  elif x == int(va.passthrough):
    return '*'
  elif x == int(va.anyPush):
    return 'Flush'
  return 'content: ' + content

def has_operator (string):
  if '+' in string:
    return True
  if '-' in string:
    return True
  if '*' in string:
    return True
  if '/' in string:
    return True
  if '>>' in string:
    return True
  if '<<' in string:
    return True
  if '&' in string:
    return True
  if '|' in string:
    return True
  return False

def dst_name( name):
  foldername = 'WRONG'
  filename = 'WRONG'
  if name == 'bp_decode':
    foldername  = 'bp_decode'
    filename    = 'bp_decode'
  elif name == 'bp_encode':
    foldername  = 'bp_encode'
    filename    = 'bp_encode'
  elif name == 'coo_csr_ptr':
    foldername  = 'coo_csr_ptr'
    filename    = 'coo_csr_ptr'
  elif name == 'csv':
    foldername  = 'csv'
    filename    = 'csv'
  elif name == 'csv_raw':
    foldername  = 'csv_indet'
    filename    = 'csv'
  elif name == 'hist':
    foldername  = 'hist'
    filename    = 'hist'
  elif name == 'gvd':
    foldername  = 'gvd'
    filename    = 'gvd'
  elif name == 'gve':
    foldername  = 'gve'
    filename    = 'gve'
  elif name == 'rle_encode':
    foldername  = 'rle_encode'
    filename    = 'rle_encode'
  elif name == 'rle_decode':
    foldername  = 'rle_decode'
    filename    = 'rle_decode'
  elif name == 'dense_csr':
    foldername  = 'dense_csr'
    filename    = 'dense_csr'
  return [foldername, filename]
















