import socket
import json
import enum

class ScoketmsgTypes(enum.Enum):
    AI_VS_AI=0
    AckAI_VS_AI=1
    moveConfigrations=2
    gameEnd=3
    gamePaused=4
    exit=5
    ack=6
    gameStart=7
    #AI vs human
    AI_VSHuman=8
    move=9
    forfeit =10
    remove = 11

def main():
    guiSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    guiSocket.bind(('127.0.0.1', 4500))
    _ ,port = guiSocket.getsockname()
    guiSocket.listen(2)
    # wait for gui to connect to socket
    guiConnection, iPAddress = guiSocket.accept()
    #recieve string containing game type
    paused = False
    msg= ""
    while (True):
        if(not paused):
            msg =recMsg(guiConnection , False)
        if(msg["type"] == ScoketmsgTypes.AI_VS_AI.value ):
            paused = aiVsAi(msg , guiConnection , paused)
        elif(msg["type"] == ScoketmsgTypes.AI_VSHuman.value ):
            aiVsHuman(msg , guiConnection)

def aiVsAi(msg , guiSocket , paused):
    ip = msg["msg"]["IP"]
    port = msg["msg"]["port"]
    # send to communication team ip and port to connect to server
    # get initialboard and history and my color
    myColor = 'b'
    initialBoardHistory = [{"x":2 , "y":2 , "color":'b'}] #after some processing i will get array of  dictionaries
    boolInitialBoard = True if (initialBoardHistory) else False
    ourRemainingTime=2
    theirRemainingTime=0
    moveCount = len(initialBoardHistory)
    # i will do send count but if they will send us history 1 by one i will change it
    if(paused):
        ackMsg = {'initialBoard':boolInitialBoard,"theirRemainingTime":theirRemainingTime , "ourRemainingTime":ourRemainingTime , "initialCount":moveCount}
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.gameStart.value)
        msg = recMsg(guiSocket, True)
    else:
        ackMsg = {'initialBoard':boolInitialBoard,"theirRemainingTime":theirRemainingTime , "ourRemainingTime":ourRemainingTime , "initialCount":moveCount}
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.AckAI_VS_AI.value)

    for i in range(moveCount):
        move = initialBoardHistory[i]
        sendMsg(guiSocket, move, ScoketmsgTypes.moveConfigrations.value)
        msg = recMsg(guiSocket , True)
    myTurn = True if (myColor == 'b' and len(initialBoardHistory)%2 == 0) or (myColor == 'w' and len(initialBoardHistory) % 2 != 0) else False

    continuePlaying = True
    gamePaused = False
    # i think we should add pause game in the condation
    while(continuePlaying):
        continuePlaying , gamePaused = playOnlineGame(myColor , myTurn , guiSocket)
        myTurn = not myTurn
    return gamePaused


