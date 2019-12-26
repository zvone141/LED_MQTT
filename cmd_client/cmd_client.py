import paho.mqtt.client as mqtt
import time
#import cmd_config as config  //uncommment if you want to use default config and comment out the line below
import cmd_custom_config as config
import random
import string


def on_connect(client, userdata, flags, rc):
    print("\r\nConnected with result code "+str(rc))
    
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(config.mqttConf['cmd_topic'])

# The callback for when a PUBLISH message is received from the server.

def on_message(client, userdata, msg):
    print("\r\n")
    print("Command sent: ")
    print(msg.payload)

client = mqtt.Client('cmd-'+(''.join([random.choice(string.ascii_letters + string.digits) for n in range(6)])))
#client.username_pw_set("firefly", password="fireflytag8192")
client.username_pw_set(config.mqttConf['user'], password=config.mqttConf['password'])
client.on_connect = on_connect
client.on_message = on_message

client.connect(config.mqttConf['server'], config.mqttConf['port'], 60)



# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.



try:
    
    client.loop_start()
    
    #client.publish(config.mqttConf['cmd_topic'], "10,0,3,100,100,100")
    #client.publish(config.mqttConf['cmd_topic'], "11,0,17,5,100,100,100")
    #client.publish(config.mqttConf['cmd_topic'], "0")

    #client.publish("led/cmd/", "0")
    
    while True:
        print("\r\n")
        print("0 = OFF, 1 = ON, 2 = NIGHTMODE, 4 = command")
        a = str(input("Input command:"))
   
        if a is "0":
           client.publish(config.mqttConf['cmd_topic'], "0")
           client.publish("led/cmd/", "0")
        elif a is "1":
            client.publish(config.mqttConf['cmd_topic'], "1")
            client.publish("led/cmd/", "1")
        elif a is "2":
            client.publish(config.mqttConf['cmd_topic'], "2")
        elif a is "4":
            c = input("command: ")
            client.publish(config.mqttConf['cmd_topic'], "%s"%(c))
    
except KeyboardInterrupt:

    print("Exiting.")