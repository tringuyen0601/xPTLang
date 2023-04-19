# item: 
# ID
# starting line
# ending line  
# nested level
from optimized import *
from output import *
from variable_curator import *
import sys
sys.path.append('./variable')
import variable as va
import helper as hp

tabsize = 2
def operator_convert( opt, src2):
  if 's' in src2:
    if opt == '+':
      return 'ADD'
    elif opt == '-':  
      return 'SUB'
    elif opt == '*':  
      return 'MUL'
    elif opt == '/':  
      return 'DIV'
    elif opt == '>>':  
      return 'RSHIFT'
    elif opt == '<<':  
      return 'LSHIFT'
    elif opt == '&':  
      return 'AND'
    elif opt == '|':  
      return 'OR'
  else:
    if opt == '+':
      return 'ADDI'
    elif opt == '-':  
      return 'SUBI'
    elif opt == '*':  
      return 'MULI'
    elif opt == '/':  
      return 'DIVI'
    elif opt == '>>':  
      return 'RSHIFTI'
    elif opt == '<<':  
      return 'LSHIFTI'
    elif opt == '&':  
      return 'ANDI'
    elif opt == '|':  
      return 'ORI'


def find_id ( targetList, item):
  for i in range (0, len(targetList) ):
    if targetList[i] == item:
      return i

class state:
  def __init__(self, content):
    self.content = content 
    self.remove = False 
  def replace_content( self, new_content):
    self.content = self.content.split(',')[0] + new_content
class transition:
  def __init__(self, src, dst, left_input, left_stack, right_output, right_stack):
    self.src = src
    self.dst = dst
    self.left_input = left_input
    self.left_stack = left_stack
    self.right_output = right_output
    self.right_stack = right_stack
    self.remove = False
class effPDT_construct:
  def __init__(self, line_number, line):
    self.line = line
    self.line_number = line_number
    self.logic_level = int ((len(line) - len(line.lstrip(' ')) ) / tabsize)
    self.child = []
    self.peer = -1
    self.parent= -1 
    self.type = 'action'
    if 'if' in self.line:
      self.type = 'if'
    if 'while' in self.line:
      self.type = 'loop'
    if 'cond' in self.line:
      self.type = 'cond'
    self.state = []
    self.transition = []

class file:
  removeState = remove_state
  def __init__(self, filename):
    self.filename = filename
    name = hp.dst_name(filename.split('/')[2].split('.')[0])
    self.foldername = name[0]
    self.testname = name[1]
    self.size = 0
    self.content = []
    fp = open ( filename,'r')
    for line in fp:
      if ( line == '\n'):
        continue
      self.content.append(line.rstrip())
      self.size += 1
    print('file: ', self.filename, ' has ',  self.size, 'lines')
    for line in self.content:
      print( line)   

    self.stateList = []
    self.transitionList = []

    self.line_list =[]
    for i in range (0,self.size):
      self.line_list.append(i)

    self.root = effPDT_construct (-1,'')

  def build_root(self):
    self.full_program = []
    current_depth = -1
    for i in range (0, len(self.content)):
      new_line = effPDT_construct( i, self.content[i])
      self.full_program.append(new_line)
      if (new_line.logic_level == (current_depth+1) ):
          self.root.child.append(new_line.line_number)

  def find_child (self, node_id):
    node_depth = self.full_program[node_id].logic_level
