from netmiko import ConnectHandler


status = input("Enter your ticket title: ")

result1 = status.find(': ')
result2 = status.find('.csl.l')
deviceCode = (status[result1+2 : result2])
print(deviceCode)

result3 = status.find('GigabitEthernet')
result4 = status.find(' is')
devicePort = (status[result3+15 : result4])
print(devicePort)




ciscoDevice = {
    'device_type': 'cisco_ios',
    'host':   deviceCode,
    'username': '<add you username>',
    'password': '<add your password>',     
}

net_connect = ConnectHandler(**ciscoDevice)
output1 = net_connect.send_command('show version')

output2 = 'A'


if devicePort :
    output2 = net_connect.send_command('show int gig '+devicePort)
    

    
print(output1)
#print('XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX')
print(output2)



