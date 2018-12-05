#!/usr/bin/env python
#
# This script reads hw model number from environment variable (defaults to "3DA0100") and 
# adds it as C preprocessor defines, so it's available both for #ifdef checks for
# configuration based on model, and also as C string for printing / passing to firmware update service.

import os
import sys

#model = os.getenv("X_MODEL_NUMBER","3DA0100");
model = sys.argv[1]
version = os.getenv("X_VERSION","0.0.1")

print("-DX_MODEL_" + model)
print("-DX_MODEL_NUMBER=\\\"" + model + "\\\"")
print("-DFIRMWARE_VERSION=\\\"" + version + "\\\"")

for arg in sys.argv[2:] :
    print(arg)
