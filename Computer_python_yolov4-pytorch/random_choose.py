import random
#设置随机数种子
random.seed()
#生成一个随机数
# print(random.random())
# #生成一个1-5之间的随机数
# print(random.uniform(1,5))
# #生成一个1-10之间的随机数
# print(random.randint(1,10))
# #生成一个1-10之间以3递增的随机数
# print(random.randrange(1,10,3))
list = ['1:Red_Block','2:Rubik_Cube','3:Bule_Block','4:Snowflake_Beer','5:Red_Bull','6:Tennis','7:AD_Calcium_Milk','8:Deluxe_Milk']
print(random.choice(list)+ '   '+random.choice(list))

random.shuffle(list)
