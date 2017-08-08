## Components
A shared object is loaded into all processes via LD_PRELOAD. The shared object has a constructor function that is called before main() in any process run via exec(). This constructor reads `/proc/self/cmdline` and publishes the contents to a UNIX socket.

Another process is responsible for monitoring the socket. Check out `bin/event_collector.py` for an example of how to handle these events.

## Example
Run the event collector via:
``` bash
$ python3 bin/event_collector.py
```

Load the shared object into `sleep` via:
``` bash
$ LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd` LD_PRELOAD=libcmdline.so /bin/sleep 1
```

You should see something like this output by the example event collector:
``` bash
$ python bin/event_collector.py
[+] Listening for connections on /tmp/audit.sock...
{
    "timestamp": "2017-08-08 22:01:34 +0000",
    "pid": 87932,
    "user": "user",
    "cmdline": "/bin/sleep 1"
}
```

## Installing
``` bash
[user]$ make libcmdline.so
[root]# echo '/usr/local/lib' >/etc/ld.so.conf.d/cmdline-audit
[root]# echo 'libcmdline.so' >>/etc/ld.so.preload
[root]# cp -v libcmdline.so /usr/local/lib
[root]# ldconfig
```
