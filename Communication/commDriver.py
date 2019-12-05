import multiprocessing
from client import Client
from serverInterface import ServerInterface
from gameEngineInterface import *


class CommunicationDriver:
    def __init__(self, clientName, serverUri):
        self.client = Client(clientName)
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
    agentName = sys.argv[1]
    comm = CommunicationDriver(sys.argv[1], "127.0.0.1:5000")
    comm.start()
    print("Commmunication statrted")
    msg = comm.recv()

    while(1):
        print(msg)
        if(msg["type"] != "END" and msg["myturn"]):
            a, b = input(f"{agentName} pick a move: ").split()
            comm.send({"type": "MOVE", "move": {
                      "type": "place", "point": {"row:": int(a), "column": int(b)}}})
        msg = comm.recv()
