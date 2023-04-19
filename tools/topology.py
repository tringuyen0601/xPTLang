import matplotlib.pyplot as plt
import pandas as pd 
import seaborn as sns
input_file = ('./Topology.csv')
my_data = pd.read_csv(input_file)

print(my_data)
print('Finish reading', input_file)
number_of_test = len(my_data['Name'].unique())
number_of_mode = len (my_data['Mode'].unique())
print( number_of_test , " benchmark")
print( number_of_mode, " mode")
average_col = []
for col_id in range( number_of_mode):
  average_col.append( (number_of_test-1) + col_id * number_of_test)
print( "Average value will be printed in col: ", average_col)

color = sns.color_palette('Set2', 13)

my_palette = [ color[4]]

sns.set_context("paper")
sns.set(font_scale=1.1)
sns.set_style("whitegrid")
fig , ax= plt.subplots(figsize = (15,6))

state_graph = sns.barplot(x="Name", y ="State", hue ="Mode", data = my_data,  palette=color)

i=0

for  bar in state_graph.patches:
  bar.set_width(bar.get_width() * 1)
  if  my_data['State'][i] < (max(my_data['State']) * 0.1):
    state_graph.annotate(('%f' % my_data['State'][i]).rstrip('0').rstrip('.') ,
                     (bar.get_x() + bar.get_width() / 2,
                      bar.get_height()*1.4 ), ha='center', va='center',
                      #1.3), ha='center', va='center',
                     size=11, xytext=(0, 5), rotation=90,
                     textcoords='offset points')
  else:
    state_graph.annotate(('%f' % my_data['State'][i]).rstrip('0').rstrip('.') ,
                     (bar.get_x() + bar.get_width() / 2,
                      bar.get_height()), ha='center', va='center',
                      #1.3), ha='center', va='center',
                     size=11, xytext=(0, -10), rotation=90,
                     textcoords='offset points')
  if i in average_col:
    bar.set_hatch('\\')
  i+=1

state_graph.set_ylabel('Number of States')
state_graph.set_xlabel('')

plt.legend(loc = "upper center", bbox_to_anchor=(0.5,1.05),  ncol=4)
plt.savefig('.'+ input_file.split('.')[1] + '_state.png')

fig , ax= plt.subplots(figsize = (15,6))
transition_graph = sns.barplot(x="Name", y ="Transition", hue ="Mode", data = my_data,  palette=color)

i=0

for  bar in transition_graph.patches:
  bar.set_width(bar.get_width() * 1)
  if  my_data['Transition'][i] < (max(my_data['Transition']) * 0.1):
    transition_graph.annotate(('%f' % my_data['Transition'][i]).rstrip('0').rstrip('.') ,
                     (bar.get_x() + bar.get_width() / 2,
                      bar.get_height()*1.4 ), ha='center', va='center',
                      #1.3), ha='center', va='center',
                     size=11, xytext=(0, 5), rotation=90,
                     textcoords='offset points')
  else:
    transition_graph.annotate(('%f' % my_data['Transition'][i]).rstrip('0').rstrip('.') ,
                     (bar.get_x() + bar.get_width() / 2,
                      bar.get_height()), ha='center', va='center',
                      #1.3), ha='center', va='center',
                     size=11, xytext=(0, -10), rotation=90,
                     textcoords='offset points')
  if i in average_col:
    bar.set_hatch('\\')
  i+=1

transition_graph.set_ylabel('Number of Transitions')
transition_graph.set_xlabel('')

plt.legend(loc = "upper center", bbox_to_anchor=(0.5,1.05),  ncol=4)
plt.savefig('.'+ input_file.split('.')[1] + '_transition.png')
