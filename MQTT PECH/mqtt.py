import json
import random
import time

from paho.mqtt import client as mqtt_client


BROKER = 'saposteamiot.tk'
PORT = 8093
TOPIC = "Entrada/01"

SSID = "ssid"
PASSWORD = "psswrd"


# generate client ID with pub prefix randomly
CLIENT_ID = "python-mqtt-ws-pub-{id}".format(id=random.randint(0, 1000))
USERNAME = 'emqx'
PASSWORD = 'public'
FLAG_CONNECTED = 0


def on_connect(client, userdata, flags, rc):
    global FLAG_CONNECTED
    if rc == 0:
        FLAG_CONNECTED = 1
        print("Connected to MQTT Broker!")
    else:
        print("Failed to connect, return code {rc}".format(rc=rc), )
        
def do_connect(SSID, PASSWORD):
    import network                            # importa el módulo network
    global sta_if
    sta_if = network.WLAN(network.STA_IF)     # instancia el objeto -sta_if- para realizar la conexión en modo STA 
    if not sta_if.isconnected():              # si no existe conexión...
        sta_if.active(True)                   # activa el interfaz STA del ESP32
        sta_if.connect(SSID, PASSWORD)        # inicia la conexión con el AP
        print('Dame un momento, wey. Me estoy conectando a ', SSID +"...")
        while not sta_if.isconnected():           # ...si no se ha establecido la conexión...
            pass                                  # ...repite el bucle...
    print('Configuración de red (IP/netmask/gw/DNS):', sta_if.ifconfig())
do_connect(SSID,PASSWORD)

def connect_mqtt():
    client = mqtt_client.Client(CLIENT_ID, transport='websockets')
    client.username_pw_set(USERNAME, PASSWORD)
    client.on_connect = on_connect
    client.connect(BROKER, PORT)
    return client


def publish(client):
    msg_count = 0
    while True:
        msg_dict = {
            'msg': msg_count
        }
        msg = json.dumps(msg_dict)
        result = client.publish(TOPIC, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print("Send `{msg}` to topic `{topic}`".format(msg=msg, topic=TOPIC))
        else:
            print("Failed to send message to topic {topic}".format(topic=TOPIC))
        msg_count += 1
        time.sleep(1)


def run():
    client = connect_mqtt()
    client.loop_start()
    time.sleep(1)
    if FLAG_CONNECTED:
        publish(client)
    else:
        client.loop_stop()


if __name__ == '__main__':
    run()
