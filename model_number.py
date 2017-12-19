#!/usr/bin/env python
import os
import sys

model = os.getenv("X_MODEL_NUMBER","3DA0100");

print("-DX_MODEL_" + model);
print("-DX_MODEL_NUMBER=\\\""+ model + "\\\"");

for arg in sys.argv[1:] :
    print(arg)
