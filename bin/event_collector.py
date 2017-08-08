#!/usr/bin/env python3
import argparse
import json
import os
import socket
import struct
from time import gmtime, strftime


MSG_STRUCT = b'<II256s1024s'
MSG_HEADER = ['timestamp', 'pid', 'user', 'cmdline']


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--socket', default='/tmp/audit.sock', help='The audit socket to listen on (filename)')
    args = parser.parse_args()
    if os.path.exists(args.socket):
        os.unlink(args.socket)

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(args.socket)
    s.listen(1)

    while True:
        print('[+] Listening for connections on {}...'.format(args.socket))
        conn, addr = s.accept()
        d = dict(zip(MSG_HEADER, struct.unpack(MSG_STRUCT, conn.recv(struct.calcsize(MSG_STRUCT)))))
        d['timestamp'] = strftime('%Y-%m-%d %H:%M:%S %z', gmtime(d['timestamp']))
        d['user'] = d['user'].rstrip('\x00')
        d['cmdline'] = d['cmdline'].rstrip('\x00')
        print(json.dumps(d, indent=4))
    return

if __name__ == '__main__':
    main()
