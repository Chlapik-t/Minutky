#!/usr/bin/env python3
# Soubor:  set_device.py
# Datum:   09.03.2024 19:02
# Autor:   Marek Nožka, nozka <@t> spseol <d.t> cz
# Autor:   Marek Nožka, marek <@t> tlapicka <d.t> net
# Licence: GNU/GPL
############################################################################
import sys
from devices import DEVICES

try:
    from pick import pick
except ImportError:
    from winpick import pick


############################################################################


femilly, index = pick(list(DEVICES), "Vyber rodinu:")
device, index = pick(list(DEVICES[femilly]), "Vyber čip:")

with open(sys.argv[1], "w") as f:
    f.write(device.lower())
