import paho.mqtt.client as mqtt
import time

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("led/rgb")

# The callback for when a PUBLISH message is received from the server.

def on_message(client, userdata, msg):

    print(msg)

client = mqtt.Client()
#client.username_pw_set("firefly", password="fireflytag8192")
client.username_pw_set("test", password="test")
client.on_connect = on_connect
client.on_message = on_message

client.connect("nektar.l-tek.io", 8192, 60)



# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.



try:
    
    client.loop_start()
    
    #client.publish("led/rgb", "10,0,3,100,100,100")
    #client.publish("led/rgb", "11,0,17,5,100,100,100")
    #client.publish("led/rgb", "0")

    
    while True:
        a = input("0 = OFF, 1 = ON, 2 = NIGHTMODE, 4 = command")
        if a is "0":
           client.publish("led/rgb", "0")
        elif a is "1":
            client.publish("led/rgb", "1")
        elif a is "2":
            client.publish("led/rgb", "2")
        elif a is "4":
            c = input("command: ")
            client.publish("led/rgb", "%s"%(c))
        elif a is "5":
            for i in range(0,100):
                client.publish("led/rgb", "1")
                time.sleep(0.03)
                client.publish("led/rgb", "0")
                time.sleep(0.03)
    
except KeyboardInterrupt:

    print("KONEC")