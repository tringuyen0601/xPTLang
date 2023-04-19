import re
stack_opt = ['flush','popall', 'pop']


def variable_curate ( struct):
# find all the stack:
  variableList = []
  struct.stackList =   []
  struct.variableList = []
  for line in struct.content:
    found = re.findall(r"s_\d", line)
    for item in found:
      variableID = int(item.split('_')[1])
      if variableID not in variableList:  
        variableList.append(variableID)
  print ('Full Variable List: ', variableList)

  for line in struct.content:
    found = re.findall(r"s_\d", line)
    for item in found:
      variableID = int(item.split('_')[1])
      for keyword in stack_opt:
        if keyword in line:
          if variableID not in struct.stackList:
            struct.stackList.append(variableID)
  print ('StackList: ', struct.stackList)
  for item in variableList:
    if item not in struct.stackList: 
      struct.variableList.append(item)
  print ('VariableList: ', struct.variableList)

  struct.bitmap = 0
  for item in struct.stackList:
    struct.bitmap = struct.bitmap +  (1 << item) 
  print( 'bitmap: ', format(struct.bitmap, 'b'), ', writting : ', struct.bitmap)
