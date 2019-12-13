import ctypes
import numpy as np
import glob
from threading import Thread

class ImplemenationWrapper():
    # find the shared library, the path depends on the platform and Python version
    libfile = glob.glob('build/*/implementation*.so')[0]

    # 1. open the shared library
    impLib = ctypes.CDLL(libfile)
    ACK = 25
    boardSize = 19
    def __init__(self):
        # define the function type in and return
        self.impLib.fill_initial.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32), 
                                    ctypes.c_int, ctypes.c_int]
        
        self.impLib.opponent_move.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32), 
                                    ctypes.c_int]
        self.impLib.opponent_move.restype = ctypes.c_int 
        
        self.impLib.AI_score.argtypes = [ctypes.c_int]
        self.impLib.AI_score.restype = ctypes.c_int 

        self.impLib.make_move.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32), 
                                    ctypes.c_int]
        # self.impLib.make_move.argtypes = [ctypes.c_int]
        
        # self.impLib.get_best_move.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32),
        #                                     np.ctypeslib.ndpointer(dtype=np.int32)]
        
        self.impLib.set_color.argtypes = [ctypes.c_int]
        
        self.impLib.is_done.restype = ctypes.c_int
        
        self.impLib.reach_initial.argtypes = [np.ctypeslib.ndpointer(dtype=np.int32), 
                                    np.ctypeslib.ndpointer(dtype=np.int32), 
                                    ctypes.c_int]
        


        # self.impLib.mysum.restype = ctypes.c_int
        # self.impLib.mysum.argtypes = [ctypes.c_int, 
        #                         np.ctypeslib.ndpointer(dtype=np.int32)]
        self.x = self.ACK
        self.y = self.ACK
        self.captured = []
        
    def fill_init_board(self, board, color, number_of_w_captures, number_of_b_captures): 
        # color is 1 when we are black, else 0
        # board is array of array, me is 1, other is -1, empty 0
        x = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        x1, y1 = np.where(board==1)
        x2, y2 = np.where(board==-1)
        x[:len(x1)] = x1 + 1
        x[len(x1):len(x1) + len(x2)] = -x2 - 1
        x[len(x1)+len(x2)] = self.ACK
        y[:len(y1)] = y1 + 1
        y[len(y1):len(y1) + len(y2)] = -y2 - 1
        y[len(y1)+len(y2)] = self.ACK
        
        self.impLib.set_color(color)
        self.impLib.fill_initial(x,y, number_of_w_captures, number_of_b_captures)
        
    def fill_init_log(self, logs): 
        # the log in shape of array of x,y,color if the play is pass the x,y == -1, -1
        x_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        # i = 0
        # for i, (x,y,color) in enumerate(logs):
        #     if x == -1 and y == -1: x_list[i], y_list[i] = 0, 0
        #     elif color == myColor: x_list[i], y_list[i] = x + 1, y + 1
        #     else: x_list[i], y_list[i] = - (x + 1), -(y + 1)
        for i, (x,y,_) in enumerate(logs): x_list[i], y_list[i] = x,y
        x_list[i+1] = self.ACK; y_list[i+1] = self.ACK
        # color of the first player of the log, if black = 1
        self.impLib.reach_initial(x_list, y_list, int(bool(logs[0][2] == 'b')))

        board = np.zeros((19,19))
        for x, y in zip(x_list, y_list): 
            if x == self.ACK or y == self.ACK: break
            elif x > 0 and y > 0: board[x-1][y-1] = 1
            elif x < 0 and y < 0: board[-x-1][-y-1] = -1
            else: assert False

        return board  

    def opponent_move(self, x, y, remaningTime):
        # take the x, y from the against player and return captured
        # if passed it should be called with -1,-1
        x_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        x_list[0] = x; y_list[0] = y;
        x_list[1] = self.ACK; y_list[1] = self.ACK

        is_valid = self.impLib.opponent_move(x_list, y_list, int(remaningTime))
        
        if is_valid:
            return is_valid, self.__get_captured(x_list, y_list)
        else:
            return is_valid, []

    def my_move(self, remaningTime):
        self.x = self.ACK
        self.y = self.ACK
        self.captured = []
        x_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        y_list = np.zeros((self.boardSize+1)*(self.boardSize+1), dtype=np.int32)
        self.impLib.make_move(x_list, y_list, int(remaningTime))
        self.x, self.y = x_list[0], y_list[0]
        self.captured = self.__get_captured(x_list[1:], y_list[1:])
        # -1 and -1 is pass move
        return self.x, self.y, self.captured


    def __get_captured(self, x_list, y_list):
        captured = []
        for x,y in zip(x_list, y_list):
            if x == self.ACK or y == self.ACK: break
            captured.append([x,y])
        
        return captured

    def get_score(self):
        blackScore = self.impLib.AI_score(1)
        whiteScore = self.impLib.AI_score(0)
        return blackScore, whiteScore

    def game_end(self):
        return self.impLib.is_done()

if __name__ == "__main__":
    print('python say: hello')
    wrap = ImplemenationWrapper()
    board = np.array([[1, 0, 0]*5 + [1] + [0]] *19)
    
    print('python say: fill_init_board')
    wrap.fill_init_board(board, 1, 3, 4)

    # print(wrap.game_end())
    print('python say: fill_init_log')
    board = wrap.fill_init_log([[1,2,'b'],[3,5,'w']])

    for _ in range(10):
        print('python say: opponent_move')
        x,y = np.random.randint(0,19), np.random.randint(0,19)
        captured = wrap.opponent_move(x,y, 300)
        print('python say: opponent play is, ', x, y)
        print('python say: get_play')
        t = Thread(target= wrap.my_move, args=(10,))
        t.start()

        t.join()
        assert wrap.x != wrap.ACK
        print('python say: play is, ', wrap.x, wrap.y, wrap.captured)
        print('python say: score = ', wrap.get_score())
        print('***********************************')

    
    print('**********************************************************************')
    print('python say: fill_init_board')
    wrap.fill_init_board(board, 1, 3, 3)
    print('python say: fill_init_log')
    wrap.fill_init_log([[1,2,'b'],[3,3,'w']], 'b', True)
    print('**********************************************************************')
    print('python say: init_my_move')
    for _ in range(10):
        print('python say: get_play')
        t = Thread(target= wrap.my_move, args=(10,))
        t.start()

        t.join()
        assert wrap.x != wrap.ACK
        print('python say: play is, ', wrap.x, wrap.y, wrap.captured)

        # print('python say: play is, ', x, y)
        print('python say: opponent_move')
        x,y = np.random.randint(0,19), np.random.randint(0,19)
        captured = wrap.opponent_move(x,y, 300)
        print('python say: opponent play is, ', x, y)
        print('***********************************')
