import matplotlib.pyplot as plt
import pandas as pd 
import seaborn as sns
input_file = ('./gpu_throughput.csv')
my_data = pd.read_csv(input_file)
my_data['Throughput'] = round ( my_data['Throughput'], 2)

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
fig , ax= plt.subplots(figsize = (17,3))

throughput_bar = sns.barplot(x="Name", y ="Throughput", hue ="Mode", data = my_data,  palette=color)

i=0

for  bar in throughput_bar.patches:
  bar.set_width(bar.get_width() * 1)
  if  my_data['Throughput'][i] < (max(my_data['Throughput']) * 0.1):
    throughput_bar.annotate(('%f' % my_data['Throughput'][i]).rstrip('0').rstrip('.') ,
                     (bar.get_x() + bar.get_width() / 2,
                      bar.get_height()*1.4 ), ha='center', va='center',
                      #1.3), ha='center', va='center',
                     size=11, xytext=(0, 12), rotation=90,
                     textcoords='offset points')
  else:
    throughput_bar.annotate(('%f' % my_data['Throughput'][i]).rstrip('0').rstrip('.') ,
                     (bar.get_x() + bar.get_width() / 2,
                      bar.get_height()), ha='center', va='center',
                      #1.3), ha='center', va='center',
                     size=11, xytext=(0, -12), rotation=90,
                     textcoords='offset points')
  if i in average_col:
    bar.set_hatch('\\')
  i+=1

throughput_bar.set_ylabel('Throughput in GB/sec')
throughput_bar.set_xlabel('')

plt.legend(loc = "upper center", bbox_to_anchor=(0.5,1.1),  ncol=4)
plt.savefig('.'+ input_file.split('.')[1] + '_top.png')
sns.axes_style()
