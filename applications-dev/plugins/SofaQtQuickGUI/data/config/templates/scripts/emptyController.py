"""type: SofaContent"""

import Sofa.Core

class %ComponentName%(Sofa.Core.Controller):
    """ custom %ComponentName% component for SOFA """

    def __init__(self, *args, **kwargs):
        Sofa.Core.Controller.__init__(self, *args, **kwargs)
        pass

    def init(self):
        pass
    
    def reinit(self):
        pass
    
    # DEFAULT EVENTS:
    def onAnimateBeginEvent(self, event):
        """ called at the beginning of each time step """
        pass

    def onAnimateEndEvent(self, event):
        """ called at the end of each time step """
        pass

    def onKeypressedEvent(self, event):
        """ called when a key release event is triggered from the UI """
        pass

    def onKeyreleasedEvent(self, event):
        """ called when a key release event is triggered from the UI """
        pass

    def onMouseEvent(self, event):
        """ called when a mouse event is triggered from the UI """
        pass

    def onScriptEvent(self, event):
        """ catches events sent from other python scripts """
        pass

    def onEvent(self, event):
        """ generic method called when no script method exists for the sent event """
        pass