#    print('Node ', node_id, ' Has logic level of ', node_depth)
    for i in range (node_id+1, self.size):
      if ( self.full_program[i].logic_level == node_depth +1):
        self.full_program[node_id].child.append(i)
      elif  (self.full_program[i].logic_level <= node_depth ):
        break
  def find_parent(self, node_id):
    node_depth = self.full_program[node_id].logic_level
    for i in range (node_id,-1,-1):
      if ( self.full_program[i].logic_level == node_depth -1):
        self.full_program[node_id].parent = i
        break;

  def find_peer (self, node_id):
    node_depth = self.full_program[node_id].logic_level
    for i in range (node_id+1, self.size):
      if ( self.full_program[i].logic_level == node_depth):
        self.full_program[node_id].peer = i
        break;
  def cond_convert( self, item):
    cond = item.split('(')[1]
    cond = cond.split(')')[0].lstrip()
    if  'true' in cond:
      return['512-0-512', '512-0-512', '512-512', '512-512']
    else:
      lhs = cond.split(' ')[0]
      lhs_id = int ( lhs.split('_')[1])
      comparator = cond.split(' ')[1]
      rhs = cond.split(' ')[2]
      print( 'condition to convert:' , lhs, comparator, rhs)
      if '_' in rhs:
        is_char = 0
        rhs_id = int( rhs.split('_')[1])
      else:
        is_char = 1
      if comparator == '!=':
        if 'input' in lhs:
          if  is_char :
            message = [va.Input[lhs_id]+'-0-' +str(int(rhs) + 256), '512-0-512', '512-512', '512-512']
          else:
            message = [va.Input[lhs_id]+'-0-' +va.NotStack[rhs_id], '512-0-512', '512-512', '512-512']
        else:
          if is_char: 
            message = ['512-0-512', va.Stack[lhs_id]+'-0-'+ str(int(rhs)+256), '512-512', '512-512']
          else:
            message=['512-0-512', va.Stack[lhs_id]+'-0-'+ va.NotStack[rhs_id], '512-512', '512-512']
      elif  comparator == '<':
        if 'input' in lhs:
          if  is_char :
            message[va.Input[lhs_id]+'-1-' +str(int(rhs)), '512-0-512', '512-512', '512-512']
          else:
            message= [va.Input[lhs_id]+'-1-' +va.Stack[rhs_id], '512-0-512', '512-512', '512-512']
        else:
          if is_char: 
            message= ['512-0-512', va.Stack[lhs_id]+'-1-'+ str(int(rhs)), '512-512', '512-512']
          else:
            message = ['512-0-512', va.Stack[lhs_id]+'-1-'+ va.Stack[rhs_id], '512-512', '512-512']
      elif  comparator == '>':
        if 'input' in lhs:
          if  is_char :
            message[va.Input[lhs_id]+'-2-' +str(int(rhs)), '512-0-512', '512-512', '512-512']
          else:
            message= [va.Input[lhs_id]+'-2-' +va.Stack[rhs_id], '512-0-512', '512-512', '512-512']
        else:
          if is_char: 
            message= ['512-0-512', va.Stack[lhs_id]+'-2-'+ str(int(rhs)), '512-512', '512-512']
          else:
            message = ['512-0-512', va.Stack[lhs_id]+'-2-'+ va.Stack[rhs_id], '512-512', '512-512']
      else:
        if 'input' in lhs:
          if  is_char :
            message[va.Input[lhs_id]+'-0-' +str(int(rhs)), '512-0-512', '512-512', '512-512']
          else:
            message= [va.Input[lhs_id]+'-0-' +va.Stack[rhs_id], '512-0-512', '512-512', '512-512']
        else:
          if is_char: 
            message= ['512-0-512', va.Stack[lhs_id]+'-0-'+ str(int(rhs)), '512-512', '512-512']
          else:
            message = ['512-0-512', va.Stack[lhs_id]+'-0-'+ va.Stack[rhs_id], '512-512', '512-512']
    
    print ('Return:', message)
    return message


  def action_convert (self, item):
    lhs = item.line.split('=')[0]
    rhs = item.line.split('=')[1]
    print('left hand-side:', lhs)
    print('right hand-side:', rhs)
    if (( 'input' in rhs) or ( 'output' in lhs)):
      Start =  state( str(len(self.stateList)) + va.e_state)
      self.stateList.append( Start)
      startID =  len(self.stateList)-1
      item.state.append( startID)
    
      end =  state( str( len(self.stateList)) +va.e_state)
      self.stateList.append( end)
      endID =  len(self.stateList)-1
      item.state.append( endID)
       
      if  'input' in rhs :
        inputID = int(rhs.split('_')[1])
        stackID = int(lhs.split('_')[1])
        tx = transition (startID, endID,va.Input[inputID] +'-0-'+va.passthrough,'512-0-512','512-512',va.Stack[stackID]+'-' + va.Input[inputID] )
      elif 'output' in  lhs:
        outputID = int(lhs.split('_')[1])
        stackID = int(rhs.split('_')[1])
        tx = transition (startID, endID,'512-0-512','512-0-512', va.Output[outputID] +'-' +va.Stack[stackID],'512-512' )

      self.transitionList.append(tx)
      item.transition.append( len(self.transitionList)-1)
    else:
      dst = lhs.split('_')[1]
      if hp.has_operator(rhs) == True:
        src1 =  rhs.split(' ')[1]
        if 's' in src1:
          src1 = src1.split('_')[1]
        src2 =  rhs.split(' ')[3]
        opt =  operator_convert(rhs.split(' ')[2], src2)
        if 's' in src2:
          src2 = src2.split('_')[1]
        Start =  state( str(len(self.stateList)) +','+ opt +','+ src1+',' + src2+',' +dst + ',0,0' )
        self.stateList.append( Start)
        startID =  len(self.stateList)-1
        item.state.append( startID)
    
        end =  state( str( len(self.stateList)) +va.e_state)
        self.stateList.append( end)
        endID =  len(self.stateList)-1
        item.state.append( endID)

        tx = transition (startID, endID,'512-0-512','512-0-512','512-512','512-512' )
        self.transitionList.append(tx)
        item.transition.append( len(self.transitionList)-1)
      else:
        Start =  state( str(len(self.stateList)) + va.e_state)
        self.stateList.append( Start)
        startID =  len(self.stateList)-1
        item.state.append( startID)
    
        end =  state( str( len(self.stateList)) +va.e_state)
        self.stateList.append( end)
        endID =  len(self.stateList)-1
        item.state.append( endID)

        tx = transition (startID, endID,'512-0-512','512-0-512','512-512','512-512' )
        if ( 'flush' in rhs):
          tx = transition (startID, endID,'512-0-512','512-0-512','512-512',va.Stack[int(dst)]+ '-' + va.anyPush )
        else:
          tx = transition (startID, endID,'512-0-512','512-0-512','512-512',va.Stack[int(dst)]+ '-' + rhs )
        self.transitionList.append(tx)
        item.transition.append( len(self.transitionList)-1)


  def build_graph (self):
    workList = self.line_list.copy()

