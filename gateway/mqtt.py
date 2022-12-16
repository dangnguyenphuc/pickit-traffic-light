###################################### MacOS, Chrome 98. ######################################
print("Hello ThingsBoard")
import paho.mqtt.client as mqttclient
import json
import time
import serial.tools.list_ports
import re
from model import *

def expand_cmd(cmd):
    if cmd<10:
        return "00" + str(cmd)
    elif cmd<100:
        return "0" + str(cmd)
    else:
        return str(cmd)

def find_port():
    for port in list(serial.tools.list_ports.comports()):
        txt = str(port)
        if "USB-SERIAL" in txt:
            return str(port).split("-")[0].strip()

BROKER_ADDRESS = "demo.thingsboard.io"
PORT = 1883
THINGS_BOARD_ACCESS_TOKEN = "4WvzYkq8gxqVnt9lOlGi"

mess = ""
global status
bbc_port = find_port()

if len(bbc_port) > 0:
    ser = serial.Serial(port=bbc_port, baudrate=9600)

def subscribed(client, userdata, mid, granted_qos):
    print("Subscribed...")


def recv_message(client, userdata, message):
    global status
    print("Received: ", message.payload.decode("utf-8"))
    # temp_data = {'status': status, 'timeOfLight1': 20, 'timeOfLight2': 20}
    cmd = ""
    #TODO: Update the cmd to control the device
    try:
        jsonobj = json.loads(message.payload)
        if jsonobj['method'] == "switchMAN":
            cmd = "!!BT:000##"
        if jsonobj['method'] == "switchTUN":
            cmd = "!!BT:100##"
        if jsonobj['method'] == "setApply":
            cmd = "!!BT:200##"
        if jsonobj['method'] == "backButton":
            cmd = "!!BT:300##"
        if jsonobj['method'] == "incButton":
            cmd = "!!BT:400##"
        if jsonobj['method'] == "decButton":
            cmd = "!!BT:500##"
        if jsonobj['method'] == "resetButton":
            cmd = "!!RS:000##"

        # Adjust
        if jsonobj['method'] == "setGreen1":
            x = expand_cmd(int(jsonobj["params"]))
            cmd = "!!G1:"+ x +"##"
        if jsonobj['method'] == "setYellow1":
            x = expand_cmd(int(jsonobj["params"]))
            cmd = "!!Y1:"+ x +"##"
        if jsonobj['method'] == "setGreen2":
            x = expand_cmd(int(jsonobj["params"]))
            cmd = "!!G2:"+ x +"##"
        if jsonobj['method'] == "setYellow2":
            x = expand_cmd(int(jsonobj["params"]))
            cmd = "!!Y2:"+ x +"##"
    except:
        pass

    # print(cmd)

    if len(bbc_port) > 0:
        ser.write((str(cmd)).encode())


def connected(client, usedata, flags, rc):
    if rc == 0:
        print("Thingsboard connected successfully!!")
        client.subscribe("v1/devices/me/rpc/request/+")
    else:
        print("Connection is failed")

def processData(data):
    '''
    !LIGHT1:TIMER_VALUE:RED1:YELLOW1:GREEN1#
    !LIGHT2:TIMER_VALUE:RED2:YELLOW2:GREEN2#
    '''
    data = data.replace("!", "")
    data = data.replace("#", "")
    splitData = data.split(":")
    print(splitData)
    #TODO: Add your source code to publish data to the server
    if splitData[0] == 'LIGHT1':
        time1 = int(splitData[1]) if splitData[1] != "DELAY" else "DELAY"
        red1 = int(splitData[2])
        yellow1 = int(splitData[3])
        green1 = int(splitData[4])
        collect_data = {'timeOfLight1': time1, "red1": red1, "yellow1": yellow1, "green1": green1}
        client.publish('v1/devices/me/attributes', json.dumps(collect_data), 1)

    if splitData[0] == 'LIGHT2':
        time2 = int(splitData[1]) if splitData[1] != "DELAY" else "DELAY"
        red2 = int(splitData[2])
        yellow2 = int(splitData[3])
        green2 = int(splitData[4])
        collect_data = {'timeOfLight2': time2, "red2": red2, "yellow2": yellow2, "green2": green2}
        client.publish('v1/devices/me/attributes', json.dumps(collect_data), 1)

    if splitData[0] == 'SET1':
        red = int(splitData[1])
        yellow = int(splitData[2])
        green = int(splitData[3])
        collect_data = {"redTime2": red, "yellowTime1": yellow, "greenTime1": green}
        client.publish('v1/devices/me/attributes', json.dumps(collect_data), 1)

    if splitData[0] == 'SET2':
        red = int(splitData[1])
        yellow = int(splitData[2])
        green = int(splitData[3])
        collect_data = {"redTime1": red, "yellowTime2": yellow, "greenTime2": green}
        client.publish('v1/devices/me/attributes', json.dumps(collect_data), 1)

    if splitData[0] == 'STATUS':
        status = int(splitData[1])
        if status in [26, 27, 28, 29]:
            status_name = "TUNING"
        elif status in [15, 16, 115, 116]:
            status_name = "MANNUAL"
        else:
            status_name = "AUTO"
        collect_data = {"status": status_name}
        client.publish('v1/devices/me/attributes', json.dumps(collect_data), 1)

def readSerial():
    bytesToRead = ser.inWaiting()
    if (bytesToRead > 0):
        global mess
        mess = mess + ser.read(bytesToRead).decode("UTF-8")
        while ("#" in mess) and ("!" in mess):
            start = mess.find("!")
            end = mess.find("#")
            processData(mess[start:end + 1])
            if (end == len(mess)):
                mess = ""
            else:
                mess = mess[end+1:]



client = mqttclient.Client("Gateway_Thingsboard")
client.username_pw_set(THINGS_BOARD_ACCESS_TOKEN)

client.on_connect = connected
client.connect(BROKER_ADDRESS, 1883)
client.loop_start()

client.on_subscribe = subscribed
client.on_message = recv_message

counter = 0

while True:

    if len(bbc_port) >  0:
        readSerial()
    
    
    # if counter >= 8:
    #     counter = 0
    #     ai_result = detect_congesion()
    #     print("AI Output: ", ai_result)

    #     if ai_result.split(" ")[0] == "1":
    #         print("Set GREEN1 time to 100s")
    #         ser.write("!!BT:100##".encode())
    #         time.sleep(0.5)
    #         ser.write("!!G1:100##".encode())
    #         time.sleep(0.5)
    #         ser.write("!!BT:200##".encode())

    #     collect_data = {"predictCongestion": ai_result}
    #     client.publish('v1/devices/me/attributes', json.dumps(collect_data), 1)

    counter += 1
    time.sleep(1)