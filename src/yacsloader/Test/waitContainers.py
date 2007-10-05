#!/usr/bin/env python

import os
import orbmodule
clt = orbmodule.client()
FactoryContainer = "/Containers/" + os.environ["NSHOST"] + "/FactoryServer"
clt.waitNS(FactoryContainer)

