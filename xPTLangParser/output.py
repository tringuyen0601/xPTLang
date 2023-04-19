import sys
sys.path.append('./variable')
import variable as va
import helper as hp
import re
def output_config (self):
  stackList = []
  inputList = []
  outputList = []
  #count number of variable, input, output in program
  for line in self.content:
    found = re.findall(r"s_\d", line)
    for item in found:
      stackList.append(int(item.split('_')[1]))
    found = re.findall(r"input_\d", line)
    for item in found:
      inputList.append(int(item.split('_')[1]))
    found = re.findall(r"output_\d", line)
    for item in found:
      outputList.append(int(item.split('_')[1]))
  print(stackList)
  print(inputList)
  if not outputList:
    outputList = [0]
  print(outputList)
  print('output_config at: ./output/' + self.foldername + '/' + self.testname + '_config.txt')
  fp = open ('./output/' + self.foldername + '/' + self.testname + '_config.txt', 'w')
  fp.write('Number of State: ' + str(len(self.stateList)) +'\n')
  fp.write('Number of Transition: ' + str(len(self.transitionList)) +'\n')
  fp.write('Number of Stack: ' + str(max(stackList) + 1) +'\n')
  fp.write('Number of Input Stream: ' + str(max(inputList)+1) +'\n')
  fp.write('Number of Output Stream: ' + str(max(outputList)+1) +'\n')
  fp.write('Stack bitmap: ' + str(self.bitmap) +'\n')
  fp.close()
def output_state (self):
  print('output_state at: ./output/' + self.foldername + '/' + self.testname + '_state.txt')
  fp = open ('./output/' + self.foldername + '/' + self.testname + '_state.txt', 'w')
  for state in self.stateList:
    fp.write (state.content + ',0\n')
  fp.close()

def output_transition (self):
  print('output_tx at: ./output/' + self.foldername + '/' + self.testname + '_transition.txt')
  fp = open ('./output/' + self.foldername + '/' + self.testname + '_transition.txt', 'w')
  for txID in range(len(self.transitionList)):
    fp.write (str(txID) +':')
    fp.write (self.transitionList[txID].left_input +', ')
    fp.write (self.transitionList[txID].left_stack +', ')
    fp.write (str(self.transitionList[txID].src) + ' | ')
    fp.write (self.transitionList[txID].right_output+', ')
    fp.write (self.transitionList[txID].right_stack+', ')
    fp.write (str(self.transitionList[txID].dst) +'\n')
  fp.close()
