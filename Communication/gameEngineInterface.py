import multiprocessing


class doubleQueuePipe:
    def __init__(self):
        self.src, self.sink = multiprocessing.Queue(), multiprocessing.Queue()

    def toSink(self, msg):
        self.sink.put(msg)

    def fromSink(self):
        return self.sink.get()

    def toSrc(self, msg):
        self.src.put(msg)

    def fromSrc(self):
        return self.src.get()

    def clean(self):
        self.src.close()
        self.sink.close()
        self.src, self.sink = multiprocessing.Queue(), multiprocessing.Queue()

    def __del__(self):
        self.src.close()
        self.sink.close()


class GEI:
    # assuming the the Game engine is the creator of this object and it's named
    def __init__(self, pipe):
        self.pipe = pipe

    def toClient(self, msg):
        self.pipe.toSink(msg)

    def fromClient(self):
        return self.pipe.fromSrc()

    def toGE(self, msg):
        self.pipe.toSrc(msg)

    def fromGE(self):
        return self.pipe.fromSink()

    def clearMessages(self):
        self.pipe.clean()

    def __del__(self):
        del self.pipe


if __name__ == '__main__':
    gei = GEI(doubleQueuePipe())
