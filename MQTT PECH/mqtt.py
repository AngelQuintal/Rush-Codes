import asyncio
import websockets

async def hello():
    async with websockets.connect("wss://saposteamiot.tk:8094/mqtt/4000/Deus/web_client/1/60/true") as websocket:
        await websocket.send("14")
        await websocket.recv()

asyncio.run(hello())