def playOnlineGame(myColor , myTurn , guiSocket):
    if(myTurn):
        # get move from implementation
        gameEnd = True
        iWon = True
        ourScore =10
        theirScore= 10
        captured = [[1,3]] # array or arrays containing x, y to be removed
        countCaptured = len(captured)
        # convert it to x , y   , color
        x , y  = 0 , 0 # get them from implementation
        # send move to communication
        #get captured from implementation and send them to gui too
        if(not gameEnd):
            move = {'color':myColor , 'x':x , 'y':y , 'countCaptured':countCaptured }
            # the same ScoketmsgTypes.moveConfigrations have different elements in the dict, kaseb
            # the pause didnt get checked here
            sendMsg(guiSocket, move, ScoketmsgTypes.moveConfigrations.value)
            msg = recMsg(guiSocket , True)
            for i in range(countCaptured):
                    remove = { 'x':captured[i][0] , 'y':captured[i][1] }
                    sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                    msg = recMsg(guiSocket , True)
            return not gameEnd , False
        else:
            end =  {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            msg = recMsg(guiSocket , True)
            return not gameEnd , False

    else:
        # get move from communication or get that game is paused duo to other player problem
        gamePaused = False
        if(gamePaused):
            return False , True
        color,x,y = 'b' , 1 , 2
        # give move to implementation
        # get captured if there are ones or get game info if game ended
        # send to gui
        gameEnd = True
        iWon = True
        ourScore =10
        theirScore= 10

        if(gameEnd):
            end = {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            _ = recMsg(guiSocket, True)
            return not gameEnd ,False
        else:
            # get captured stones locations if there are ones
            captured = [[1,3]] # array or arrays containing x, y to be removed
            countCaptured = len(captured)
            hisColor = 'w' if myColor == 'b' else 'b'
            move = {'color':hisColor , 'x':x , 'y':y , 'countCaptured':countCaptured }
            sendMsg(guiSocket, move, ScoketmsgTypes.moveConfigrations.value)
            msg = recMsg(guiSocket ,True)
            for i in range(countCaptured):
                remove = { 'x':captured[i][0] , 'y':captured[i][1] }
                sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                msg = recMsg(guiSocket , True)
            return not gameEnd ,False


def aiVsHuman(msg , guiSocket):
    ackMsg = {'ack':'ack'}
    sendMsg(guiSocket, ackMsg, ScoketmsgTypes.ack.value)
    myColor= msg["msg"]["myColor"]
    # maybe process on mycolor if they want it 0 or 1
    # send color to implementation team
    initialCount = msg["msg"]["initialCount"]
    blackCount , whiteCount = 0 , 0
    for i in range(initialCount):
        msg =recMsg(guiSocket , False)
        if(msg["type"] == ScoketmsgTypes.move.value):
            color,x,y = msg["msg"]["color"], msg["msg"]["x"],msg["msg"]["y"]
            if(color == 'b'):
                blackCount+=1
            elif(color == 'w'):
                whiteCount+=1
            # same comment as above
            # send move to implementation team
        else:
            print("wrong values from gui")
        sendMsg(guiSocket, ackMsg, ScoketmsgTypes.ack.value)
    whoPlayFirst= 'b'
    if(blackCount>whiteCount):
        whoPlayFirst='w'
    myTurn = True
    if(myColor != whoPlayFirst ):
        myTurn = False

    continuePlaying = True
    while(continuePlaying):
        continuePlaying , myTurn = playGame(myColor , myTurn , guiSocket)


def playGame(myColor , myTurn,guiSocket ):
    if(myTurn):
        # get move from implementation
        # get move or game is finished and i get who won , my score , his score
        gameEnd = True
        iWon = True
        ourScore =10
        theirScore= 10
        captured = [[1,3]] # array or arrays containing x, y to be removed
        countCaptured = len(captured)
        # convert it to x , y   , color
        x , y  = 0 , 0 # get them from implementation
        if(not gameEnd):
            move = {'color':myColor , 'x':x , 'y':y , 'countCaptured':countCaptured }
            sendMsg(guiSocket, move, ScoketmsgTypes.move.value)
            msg = recMsg(guiSocket , False)
            for i in range(countCaptured):
                    remove = { 'x':x , 'y':y }
                    sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                    msg = recMsg(guiSocket ,False)
            return not gameEnd , not myTurn
        else:
            end =  {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
            sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
            msg = recMsg(guiSocket , False)
            return not gameEnd , not myTurn

    else:
        # two rec in row, kaseb
        msg = recMsg(guiSocket, False)
        if(msg["type"] == ScoketmsgTypes.forfeit.value):
            return False, False
        elif(msg["type"] == ScoketmsgTypes.move.value):
            color,x,y = msg["msg"]["color"], msg["msg"]["x"],msg["msg"]["y"]
            # send move to implementation and wait if it's valid or not
            # and if it's valid it may end the game
            valid = True
            gameEnd = True
            iWon = True
            ourScore =10
            theirScore= 10
            reason = ""
            if(gameEnd):
                end = {'win':iWon , 'ourScore':ourScore , 'theirScore':theirScore}
                sendMsg(guiSocket, end, ScoketmsgTypes.gameEnd.value)
                return not gameEnd ,not myTurn
            else:
                # get captured stones locations if there are ones
                captured = [[1,3]] # array or arrays containing x, y to be removed
                countCaptured = len(captured)
                end =  {'valid':valid , 'reason':reason , 'countCaptured':countCaptured}
                sendMsg(guiSocket, end, ScoketmsgTypes.ack.value)
                if(valid):
                    for i in range(countCaptured):
                        remove = { 'x':x , 'y':y }
                        sendMsg(guiSocket, remove, ScoketmsgTypes.remove.value)
                        msg = recMsg(guiSocket ,False)
                    return not gameEnd ,not myTurn
                return not gameEnd , myTurn

def recMsg(mySocket,aiVsAi):
    msg =  json.loads(mySocket.recv(4096).decode())
    if(msg["type"] == ScoketmsgTypes.exit.value):
        #end implementation
        if(aiVsAi):
            pass
        #end communication
        exit()
    return msg

def sendMsg(socket, msg, mType):
    sendedMsg = {'type':mType , 'msg':msg}
    socket.send(str.encode(json.dumps(sendedMsg) + '\n'))

if __name__ == '__main__':
    main()