#create root state
    rootStart =  state('0' + va.e_state)
    self.stateList.append( rootStart)
    self.root.state.append( len(self.stateList)-1)
    rootEnd =  state('1'+ va.e_state)
    print ( self.root.state)
    self.stateList.append( rootEnd)
    self.root.state.append( len(self.stateList)-1 )

    print ( 'root node:', self.root.state)

    for item in self.full_program:
      print( '-----------------------------------------')
      if ( item.type == 'action'):
        self.action_convert(item)
      elif (( item.type == 'cond') or ( item.type == 'loop') or ( item.type == 'if')):
        Start =  state( str(len(self.stateList)) + ','+ item.line)
        self.stateList.append( Start)
        startID =  len(self.stateList)-1
        item.state.append( startID)
        for child_node in item.child:
          item.state.append('line '+str( child_node)) 

        end =  state( str(len(self.stateList)) + va.e_state)
        self.stateList.append( end)
        endID =  len(self.stateList)-1
        item.state.append( endID)
      print( 'line: ', item.line_number, ': ', item.line)
      print( 'state: ', item.state, ' Transition: ', item.transition)

  def connect_root(self):
    bodyStart = self.root.state[0]
    bodyEnd = self.root.state[1]

    body = self.root.child

    targetList = self.full_program[body[0]].state  
    targetStart = targetList[0]
    targetEnd   = targetList[ len(targetList)-1]

    tx = transition( bodyStart, targetStart, '512-0-512', '512-0-512', '512-512', '512-512')
    self.transitionList.append(tx )
    self.root.transition.append( len(self.transitionList)-1)
    print(' adding tx between ', bodyStart, ' and', targetStart)
    replaceID = len(self.root.state )-1 
    print('list of ', self.root.state, ' item ', str(body[0]), ' id ', replaceID)  
    self.root.state[replaceID:replaceID] = self.full_program[body[0] ].state
    print( 'new root State List: ', self.root.state)

    for bodyID in range ( 1, len (body)):
      targetList = self.full_program[body[bodyID]].state  
      targetStart = targetList[0]

      tx = transition( targetEnd, targetStart, '512-0-512', '512-0-512', '512-512', '512-512')
      print(' adding tx between ', targetEnd, ' and', targetStart)
      self.transitionList.append(tx )
      self.root.transition.append( len(self.transitionList)-1)
          
      targetEnd   = targetList[ len(targetList)-1]

      replaceID = len(self.root.state )-1 
      print('list of ', self.root.state, ' item ', str(body[bodyID]), ' id ', replaceID)  
      self.root.state[replaceID:replaceID] = self.full_program[body[bodyID] ].state
      print( 'new root State List: ', self.root.state)
          

    tx = transition( targetEnd, bodyEnd, '512-0-512', '512-0-512', '512-512', '512-512')
    print(' adding tx between ', targetEnd, ' and', bodyEnd)
    self.transitionList.append(tx )
    self.root.transition.append( len(self.transitionList)-1)

  def back_substitution( self):
    for i in range ( len( self.full_program)-1 ,-1,-1):
      print('\n')
      print('CONSIDER line: ', i)
      self.dot_output('back_substitution'+ str(i))
      bodyStart = self.full_program[i].state[0]
      bodyEnd   = self.full_program[i].state[ len (self.full_program[i].state)-1 ]
      body = []
      for item in self.full_program[i].state:
        if type(item) == str:
          lineid = int(item.split(' ')[1])
          body.append( lineid)
      if body:
        print( 'line ',i, 'need substitution with line ', body)
      if self.full_program[i].type == 'cond':
        self.stateList[bodyStart].replace_content(va.e_state)
        if body:
          targetList = self.full_program[body[0]].state  
          targetStart = targetList[0]
          targetEnd   = targetList[ len(targetList)-1]

          tx = transition( bodyStart, targetStart, '512-0-512', '512-0-512', '512-512','512-512')
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)
          print(' adding tx between ', bodyStart, ' and', targetStart)
          replaceID = find_id (self.full_program[i].state,'line ' +str( body[0]) )
          print('list of ', self.full_program[i].state, ' item ', str(body[0]), ' id ', replaceID)  
          self.full_program[i].state[replaceID:replaceID] = self.full_program[body[0] ].state
          self.full_program[i].state.remove('line '+  str(body[0] ))
          print( 'new State List: ', self.full_program[i].state)
        
          for bodyID in range ( 1, len (body)):
            targetList = self.full_program[body[bodyID]].state  
            targetStart = targetList[0]

            tx = transition( targetEnd, targetStart, '512-0-512', '512-0-512', '512-512', '512-512')
            print(' adding tx between ', targetEnd, ' and', targetStart)
            self.transitionList.append(tx )
            self.full_program[i].transition.append( len(self.transitionList)-1)
          
            targetEnd   = targetList[ len(targetList)-1]

            replaceID = find_id (self.full_program[i].state,'line ' +str( body[bodyID]) )
            print('list of ', self.full_program[i].state, ' item ', str(body[bodyID]), ' id ', replaceID)  
            self.full_program[i].state[replaceID:replaceID] = self.full_program[body[bodyID] ].state
            self.full_program[i].state.remove('line '+  str(body[bodyID] ))
            print( 'new State List: ', self.full_program[i].state)
          

          tx = transition( targetEnd, bodyEnd, '512-0-512','512-0-512', '512-512', '512-512')
          print(' adding tx between ', targetEnd, ' and', bodyEnd)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)
                
        else:
          tx = transition( bodyStart, bodyEnd, '512-0-512','512-0-512', '512-512', '512-512')
          print(' adding tx between ', bodyStart, ' and', bodyEnd)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)


      elif self.full_program[i].type == 'if':
        self.stateList[bodyStart].replace_content(va.e_state)
        print('If state list of ', self.full_program[i].state,)  
        for bodyID in range ( 0, len (body)):
          targetList = self.full_program[body[bodyID]].state  
          targetStart = targetList[0]
          targetEnd   = targetList[ len(targetList)-1]

          replaceID = find_id (self.full_program[i].state,'line ' +str( body[bodyID]) )
          print('list of ', self.full_program[i].state, ' item ', str(body[bodyID]), ' id ', replaceID)  
          self.full_program[i].state[replaceID:replaceID] = self.full_program[body[bodyID] ].state
          self.full_program[i].state.remove('line '+  str(body[bodyID] ))
          print( 'new State List: ', self.full_program[i].state)

          lineadd  =  self.full_program[body[bodyID]].line.replace('cond','')
          tx_body = self.cond_convert(lineadd)
          tx = transition( bodyStart, targetStart, tx_body[0], tx_body[1], tx_body[2], tx_body[3])
          print(' adding tx',lineadd ,' between ', bodyStart, ' and', targetStart)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)
          
          tx = transition( targetEnd, bodyEnd, '512-0-512','512-0-512','512-512','512-512')
          print(' adding tx between ', targetEnd, ' and', bodyEnd)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)


      elif self.full_program[i].type == 'loop':
        print('Loop state list of ', self.full_program[i].state,)  
        self.stateList[bodyStart].replace_content(va.e_state)
        for bodyID in range ( 0, len (body)):
          targetList = self.full_program[body[bodyID]].state  
          targetStart = targetList[0]
          targetEnd   = targetList[ len(targetList)-1]

          replaceID = find_id (self.full_program[i].state,'line ' +str( body[bodyID]) )
          print('list of ', self.full_program[i].state, ' item ', str(body[bodyID]), ' id ', replaceID)  
          self.full_program[i].state[replaceID:replaceID] = self.full_program[body[bodyID] ].state
          self.full_program[i].state.remove('line '+  str(body[bodyID] ))
          print( 'new State List: ', self.full_program[i].state)

          lineadd  =  self.full_program[i].line.replace('while','')
          tx_body = self.cond_convert(lineadd)
          tx = transition( bodyStart, targetStart, tx_body[0], tx_body[1], tx_body[2], tx_body[3])
          print(' adding tx',lineadd ,' between ', bodyStart, ' and', targetStart)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)
          if 'true' in lineadd: 
            tx = transition( bodyStart, bodyEnd, 'cut', 'cut','cut','cut')
          else: 
            if '!' in lineadd:
              comparator = '=='
              orig_comparator = '!='
            else:
              comparator = '!='
              orig_comparator = '=='
            lhs = lineadd.split(orig_comparator)[0]
            rhs = lineadd.split(orig_comparator)[1]
            print(lhs ,' ' ,comparator,' ' ,rhs)
            tx_body = self.cond_convert(lhs  + comparator + rhs)
            #tx_body = self.cond_convert(lineadd)
            tx = transition( bodyStart, bodyEnd, tx_body[0], tx_body[1], tx_body[2], tx_body[3])
          print(' adding tx','!' + lineadd ,' between ', bodyStart, ' and', targetStart)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)

          tx = transition( targetEnd, bodyStart, '512-0-512', '512-0-512', '512-512', '512-512')
          print(' adding tx between ', targetEnd, ' and', bodyStart)
          self.transitionList.append(tx )
          self.full_program[i].transition.append( len(self.transitionList)-1)
      print('\n')
  def optimized( self):
    mark_remove(self)
    print('List of state to remove: ', self.removeList)
    for stateID in self.removeList:
      remove_state (self, stateID)
    result = rebuild(self)
    self.stateList = result[0]
    self.transitionList = result[1]
    for stateID in range ( len(self.stateList)):
      self.stateList[stateID].oldID= stateID
  
  def remove_level1(self):
    remove_multiple_incoming(self)

  def remove_level2(self):
    remove_multiple_incoming_2(self)

  def dot_output ( self, name):
    dot_file = self.filename.split('/')[2].split('.')[0]
    self.dot_file = './paper/' + dot_file +'_'+name+ '.dot'
    fp = open ( self.dot_file,'w')
    fp.write('digraph \" graph\" { \n')
    fp.write('\trankdir=LR;\n')

    fp.write('\tsubgraph statemachine {\n')    
    for stateID in range(len(self.stateList)):
      fp.write('N'+str(stateID))
      fp.write('\t\t[shape = circle,\n')
      if  ('EPSILON' in self.stateList[stateID].content) == False  :
        fp.write('label=\"'+ str(stateID)+ ': ' + str(hp.print_action(self.stateList[stateID].content))+ '\"')
      else: 
        fp.write('label=\"'+ str(stateID)+  '\"')
      fp.write('\t\tcolor=\"black\"];\n') 
    
    for txID in range(len(self.transitionList)):
      fp.write('\tN'+str(self.transitionList[txID].src)+'->N'+str( self.transitionList[txID].dst))
      fp.write('[label=<' + str(txID) + ': ')  
    
      if (('512' in self.transitionList[txID].left_input)== False ) and  (('cut' in self.transitionList[txID].left_input)== False ):
        fp.write ( str( hp.tx_convert(self.transitionList[txID].left_input.split('-')[0])) + '==')
        fp.write ( str( hp.tx_convert(self.transitionList[txID].left_input.split('-')[1])))
      if (('512' in self.transitionList[txID].left_stack)== False ) and  (('cut' in self.transitionList[txID].left_stack)== False ):
        fp.write ( str( hp.tx_convert(self.transitionList[txID].left_stack.split('-')[0]))+ '==')
        fp.write ( str( hp.tx_convert(self.transitionList[txID].left_stack.split('-')[1])))
      fp.write('|  ')

      if (('512' in self.transitionList[txID].right_output)== False ) and  (('cut' in self.transitionList[txID].right_output)== False ):
        fp.write ( str( hp.tx_convert(self.transitionList[txID].right_output.split('-')[0]))+ '=')
        fp.write ( str( hp.tx_convert(self.transitionList[txID].right_output.split('-')[1])))
      if (('512' in self.transitionList[txID].right_stack)== False ) and  (('cut' in self.transitionList[txID].right_stack)== False ):
        fp.write ( str( hp.tx_convert(self.transitionList[txID].right_stack.split('-')[0]))+ '=')
        fp.write ( str( hp.tx_convert(self.transitionList[txID].right_stack.split('-')[1])))
      fp.write('>,color=\"black\"];\n');
    fp.write('\t}')
    fp.write('\t}')
    fp.close()
  def print_fst(self):
    for stateID in range(len(self.stateList)):
        print('State: ' , str(hp.print_action(self.stateList[stateID].content)))
    for txID in range(len(self.transitionList)):
        print('Tx: ', txID,'[ From state ', self.transitionList[txID].src, ': ',  end ="")

        if (('512' in self.transitionList[txID].left_input)== False ) and  (('cut' in self.transitionList[txID].left_input)== False ):
          print ( str( hp.tx_convert(self.transitionList[txID].left_input.split('-')[0])), '==', end ="")
          print ( str( hp.tx_convert(self.transitionList[txID].left_input.split('-')[1])),  end = "")

        if (('512' in self.transitionList[txID].left_stack)== False ) and  (('cut' in self.transitionList[txID].left_stack)== False ):
          print ( str( hp.tx_convert(self.transitionList[txID].left_stack.split('-')[0])), '==', end ="")
          print ( str( hp.tx_convert(self.transitionList[txID].left_stack.split('-')[1])), end = "")
        print('| To state ',  self.transitionList[txID].dst,': ', end="")

        if (('512' in self.transitionList[txID].right_output)== False ) and  (('cut' in self.transitionList[txID].right_output)== False ):
          print ( str( hp.tx_convert(self.transitionList[txID].right_output.split('-')[0])), '=', end ="")
          print ( str( hp.tx_convert(self.transitionList[txID].right_output.split('-')[1])),  end = "")

        if (('512' in self.transitionList[txID].right_stack)== False ) and  (('cut' in self.transitionList[txID].right_stack)== False ):
          print ( str( hp.tx_convert(self.transitionList[txID].right_stack.split('-')[0])), '=', end ="")
          print ( str( hp.tx_convert(self.transitionList[txID].right_stack.split('-')[1])),  end = "")
        print(']')
  def output(self):
    output_state(self)
    output_transition(self)
    output_config(self)
