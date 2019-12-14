import multiprocessing
from client import Client
from serverInterface import ServerInterface
from gameEngineInterface import *


class CommunicationDriver:
    def __init__(self, clientName, serverUri, protocolVersion="v2", pingInterval=.5):
        self.client = Client(clientName, protocolVersion, pingInterval)
        self.serverInterface = ServerInterface(serverUri)
        self.gameEngineInterface = GEI(doubleQueuePipe())

    def start(self):
        self.clientProcess = multiprocessing.Process(
            target=self.client.start, args=(self.serverInterface, self.gameEngineInterface))
        self.clientProcess.start()

    def send(self, msg):  # send to clinet first al client will handel the send to server
        self.gameEngineInterface.toClient(msg)

    def recv(self):  # blocking wait on msg from the clinet which is came from server
        return self.gameEngineInterface.fromClient()

    def __del__(self):
        del self.gameEngineInterface
        del self.serverInterface
        del self.client
        try:
            self.clientProcess.join()
        except:
            pass


if __name__ == '__main__':
    import sys
    from random import randrange
    from time import sleep
    agentName = sys.argv[1]
    comm = CommunicationDriver(sys.argv[1], "127.0.0.1:8080")
    comm.start()
    print("Commmunication statrted")
    msg = comm.recv()

    moves = ((i, j) for i in range(19) for j in range(19))
    while(1):
        while(msg["type"] != "END" and msg["myturn"]):
            try:
                move = next(moves)
            except:
                moves = ((i, j) for i in range(19)
                         for j in range(19))
                move = next(moves)
            comm.send({"type": "MOVE", "move": {"type": "place",
                                                "point": {"row": move[0], "column": move[1]}}})
            msg = comm.recv()
            # print(msg)
        else:
            msg = comm.recv()
            if(msg["type"] == "END"):
                msg = comm.recv()
        sleep(.1)
