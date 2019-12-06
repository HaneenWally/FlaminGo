from serverInterface import ServerInterface as ServerConnection
import asyncio
import concurrent.futures
import websockets
import multiprocessing
from time import sleep
from helpers import *
import logging


class Init:
    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["NAME", "END"]  # TODO
        await server.send({"type": "NAME", "name": client.clientName})
        return Ready()


class Ready:
    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["START", "END"]  # TODO

        if(serverMsg["type"] == "END"):
            gameEngine.toGE(serverMsg)
            return Ready()

        mycolor = serverMsg["color"]
        moveLog = serverMsg["configuration"]["moveLog"]
        intialStateTurn = serverMsg["configuration"]["initialState"]["turn"]

        if((len(moveLog) % 2 == 0) == (mycolor == intialStateTurn)):
            # == means xnor both should be true or both false to move to Thinkgin
            serverMsg["myturn"] = True
            gameEngine.toGE(serverMsg)
            return Thinking()
        else:
            serverMsg["myturn"] = False
            gameEngine.toGE(serverMsg)
            return Idle()


class Thinking:
    def __init__(self, prevState=None):
        self.prevState = prevState

    async def handle(self, server, gameEngine, client):
        if(self.prevState and isinstance(self.prevState, WaitingMoveResponse)):
            gameEngine.toGE(
                {"type": "INTERNAL", "ReThink": True, "myturn": True})
            self.prevState = self
            return self

        gameEngineMsg = await blockingToAsync(gameEngine.fromGE)

        if(gameEngineMsg["type"] in ["MOVE"]):
            await server.send(gameEngineMsg)
            return WaitingMoveResponse()
        else:
            logging.warn(
                f'Unexpected msg form game engine {gameEngineMsg["type"]}.. Return to READY_STATE')
            return Ready()


class WaitingMoveResponse:
    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["VALID", "INVALID", "END"]  # TODO

        if(serverMsg["type"] == "END"):
            gameEngine.toGE(serverMsg)
            return Ready()

        if(serverMsg["type"] == "VALID"):
            return Idle()
        else:
            return Thinking(self)


class Idle:
    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["MOVE", "END"]  # TODO

        if(serverMsg["type"] == "END"):
            gameEngine.toGE(serverMsg)
            return Ready()

        if(serverMsg["type"] == "MOVE"):
            serverMsg["myturn"] = True  # to know it has to move
            gameEngine.toGE(serverMsg)
            return Thinking()


class Disconnected:
    async def handle(self, server, gameEngine, client):
        if(await server.connect()):
            return Init()
        return self


class Client:
    def __init__(self, clientName):
        logging.basicConfig(filename='client.log', filemode='w',
                            format='%(name)s - %(levelname)s - %(message)s', level=logging.INFO)
        self.clientName = clientName
        self.intialState = Disconnected()
        self.currState = self.intialState

    async def _returnToIntial(self, server, gameEngine):
        self.intialState = Disconnected()
        self.currState = self.intialState
        await server.disconnect()

    async def _communicate(self, server, gameEngine):
        while(1):
            if(isinstance(self.currState, Disconnected)):
                self.currState = await self.currState.handle(server, gameEngine, self)
            else:
                # this function recive only msg commands and discard heart beat
                try:
                    logging.debug(
                        f'Client curr state is {type(self.currState).__name__}')
                    self.currState = await self.currState.handle(server, gameEngine, self)
                except Exception as e:
                    logging.error(
                        f"EXEPCTION: {e} in client._commuincate cause returing to intialstate")
                    await self._returnToIntial(server, gameEngine)

    def start(self, server, gameEngine):
        # self._startHeartBeatProcess(server)
        asyncio.get_event_loop().run_until_complete(
            self._communicate(server, gameEngine))


if __name__ == "__main__":
    clientName = "Flamingo"
    server_connection = ServerConnection("addr")
    client = Client("Flamingo")
