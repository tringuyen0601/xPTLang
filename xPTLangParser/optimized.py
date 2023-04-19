from construct import *

def remove_state( self, stateID):
  print( 'Remove: ', stateID)
  print( self.stateList[stateID].content)

  incomingTx =[]
  outgoingTx =[]
  nextState = 'epsilon'
  lastState = 'epsilon'
  for txID in range( len ( self.transitionList)):
    if (self.transitionList[txID].dst == stateID):
      incomingTx.append(txID)
      lastState = self.transitionList[txID].src

    if (self.transitionList[txID].src == stateID):
      outgoingTx.append(txID)
      nextState = self.transitionList[txID].dst

  print ( 'Incoming Tx: ', incomingTx)
  print (' Out going Tx: ', outgoingTx)
  print (' Substitute St: ', nextState)
  
  for txID in range ( len (incomingTx)):
    self.transitionList[incomingTx[txID]].dst = nextState
  for txID in range ( len (outgoingTx)):
    self.transitionList[outgoingTx[txID]].dst = self.transitionList[outgoingTx[txID]].src
    self.transitionList[outgoingTx[txID]].remove = True

def get_state_info ( self, stateID):
  incomingTx =[]
  outgoingTx =[]
  opt = 'effective'
  if ('EPSILON' in  self.stateList[stateID].content):
    opt = 'epsilon'

  for txID in range( len ( self.transitionList)):
    if (self.transitionList[txID].dst == stateID):
      incomingTx.append(txID)
  
    if (self.transitionList[txID].src == stateID):
      outgoingTx.append(txID)

  incoming = 'epsilon'
  for txID in range ( len (incomingTx)):  
    left_input = self.transitionList[incomingTx[txID]].left_input
    left_stack = self.transitionList[incomingTx[txID]].left_stack
    right_output = self.transitionList[incomingTx[txID]].right_output
    right_stack = self.transitionList[incomingTx[txID]].right_stack
    effect = True
    if ('512' in left_input) and ( '512' in left_stack) and ('512' in right_output) and ('512' in right_stack):
      effect = False
    if effect == True:
      incoming = 'efective'  

  outgoing = 'effective'
  if  len(outgoingTx) == 0:
    outgoing = 'epsilon'
  if  len(outgoingTx) == 1: 
    left_input = self.transitionList[outgoingTx[0]].left_input
    left_stack = self.transitionList[outgoingTx[0]].left_stack
    right_output = self.transitionList[outgoingTx[0]].right_output
    right_stack = self.transitionList[outgoingTx[0]].right_stack
    if ('512' in left_input) and ( '512' in left_stack) and ('512' in right_output) and ('512' in right_stack):
      outgoing = 'epsilon'   
  return [ len(incomingTx), outgoing, opt, incoming]
def mark_remove (self):
  self.removeList = []
  for stateID in range ( len ( self.stateList)):
    result = get_state_info ( self, stateID)  
    print( result)
    if (result[0] <= 1) and (result[1] == 'epsilon') and (result[2] == 'epsilon'):
      self.stateList[stateID].remove = True 
      self.removeList.append(stateID)

def rebuild(self):
  stateList = []
  txList = []
  for stateID in range ( len(self.stateList)):
    self.stateList[stateID].oldID = stateID
    old_content = self.stateList[stateID].content
    new_content = str(stateID) +',' + old_content.split(',')[1] +',' + old_content.split(',')[2]  +',' + old_content.split(',')[3] +',' + old_content.split(',')[4]+',' + old_content.split(',')[5]  
    self.stateList[stateID].content = new_content
    #new_content = new_content +', ' + old_content.split(',')[6]  
    if ( self.stateList[stateID].remove == False):
      stateList.append(self.stateList[stateID])
  for txID in range ( len(self.transitionList)):

    self.transitionList[txID].remove = True
    dst =  self.transitionList[txID].dst
    for sID in range(len(stateList)):
      if stateList[sID].oldID == dst:
        self.transitionList[txID].remove = False

    if self.transitionList[txID].remove == False:
      txList.append(self.transitionList[txID])
       
 # redirectID
  for txID in range ( len (txList)):
    for sID in range(len(stateList)):
      if stateList[sID].oldID == txList[txID].src:
        txList[txID].src = sID
      if stateList[sID].oldID == txList[txID].dst:
        txList[txID].dst = sID
 
  for nextstateID in range ( len ( stateList)):
    print ( 'State: ', stateList[nextstateID].oldID)

  return [stateList, txList]

def remove_multiple_incoming( self):
  times = 0
  stateID = find_multiple_incoming_epsilon(self)
  while (stateID!= 'none'):
    print( 'state to remove: ', str(stateID))
    self.stateList[stateID].remove = True
    remove_state(self,stateID)
    result = rebuild (self)
    self.stateList = result[0]
    self.transitionList = result[1]
    for sID in range ( len ( self.stateList)):
      self.stateList[sID].oldID = sID
    #self.dot_output('l1_remove_' + str(times))
    times +=1
    stateID = find_multiple_incoming(self)

def find_multiple_incoming_epsilon( self):
  state_result = 'none'
  for stateID in range ( len ( self.stateList)):
    result = get_state_info ( self, stateID)  
    print( result)
    if (result[0] > 1) and (result[1] == 'epsilon') and (result[2] == 'epsilon') and (result[3] == 'epsilon'):
      state_result = stateID
      print( 'find state: ', state_result)
      break 

  return state_result

def find_multiple_incoming( self):
  state_result = 'none'
  for stateID in range ( len ( self.stateList)):
    result = get_state_info ( self, stateID)  
    print( result)
    if (result[0] > 1) and (result[1] == 'epsilon') and (result[2] == 'epsilon'):
      state_result = stateID
      print( 'find state: ', state_result)
      break 

  return state_result


def remove_multiple_incoming_2( self):
  times = 0
  stateID = find_multiple_incoming(self)
  while (stateID!= 'none'):
    print( 'state to remove: ', str(stateID))
    self.stateList[stateID].remove = True
    remove_state(self,stateID)
    result = rebuild (self)
    self.stateList = result[0]
    self.transitionList = result[1]
    for sID in range ( len ( self.stateList)):
      self.stateList[sID].oldID = sID
    #self.dot_output('l2_remove_' + str(times))
    times +=1
    stateID = find_multiple_incoming(self)
