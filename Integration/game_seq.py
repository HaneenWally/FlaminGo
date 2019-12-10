import ctypes
import numpy as np
import glob


class ImplemenationWrapper():
    # find the shared library, the path depends on the platform and Python version
    libfile = glob.glob('build/*/implementation*.so')[0]

    # 1. open the shared library
    impLib = ctypes.CDLL(libfile)
    ACK = 25
    boardSize = 19
    def __init__(self):
        # define the function type in and return
        self.impLib.fill_init.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32)]
        
        self.impLib.make_move.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32), 
                                    ctypes.c_int]
        
        self.impLib.get_best_move.argtypes = [ctypes.c_int, ctypes.c_int]
        
        self.impLib.set_color.argtypes = [ctypes.c_int]
        
        self.impLib.reach_initial.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32), 
                                    ctypes.c_int]
        


        self.impLib.mysum.restype = ctypes.c_int
        self.impLib.mysum.argtypes = [ctypes.c_int, 
                                np.ctypeslib.ndpointer(dtype=np.int32)]
        
    def fill_init_board(self, board, color): 
        # color is 1 when we are black, else 0
        # board is array of array, me is 1, other is -1, empty 0
        x = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        x1, y1 = np.where(board==1)
        x2, y2 = np.where(board==-1)
        
        x[:len(x1)] = x1 + 1
        x[len(x1):len(x2)] = -x2 - 1
        x[len(x1)+len(x2)] = self.ACK
        y[:len(y1)] = y1 + 1
        y[len(y1):len(y2)] = -y2 - 1
        y[len(y1)+len(y2)] = self.ACK
        
        self.impLib.set_color(1)
        self.impLib.fill_init(x,y)
        
    def fill_init_log(self, logs, myColor, is_me_first_play_in_log): 
        # the log in shape of array of x,y,color if the play is pass the x,y == -1, -1
        x_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        # i = 0
        # for i, (x,y,color) in enumerate(logs):
        #     if x == -1 and y == -1: x_list[i], y_list[i] = 0, 0
        #     elif color == myColor: x_list[i], y_list[i] = x + 1, y + 1
        #     else: x_list[i], y_list[i] = - (x + 1), -(y + 1)
        for i, (x,y,_) in enumerate(logs): x_list[i], y_list[i] = x,y
        
        self.impLib.reach_initial(x_list, y_list, int(bool(is_me_first_play_in_log)))

        board = np.zeros((19,19))

        for x, y in zip(x_list, y_list): 
            if x == self.ACK or y == self.ACK: break
            elif x > 0 and y > 0: board[x][y] = 1
            elif x < 0 and y < 0: board[-x][-y] = -1
            else: assert False

        return board  

    def play_tret(self, x, y, remaningTime):
        # take the x, y from the against player and return captured
        x_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        x_list[0] = x; y_list[0] = y;
        x_list[1] = self.ACK; y_list[1] = self.ACK

        self.impLib.make_move(x_list, y_list, int(remaningTime))

        captured = []
        for x,y in zip(x_list, y_list):
            if x == self.ACK or y == self.ACK: break
            captured.insert([x,y])
        
        return captured

    def get_play(self):
        x, y = 25, 25
        self.impLib.get_best_move(ctypes.byref(x), ctypes.byref(y))
        assert x != 25 and y != 25
        return x, y      


if __name__ == "__main__":
    wrap = ImplemenationWrapper()
    board = np.array([[1, 0, -1]*6 + [0]] *19)
    
    wrap.fill_init_board(board, 1)
    wrap.fill_init_log([[1,2,'b'],[3,3,'w']], 'b', True)

    for _ in range(10):
        wrap.play_tret(np.random.randint(0,19), np.random.randint(0,19), 300)
        x, y = wrap.get_play()
        print(x, y)