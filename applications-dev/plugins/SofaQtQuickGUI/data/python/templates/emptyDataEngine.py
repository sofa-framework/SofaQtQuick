"""type: SofaContent"""

import Sofa.Core
from Sofa.Helper import msg_info


class %ComponentName%(Sofa.Core.DataEngine):
    """ custom %ComponentName% component for SOFA """

    def __init__(self, *args, **kwargs):
        Sofa.Core.DataEngine.__init__(self, *args, **kwargs)
        pass

    def init(self):
        pass

    def update(self):
        """
        called anytime an output is accessed while the component
        is dirty (input has changed)
        """
        msg_info('Not implemented yet')
        pass
    
