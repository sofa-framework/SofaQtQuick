import sys
import PythonConsole

## out => file + stdout
class ConsoleLogger(object):
    def __init__(self, stream):
        self.out = stream

    def write(self, message):
        self.out.write(message)
        self.flush()
        PythonConsole.write(message)

    def flush(self):
        self.out.flush()

sys.stdout = ConsoleLogger(sys.stdout)
sys.stderr = ConsoleLogger(sys.stderr)
