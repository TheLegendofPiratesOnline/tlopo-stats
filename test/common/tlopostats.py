import subprocess
import atexit
import time
import os


class Daemon(object):
    DAEMON_PATH = './tlopostats'
    DATABASE = 'tlopo_stats_test'

    _running = None

    def __init__(self):
        self.daemon = None

        atexit.register(self.stop)

    def __del__(self):
        self.stop()

    def start(self, resetCache=True):
        if Daemon._running:
            Daemon._running.stop()

        Daemon._running = self

        if resetCache:
            try:
                os.remove('avMgr.cache')

            except:
                pass

            try:
                os.remove('collectors.cache')

            except:
                pass

        args = ['--db', 'mongodb://127.0.0.1:27017/%s' % self.DATABASE]
        self.daemon = subprocess.Popen([self.DAEMON_PATH] + args)
        time.sleep(1.0)

    def stop(self):
        if self.daemon:
            self.daemon.kill()
            self.deamon = None
            Daemon._running = None
            time.sleep(1.0)

    def restart(self):
        self.stop()
        self.start(False)
