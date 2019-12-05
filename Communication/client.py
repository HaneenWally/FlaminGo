from serverInterface import ServerInterface as ServerConnection
import asyncio
import websockets
import multiprocessing
from time import sleep


class Init:
    def __init__(self):
        # these variables are meant for debuging and handling special cases like (not yet known :) )
        self.lasMsgToServer = self.lasMsgToGE = self.lasMsgFromServer = self.lasMsgFromGE = None

    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["NAME", "END"]  # TODO
        await server.send({"type": "NAME", "name": client.clientName})
        return Ready()


class Ready:
    def __init__(self):
        self.lasMsgToServer = self.lasMsgToGE = self.lasMsgFromServer = self.lasMsgFromGE = None

    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["START", "END"]  # TODO

        if(serverMsg["type"] == "END"):
            gameEngine.toGE(serverMsg)
            return Init()

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
    def __init__(self):
        self.lasMsgToServer = self.lasMsgToGE = self.lasMsgFromServer = self.lasMsgFromGE = None

    async def handle(self, server, gameEngine, client):
        if(isinstance(client.prevState, WaitingMoveResponse)):
            gameEngine.toGE({"type": "INTERNAL", "ReThink": True})
        else:
            gameEngineMsg = gameEngine.fromGE()
            if(gameEngineMsg["type"] in ["MOVE"]):
                await server.send(gameEngineMsg)
                return WaitingMoveResponse()
            else:
                print(
                    f'Unexpected msg form game engine {gameEngineMsg["type"]}.. Return to READY_STATE')
                return Ready()


class WaitingMoveResponse:
    def __init__(self):
        self.lasMsgToServer = self.lasMsgToGE = self.lasMsgFromServer = self.lasMsgFromGE = None

    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["VALID", "INVALID", "END"]  # TODO

        if(serverMsg["type"] == "END"):
            gameEngine.toGE(serverMsg)
            return Init()

        if(serverMsg["type"] == "VALID"):
            return Idle()
        else:
            return Thinking()


class Idle:
    def __init__(self):
        self.lasMsgToServer = self.lasMsgToGE = self.lasMsgFromServer = self.lasMsgFromGE = None

    async def handle(self, server, gameEngine, client):
        serverMsg = await server.receive()
        assert serverMsg["type"] in ["MOVE", "END"]  # TODO

        if(serverMsg["type"] == "END"):
            gameEngine.toGE(serverMsg)
            return Init()

        if(serverMsg["type" == "MOVE"]):
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
        self.clientName = clientName
        self.intialState = Disconnected()
        self.prevState = self.currState = self.intialState

    def _returnToIntial(self, server, gameEngine):
        self.intialState = Disconnected()
        self.prevState = self.currState = self.intialState
        server.disconnect()
        gameEngine.clearMessages()

    async def _communicate(self, server, gameEngine):
        while(1):
            if(isinstance(self.currState, Disconnected)):
                self.currState = await self.currState.handle(server, gameEngine, self)
            else:
                # this function recive only msg commands and discard heart beat
                try:
                    print(self.currState)
                    self.prevState = self.currState
                    self.currState = await self.currState.handle(server, gameEngine, self)
                except Exception as e:
                    print("EXEPCTION in client._commuincate ", e)
                    self._returnToIntial(server, gameEngine)

    def start(self, server, gameEngine):
        # self._startHeartBeatProcess(server)
        asyncio.get_event_loop().run_until_complete(
            self._communicate(server, gameEngine))

    def __del__(self):
        try:
            self.heartBeatProcess.join()
        except:
            pass

    # async def _heartBeat(self, server):
    #     while(1):
    #         if(await server.connect()):
    #             await server.receive()
    #             await server.send({})
    #         else:
    #             sleep(.5)

    # def _startHeartBeatProcess(self, server):
    #     self.heartBeatProcess = multiprocessing.Process(target=lambda server: asyncio.get_event_loop(
    #     ).run_until_complete(self._heartBeat(server)), args=(server,))
    #     self.heartBeatProcess.start()


if __name__ == "__main__":
    clientName = "Flamingo"
    server_connection = ServerConnection("addr")
    client = Client("Flamingo")
