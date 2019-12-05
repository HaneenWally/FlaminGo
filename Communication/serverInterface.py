import asyncio
import websockets
import json
import re


class ServerInterface:
    connection = None
    serverUri = ''

    def __init__(self, serverUri):
        ipport = re.compile("^\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}:\d+$")
        if(ipport.match(serverUri)):
            self.serverUri = "ws://" + serverUri
        else:
            raise "Wrong server uri it shoud follow that format ip:port ex: 192.168.1.1:5000"

    @property
    def _isconnected(self):
        return self.connection and self.connection.open

    async def connect(self):
        if(self._isconnected):  # if connection is still okay return true
            return True
        else:
            await self.disconnect()  # if old connection needs clean up
            try:
                self.connection = await websockets.connect(self.serverUri)
                return True if self._isconnected else False
            except Exception as e:
                print(f"Cannot connect to {self.serverUri}")
                return False

    async def send(self, msg):
        assert self._isconnected
        return await self.connection.send(json.dumps(msg))

    async def receive(self):
        assert self._isconnected
        return json.loads(await self.connection.recv())

    async def disconnect(self):
        # close connection
        if(self._isconnected):
            await self.connection.close()
        self.connection = None

    # def __del__(self):
    #     asyncio.get_event_loop().run_until_complete(self.disconnect())
