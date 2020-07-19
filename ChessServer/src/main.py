import sys
import os
import argparse

import socket
import threading
import select

class ClientConnection:
    def __init__(self, socket, address):
        self.socket = socket
        self.address = address
        self.active = False
        self.username = None

    def send(self, message):
        self.socket.send(message.encode("utf-8"))

def select_connections(connections):
    sockets = map(lambda c: c.socket, connections)
    read, write, exception = select.select(sockets, sockets, sockets)

    read_connections = []
    write_connections = []
    error_connections = []
    for connection in connections:
        if connection.socket in read:
            read_connections.append(connection)
        if connection.socket in write:
            write_connections.append(connection)
        if connection.socket in exception:
            error_connections.append(connection)
    return read_connections, write_connections, error_connections

class ActiveGame:
    CURRENT_GAME_ID = 0

    def __init__(self, owner):
        self.owner = owner
        self.other = None
        self.id = ActiveGame.CURRENT_GAME_ID
        ActiveGame.CURRENT_GAME_ID += 1

    def in_progress(self):
        return self.other is not None

    def format(self):
        return "{} {}".format(self.id, self.owner.username)

class Server:
    def __init__(self, address, port):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.address = address
        self.port = port

        self.connections_mutex = threading.Lock()
        self.connections = []

        self.message_handlers = {
            "CONNECT": self.handle_connection,
            "DISCONNECT": self.handle_disconnection,
            "LIST_GAMES": self.list_games,
            "CREATE_GAME": self.create_game,
            "FINISH_GAME": self.finish_game,
            "JOIN_GAME": self.join_game
        }

        self.active_games = {}

    def start(self, backlog=10):
        self.socket.bind((self.address, self.port))
        self.socket.listen(backlog)

        threading.Thread(target=self._accept_connections).start()

        while True:
            self._get_connections_lock()
            if len(self.connections) > 0:
                read_connections, write_connections, error_connections = select_connections(self.connections)
                for connection in read_connections:
                    data = connection.socket.recv(1024)
                    if len(data) > 0:
                        message = data.decode("utf-8")
                        lines = message.split('\n')
                        if len(lines) > 0:
                            message_type = lines[0]
                            if message_type in self.message_handlers:
                                self.message_handlers[message_type](connection, lines[1:])
                            else:
                                connection.send("INVALID_MESSAGE_TYPE")
                        else:
                            connection.send("INVALID_MESSAGE")
            self._release_connections_lock()

    def handle_connection(self, connection, args):
        if len(args) == 1:
            username = args[0]
            print("Client connected: {} as {}".format(connection.address, username))
            connection.active = True
            connection.username = username
            self._signal_ok(connection)
        else:
            connection.send("ERR_NO_USERNAME")

    def handle_disconnection(self, connection, args):
        print("Client disconnected {} as {}".format(connection.address, connection.username))
        self.connections.remove(connection)
        self._signal_ok(connection)

    def list_games(self, connection, args):
        response = ["OK"]
        for game_id in self.active_games:
            game = self.active_games[game_id]
            if not game.in_progress():
                response.append(game.format())
        print("Listing active games... {}".format(len(response) - 1))
        connection.send(self._encode_message(response))

    def create_game(self, connection, args):
        game = ActiveGame(connection)
        self.active_games[game.id] = game
        print("{} Started new game {}".format(connection.username, game.id))
        connection.send(self._encode_message(["OK", str(game.id)]))

    def finish_game(self, connection, args):
        if len(args) == 1:
            game_id = int(args[0])
            if game_id in self.active_games:
                print("Game {} finished.".format(game_id))
                del self.active_games[game_id]
            self._signal_ok(connection)
        else:
            connection.send("ERR_NO_GAME_ID")

    def join_game(self, connection, args):
        self._signal_ok(connection)

    def _signal_ok(self, connection):
        connection.send("OK")

    def _encode_message(self, message: [str]):
        return '\n'.join(message)

    def _decode_message(self, message: str):
        return message.split('\n')

    def _accept_connections(self):
        while True:
            client_socket, address = self.socket.accept()
            self._get_connections_lock()
            self.connections.append(ClientConnection(client_socket, address))
            self._release_connections_lock()

    def _get_connections_lock(self):
        self.connections_mutex.acquire()

    def _release_connections_lock(self):
        self.connections_mutex.release()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--server", type=str, default=None, help="Address to run server on")
    parser.add_argument("--port", type=int, default=9000, help="Port to run server on")

    args = parser.parse_args()
    address = args.server
    if address is None:
        address = "localhost"

    server = Server(address, args.port)
    server.start()
