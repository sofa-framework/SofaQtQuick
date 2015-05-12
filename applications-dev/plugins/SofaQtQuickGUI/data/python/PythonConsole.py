import sys
import PythonConsole

## out => file + stdout
class ConsoleLogger(object):
    def __init__(self, stream):
        self.out = stream

    def write(self, message):
        self.out.write(message)
        self.out.flush()
        PythonConsole.write(message)

sys.stdout = ConsoleLogger(sys.stdout)
sys.stderr = ConsoleLogger(sys.stderr)
